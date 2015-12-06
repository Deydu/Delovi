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



#include "SimpleFilterDialog.hpp.h"

#include <vector>
#include <string>



// forward declaration(s)
namespace Ui { class SimpleFilterDialog; }



class SimpleFilterDialog : public QDialog
{
	Q_OBJECT
	
	public:
		SimpleFilterDialog
		(
			std::vector<std::string> filter_strings,
			bool keep_lines_containing_filter_string,
			QWidget* const parent=nullptr,
			const Qt::WindowFlags flags=0
		);
		SimpleFilterDialog(const SimpleFilterDialog& other) = delete;
		SimpleFilterDialog& operator=(const SimpleFilterDialog& other) = delete;
		bool keepLinesContainingFilterString(void) const
		{
			return _keep_lines_containing_filter_string;
		}
		std::vector<std::string> takeFilterStrings(void);
		
	public slots:
		void on_rb_filter_strings_contain_clicked(void)
		{
			_keep_lines_containing_filter_string = true;
		}
		void on_rb_filter_strings_not_contain_clicked(void)
		{
			_keep_lines_containing_filter_string = false;
		}
		
	private:
		Ui::SimpleFilterDialog* _ui;
		std::string _filter_strings;
		bool _keep_lines_containing_filter_string;
};
