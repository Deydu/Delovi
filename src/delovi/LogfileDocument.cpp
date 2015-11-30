/*****************************************************************************
 *
 * Copyright (C) 2015 Denny Duphorn <deydu@gmx-topmail.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License version 2
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * Alternatively, you find an online version of the license text under
 * http://www.gnu.org/licenses/gpl-2.0.html.
 *
 *****************************************************************************/



#define DELOVI_DOCUMENT_READS_FILES_BLOCKWISE
//#undef DELOVI_DOCUMENT_READS_FILES_BLOCKWISE



#include "LogfileDocument.hpp"
#include "LogfileDocument_cpp.h"

#ifdef DELOVI_DOCUMENT_READS_FILES_BLOCKWISE
#	include <load_textfile.hpp>
#else
#	include <TextStreamLineParser.hpp>
#endif // DELOVI_DOCUMENT_READS_FILES_BLOCKWISE
#include <helper_macros.h>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>

#include <fstream>
#include <algorithm>
#include <functional>



QString LogfileDocument::getStrippedName(const QString& filename)
{
	return QFileInfo(filename).fileName();
}

LogfileDocument::LogfileDocument(QWidget* const parent)
	: LogfileDocumentBase(parent)
	, _file_system_watcher(nullptr)
	, _config_entry()
	, _config_mutex(std::make_unique<boost::mutex>())
	, _file_length(0)
	, _file_reloading(false)
	, _file_reloading_mutex(std::make_unique<boost::mutex>())
	, _update_thread(nullptr)
	, _update_thread_active(false)
	, _update_thread_is_active(false)
	, _new_content(std::make_unique<QString>())
	, _new_content_mutex(std::make_unique<boost::mutex>())
{
	if (auto* const doc_ptr = document())
	{
		auto to = doc_ptr->defaultTextOption();
		to.setWrapMode(QTextOption::NoWrap);
		doc_ptr->setDefaultTextOption(to);
		
		doc_ptr->setModified(false);
		QFont font("Courier");
		font.setStyleHint(QFont::Monospace);
		font.setPointSize(8);
		font.setFixedPitch(true);
		doc_ptr->setDefaultFont(font);
	}
	setTabStopWidth(30);
	connect(this, SIGNAL(signal_updateContent(const bool)), this, SLOT(slot_updateContent(const bool)));
	_update_thread = std::make_unique<boost::thread>(
		[this]
		{
			try
			{
				while (!boost::this_thread::interruption_requested())
				{
					if (_update_thread_active)
					{
						bool expected = false;
						_update_thread_is_active.compare_exchange_strong(expected, true);
						reloadFileUpdateTextDisplayViaSignal();
					}
					else
					{
						bool expected = true;
						_update_thread_is_active.compare_exchange_strong(expected, false);
					}
					boost::this_thread::sleep(boost::posix_time::milliseconds(250));
				}
			}
			catch (const boost::thread_interrupted&) { }
		});
}

LogfileDocument::~LogfileDocument(void)
{
	_update_thread_active = false;
	_update_thread->interrupt();
	_update_thread->join();
	if (nullptr != _file_system_watcher)
	{
//		boost::lock_guard<decltype(_config_mutex)> lock(_config_mutex);
		COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
		_file_system_watcher->removePath(_config_entry._filename.c_str());
	}
}

void LogfileDocument::setFileSystemWatcher(QFileSystemWatcher* const fsw_ptr)
{
	if (nullptr != _file_system_watcher && fsw_ptr != _file_system_watcher)
	{
		_file_system_watcher->removePath(getFilename());
	}
	_file_system_watcher = fsw_ptr;
}

bool LogfileDocument::loadFile(const QString& filename)
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	if (!loadFileInternal(filename, true, false))
	{
		return false;
	}
	setFilenameInternal(filename);
	return true;
}

bool LogfileDocument::reloadFileFromGuiThread(void)
{
	return loadFileInternal(getFilename(), false, false);
}

void LogfileDocument::reloadFileUpdateTextDisplayViaSignal(void)
{
	loadFileInternal(getFilename(), false, true);
}

void LogfileDocument::setFilename(const QString& filename)
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	setFilenameInternal(filename);
}

QString LogfileDocument::getFilename(void) const
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	return _config_entry._filename.c_str();
}

QString LogfileDocument::getUserFriendlyFilename(void) const
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	return getStrippedName(_config_entry._filename.c_str());
}

bool LogfileDocument::getFilterEmptyLines(void) const
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	return _config_entry._filter_empty_lines;
}

