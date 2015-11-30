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



#pragma once



//#define DELOVI_DOCUMENT_USES_TEXTEDIT 1 // QTextEdit
#define DELOVI_DOCUMENT_USES_TEXTEDIT 2 // QPlainTextEdit



// forward declaration(s)
class QFileSystemWatcher;
namespace boost { class thread; };
namespace boost { class mutex; };



#include <Configuration.hpp>
#include "LogfileDocument_hpp.h"

#include <memory>
#include <utility>
#include <atomic>



#if 1 == DELOVI_DOCUMENT_USES_TEXTEDIT
using LogfileDocumentBase = QTextEdit;
#elif 2 == DELOVI_DOCUMENT_USES_TEXTEDIT
using LogfileDocumentBase = QPlainTextEdit;
#endif // DELOVI_DOCUMENT_USES_TEXTEDIT



class LogfileDocument : public LogfileDocumentBase
{
	Q_OBJECT
	
	public:
		using SimpleFilterPair = delovi::ConfigurationEntry::SimpleFilterPair;
		using RegexFilterPair  = delovi::ConfigurationEntry::RegexFilterPair;
		
	public:
		static QString getStrippedName(const QString& filename);
		
	public:
		LogfileDocument(QWidget* const parent=nullptr);
		LogfileDocument(const LogfileDocument&) = delete;
		virtual ~LogfileDocument(void);
		LogfileDocument& operator=(const LogfileDocument&) = delete;
		
		void setFileSystemWatcher(QFileSystemWatcher* const fsw_ptr=nullptr);
		bool loadFile(const QString& filename);
		bool reloadFileFromGuiThread(void);
		void reloadFileUpdateTextDisplayViaSignal(void);
		void setFilename(const QString& filename);
		QString getFilename(void) const;
		QString getUserFriendlyFilename(void) const;

		bool getFilterEmptyLines(void) const;
		void setFilterEmptyLines(const bool newstate);

		bool getSimpleFilterState(void) const;
		delovi::ConfigurationEntry::SimpleFilterPair getSimpleFilter(void) const;
		void setSimpleFilter(delovi::ConfigurationEntry::SimpleFilterPair simple_filter);
		void setSimpleFilter(std::vector<std::string> filter_strings, const bool keep_lines_containing_string);
		
		bool getRegexFilterState(void) const;
		delovi::ConfigurationEntry::RegexFilterPair getRegexFilter(void) const;
		void setRegexFilter(delovi::ConfigurationEntry::RegexFilterPair regex_filter);
		void setRegexFilter(std::string regex, const int syntax);
		
		bool getScrollToBottom(void) const;
		void setScrollToBottom(const bool newstate);
		unsigned int getNumberOfLinesToLoad(void) const;
		void setNumberOfLinesToLoad(const unsigned int number);
		
	signals:
		void signal_updateContent(const bool scroll_to_bottom);
		
	public slots:
		void slot_updateContent(const bool scroll_to_bottom);
		
	private:
		QFileSystemWatcher*            _file_system_watcher;
		delovi::ConfigurationEntry     _config_entry;
		std::unique_ptr<boost::mutex>  _config_mutex;
		std::size_t                    _file_length;
		bool                           _file_reloading;
		std::unique_ptr<boost::mutex>  _file_reloading_mutex;
		std::unique_ptr<boost::thread> _update_thread;
		std::atomic<bool>              _update_thread_active;
		std::atomic<bool>              _update_thread_is_active;
		std::unique_ptr<QString>       _new_content;
		std::unique_ptr<boost::mutex>  _new_content_mutex;
		
	private:
		bool loadFileInternal(const QString& filename, const bool show_error_messagebox, const bool update_text_display_via_signal);
		void setFilenameInternal(const QString& filename);
};

