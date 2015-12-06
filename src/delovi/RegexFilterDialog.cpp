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



#include "RegexFilterDialog.hpp"

#include "RegexFilterDialog.cpp.h"

#include <make_unique.hpp>



RegexFilterDialog::RegexFilterDialog
(
	std::string regex,
	const int syntax,
	QWidget* const parent,
	const Qt::WindowFlags flags
)
	: QDialog(parent, flags)
	, _ui(nullptr)
	, _regex(std::move(regex))
	, _syntax(syntax)
{
	auto tmp_ui = std::make_unique<Ui::RegexFilterDialog>();
	tmp_ui->setupUi(this);
	_ui = tmp_ui.release();
	
	_ui->le_regular_expression->setText(regex.c_str());
	switch (syntax)
	{
		case QRegExp::RegExp:			_ui->rb_syntax_regexp1->setChecked(true); break;
		case QRegExp::RegExp2:			_ui->rb_syntax_regexp2->setChecked(true); break;
		case QRegExp::Wildcard:			_ui->rb_syntax_wildcard->setChecked(true); break;
		case QRegExp::WildcardUnix:		_ui->rb_syntax_wildcardunix->setChecked(true); break;
		case QRegExp::FixedString:		_ui->rb_syntax_fixedstring->setChecked(true); break;
		case QRegExp::W3CXmlSchema11:	_ui->rb_syntax_w3cxmlschema11->setChecked(true); break;
		default:						_ui->rb_syntax_boost->setChecked(true); break;
	}
}