void LogfileDocument::setFilterEmptyLines(const bool newstate)
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	_config_entry._filter_empty_lines = newstate;
	_file_length = 0;
}

bool LogfileDocument::getSimpleFilterState(void) const
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	return !_config_entry._simple_filter.first.empty() && 0 != _config_entry._simple_filter.second;
}

delovi::ConfigurationEntry::SimpleFilterPair LogfileDocument::getSimpleFilter(void) const
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	return _config_entry._simple_filter;
}

void LogfileDocument::setSimpleFilter(delovi::ConfigurationEntry::SimpleFilterPair simple_filter)
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	if (simple_filter.first.empty())
	{
		_config_entry._simple_filter.second = 0;
	}
	else
	{
		_config_entry._simple_filter.swap(simple_filter);
	}
	_file_length = 0;
}

void LogfileDocument::setSimpleFilter(std::vector<std::string> filter_strings, const bool keep_lines_containing_string)
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	if (filter_strings.empty())
	{
		_config_entry._simple_filter.second = 0;
	}
	else
	{
		_config_entry._simple_filter.first  = std::move(filter_strings);
		_config_entry._simple_filter.second = keep_lines_containing_string ? 1 : -1;
	}
	_file_length = 0;
}

bool LogfileDocument::getRegexFilterState(void) const
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	return !_config_entry._regex_filter.first.empty() && -1 <= _config_entry._regex_filter.second && QRegExp::W3CXmlSchema11 >= _config_entry._regex_filter.second;
}

delovi::ConfigurationEntry::RegexFilterPair LogfileDocument::getRegexFilter(void) const
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	return _config_entry._regex_filter;
}

void LogfileDocument::setRegexFilter(RegexFilterPair regex_filter)
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	if (regex_filter.first.empty())
	{
		_config_entry._regex_filter.second = -99;
	}
	else
	{
		_config_entry._regex_filter.first  = std::move(regex_filter.first);
		_config_entry._regex_filter.second = regex_filter.second;
	}
	_file_length = 0;
}

void LogfileDocument::setRegexFilter(std::string regex, const int syntax)
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	if (regex.empty())
	{
		_config_entry._regex_filter.second = -99;
	}
	else
	{
		_config_entry._regex_filter.first  = std::move(regex);
		_config_entry._regex_filter.second = syntax;
	}
	_file_length = 0;
}

bool LogfileDocument::getScrollToBottom(void) const
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	return _config_entry._scroll_to_bottom;
}

void LogfileDocument::setScrollToBottom(const bool newstate)
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	_config_entry._scroll_to_bottom = newstate;
}

unsigned int LogfileDocument::getNumberOfLinesToLoad(void) const
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	return _config_entry._number_of_lines_to_show;
}

void LogfileDocument::setNumberOfLinesToLoad(const unsigned int number)
{
	COMMON_LOCK_GUARD(boost::lock_guard, *_config_mutex);
	_config_entry._number_of_lines_to_show = number;
	_file_length = 0;
}

void LogfileDocument::slot_updateContent(const bool scroll_to_bottom)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
	auto* const vscrollbar = verticalScrollBar();
	const auto scrollbar_value = nullptr != vscrollbar ? vscrollbar->value() : -1;
	
	QString newtext;
	{
		COMMON_LOCK_GUARD(boost::lock_guard, *_new_content_mutex);
		_new_content->swap(newtext);
	}
	setPlainText(newtext);
	
	QApplication::restoreOverrideCursor();
	if (scroll_to_bottom)
	{
		auto c = textCursor();
		c.movePosition(QTextCursor::End);
		c.movePosition(QTextCursor::StartOfLine);
		setTextCursor(c);
	}
	else
	{
		if (nullptr != vscrollbar && -1 != scrollbar_value)
		{
			vscrollbar->setValue(scrollbar_value);
		}
	}
}

