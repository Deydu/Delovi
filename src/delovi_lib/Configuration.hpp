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



// forward declaration(s)
namespace delovi { class ConfigurationPrivateAccessor; }



#include <list>
#include <vector>
#include <string>
#include <algorithm>



namespace delovi {


struct ConfigurationEntry
{
	using SimpleFilterPair = std::pair<std::vector<std::string>, int>;
	using RegexFilterPair  = std::pair<std::string, int>;
	
	enum defaults_e
	{
		MIN_NUMBER_OF_LINES_TO_SHOW		= 1,
		DEFAULT_NUMBER_OF_LINES_TO_SHOW	= 100,
		MAX_NUMBER_OF_LINES_TO_SHOW		= 100000
	};
	
	struct Rect
	{
		int _x;
		int _y;
		int _width;
		int _height;
		
		Rect(void)
			: _x(0)
			, _y(0)
			, _width(0)
			, _height(0)
		{ }
	};
	
	std::string      _filename;
	Rect             _window_rect;
	bool             _scroll_to_bottom;
	unsigned int     _number_of_lines_to_show;
	bool             _filter_empty_lines;
	SimpleFilterPair _simple_filter;
	RegexFilterPair  _regex_filter;
	
	ConfigurationEntry(void);
};



class Configuration
{
	friend class ConfigurationPrivateAccessor;
	
	public:
		enum serialization_file_types_e
		{
			SERIALIZATIONFILETYPE_UNSPECIFIED	= 0,
			SERIALIZATIONFILETYPE_XML			= 1,
			SERIALIZATIONFILETYPE_TXT			= 2,
			SERIALIZATIONFILETYPE_BIN			= 3
		};
		
		enum error_codes_e
		{
			ERROR_NONE					= 0,
			ERROR_UNKNOWN_FILE_TYPE		= -1,
			ERROR_FILE_NOT_OPENED		= -2,
			ERROR_FILE_READING_FAILED	= -3,
			ERROR_FILE_WRITING_FAILED	= -4
		};
		
		using ConfigEntriesContainer = std::list<ConfigurationEntry>;
		
	public:
		static std::string toString(const serialization_file_types_e sft);
		
	public:
		Configuration(void);
		int loadFromFile(const std::string& filename, serialization_file_types_e sft=SERIALIZATIONFILETYPE_UNSPECIFIED);
		int saveToFile(const std::string& filename, serialization_file_types_e sft=SERIALIZATIONFILETYPE_UNSPECIFIED);
		void addOpenedDocument(ConfigurationEntry ce);
		
		template <typename FUNC> void doForEach(FUNC func) const
		{
			std::for_each(_opened_documents.begin(), _opened_documents.end(), func);
		}
		
	private:
		ConfigEntriesContainer _opened_documents;
};


} // namespace delovi
