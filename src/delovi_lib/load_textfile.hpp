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



#include "delovi_definitions.h"

#include <fstream>
#include <functional>
#include <list>



namespace delovi {


/// Load all lines from input stream and keep those matching a condition.
/// @param[in,out] ifs Stream to load text data from.
/// @param[in] condition_to_load Functor for checking whether data from input stream is actually to be read.
/// @param[in] condition_for_line Functor for checking whether a line from input stream is to be kept for output.
/// @param[out] text_lines_from_file List of lines read from input stream.
/// @param[out] file_size Size of read file-stream.
/// @param[out] contains_a_utf8_sequence Indicates whether a UTF-8 char was found in text data.
/// @return ==0 if no data was read, >0 indicating how many bytes were read (should be equal to returned file_size), <0 on error (-1 if stream was not opened, -2 if stream had no good-bit set, -3 if not enough memory could be allocated, -4 if another exception was thrown during memory allocation).
int load_all_lines_from_stream
(
	std::ifstream& ifs,
	std::function<bool(const std::size_t)> condition_to_load,
	std::function<bool(const std::string&)> condition_for_line,
	std::list<std::string>& text_lines_from_file,
	std::size_t& file_size,
	bool& contains_a_utf8_sequence
);

/// Load the last n lines from input stream and keep those matching a condition.
/// \param[in] number_of_lines Number of lines to read.
/// \param[in,out] ifs Stream to load text data from.
/// \param[in] condition_to_load Functor for checking whether data from input stream is actually to be read.
/// \param[in] condition_for_line Functor for checking whether a line from input stream is to be kept for output.
/// \param[out] text_lines_from_file List of lines read from input stream.
/// \param[out] file_size Size of read file-stream.
/// \param[out] contains_a_utf8_sequence Indicates whether a UTF-8 char was found in text data.
/// \param[in] read_block_size Block size to be used when reading text file blockwise. Less than 256 will not be accepted.
/// \return ==0 if no data was read, >0 indicating how many bytes were read (should be equal to returned file_size), <0 on error (-1 if stream was not opened, -2 if stream had no good-bit set, -3 if not enough memory could be allocated, -4 if another exception was thrown during memory allocation).
int load_n_lines_from_stream
(
	const std::size_t number_of_lines,
	std::ifstream& ifs,
	std::function<bool(const std::size_t)> condition_to_load,
	std::function<bool(const std::string&)> condition_for_line,
	std::list<std::string>& text_lines_from_file,
	std::size_t& file_size,
	bool& contains_a_utf8_sequence,
	const std::size_t read_block_size=1024
);

/// Load all lines from text file and keep those matching a condition.
/// @param[in] filepath Path to file to read text data from.
/// @param[in] condition_to_load Functor for checking whether data from input stream is actually to be read.
/// @param[in] condition_for_line Functor for checking whether a line from input stream is to be kept for output.
/// @param[out] text_lines_from_file List of lines read from input stream.
/// @param[out] file_size Size of read file-stream.
/// @param[out] contains_a_utf8_sequence Indicates whether a UTF-8 char was found in text data.
/// @return ==0 if no data was read, >0 indicating how many bytes were read (should be equal to returned file_size), <0 on error (-1 if stream was not opened, -2 if stream had no good-bit set, -3 if not enough memory could be allocated, -4 if another exception was thrown during memory allocation).
int load_all_lines_from_file
(
	std::string filepath,
	std::function<bool(const std::size_t)> condition_to_load,
	std::function<bool(const std::string&)> condition_for_line,
	std::list<std::string>& text_lines_from_file,
	std::size_t& file_size,
	bool& contains_a_utf8_sequence
);

/// Load the last n lines from text file and keep those matching a condition.
/// @param[in] number_of_lines Number of lines to read.
/// @param[in] filepath Path to file to read text data from.
/// @param[in] condition_to_load Functor for checking whether data from input stream is actually to be read.
/// @param[in] condition_for_line Functor for checking whether a line from input stream is to be kept for output.
/// @param[out] text_lines_from_file List of lines read from input stream.
/// @param[out] file_size Size of read file-stream.
/// @param[out] contains_a_utf8_sequence Indicates whether a UTF-8 char was found in text data.
/// @param[in] read_block_size Block size to be used when reading text file blockwise. Less than 256 will not be accepted.
/// @return ==0 if no data was read, >0 indicating how many bytes were read (should be equal to returned file_size), <0 on error (-1 if stream was not opened, -2 if stream had no good-bit set, -3 if not enough memory could be allocated, -4 if another exception was thrown during memory allocation).
int load_n_lines_from_file
(
	const std::size_t number_of_lines,
	std::string filepath,
	std::function<bool(const std::size_t)> condition_to_load,
	std::function<bool(const std::string&)> condition_for_line,
	std::list<std::string>& text_lines_from_file,
	std::size_t& file_size,
	bool& contains_a_utf8_sequence,
	const std::size_t read_block_size=1024
);


} // namespace delovi