bool LogfileDocument::loadFileInternal(const QString& filename, const bool show_error_messagebox, const bool update_text_display_via_signal)
{
	if (filename.isEmpty())
	{
		return false;
	}
	
#ifdef DELOVI_DOCUMENT_READS_FILES_BLOCKWISE
	if (_file_reloading)
	{
		return true;
	}

	// set aquire lock
	{
		COMMON_LOCK_GUARD(boost::lock_guard, *_file_reloading_mutex);
		if (_file_reloading)
		{
			return true;
		}
		_file_reloading = true;
	}
	COMMON_SCOPE_EXIT([this] { _file_reloading = false; });

	const std::string& filepath = filename.toStdString();
	std::ifstream ifs(filepath, std::ios::binary);
	std::size_t file_size = 0;
	QString allnewtext;
	if (ifs.is_open())
	{
		const unsigned int number_of_lines_to_load = _config_entry._number_of_lines_to_show;
		const bool filter_simple_activated = !_config_entry._simple_filter.first.empty() && 0 != _config_entry._simple_filter.second;
		const bool filter_regex_activated  = !_config_entry._regex_filter.first.empty() && -1 <= _config_entry._regex_filter.second && QRegExp::W3CXmlSchema11 >= _config_entry._regex_filter.second;
		const QRegExp qre(filter_regex_activated ? _config_entry._regex_filter.first.c_str() : "", Qt::CaseSensitive, static_cast<QRegExp::PatternSyntax>(_config_entry._regex_filter.second));
		const boost::regex bre(filter_regex_activated ? _config_entry._regex_filter.first : "");
		auto condition_to_load = [&] (const std::size_t file_size) -> bool
		{
			return _file_length != file_size;
		};
		auto condition_for_line = [&] (const std::string& line) -> bool
		{
//			return !line.empty();
			if (_config_entry._filter_empty_lines && line.empty())
			{
//				qDebug("[DEBUG][%s][%u] removing empty line", __FILE__, __LINE__);
				return false;
			}
			if (filter_simple_activated)
			{
//				qDebug("[DEBUG][LogfileDocument::loadFileInternal][%u] applying simple filter...", __LINE__);
				for (const std::string& str : _config_entry._simple_filter.first)
				{
//					qDebug("[DEBUG][LogfileDocument::loadFileInternal][%u] simple filter, testing: line \"%s\" vs. \"%s\"", __LINE__, line.substr(0, 30).c_str(), str.c_str());
					if (boost::algorithm::contains(line, str))
					{
//						qDebug("[DEBUG][LogfileDocument::loadFileInternal][%u] simple filter, line \"%s\" contains \"%s\"", __LINE__, line.substr(0, 30).c_str(), str.c_str());
						return 0 < _config_entry._simple_filter.second;
					}
				}
//				qDebug("[DEBUG][LogfileDocument::loadFileInternal][%u] finished simple filter.", __LINE__);
				return 0 > _config_entry._simple_filter.second;
			}
			if (filter_regex_activated)
			{
				if (0 <= _config_entry._regex_filter.second)
				{
					return qre.exactMatch(line.c_str());
				}
				else
				{
					return boost::regex_match(line, bre);
				}
			}
			return true;
		};
		std::list<std::string> lines_from_file;
		bool contains_a_utf8_sequence = false;
		int rc = 0;
		if (0 == number_of_lines_to_load)
		{
//			qDebug("[DEBUG][%s][%u] loading file content (forward)...", __FILE__, __LINE__);
			rc = delovi::load_all_lines_from_stream
			(
				ifs,
				condition_to_load,
				condition_for_line,
				lines_from_file,
				file_size,
				contains_a_utf8_sequence
			);
//			qDebug("[DEBUG][%s][%u] file content loaded (forward).", __FILE__, __LINE__);
		}
		else
		{
//			qDebug("[DEBUG][%s][%u] loading file content (backward)...", __FILE__, __LINE__);
			const std::size_t block_size = 1024;
			rc = delovi::load_n_lines_from_stream
			(
				number_of_lines_to_load,
				ifs,
				condition_to_load,
				condition_for_line,
				lines_from_file,
				file_size,
				contains_a_utf8_sequence,
				block_size
			);
//			qDebug("[DEBUG][%s][%u] file content loaded (backward).", __FILE__, __LINE__);
		}
		ifs.close();
		if (0 < rc)
		{
			allnewtext.reserve(static_cast<int>(file_size));
			if (contains_a_utf8_sequence)
			{
				for (const std::string& str : lines_from_file)
				{
					allnewtext += QString::fromUtf8(str.c_str());
				}
			}
			else
			{
				for (const std::string& str : lines_from_file)
				{
					allnewtext += str.c_str();
				}
			}
			if ('\n' == allnewtext.at(allnewtext.size() - 1))
			{
				allnewtext.remove(allnewtext.size() - 1, 1);
			}
//			qDebug("[DEBUG][%s][%u] swapping file content...", __FILE__, __LINE__);
			{
				COMMON_LOCK_GUARD(boost::lock_guard, *_new_content_mutex);
				_new_content->swap(allnewtext);
			}
//			qDebug("[DEBUG][%s][%u] file content swapped.", __FILE__, __LINE__);
//			qDebug("[DEBUG][%s][%u] calling content-updating-method...", __FILE__, __LINE__);
			if (update_text_display_via_signal)
			{
				emit signal_updateContent(_config_entry._scroll_to_bottom);
			}
			else
			{
				slot_updateContent(_config_entry._scroll_to_bottom);
			}
//			qDebug("[DEBUG][%s][%u] content-updating-method called.", __FILE__, __LINE__);
			_file_length = file_size;
		}
		else if (0 > rc)
		{
			qWarning("[WARNING][%s][%u] file \"%s\" could not be loaded!", __FILE__, __LINE__, filepath.c_str());
			if (show_error_messagebox)
			{
				QMessageBox::warning(this, tr("Reading File"), tr("Cannot read file \"%1\".").arg(filename));
			}
//			qDebug("[DEBUG][%s][%u] swapping file content...", __FILE__, __LINE__);
			{
				COMMON_LOCK_GUARD(boost::lock_guard, *_new_content_mutex);
				_new_content->swap(allnewtext);
			}
//			qDebug("[DEBUG][%s][%u] file content swapped.", __FILE__, __LINE__);
//			qDebug("[DEBUG][%s][%u] calling content-updating-method...", __FILE__, __LINE__);
			if (update_text_display_via_signal)
			{
				emit signal_updateContent(_config_entry._scroll_to_bottom);
			}
			else
			{
				slot_updateContent(_config_entry._scroll_to_bottom);
			}
//			qDebug("[DEBUG][%s][%u] content-updating-method called.", __FILE__, __LINE__);
			_file_length = 0;
		}
	}
	else // ! ifs.is_open()
	{
		if (show_error_messagebox)
		{
			QMessageBox::warning(this, tr("Reading File"), tr("Cannot read file \"%1\".").arg(filename));
		}
//		qDebug("[DEBUG][%s][%u] swapping file content...", __FILE__, __LINE__);
		{
			COMMON_LOCK_GUARD(boost::lock_guard, *_new_content_mutex);
			_new_content->swap(allnewtext);
		}
//		qDebug("[DEBUG][%s][%u] file content swapped.", __FILE__, __LINE__);
//		qDebug("[DEBUG][%s][%u] calling content-updating-method...", __FILE__, __LINE__);
		if (update_text_display_via_signal)
		{
			emit signal_updateContent(_config_entry._scroll_to_bottom);
		}
		else
		{
			slot_updateContent(_config_entry._scroll_to_bottom);
		}
//		qDebug("[DEBUG][%s][%u] content-updating-method called.", __FILE__, __LINE__);
		_file_length = 0;
	}
	return true;
#else
	if (_file_reloading)
	{
		return true;
	}

	// set aquire lock
	{
		COMMON_LOCK_GUARD(boost::lock_guard, *_file_reloading_mutex);
		if (_file_reloading)
		{
			return true;
		}
		_file_reloading = true;
	}
	COMMON_SCOPE_EXIT([this] { _file_reloading = false; });
	
	bool retval = false;
	std::ifstream ifs(filename.toStdString().c_str(), std::ios::binary);
	if (ifs.is_open())
	{
		ifs.seekg(0, std::ios::end);
		const std::size_t length = static_cast<std::size_t>(ifs.tellg());
		if (length != _file_length)
		{
			qDebug("[DEBUG][%s][%u] loading file %s", __FILE__, __LINE__, filename.toStdString().c_str());
			std::string line;
			line.reserve(1024);
			QString allnewtext;
			allnewtext.reserve(static_cast<int>(length));
			const unsigned int number_of_lines_to_load = _config_entry._number_of_lines_to_show;
			if (0 == number_of_lines_to_load)
			{
				ifs.seekg(0, std::ios::beg);
				while (ifs.good())
				{
					const int rc = delovi::TextStreamLineParser::parseLineForward(ifs, line, true);
					if (delovi::TextStreamLineParser::NO_CHAR_READ == rc)
					{
						break;
					}
					if (delovi::TextStreamLineParser::EMPTY_LINE == rc || line.empty())
					{
						// after all we did say to add append a line-break to the end of the line
						continue;
					}
//					append(QString::fromUtf8(line.c_str()));  -->  line break
					if (delovi::TextStreamLineParser::UTF8 == rc)
					{
						allnewtext += QString::fromUtf8(line.c_str());
					}
					else
					{
						allnewtext += line.c_str();
					}
				} // while
//				qDebug("[DEBUG][%s][%u] file content loaded (forward).", __FILE__, __LINE__);
			}
			else
			{
//				qDebug("[DEBUG][%s][%u] loading file content (backward)...", __FILE__, __LINE__);
				std::vector<QString> readlines;
				readlines.reserve(number_of_lines_to_load);
				while (ifs.good() && number_of_lines_to_load > readlines.size())
				{
					const int rc = delovi::TextStreamLineParser::parseLineBackward(ifs, line, true);
					if (delovi::TextStreamLineParser::NO_CHAR_READ == rc)
					{
						break;
					}
					if (delovi::TextStreamLineParser::EMPTY_LINE == rc || line.empty())
					{
						// after all we did say to add append a line-break to the end of the line
						continue;
					}
					if (delovi::TextStreamLineParser::UTF8 == rc)
					{
						readlines.push_back(QString::fromUtf8(line.c_str()));
					}
					else
					{
						readlines.push_back(line.c_str());
					}
				} // while
//				qDebug("[DEBUG][%s][%u] file content loaded (backward).", __FILE__, __LINE__);
				using readlines_cont = decltype(readlines);
				std::for_each(readlines.rbegin(), readlines.rend(), [&allnewtext] (const readlines_cont::value_type& readlines_value) { allnewtext.append(readlines_value); });
//				qDebug("[DEBUG][%s][%u] file lines reverse appended.", __FILE__, __LINE__);
			}
			if ('\n' == allnewtext.at(allnewtext.size() - 1))
			{
				allnewtext.remove(allnewtext.size() - 1, 1);
			}
//			qDebug("[DEBUG][%s][%u] swapping file content...", __FILE__, __LINE__);
			{
				COMMON_LOCK_GUARD(boost::lock_guard, *_new_content_mutex);
				_new_content->swap(allnewtext);
			}
//			qDebug("[DEBUG][%s][%u] file content swapped.", __FILE__, __LINE__);
//			qDebug("[DEBUG][%s][%u] calling content-updating-method...", __FILE__, __LINE__);
			if (update_text_display_via_signal)
			{
				emit signal_updateContent(_config_entry._scroll_to_bottom);
			}
			else
			{
				slot_updateContent(_config_entry._scroll_to_bottom);
			}
//			qDebug("[DEBUG][%s][%u] content-updating-method called.", __FILE__, __LINE__);
			_file_length = length;
		}
//		else // length == _file_length
//		{
//			// do nothing at the moment
//		}
		retval = true;
	}
	else // ! ifs.is_open()
	{
		if (show_error_messagebox)
		{
			QMessageBox::warning(this, tr("Reading File"), tr("Cannot read file \"%1\".").arg(filename));
		}
		QString new_content;
		{
			COMMON_LOCK_GUARD(boost::lock_guard, *_new_content_mutex);
			_new_content->swap(new_content);
		}
		if (update_text_display_via_signal)
		{
			emit signal_updateContent(_config_entry._scroll_to_bottom);
		}
		else
		{
			slot_updateContent(_config_entry._scroll_to_bottom);
		}
		_file_length = 0;
		retval = true;
	}
	
	return retval;
#endif // DELOVI_DOCUMENT_READS_FILES_BLOCKWISE
}

