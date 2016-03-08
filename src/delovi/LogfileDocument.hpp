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

#if defined __GNUC__
//#	pragma GCC diagnostic push
//#	pragma GCC diagnostic ignored "-Wall"
#elif defined __SUNPRO_CC
#	pragma disable_warn
#elif defined _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4127)	// conditional expression is constant
//#	pragma warning(disable : 4244)	// 'return' : type conversion, possible loss of data
#	pragma warning(disable : 4251)	// class needs to have dll-interface to be used
//#	pragma warning(disable : 4311)	// 'reinterpret_cast' : pointer truncation
//#	pragma warning(disable : 4312)	// 'reinterpret_cast' : conversion to <type> of greater size
//#	pragma warning(disable : 4365)	// 'argument' : conversion from 'uint' to 'int', signed/unsigned mismatch
//#	pragma warning(disable : 4512)	// assignment operator could not be generated
//#	pragma warning(disable : 4514)	//  unreferenced inline function has been removed
//#	pragma warning(disable : 4619)	// there is no warning number '4660'
//#	pragma warning(disable : 4625)	// copy constructor could not be generated because a base class copy constructor is inaccessible
//#	pragma warning(disable : 4626)	// assignment operator could not be generated because a base class assignment operator is inaccessible
#	pragma warning(disable : 4800)	// forcing value to bool 'true' or 'false' (performance warning)
//#	pragma warning(disable : 4820)	// n bytes padding added after data member
//#	pragma warning(disable : 4986)	// 'operator new[]': exception specification does not match previous declaration
#endif
#if 1 == DELOVI_DOCUMENT_USES_TEXTEDIT
#	include <QTextEdit>
#elif 2 == DELOVI_DOCUMENT_USES_TEXTEDIT
#	include <QPlainTextEdit>
#endif
#if defined __GNUC__
//#	pragma GCC diagnostic pop
#elif defined __SUNPRO_CC
#	pragma enable_warn
#elif defined _MSC_VER
#	pragma warning(pop)
#endif

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

