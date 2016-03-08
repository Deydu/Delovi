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



#if defined __GNUC__
//#	pragma GCC diagnostic push
//#	pragma GCC diagnostic ignored "-Wall"
#elif defined __SUNPRO_CC
#	pragma disable_warn
#elif defined _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4127)	// conditional expression is constant
#	pragma warning(disable : 4251)	// class needs to have dll-interface to be used
//#	pragma warning(disable : 4514)	// unreferenced inline function has been removed
#endif
#include <QDialog>
#if defined __GNUC__
//#	pragma GCC diagnostic pop
#elif defined __SUNPRO_CC
#	pragma enable_warn
#elif defined _MSC_VER
#	pragma warning(pop)
#endif

#include <vector>
#include <string>



// forward declaration(s)
namespace Ui { class RegexFilterDialog; }



class RegexFilterDialog : public QDialog
{
	Q_OBJECT
	
	public:
		RegexFilterDialog
		(
			std::string regex,
			const int syntax,
			QWidget* const parent=nullptr,
			const Qt::WindowFlags flags=0
		);
		RegexFilterDialog(const RegexFilterDialog& other) = delete;
		RegexFilterDialog& operator=(const RegexFilterDialog& other) = delete;
		std::string takeRegEx(void)
		{
			return std::move(_regex);
		}
		int Syntax(void) const
		{
			return _syntax;
		}
		
	public slots:
		void on_rb_syntax_regexp1_clicked(void)
		{
			_syntax = QRegExp::RegExp;
		}
		void on_rb_syntax_regexp2_clicked(void)
		{
			_syntax = QRegExp::RegExp2;
		}
		void on_rb_syntax_wildcard_clicked(void)
		{
			_syntax = QRegExp::Wildcard;
		}
		void on_rb_syntax_wildcardunix_clicked(void)
		{
			_syntax = QRegExp::WildcardUnix;
		}
		void on_rb_syntax_fixedstring_clicked(void)
		{
			_syntax = QRegExp::FixedString;
		}
		void on_rb_syntax_w3cxmlschema11_clicked(void)
		{
			_syntax = QRegExp::W3CXmlSchema11;
		}
		void on_rb_syntax_boost_clicked(void)
		{
			_syntax = -1;
		}
		
	private:
		Ui::RegexFilterDialog* _ui;
		std::string _regex;
		int _syntax;
};