void LogfileDocument::setFilenameInternal(const QString& filename)
{
	if (_config_entry._filename.empty())
	{
		if (filename.isEmpty())
		{
			// nothing I can do here
			return;
		}
		else
		{
			// mpFileSystemWatcher: nothing to do because I cannot remove a path I don't know any more, that must have been done previously
			// mpUpdateThread: thread must have been stopped before or was never started
		}
	}
	else
	{
		if (filename.isEmpty())
		{
			if (nullptr != _file_system_watcher)
			{
				_file_system_watcher->removePath(_config_entry._filename.c_str());
			}
			_update_thread_active = false;
			while (_update_thread_is_active);
		}
		else
		{
			if (nullptr != _file_system_watcher && filename.toStdString() != _config_entry._filename)
			{
				_file_system_watcher->removePath(_config_entry._filename.c_str());
			}
			// mpUpdateThread: thread will be started below, the fact that it is already started is not a problem
		}
	}
	_config_entry._filename = filename.toStdString();
	if (nullptr != _file_system_watcher)
	{
		_file_system_watcher->addPath(filename);
	}
	_update_thread_active = true;
	while (!_update_thread_is_active);
	setWindowModified(false);
//	setWindowTitle(getStrippedName(filename) + "[*]");
	setWindowTitle(getStrippedName(filename));
}
