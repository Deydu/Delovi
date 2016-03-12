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



#ifdef _MSC_VER
#	define _CRT_SECURE_NO_WARNINGS
#endif // _MSC_VER

#include "Mainwindow.hpp"
#include "LogfileDocument.hpp"
#include "SimpleFilterDialog.hpp"
#include "RegexFilterDialog.hpp"

#include <delovi_definitions.h>
#include <Configuration.hpp>
#ifdef DELOVI_COMPILER_IS_GNU
#	include <make_unique.hpp>
#endif // DELOVI_COMPILER_IS_GNU

#if defined __GNUC__
//#	pragma GCC diagnostic push
//#	pragma GCC diagnostic ignored "-Wall"
#elif defined __SUNPRO_CC
#	pragma disable_warn
#elif defined _MSC_VER
#	pragma warning(push)
//#	pragma warning(disable : 4127)	// conditional expression is constant
#	pragma warning(disable : 4251)	// class needs to have dll-interface to be used
//#	pragma warning(disable : 4514)	//  unreferenced inline function has been removed
#endif
#include "ui_main_window.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QMdiSubWindow>
#include <QSignalMapper>
#include <QDragEnterEvent>
#include <QDebug>
#include <QUrl>
#include <QFileSystemWatcher>
#include <QInputDialog>
#if defined __GNUC__
//#	pragma GCC diagnostic pop
#elif defined __SUNPRO_CC
#	pragma enable_warn
#elif defined _MSC_VER
#	pragma warning(pop)
#endif

#include <boost/version.hpp>



namespace { // anonymous


QAction* create_action(QObject* const parent, const QString& text, const QString& shortcut)
{
	auto* const action = new QAction(text, parent);
#ifndef QT_NO_STATUSTIP
	action->setStatusTip(text);
#endif
	if (!shortcut.isEmpty())
	{
		action->setShortcut(shortcut);
	}
	return action;
}

void reset_action_text(QAction& action, const QString& text)
{
	action.setText(text);
#ifndef QT_NO_STATUSTIP
	action.setStatusTip(text);
#endif
}

//std::string tostdstring(const std::string& loglevel, const std::string& filename, const unsigned int line_number)
//{
//	std::stringstream ss;
//	ss << "[" << loglevel << "][" << filename << "][" << line_number << "]";
//	return ss.str();
//}

//QString toqstring(const QString& loglevel, const QString& filename, const unsigned int line_number)
//{
//	const QString retval = QString("[%1][%2][%3]").arg(loglevel).arg(filename).arg(line_number);
//	return retval;
//}


} // anonymous namespace



QString Mainwindow::toString(const eShortcuts sc)
{
	switch (sc)
	{
		case SHORTCUT_CTRL_O:	return tr("Ctrl+O");
		case SHORTCUT_CTRL_Q:	return tr("Ctrl+Q");
		case SHORTCUT_ALT_T:	return tr("Alt+T");
		case SHORTCUT_ALT_C:	return tr("Alt+C");
		case SHORTCUT_ALT_A:	return tr("Alt+A");
		case SHORTCUT_ALT_Q:	return tr("Alt+Q");
		default:				return "";
	}
}

