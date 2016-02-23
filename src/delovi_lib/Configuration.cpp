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
//#	define _CRT_SECURE_NO_WARNINGS
#	define _SCL_SECURE_NO_WARNINGS
#endif

#include "Configuration.hpp"

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4100)	// unreferenced formal parameter
#endif // _MSC_VER
#include <boost/serialization/version.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#ifdef _MSC_VER
#	pragma warning(pop)
#endif // _MSC_VER
#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4244)	// type conversion, possible loss of data
#	pragma warning(disable : 4310)	// cast truncates constant value
#	pragma warning(disable : 4996)	// function or variable may be unsafe. Consider using ... instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS.
#endif // _MSC_VER
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#ifdef _MSC_VER
#	pragma warning(pop)
#endif // _MSC_VER
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <fstream>



namespace boost { namespace serialization {


template<class ARCHIVE> void serialize(ARCHIVE& ar, delovi::ConfigurationEntry& ce, const unsigned int version)
{
	if (5 <= version)
	{
		ar & make_nvp("Filename", ce._filename);
		ar & make_nvp("WindowPosX", ce._window_rect._x);
		ar & make_nvp("WindowPosY", ce._window_rect._y);
		ar & make_nvp("WindowWidth", ce._window_rect._width);
		ar & make_nvp("WindowHeight", ce._window_rect._height);
		ar & make_nvp("ScrollToBottom", ce._scroll_to_bottom);
		ce._number_of_lines_to_show = std::min(std::max(ce._number_of_lines_to_show, static_cast<unsigned int>(delovi::ConfigurationEntry::MIN_NUMBER_OF_LINES_TO_SHOW)), static_cast<unsigned int>(delovi::ConfigurationEntry::MAX_NUMBER_OF_LINES_TO_SHOW));
		ar & make_nvp("NumberOfLinesToShow", ce._number_of_lines_to_show);
		ce._number_of_lines_to_show = std::min(std::max(ce._number_of_lines_to_show, static_cast<unsigned int>(delovi::ConfigurationEntry::MIN_NUMBER_OF_LINES_TO_SHOW)), static_cast<unsigned int>(delovi::ConfigurationEntry::MAX_NUMBER_OF_LINES_TO_SHOW));
		ar & make_nvp("FilterEmptyLines", ce._filter_empty_lines);
		ar & make_nvp("FilterSimple", ce._simple_filter);
		ar & make_nvp("FilterRegEx", ce._regex_filter);
	}
	else if (4 <= version)
	{
		ar & make_nvp("Filename", ce._filename);
		ar & make_nvp("WindowPosX", ce._window_rect._x);
		ar & make_nvp("WindowPosY", ce._window_rect._y);
		ar & make_nvp("WindowWidth", ce._window_rect._width);
		ar & make_nvp("WindowHeight", ce._window_rect._height);
		ar & make_nvp("ScrollToBottom", ce._scroll_to_bottom);
		ce._number_of_lines_to_show = std::min(std::max(ce._number_of_lines_to_show, static_cast<unsigned int>(delovi::ConfigurationEntry::MIN_NUMBER_OF_LINES_TO_SHOW)), static_cast<unsigned int>(delovi::ConfigurationEntry::MAX_NUMBER_OF_LINES_TO_SHOW));
		ar & make_nvp("NumberOfLinesToShow", ce._number_of_lines_to_show);
		ce._number_of_lines_to_show = std::min(std::max(ce._number_of_lines_to_show, static_cast<unsigned int>(delovi::ConfigurationEntry::MIN_NUMBER_OF_LINES_TO_SHOW)), static_cast<unsigned int>(delovi::ConfigurationEntry::MAX_NUMBER_OF_LINES_TO_SHOW));
		ar & make_nvp("FilterEmptyLines", ce._filter_empty_lines);
		ar & make_nvp("FilterSimple", ce._simple_filter);
	}
	else if (3 <= version)
	{
		ar & make_nvp("Filename", ce._filename);
		ar & make_nvp("WindowPosX", ce._window_rect._x);
		ar & make_nvp("WindowPosY", ce._window_rect._y);
		ar & make_nvp("WindowWidth", ce._window_rect._width);
		ar & make_nvp("WindowHeight", ce._window_rect._height);
		ar & make_nvp("ScrollToBottom", ce._scroll_to_bottom);
		ce._number_of_lines_to_show = std::min(std::max(ce._number_of_lines_to_show, static_cast<unsigned int>(delovi::ConfigurationEntry::MIN_NUMBER_OF_LINES_TO_SHOW)), static_cast<unsigned int>(delovi::ConfigurationEntry::MAX_NUMBER_OF_LINES_TO_SHOW));
		ar & make_nvp("NumberOfLinesToShow", ce._number_of_lines_to_show);
		ce._number_of_lines_to_show = std::min(std::max(ce._number_of_lines_to_show, static_cast<unsigned int>(delovi::ConfigurationEntry::MIN_NUMBER_OF_LINES_TO_SHOW)), static_cast<unsigned int>(delovi::ConfigurationEntry::MAX_NUMBER_OF_LINES_TO_SHOW));
		ar & make_nvp("FilterEmptyLines", ce._filter_empty_lines);
		std::pair<std::string, bool> filter_simple;
		ar & make_nvp("FilterSimple", filter_simple);
		ce._simple_filter.first.clear();
//		ce._simple_filter.first.emplace_back(std::move(filter_simple.first));
		boost::algorithm::split(ce._simple_filter.first, filter_simple.first, boost::is_any_of("\n"));
		ce._simple_filter.first.erase(std::remove_if(ce._simple_filter.first.begin(), ce._simple_filter.first.end(), [] (std::string& str) { boost::algorithm::trim(str); return str.empty(); }), ce._simple_filter.first.end());
		ce._simple_filter.second = filter_simple.second ? 1 : -1;
	}
	else if (2 <= version)
	{
		ar & make_nvp("Filename", ce._filename);
		ar & make_nvp("WindowPosX", ce._window_rect._x);
		ar & make_nvp("WindowPosY", ce._window_rect._y);
		ar & make_nvp("WindowWidth", ce._window_rect._width);
		ar & make_nvp("WindowHeight", ce._window_rect._height);
		ar & make_nvp("ScrollToBottom", ce._scroll_to_bottom);
		ce._number_of_lines_to_show = std::min(std::max(ce._number_of_lines_to_show, static_cast<unsigned int>(delovi::ConfigurationEntry::MIN_NUMBER_OF_LINES_TO_SHOW)), static_cast<unsigned int>(delovi::ConfigurationEntry::MAX_NUMBER_OF_LINES_TO_SHOW));
		ar & make_nvp("NumberOfLinesToShow", ce._number_of_lines_to_show);
		ce._number_of_lines_to_show = std::min(std::max(ce._number_of_lines_to_show, static_cast<unsigned int>(delovi::ConfigurationEntry::MIN_NUMBER_OF_LINES_TO_SHOW)), static_cast<unsigned int>(delovi::ConfigurationEntry::MAX_NUMBER_OF_LINES_TO_SHOW));
		ar & make_nvp("FilterEmptyLines", ce._filter_empty_lines);
		std::pair<bool, std::string> filter_simple;
		ar & make_nvp("FilterSimple", filter_simple);
		ce._simple_filter.first.clear();
//		ce._simple_filter.first.emplace_back(std::move(filter_simple.second));
		boost::algorithm::split(ce._simple_filter.first, filter_simple.second, boost::is_any_of("\n"));
		ce._simple_filter.first.erase(std::remove_if(ce._simple_filter.first.begin(), ce._simple_filter.first.end(), [] (std::string& str) { boost::algorithm::trim(str); return str.empty(); }), ce._simple_filter.first.end());
		ce._simple_filter.second = filter_simple.first ? 1 : -1;
	}
	else if (1 <= version)
	{
		ar & make_nvp("Filename", ce._filename);
		ar & make_nvp("WindowPosX", ce._window_rect._x);
		ar & make_nvp("WindowPosY", ce._window_rect._y);
		ar & make_nvp("WindowWidth", ce._window_rect._width);
		ar & make_nvp("WindowHeight", ce._window_rect._height);
		ar & make_nvp("ScrollToBottom", ce._scroll_to_bottom);
	}
	else if (0 == version)
	{
		ar & make_nvp("Filename", ce._filename);
		ar & make_nvp("WindowPosX", ce._window_rect._x);
		ar & make_nvp("WindowPosY", ce._window_rect._y);
		ar & make_nvp("WindowWidth", ce._window_rect._width);
		ar & make_nvp("WindowHeight", ce._window_rect._height);
	}
}


} } // namespace serialization // namespace boost



