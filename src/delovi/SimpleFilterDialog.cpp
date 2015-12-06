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
#	define _SCL_SECURE_NO_WARNINGS
#endif // _MSC_VER

#include "SimpleFilterDialog.hpp"
#include "SimpleFilterDialog.cpp.h"

#include <make_unique.hpp>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>



SimpleFilterDialog::SimpleFilterDialog
(
	std::vector<std::string> filter_strings,
	bool keep_lines_containing_filter_string,
	QWidget* const parent,
	const Qt::WindowFlags flags
)
	: QDialog(parent, flags)
	, _ui(nullptr)
	, _filter_strings()
	, _keep_lines_containing_filter_string(keep_lines_containing_filter_string)
{
	auto tmp_ui = std::make_unique<Ui::SimpleFilterDialog>();
	tmp_ui->setupUi(this);
	_ui = tmp_ui.release();
	
	std::size_t filter_strings_size = 0;
	for (const auto& str : filter_strings)
	{
		filter_strings_size += str.size();
		filter_strings_size += 1;
	}
	for (const auto& str : filter_strings)
	{
		_filter_strings.append(str);
		_filter_strings.append("\n");
	}
	_ui->ptxted_filter_strings->appendPlainText(_filter_strings.c_str());
	if (_keep_lines_containing_filter_string)
	{
		_ui->rb_filter_strings_contain->setChecked(true);
	}
	else
	{
		_ui->rb_filter_strings_not_contain->setChecked(true);
	}
}

std::vector<std::string> SimpleFilterDialog::takeFilterStrings(void)
{
	std::vector<std::string> retval;
	if (auto* const td = _ui->ptxted_filter_strings->document())
	{
		_filter_strings = std::move(td->toPlainText().toStdString());
		boost::algorithm::split(retval, _filter_strings, boost::is_any_of("\n"));
		retval.erase(std::remove_if(retval.begin(), retval.end(), [] (const std::string& str) {return str.empty(); }), retval.end());
	}
	return std::move(retval);
}
