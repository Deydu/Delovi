/*****************************************************************************
 *
 * Copyright (C) 2016 Denny Duphorn <deydu@gmx-topmail.de>
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



// forward declaration(s)
namespace Ui { class Mainwindow; }
namespace delovi { class Configuration; }
class QMdiSubWindow;
class LogfileDocument;
class QAction;
class QSignalMapper;
class QDragEnterEvent;
class QFileSystemWatcher;



#if defined __GNUC__
//#	pragma GCC diagnostic push
//#	pragma GCC diagnostic ignored "-Wall"
#elif defined __SUNPRO_CC
#	pragma disable_warn
#elif defined _MSC_VER
//#	pragma warning(push, 0)
#	pragma warning(push)
#	pragma warning(disable : 4127)	// conditional expression is constant
//#	pragma warning(disable : 4251)	// class needs to have dll-interface to be used
//#	pragma warning(disable : 4365)	// 'argument' : conversion from 'uint' to 'int', signed/unsigned mismatch
//#	pragma warning(disable : 4512)	// assignment operator could not be generated
//#	pragma warning(disable : 4571)	// Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
//#	pragma warning(disable : 4619)	// there is no warning number '4660'
//#	pragma warning(disable : 4625)	// copy constructor could not be generated because a base class copy constructor is inaccessible
//#	pragma warning(disable : 4626)	// assignment operator could not be generated because a base class assignment operator is inaccessible
//#	pragma warning(disable : 4820)	// n bytes padding added after data member
//#	pragma warning(disable : 4986)	// 'operator new[]': exception specification does not match previous declaration
#endif
#include <QMainWindow>
#if defined __GNUC__
//#	pragma GCC diagnostic pop
#elif defined __SUNPRO_CC
#	pragma enable_warn
#elif defined _MSC_VER
#	pragma warning(pop)
#endif


class Mainwindow : public QMainWindow
{
	Q_OBJECT
	
	public: // types
		enum eShortcuts
		{
			SHORTCUT_CTRL_O					= 101,
			SHORTCUT_CTRL_Q					= 102,
			
			SHORTCUT_ALT_T					= 201,
			SHORTCUT_ALT_C					= 202,
			SHORTCUT_ALT_A					= 203,
			SHORTCUT_ALT_Q					= 204,
			
			SHORTCUT_FILE_OPEN				= SHORTCUT_CTRL_O,
			SHORTCUT_FILE_QUIT				= SHORTCUT_CTRL_Q,
			SHORTCUT_WINDOW_TILE			= SHORTCUT_ALT_T,
			SHORTCUT_WINDOW_CASCADE			= SHORTCUT_ALT_C,
			SHORTCUT_ABOUT					= SHORTCUT_ALT_A,
			SHORTCUT_ABOUT_QT				= SHORTCUT_ALT_Q
		};
		
	public: // static methods
		static QString toString(const eShortcuts sc);
		
	public: // methods
		Mainwindow(QWidget* const parent=nullptr, const Qt::WindowFlags flags=0);
		Mainwindow(const Mainwindow&) = delete;
		virtual ~Mainwindow(void);
		Mainwindow& operator=(const Mainwindow&) = delete;
		void openFile
		(
			const QString& filename,
			QMdiSubWindow** const mdi_subwindow_pptr=nullptr,
			LogfileDocument** const document_pptr=nullptr,
			const bool* const filter_empty_lines=nullptr,
			const std::pair<std::vector<std::string>, int>* const filter_simple=nullptr,
			int number_of_lines_to_load=-1
		);
		
	public slots:
		void on_action_file_open_triggered(void);
		void on_action_quit_triggered(void);
		void on_action_about_triggered(void);
		void on_action_about_qt_triggered(void);
		
	protected: // methods
		virtual void dragEnterEvent(QDragEnterEvent* dragevent);
		virtual void dropEvent(QDropEvent* dropevent);
		
	private slots:
		void slot_updateFilterMenu(void);
		void slot_updatePreferencesMenu(void);
		void slot_updateWindowsMenu(void);
		void slot_setActiveSubWindow(QWidget* window);
		void slot_handleWindowTileTriggered(void);
		void slot_handleWindowCascadeTriggered(void);
		void slot_handleFileChange(const QString& filename);
		void slot_handleScrollToBottom(const bool checked);
		void slot_handleNumberOfLinesToShow(void);
		void slot_handleFilterEmptyLines(const bool checked);
		void slot_handleFilterSimple(const bool checked);
		void slot_handleFilterRegExpr(const bool checked);
		
	private: // members
		Ui::Mainwindow* _ui;
		QSignalMapper* _window_mapper;
		QAction* _action_filter_empty_lines;
		QAction* _action_simple_filter;
		QAction* _action_regex_filter;
		QAction* _action_config_scroll_to_bottom;
		QAction* _action_config_number_of_lines_to_show;
		QAction* _action_windows_tile;
		QAction* _action_windows_cascade;
		QAction* _action_windows_separator;
		QFileSystemWatcher* _file_system_watcher;
		std::string _config_filename;

	private: // methods
		QMdiSubWindow* findDocumentChild(const QString& filename);
		LogfileDocument* getActiveDocumentChild(void);
};