namespace delovi {


class ConfigurationPrivateAccessor
{
	friend class boost::serialization::access;
	
	public:
		ConfigurationPrivateAccessor(Configuration& cfg)
			: _cfg(cfg)
		{ }
		
	private:
		Configuration& _cfg;
		
	private:
		ConfigurationPrivateAccessor& operator=(const ConfigurationPrivateAccessor&) = delete;
		
	// boost serialization:
		template <typename OARCHIVE> void save(OARCHIVE& ar, const unsigned int version) const
		{
			std::ignore = version;
			using boost::serialization::make_nvp;
			ar & make_nvp("OpenedDocuments", _cfg._opened_documents);
		}
		template <typename IARCHIVE> void load(IARCHIVE& ar, const unsigned int version)
		{
			using boost::serialization::make_nvp;
			if (0 <= version)
			{
				_cfg._opened_documents.clear();
				ar & make_nvp("OpenedDocuments", _cfg._opened_documents);
			}
		}
		
		BOOST_SERIALIZATION_SPLIT_MEMBER()
};



ConfigurationEntry::ConfigurationEntry(void)
	: _filename()
	, _window_rect()
	, _scroll_to_bottom(true)
	, _number_of_lines_to_show(DEFAULT_NUMBER_OF_LINES_TO_SHOW)
	, _filter_empty_lines(false)
	, _simple_filter(std::vector<std::string>(), 0)
	, _regex_filter("", 0)
{ }

std::string Configuration::toString(const serialization_file_types_e sft)
{
	std::string retval;
	switch (sft)
	{
		case SERIALIZATIONFILETYPE_XML:	retval.assign(".cfgx"); break;
		case SERIALIZATIONFILETYPE_TXT:	retval.assign(".cfgt"); break;
		case SERIALIZATIONFILETYPE_BIN:	retval.assign(".cfgb"); break;
		default:						break;
	}
	return retval;
}

Configuration::Configuration(void)
	: _opened_documents()
{ }

int Configuration::loadFromFile(const std::string& filename, serialization_file_types_e sft)
{
	if (SERIALIZATIONFILETYPE_XML != sft && SERIALIZATIONFILETYPE_TXT != sft && SERIALIZATIONFILETYPE_BIN != sft)
	{
		const std::string& ext = filename.substr(filename.length() - 5);
		sft =	toString(SERIALIZATIONFILETYPE_XML) == ext ?	SERIALIZATIONFILETYPE_XML :
				toString(SERIALIZATIONFILETYPE_TXT) == ext ?	SERIALIZATIONFILETYPE_TXT :
				toString(SERIALIZATIONFILETYPE_BIN) == ext ?	SERIALIZATIONFILETYPE_BIN :
																SERIALIZATIONFILETYPE_UNSPECIFIED;
	}
	if (SERIALIZATIONFILETYPE_UNSPECIFIED == sft)
	{
		return ERROR_UNKNOWN_FILE_TYPE;
	}
	std::ifstream ifs(filename.c_str(), std::ios::binary);
	if (!ifs.is_open())
	{
		return ERROR_FILE_NOT_OPENED;
	}
	ConfigurationPrivateAccessor cpa(*this);
	try
	{
		switch (sft)
		{
			case SERIALIZATIONFILETYPE_XML:
			{
				boost::archive::xml_iarchive ia(ifs);
				ia >> boost::serialization::make_nvp("Configuration", cpa);
				break;
			}
			case SERIALIZATIONFILETYPE_TXT:
			{
				boost::archive::text_iarchive ia(ifs);
				ia >> boost::serialization::make_nvp("Configuration", cpa);
				break;
			}
			case SERIALIZATIONFILETYPE_BIN:
			{
				boost::archive::binary_iarchive ia(ifs);
				ia >> boost::serialization::make_nvp("Configuration", cpa);
				break;
			}
			default:
			{
				return ERROR_UNKNOWN_FILE_TYPE;
			}
		}
	}
	catch (const std::exception&)
	{
		_opened_documents.clear();
		return ERROR_FILE_READING_FAILED;
	}
	catch (...)
	{
		_opened_documents.clear();
		return ERROR_FILE_READING_FAILED;
	}
	return 0;
}

int Configuration::saveToFile(const std::string& filename, serialization_file_types_e sft)
{
	if (SERIALIZATIONFILETYPE_XML != sft && SERIALIZATIONFILETYPE_TXT != sft && SERIALIZATIONFILETYPE_BIN != sft)
	{
		const std::string& ext = filename.substr(filename.length() - 5);
		sft =	toString(SERIALIZATIONFILETYPE_XML) == ext ?	SERIALIZATIONFILETYPE_XML :
				toString(SERIALIZATIONFILETYPE_TXT) == ext ?	SERIALIZATIONFILETYPE_TXT :
				toString(SERIALIZATIONFILETYPE_BIN) == ext ?	SERIALIZATIONFILETYPE_BIN :
																SERIALIZATIONFILETYPE_UNSPECIFIED;
	}
	if (SERIALIZATIONFILETYPE_UNSPECIFIED == sft)
	{
		return ERROR_UNKNOWN_FILE_TYPE;
	}
	std::ofstream ofs(filename.c_str(), std::ios::binary);
	if (!ofs.is_open())
	{
		return ERROR_FILE_NOT_OPENED;
	}
	ConfigurationPrivateAccessor cpa(*this);
	try
	{
		switch (sft)
		{
			case SERIALIZATIONFILETYPE_XML:
			{
				boost::archive::xml_oarchive oa(ofs);
				oa << boost::serialization::make_nvp("Configuration", cpa);
				break;
			}
			case SERIALIZATIONFILETYPE_TXT:
			{
				boost::archive::text_oarchive oa(ofs);
				oa << boost::serialization::make_nvp("Configuration", cpa);
				break;
			}
			case SERIALIZATIONFILETYPE_BIN:
			{
				boost::archive::binary_oarchive oa(ofs);
				oa << boost::serialization::make_nvp("Configuration", cpa);
				break;
			}
			default:
			{
				return ERROR_UNKNOWN_FILE_TYPE;
			}
		}
	}
	catch (const std::exception&)
	{
		return ERROR_FILE_WRITING_FAILED;
	}
	catch (...)
	{
		return ERROR_FILE_WRITING_FAILED;
	}
	return 0;
}

void Configuration::addOpenedDocument(ConfigurationEntry ce)
{
	if (!ce._filename.empty() && 0 != ce._window_rect._width && 0 != ce._window_rect._height)
	{
		_opened_documents.emplace_back(std::move(ce));
	}
}


} // namespace delovi


BOOST_CLASS_VERSION(delovi::ConfigurationEntry, 5)