Mainwindow::Mainwindow(QWidget* const parent, const Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
	, _ui(nullptr)
	, _window_mapper(nullptr)
	, _action_filter_empty_lines(nullptr)
	, _action_simple_filter(nullptr)
	, _action_regex_filter(nullptr)
	, _action_config_scroll_to_bottom(nullptr)
	, _action_config_number_of_lines_to_show(nullptr)
	, _action_windows_tile(nullptr)
	, _action_windows_cascade(nullptr)
	, _action_windows_separator(nullptr)
	, _file_system_watcher(nullptr)
	, _config_filename()
{
	auto tmp = std::make_unique<Ui::Mainwindow>();
	tmp->setupUi(this);
	_ui = tmp.release();
	
	setWindowTitle(QApplication::applicationName());
	_ui->action_file_open->setShortcut(toString(SHORTCUT_FILE_OPEN));
	_ui->action_quit->setShortcut(toString(SHORTCUT_FILE_QUIT));
	_ui->action_about->setShortcut(toString(SHORTCUT_ABOUT));
	_ui->action_about_qt->setShortcut(toString(SHORTCUT_ABOUT_QT));
	
	setCentralWidget(_ui->mdi_area);
	connect(_ui->menu_filter, SIGNAL(aboutToShow(void)), this, SLOT(slot_updateFilterMenu(void)));
	connect(_ui->menu_preferences, SIGNAL(aboutToShow(void)), this, SLOT(slot_updatePreferencesMenu(void)));
	connect(_ui->menu_window, SIGNAL(aboutToShow(void)), this, SLOT(slot_updateWindowsMenu(void)));
	
	_window_mapper = new QSignalMapper(this);
	connect(_window_mapper, SIGNAL(mapped(QWidget*)), this, SLOT(slot_setActiveSubWindow(QWidget*)));
	
	_file_system_watcher = new QFileSystemWatcher(this);
	connect(_file_system_watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(slot_handleFileChange(const QString&)));
	
	setAcceptDrops(true);
	
	delovi::Configuration tp;
#ifdef WIN32
	if (const char* const homedrive = std::getenv("HOMEDRIVE"))
	{
		if (const char* const homepath = std::getenv("HOMEPATH"))
		{
			_config_filename = homedrive;
			_config_filename += homepath;
			_config_filename += "\\";
		}
	}
#elif LINUX
	if (const char* const homepath = std::getenv("HOME"))
	{
		_config_filename = homepath;
		_config_filename += "/";
	}
#endif
	_config_filename += DELOVI_CONFIGURATION_FILENAME;
	tp.loadFromFile(_config_filename);
	tp.doForEach(
		[this] (const delovi::ConfigurationEntry& pref)
		{
			QMdiSubWindow* mdi_subwindow = nullptr;
			LogfileDocument* currdoc = nullptr;
			openFile
			(
				QString(pref._filename.c_str()),
				&mdi_subwindow,
				&currdoc,
				&pref._filter_empty_lines,
				&pref._simple_filter,
				// reg-expr filter settings
				pref._number_of_lines_to_show
			);
//			if (nullptr != mdi_subwindow && 0 != pref._window_rect._width && 0 != pref._window_rect._height)
			if (nullptr != mdi_subwindow)
			{
				mdi_subwindow->move(pref._window_rect._x, pref._window_rect._y);
//				mdi_subwindow->resize(pref._window_rect._width, pref._window_rect._height);
				mdi_subwindow->resize(0 == pref._window_rect._width ? 100 : pref._window_rect._width, 0 == pref._window_rect._height ? 100 : pref._window_rect._height);
			}
			if (nullptr != currdoc)
			{
				currdoc->setScrollToBottom(pref._scroll_to_bottom);
//				currdoc->setNumberOfLinesToLoad(pref._number_of_lines_to_show);
			}
		});
}

Mainwindow::~Mainwindow(void)
{
	delovi::Configuration config;
	const auto& sub_window_list = _ui->mdi_area->subWindowList();
	for (const auto& currwindow : sub_window_list)
	{
		if (const auto* const currdoc = qobject_cast<const LogfileDocument*>(currwindow->widget()))
		{
			const auto& currwindowpos = currwindow->pos();
			const auto& currwindowsize = currwindow->size();
			auto simple_filter = currdoc->getSimpleFilter();
			
			delovi::ConfigurationEntry ce;
			ce._filename				= std::move(currdoc->getFilename().toStdString());
			ce._window_rect._x			= currwindowpos.x();
			ce._window_rect._y			= currwindowpos.y();
			ce._window_rect._width		= currwindowsize.width();
			ce._window_rect._height		= currwindowsize.height();
			ce._scroll_to_bottom		= currdoc->getScrollToBottom();
			ce._number_of_lines_to_show	= currdoc->getNumberOfLinesToLoad();
			ce._filter_empty_lines		= currdoc->getFilterEmptyLines();
			ce._simple_filter.first		= std::move(simple_filter.first);
			ce._simple_filter.second	= simple_filter.second;
			
			config.addOpenedDocument(std::move(ce));
		}
	}
	config.saveToFile(_config_filename);
}

void Mainwindow::openFile
(
	const QString& filename,
	QMdiSubWindow** const mdi_subwindow_pptr,
	LogfileDocument** const document_pptr,
	const bool* const filter_empty_lines,
	const std::pair<std::vector<std::string>, int>* const filter_simple,
	int number_of_lines_to_load
)
{
	if (filename.isEmpty())
	{
		return;
	}
	LogfileDocument* currdoc = nullptr;
	if (auto* mdi_subwindow = findDocumentChild(filename))
	{
		_ui->mdi_area->setActiveSubWindow(mdi_subwindow);
		currdoc = qobject_cast<LogfileDocument*>(mdi_subwindow->widget());
		if (nullptr != mdi_subwindow_pptr)
		{
			*mdi_subwindow_pptr = mdi_subwindow;
		}
	}
	else
	{
		currdoc = new LogfileDocument(this);
		if (0 > number_of_lines_to_load)
		{
			number_of_lines_to_load = delovi::ConfigurationEntry::DEFAULT_NUMBER_OF_LINES_TO_SHOW;
		}
		else if (delovi::ConfigurationEntry::MAX_NUMBER_OF_LINES_TO_SHOW < number_of_lines_to_load)
		{
			number_of_lines_to_load = delovi::ConfigurationEntry::MAX_NUMBER_OF_LINES_TO_SHOW;
		}
		if (nullptr != filter_empty_lines)
		{
			currdoc->setFilterEmptyLines(*filter_empty_lines);
		}
//		if (nullptr != filter_simple_expr && nullptr != show_lines_containing_simple_expr)
//		{
//			currdoc->setFilterSimple(filter_simple_expr->c_str(), *show_lines_containing_simple_expr);
//		}
		if (nullptr != filter_simple)
		{
			currdoc->setSimpleFilter(*filter_simple);
		}
		currdoc->setNumberOfLinesToLoad(static_cast<unsigned int>(number_of_lines_to_load));
		if (!currdoc->loadFile(filename))
		{
			statusBar()->showMessage(tr("File NOT loaded!"), 5000);
			currdoc->close();
			currdoc = nullptr;
		}
		else
		{
			statusBar()->showMessage(tr("File loaded."), 5000);
			currdoc->setFileSystemWatcher(_file_system_watcher);
			mdi_subwindow = _ui->mdi_area->addSubWindow(currdoc);
			currdoc->show();
		}
		if (nullptr != mdi_subwindow_pptr)
		{
			*mdi_subwindow_pptr = mdi_subwindow;
		}
	}
	if (nullptr != document_pptr)
	{
		*document_pptr = currdoc;
	}
}


void Mainwindow::on_action_file_open_triggered(void)
{
	const auto& files_to_open = QFileDialog::getOpenFileNames
	(
		this,
		tr("Open File(s)"),
		QString(), // no specific start dir
		QString("Text Files (*.txt *.log);;All Files (*.*)"),
		nullptr, // no specific filter selected
		DELOVI_FILE_DIALOG_OPTIONS
	);
//	qDebug("Number of files to be opened: %u.", files_to_open.size());
	for (const auto& qstr : files_to_open)
	{
		openFile(qstr);
	}
}

void Mainwindow::on_action_quit_triggered(void)
{
	close();
}

void Mainwindow::on_action_about_triggered(void)
{
	static const QString list_entry = "&nbsp;&nbsp;&nbsp;&nbsp;&bull;&nbsp;";
#ifdef NDEBUG
	static const auto compile_mode = tr("Release");
#else
	static const auto compile_mode = tr("Debug");
#endif // NDEBUG
	QMessageBox::about
	(
		this,
		tr("About ") + QCoreApplication::applicationName(),
		"<b>" + QCoreApplication::applicationName() + "</b>" + tr(" Version ") + QCoreApplication::applicationVersion() + "<br>" + "<br>" + tr("Author: ") + "Denny Duphorn" + "<br>" + "<br>" + tr("Build Configuration: ") + compile_mode + "<br>" + "<br>" + tr("Build Dependencies:") + "<br>" + list_entry + tr("Boost Version %1.%2.%3").arg(BOOST_VERSION / 100000).arg(BOOST_VERSION / 100 % 1000).arg(BOOST_VERSION % 100) + "<br>" + list_entry + tr("Qt Version ") + QT_VERSION_STR
	);
}

void Mainwindow::on_action_about_qt_triggered(void)
{
	qApp->aboutQt();
}

void Mainwindow::dragEnterEvent(QDragEnterEvent* dragevent)
{
	if (nullptr == dragevent)
	{
		qWarning("[WARNING][%s][%u] got a null pointer drag-event!", __FILE__, __LINE__);
		return;
	}
//	qDebug("[DEBUG][%s][%u] got a drag-event.", __FILE__, __LINE__);
	if (const auto* const dragmime_ptr = dragevent->mimeData())
	{
//		if (dragmime_ptr->hasFormat("text/uri-list"))
		if (dragmime_ptr->hasUrls())
		{
//			qDebug("[DEBUG][%s][%u] got a drag-event of type \"text/uri-list\".", __FILE__, __LINE__);
			dragevent->acceptProposedAction();
		}
//		//else if (dragmime_ptr->hasFormat("text/plain"))
//		else if (dragmime_ptr->hasText())
//		{
//			qDebug("[DEBUG][%s][%u] got a drag-event of type \"text/plain\".", __FILE__, __LINE__);
//		}
#ifndef QT_NO_DEBUG_OUTPUT
		else
		{
//			qDebug() << toqstring("DEBUG", __FILE__, __LINE__) << "got a drag-event with following formats:" << dragmime_ptr->formats();
		}
#endif // QT_NO_DEBUG_OUTPUT
	}
	else
	{
		qWarning("[WARNING][%s][%u] got a drag-event containing no mimedata!", __FILE__, __LINE__);
		return;
	}
}

void Mainwindow::dropEvent(QDropEvent* dropevent)
{
	if (nullptr == dropevent)
	{
		qWarning("[WARNING][%s][%u] got a null pointer drop-event!", __FILE__, __LINE__);
	}
//	qDebug("[DEBUG][%s][%u] got a drop-event.", __FILE__, __LINE__);
	else if (const auto* const dropmime_ptr = dropevent->mimeData())
	{
//		if (dropmime_ptr->hasFormat("text/uri-list"))
		if (dropmime_ptr->hasUrls())
		{
//			qDebug("[DEBUG][%s][%u] got a drop-event of type \"text/uri-list\".", __FILE__, __LINE__);
			dropevent->acceptProposedAction();
			const auto& dropurls = dropmime_ptr->urls();
//			qDebug() << toqstring("DEBUG", __FILE__, __LINE__) << "got a drop-event with following urls:" << dropurls;
			for (const auto& qurl : dropurls)
			{
				openFile(qurl.toLocalFile());
			}
		}
#ifndef QT_NO_DEBUG_OUTPUT
		else
		{
//			qDebug() << toqstring("DEBUG", __FILE__, __LINE__) << "got a drop-event with following formats:" << dropmime_ptr->formats();
		}
#endif // QT_NO_DEBUG_OUTPUT
	}
	else
	{
		qWarning("[WARNING][%s][%u] got a drop-event containing no mimedata!", __FILE__, __LINE__);
	}
}

void Mainwindow::slot_updateFilterMenu(void)
{
	_ui->menu_filter->clear();
	if (auto* const currdoc = getActiveDocumentChild())
	{
		if (nullptr == _action_filter_empty_lines)
		{
			_action_filter_empty_lines = create_action(this, tr("Remove Empty Lines"), "");
			_action_filter_empty_lines->setCheckable(true);
			_action_filter_empty_lines->setVisible(true);
			connect(_action_filter_empty_lines, SIGNAL(triggered(bool)), this, SLOT(slot_handleFilterEmptyLines(const bool)));
		}
		_action_filter_empty_lines->setChecked(currdoc->getFilterEmptyLines());
		_ui->menu_filter->addAction(_action_filter_empty_lines);
		
		if (nullptr == _action_simple_filter)
		{
			_action_simple_filter = create_action(this, tr("Simple Filter"), "");
			_action_simple_filter->setCheckable(true);
			_action_simple_filter->setVisible(true);
			connect(_action_simple_filter, SIGNAL(triggered(bool)), this, SLOT(slot_handleFilterSimple(const bool)));
		}
		_action_simple_filter->setChecked(currdoc->getSimpleFilterState());
		_ui->menu_filter->addAction(_action_simple_filter);
		
		if (nullptr == _action_regex_filter)
		{
			_action_regex_filter = create_action(this, tr("Filter Via Regular Expression"), "");
			_action_regex_filter->setCheckable(true);
			_action_regex_filter->setVisible(true);
			connect(_action_regex_filter, SIGNAL(triggered(bool)), this, SLOT(slot_handleFilterRegExpr(const bool)));
		}
		_action_regex_filter->setChecked(currdoc->getRegexFilterState());
		_ui->menu_filter->addAction(_action_regex_filter);
	}
}

void Mainwindow::slot_updatePreferencesMenu(void)
{
	_ui->menu_preferences->clear();
	if (auto* const currdoc = getActiveDocumentChild())
	{
		if (nullptr == _action_config_scroll_to_bottom)
		{
			_action_config_scroll_to_bottom = create_action(this, tr("Scroll to Bottom"), "");
			_action_config_scroll_to_bottom->setCheckable(true);
			_action_config_scroll_to_bottom->setVisible(true);
			connect(_action_config_scroll_to_bottom, SIGNAL(triggered(bool)), this, SLOT(slot_handleScrollToBottom(const bool)));
		}
		_action_config_scroll_to_bottom->setChecked(currdoc->getScrollToBottom());
		_ui->menu_preferences->addAction(_action_config_scroll_to_bottom);
		
		if (nullptr == _action_config_number_of_lines_to_show)
		{
			_action_config_number_of_lines_to_show = create_action(this, tr("Number of Lines to Show: %1").arg(currdoc->getNumberOfLinesToLoad()), "");
			_action_config_number_of_lines_to_show->setVisible(true);
			connect(_action_config_number_of_lines_to_show, SIGNAL(triggered(void)), this, SLOT(slot_handleNumberOfLinesToShow(void)));
		}
		else
		{
			reset_action_text(*_action_config_number_of_lines_to_show, tr("Number of Lines to Show: %1").arg(currdoc->getNumberOfLinesToLoad()));
		}
		_ui->menu_preferences->addAction(_action_config_number_of_lines_to_show);
	}
}

void Mainwindow::slot_updateWindowsMenu(void)
{
	_ui->menu_window->clear();
	
	if (nullptr == _action_windows_tile)
	{
		_action_windows_tile = create_action(this, tr("Tile"), toString(SHORTCUT_WINDOW_TILE));
		connect(_action_windows_tile, SIGNAL(triggered(void)), this, SLOT(slot_handleWindowTileTriggered(void)));
	}
	_ui->menu_window->addAction(_action_windows_tile);
	
	if (nullptr == _action_windows_cascade)
	{
		_action_windows_cascade = create_action(this, tr("Cascade"), toString(SHORTCUT_WINDOW_CASCADE));
		connect(_action_windows_cascade, SIGNAL(triggered(void)), this, SLOT(slot_handleWindowCascadeTriggered(void)));
	}
	_ui->menu_window->addAction(_action_windows_cascade);
	
	if (nullptr == _action_windows_separator)
	{
		_action_windows_separator = new QAction(this);
		_action_windows_separator->setSeparator(true);
	}
	_ui->menu_window->addAction(_action_windows_separator);
	
	auto windows = _ui->mdi_area->subWindowList();
	_action_windows_separator->setVisible(!windows.isEmpty());
	
	for (int index=0; windows.size() > index; ++index)
	{
		if (auto* const subwindow = windows.at(index))
		{
			if (auto* const subwidget = subwindow->widget())
			{
				if (auto* const child = qobject_cast<LogfileDocument*>(subwidget))
				{
					if (auto* const doc_item_action = _ui->menu_window->addAction(tr(9 > index ? "&%1 %2" : "%1 %2").arg(index + 1).arg(child->getUserFriendlyFilename())))
					{
						doc_item_action->setCheckable(true);
						doc_item_action->setChecked(getActiveDocumentChild() == child);
						connect(doc_item_action, SIGNAL(triggered(void)), _window_mapper, SLOT(map(void)));
						_window_mapper->setMapping(doc_item_action, subwindow);
					}
				}
			}
		}
	}
}

void Mainwindow::slot_setActiveSubWindow(QWidget* window)
{
	if (nullptr == window)
	{
		return;
	}
	if (auto* const subwindow = qobject_cast<QMdiSubWindow* const>(window))
	{
		_ui->mdi_area->setActiveSubWindow(subwindow);
	}
}

void Mainwindow::slot_handleWindowTileTriggered(void)
{
	_ui->mdi_area->tileSubWindows();
}

void Mainwindow::slot_handleWindowCascadeTriggered(void)
{
	_ui->mdi_area->cascadeSubWindows();
}

void Mainwindow::slot_handleFileChange(const QString& filename)
{
	const auto& sub_window_list = _ui->mdi_area->subWindowList();
	for (const auto& currwindow : sub_window_list)
	{
		if (auto* const currdoc = qobject_cast<LogfileDocument*>(currwindow->widget()))
		{
			if (filename == currdoc->getFilename())
			{
				currdoc->reloadFileFromGuiThread();
				return;
			}
		}
	}
}

void Mainwindow::slot_handleScrollToBottom(const bool checked)
{
	if (auto* const currdoc = getActiveDocumentChild())
	{
		currdoc->setScrollToBottom(checked);
	}
}

void Mainwindow::slot_handleNumberOfLinesToShow(void)
{
	if (auto* const currdoc = getActiveDocumentChild())
	{
		bool conversion_ok = false;
		auto new_value = static_cast<unsigned int>(QInputDialog::getInt(this, tr("Number of Lines to Show"), tr("Specify how many lines are to be displayed (0 means all): "), static_cast<int>(currdoc->getNumberOfLinesToLoad()), 0, delovi::ConfigurationEntry::MAX_NUMBER_OF_LINES_TO_SHOW, 1, &conversion_ok, Qt::Dialog));
		if (!conversion_ok)
		{
			statusBar()->showMessage(tr("Invalid value given!"), 5000);
			return;
		}
		if (currdoc->getNumberOfLinesToLoad() == new_value)
		{
			statusBar()->showMessage(tr("No changes done."), 5000);
			// do not return here because this less important message might be overwritten with more important ones below
		}
		if (delovi::ConfigurationEntry::MAX_NUMBER_OF_LINES_TO_SHOW < new_value)
		{
			statusBar()->showMessage(tr("Cannot show more than %1 lines!").arg(delovi::ConfigurationEntry::MAX_NUMBER_OF_LINES_TO_SHOW), 5000);
			new_value = delovi::ConfigurationEntry::MAX_NUMBER_OF_LINES_TO_SHOW;
		}
		if (0 == new_value)
		{
			statusBar()->showMessage(tr("Showing all lines of a large file might bring serious performance issues or might not work at all (memory consumption)!"), 5000);
		}
		if (currdoc->getNumberOfLinesToLoad() == new_value)
		{
			// we've shown a message about "no changes done" above
			return;
		}
		currdoc->setNumberOfLinesToLoad(new_value);
	}
}

void Mainwindow::slot_handleFilterEmptyLines(const bool checked)
{
	if (auto* const currdoc = getActiveDocumentChild())
	{
		currdoc->setFilterEmptyLines(checked);
	}
}

void Mainwindow::slot_handleFilterSimple(const bool checked)
{
	if (auto* const currdoc = getActiveDocumentChild())
	{
		if (checked)
		{
			auto fs = currdoc->getSimpleFilter();
			SimpleFilterDialog sfd(std::move(fs.first), 0 <= fs.second, this);
			const auto rc = static_cast<QDialog::DialogCode>(sfd.exec());
			if (QDialog::Accepted == rc)
			{
//				qDebug("[DEBUG][%s][%u] filter simple: accepted!", __FILE__, __LINE__);
//				currdoc->setFilterSimple("simplefiltertest", true);
				currdoc->setSimpleFilter(sfd.takeFilterStrings(), sfd.keepLinesContainingFilterString());
			}
//			else
//			{
//				qDebug("[DEBUG][%s][%u] filter simple: rejected!", __FILE__, __LINE__);
//			}
		}
		else
		{
			currdoc->setSimpleFilter(std::vector<std::string>(), true);
		}
	}
}

void Mainwindow::slot_handleFilterRegExpr(const bool checked)
{
	if (auto* const currdoc = getActiveDocumentChild())
	{
		if (checked)
		{
			auto fr = currdoc->getRegexFilter();
			RegexFilterDialog rfd(std::move(fr.first), fr.second, this);
			const auto rc = static_cast<QDialog::DialogCode>(rfd.exec());
			if (QDialog::Accepted == rc)
			{
//				qDebug("[DEBUG][%s][%u] filter regex: accepted!", __FILE__, __LINE__);
//				currdoc->setFilterRegEx("regexfiltertest", true);
				currdoc->setRegexFilter(rfd.takeRegEx(), rfd.Syntax());
			}
//			else
//			{
//				qDebug("[DEBUG][%s][%u] filter regex: rejected!", __FILE__, __LINE__);
//			}
		}
		else
		{
			currdoc->setRegexFilter("", -99);
		}
	}
}

QMdiSubWindow* Mainwindow::findDocumentChild(const QString& filename)
{
	const auto& sub_window_list = _ui->mdi_area->subWindowList();
	for (const auto& currwindow : sub_window_list)
	{
		if (auto* const currdoc = qobject_cast<LogfileDocument*>(currwindow->widget()))
		{
			if (filename == currdoc->getFilename())
			{
				return currwindow;
			}
		}
	}
	return nullptr;
}

LogfileDocument* Mainwindow::getActiveDocumentChild(void)
{
	if (auto* const active_sub_window = _ui->mdi_area->activeSubWindow())
	{
		return qobject_cast<LogfileDocument*>(active_sub_window->widget());
	}
	return nullptr;
}
