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



#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MiscTests



#include "ut1.cpp.h"

#include <delovi_definitions.h>
#include <load_textfile.hpp>
#include <make_unique.hpp>

#include <iostream>



BOOST_AUTO_TEST_CASE(compiler_version)
{
#if 1800 == _MSC_VER
	BOOST_REQUIRE_EQUAL(199711L, __cplusplus);
#elif 1900 == _MSC_VER
	BOOST_REQUIRE_EQUAL(199711L, __cplusplus);
#else
	// gcc
	BOOST_REQUIRE_EQUAL(201103L, __cplusplus);
#endif // MSC_VER
	std::cout
		<< "Delovi version : " << DELOVI_VERSION_MAJOR << "." << DELOVI_VERSION_MINOR << "." << DELOVI_VERSION_BUILD << "\n"
		<< "Build date/time: " << __DATE__ << " " << __TIME__ << "\n"
		<< "Compiler       : "
#ifdef __GNUC__
			<< __VERSION__
#elif defined _MSC_VER
			<< "MSVC: " << _MSC_FULL_VER
#endif // __GNUC__ / _MSC_VER
		<< std::endl;
}

int add(const int i, const int j)
{
	return i+j;
}

BOOST_AUTO_TEST_CASE(boost_example_tests)
{
	// seven ways to detect and report the same error:
	BOOST_CHECK(4 == add(2, 2));		// #1 continues on error
	BOOST_REQUIRE(4 == add(2, 2));		// #2 throws on error
	if (4 != add(2, 2))
	{
		BOOST_ERROR("Ouch...");			// #3 continues on error
	}
	if (4 != add(2, 2))
	{
		BOOST_FAIL("Ouch...");			// #4 throws on error
	}
	if (4 != add(2, 2))
	{
		throw "Ouch...";				// #5 throws on error
	}
	BOOST_CHECK_MESSAGE					// #6 continues on error
	(
		4 == add(2, 2),
		"add(..) result: " << add(2, 2)
	);
	BOOST_CHECK_EQUAL(4, add(2, 2));	// #7 continues on error
}

BOOST_AUTO_TEST_CASE(unique_and_shared_ptr)
{
	std::unique_ptr<std::string> str_uptr = std::make_unique<std::string>("blubb");
//	std::unique_ptr<std::string> str_uptr(new std::string("blubb"));
//	BOOST_REQUIRE_NE(nullptr, str_uptr.get());  <--  compile error
	BOOST_REQUIRE(nullptr != str_uptr.get());
	BOOST_REQUIRE_EQUAL("blubb", *str_uptr);
	std::shared_ptr<std::string> str_shptr = std::make_shared<std::string>("blah");
	BOOST_REQUIRE(nullptr != str_shptr.get());
	BOOST_REQUIRE_EQUAL(0, str_shptr->compare("blah"));
}

BOOST_AUTO_TEST_CASE(load_textfile_binarymode_forward)
{
	std::ifstream ifs("tests/some.log", std::ios::binary);
	BOOST_REQUIRE(ifs.is_open());
	BOOST_REQUIRE(ifs.good());
	std::list<std::string> lines_from_file;
	std::size_t file_size = 0;
	bool contains_a_utf8_sequence = false;
	auto condition_to_load  = [] (const std::size_t file_size) -> bool { return 0 < file_size; };
	auto condition_for_line = [] (const std::string& line) -> bool { return !line.empty(); };
	const int rc = delovi::load_all_lines_from_stream
	(
		ifs,
		condition_to_load,
		condition_for_line,
		lines_from_file,
		file_size,
		contains_a_utf8_sequence
	);
	BOOST_REQUIRE_EQUAL(static_cast<std::size_t>(rc), file_size);
	BOOST_REQUIRE_EQUAL(30u, lines_from_file.size());
	BOOST_REQUIRE_EQUAL(2626u, file_size);
	BOOST_REQUIRE(ifs.is_open());
	ifs.close();
}

BOOST_AUTO_TEST_CASE(load_textfile_binarymode_backward)
{
	std::list<std::string> lines_from_file;
	std::size_t file_size = 0;
	bool contains_a_utf8_sequence = false;
	auto condition_to_load  = [] (const std::size_t file_size) -> bool { return 0 < file_size; };
	auto condition_for_line = [] (const std::string& line) -> bool { return !line.empty(); };
	const int rc = delovi::load_n_lines_from_file
	(
		10,
		"tests/some.log",
		condition_to_load,
		condition_for_line,
		lines_from_file,
		file_size,
		contains_a_utf8_sequence,
		256
	);
	BOOST_REQUIRE_EQUAL(static_cast<std::size_t>(rc), file_size);
	BOOST_REQUIRE_EQUAL(10u, lines_from_file.size());
	BOOST_REQUIRE_EQUAL(2626u, file_size);
//	std::cout << "\n\n" << std::endl;
//	std::cout << "[DEBUG][load_textfile_binarymode_backward] lines_from_file:" << std::endl;
//	for (const std::string& str : lines_from_file)
//	{
//		std::cout << "\"" << str << "\"" << std::endl;
//	}
	std::list<std::string>::const_iterator cit = lines_from_file.cbegin();
	BOOST_REQUIRE_EQUAL('A', (*cit)[0]);	// 1
	++cit;
	BOOST_REQUIRE_EQUAL('B', (*cit)[0]);	// 2
	++cit;
	BOOST_REQUIRE_EQUAL('D', (*cit)[0]);	// 3
	++cit;
	BOOST_REQUIRE_EQUAL('E', (*cit)[0]);	// 4
	++cit;
	BOOST_REQUIRE_EQUAL('G', (*cit)[0]);	// 5
	++cit;
	BOOST_REQUIRE_EQUAL('H', (*cit)[0]);	// 6
	++cit;
	BOOST_REQUIRE_EQUAL('I', (*cit)[0]);	// 7
	++cit;
	BOOST_REQUIRE_EQUAL('J', (*cit)[0]);	// 8
	++cit;
	BOOST_REQUIRE_EQUAL('L', (*cit)[0]);	// 9
	++cit;
	BOOST_REQUIRE_EQUAL('N', (*cit)[0]);	// 10
	++cit;
	BOOST_REQUIRE(lines_from_file.cend() == cit);
}
