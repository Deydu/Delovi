#include "load_textfile.hpp"
#include "utf8_detection.hpp"

#include <boost/algorithm/string/trim.hpp>

#include <iostream>
#include <vector>



namespace delovi {


inline bool is_line_break_char(const char ch)
{
	return 0x0A == ch || 0x0D == ch;
}

int load_all_lines_from_stream
(
	std::ifstream& ifs,
	std::function<bool(const std::size_t)> condition_to_load,
	std::function<bool(const std::string&)> condition_for_line,
	std::list<std::string>& text_lines_from_file,
	std::size_t& file_size,
	bool& contains_a_utf8_sequence
)
{
	text_lines_from_file.clear();
	if (!ifs.is_open())
	{
		return -1;
	}
	if (!ifs.good())
	{
		return -2;
	}
	ifs.seekg(0, std::ios::end);
	file_size = static_cast<std::size_t>(ifs.tellg());
	if (!condition_to_load(file_size))
	{
		return 0;
	}
//	ifs.seekg(0, std::ios::beg);
	ifs.seekg(0);
	std::vector<char> entire_file_contents;
	try
	{
		entire_file_contents.resize(file_size, '\0');
	}
	catch (const std::bad_alloc&)
	{
		return -3;
	}
//	catch (const std::exception&)
	catch (...)
	{
		return -4;
	}
	ifs.read(entire_file_contents.data(), file_size);
//	ifs.read(entire_file_contents.data(), static_cast<std::streamsize>(file_size));
//	contains_a_utf8_sequence = does_text_contain_a_utf8_sequence(entire_file_contents.cbegin(), entire_file_contents.cend());
	std::string line;
	for (std::vector<char>::const_iterator citer = entire_file_contents.cbegin(), cend = entire_file_contents.cend(); cend != citer; ++citer)
	{
		std::vector<char>::const_iterator found = std::find_if(citer, cend, is_line_break_char);
		if (found == cend)
		{
			break;
		}
		// check if windows format with two linebreak-chars: 0x0d,0x0a
		{
			std::vector<char>::const_iterator foundnext = found;
			++foundnext;
			if (cend != foundnext && 0x0D == *found && 0x0A == *foundnext)
			{
				found = foundnext;
			}
		}
		line.assign(citer, found);
		boost::algorithm::trim(line);
		if (condition_for_line(line))
		{
			line.append(1, '\n');
			text_lines_from_file.emplace_back(std::move(line));
		}
		citer = found;
	}
	for (std::list<std::string>::const_iterator citer = text_lines_from_file.cbegin(), cend = text_lines_from_file.cend(); !contains_a_utf8_sequence && cend != citer; ++citer)
	{
		const std::string& str = *citer;
		contains_a_utf8_sequence = does_text_contain_a_utf8_sequence(str.cbegin(), str.cend());
	}
	int retval = static_cast<int>(file_size);
	if (0 > retval)
	{
		// dunno if this is actually possible
		retval = std::numeric_limits<int>::max();
	}
	return retval;
}

int load_n_lines_from_stream
(
	const std::size_t number_of_lines,
	std::ifstream& ifs,
	std::function<bool(const std::size_t)> condition_to_load,
	std::function<bool(const std::string&)> condition_for_line,
	std::list<std::string>& text_lines_from_file,
	std::size_t& file_size,
	bool& contains_a_utf8_sequence,
	const std::size_t read_block_size
)
{
	text_lines_from_file.clear();
	if (!ifs.is_open())
	{
		return -1;
	}
	if (!ifs.good())
	{
		return -2;
	}
	ifs.seekg(0, std::ios::end);
	file_size = static_cast<std::size_t>(ifs.tellg());
	if (!condition_to_load(file_size))
	{
		return 0;
	}
	std::size_t block_size = std::max<std::size_t>(256, read_block_size);
	if (file_size < block_size)
	{
		block_size = file_size;
	}
	std::size_t file_pos = file_size - block_size;
	std::vector<char> file_data;
	std::vector<char> tmp_data;
	file_data.reserve(block_size * 2 + 2);
	tmp_data.reserve(block_size * 2 + 2);
	std::string line;
	while (number_of_lines > text_lines_from_file.size())
	{
		ifs.seekg(file_pos);
		line.clear();
		file_data.clear();
		file_data.resize(block_size, '\0');
		ifs.read(file_data.data(), block_size);
		std::copy(tmp_data.begin(), tmp_data.end(), std::back_inserter(file_data));
		file_data.swap(tmp_data);
		std::list<std::string> new_text_lines;
		std::vector<char>::iterator iter1 = std::find_if(tmp_data.begin(), tmp_data.end(), is_line_break_char);
		while (tmp_data.end() != iter1)
		{
			std::vector<char>::iterator iter1_next = iter1;
			++iter1_next;
			if (tmp_data.end() == iter1_next)
			{
				tmp_data.erase(iter1);
				break;
			}
			else if (0x0D == *iter1 && 0x0A == *iter1_next)
			{
				iter1 = tmp_data.erase(iter1);
				iter1 = tmp_data.erase(iter1);
			}
			std::vector<char>::iterator iter2 = std::find_if(iter1, tmp_data.end(), is_line_break_char);
			if (tmp_data.end() != iter2)
			{
				std::vector<char>::iterator iter2_next = iter2;
				++iter2_next;
				if (tmp_data.end() != iter2_next && 0x0D == *iter2 && 0x0A == *iter2_next)
				{
					iter2 = iter2_next;
				}
			}
			line.assign(iter1, iter2);
			boost::algorithm::trim(line);
			if (condition_for_line(line))
			{
//				std::cout << "[DEBUG][load_n_lines_from_stream][" << __LINE__ << "] line: \"" << line << "\", length: " << line.length();
//				if (1 == line.length())
//				{
//					std::cout << ", line[0]: " << static_cast<int>(line[0]);
//				}
//				std::cout << std::endl;
				line.append(1, '\n');
				new_text_lines.emplace_back(std::move(line));
			}
			if (tmp_data.end() != iter2)
			{
				++iter2;
			}
			iter1 = tmp_data.erase(iter1, iter2);
		}
		std::move(new_text_lines.rbegin(), new_text_lines.rend(), std::front_inserter(text_lines_from_file));
		if (number_of_lines <= text_lines_from_file.size())
		{
			break;
		}
		if (0 == file_pos)
		{
			if (!tmp_data.empty())
			{
				line.assign(tmp_data.begin(), tmp_data.end());
				boost::algorithm::trim(line);
				if (condition_for_line(line))
				{
//					std::cout << "[DEBUG][load_n_lines_from_stream][" << __LINE__ << "] line: \"" << line << "\", length: " << line.length();
//					if (1 == line.length())
//					{
//						std::cout << ", line[0]: " << static_cast<int>(line[0]);
//					}
//					std::cout << std::endl;
					line.append(1, '\n');
					text_lines_from_file.emplace_front(std::move(line));
				}
			}
			break;
		}
		if (file_pos > block_size)
		{
			file_pos -= block_size;
		}
		else
		{
			block_size = file_pos;
			file_pos = 0;
		}
	}
	if (number_of_lines < text_lines_from_file.size())
	{
		std::list<std::string>::iterator iter = text_lines_from_file.begin();
		std::advance(iter, text_lines_from_file.size() - number_of_lines);
		text_lines_from_file.erase(text_lines_from_file.begin(), iter);
	}
	for (std::list<std::string>::const_iterator citer = text_lines_from_file.cbegin(), cend = text_lines_from_file.cend(); !contains_a_utf8_sequence && cend != citer; ++citer)
	{
		const std::string& str = *citer;
		contains_a_utf8_sequence = does_text_contain_a_utf8_sequence(str.cbegin(), str.cend());
	}
	int retval = static_cast<int>(file_size);
	if (0 > retval)
	{
		// dunno if this is actually possible
		retval = std::numeric_limits<int>::max();
	}
	return retval;
}

int load_all_lines_from_file
(
	std::string filepath,
	std::function<bool(const std::size_t)> condition_to_load,
	std::function<bool(const std::string&)> condition_for_line,
	std::list<std::string>& text_lines_from_file,
	std::size_t& file_size,
	bool& contains_a_utf8_sequence
)
{
	std::ifstream ifs(filepath, std::ios::binary);
	const int retval = load_all_lines_from_stream
	(
		ifs,
		condition_to_load,
		condition_for_line,
		text_lines_from_file,
		file_size,
		contains_a_utf8_sequence
	);
	ifs.close();
	return retval;
}

int load_n_lines_from_file
(
	const std::size_t number_of_lines,
	std::string filepath,
	std::function<bool(const std::size_t)> condition_to_load,
	std::function<bool(const std::string&)> condition_for_line,
	std::list<std::string>& text_lines_from_file,
	std::size_t& file_size,
	bool& contains_a_utf8_sequence,
	const std::size_t // read_block_size
)
{
	std::ifstream ifs(filepath, std::ios::binary);
	const int retval = load_n_lines_from_stream
	(
		number_of_lines,
		ifs,
		condition_to_load,
		condition_for_line,
		text_lines_from_file,
		file_size,
		contains_a_utf8_sequence
	);
	ifs.close();
	return retval;
}


} // namespace delovi
