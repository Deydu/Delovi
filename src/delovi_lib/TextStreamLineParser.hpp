/*****************************************************************************
 *
 * Copyright (C) 2015 Denny Duphorn
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



namespace delovi {


class TextStreamLineParser
{
	public:
		enum return_codes_e
		{
			NO_CHAR_READ	= -1,
			EMPTY_LINE		= 0,	// empty line, only containing line-break if append_line_break is set to "true"
			ASCII			= 1,	// line contains only ASCII-chars
			UTF8			= 2		// line contains UTF8-chars
		};
		
//		static int parseLineBackward(std::istream& is, std::string& line, const bool append_line_break)
		template <typename ISTREAM> static int parseLineBackward(ISTREAM& is, std::string& line, const bool append_line_break)
		{
//			typedef std::istream::pos_type stream_pos_t;
			typedef typename ISTREAM::pos_type stream_pos_t;
			
			line.clear();
			line.reserve(1024);
			
			if (!is.good())
			{
				return NO_CHAR_READ;
			}
			auto is_streampos = is.tellg();
			if (0 == is_streampos)
			{
				return NO_CHAR_READ;
			}
			
			std::vector<char> readdata;
			readdata.reserve(1024);
			bool found_utf8_encoding = false;
			bool end_of_previous_line_parsed = false;
			
			while (is.good() && 0 < is_streampos)
			{
				is_streampos -= 1;
				is.seekg(is_streampos);
				// expecting 1 or 2 line break chars now
				if (!is.good())
				{
					break;
				}
				char stmpc = static_cast<char>(is.peek());
				unsigned char tmpc = stmpc;
				
				// 0D0A == DOS
				//  0A  == UNIX
				//  0D  == MAC
				if (0x0A == tmpc)
				{
					// DOS or UNIX
					if (end_of_previous_line_parsed)
					{
						// set stream-pos back (forward)
						if (is.good())
						{
							is_streampos += 1;
							is.seekg(is_streampos);
						}
						break;
					}
					end_of_previous_line_parsed = true;
					
					if (is.good() && 0 < is_streampos)
					{
						is_streampos -= 1;
						is.seekg(is_streampos);
						if (!is.good())
						{
							break;
						}
						stmpc = static_cast<char>(is.peek());
						tmpc = stmpc;
						if (0x0D != tmpc)
						{
							// UNIX  -->  set stream-pos back (forward)
							if (!is.good())
							{
								break;
							}
							is_streampos += 1;
							is.seekg(is_streampos);
						}
						// else: DOS  -->  nothing to do
					}
					continue;
				}
				if (0x0D == tmpc)
				{
					// MAC
					if (end_of_previous_line_parsed)
					{
						// set stream-pos back (forward)
						if (is.good())
						{
							is_streampos += 1;
							is.seekg(is_streampos);
						}
						break;
					}
					end_of_previous_line_parsed = true;
					continue;
				}
				if (0x00 == tmpc)
				{
					break;
				}
				
				if (0x7F < tmpc)
				{
					found_utf8_encoding = true;
				}
				readdata.push_back(tmpc);
			} // while
			
			line.assign(readdata.rbegin(), readdata.rend());
			if (append_line_break)
			{
				line.append(1, '\n');
			}
			return readdata.empty() ? EMPTY_LINE : found_utf8_encoding ? UTF8 : ASCII;
		}
		
		template <typename ISTREAM>
		static int parseLineForward(ISTREAM& is, std::string& line, const bool append_line_break)
		{
			line.clear();
			line.reserve(1024);
			
			if (!is.good())
			{
				return NO_CHAR_READ;
			}
			
			std::vector<char> readdata;
			readdata.reserve(1024);
			bool found_utf8_encoding = false;
			
			while (is.good())
			{
				char stmpc = 0;
				is.get(stmpc);
				unsigned char tmpc = stmpc;
				
				// 0D0A == DOS
				//  0A  == UNIX
				//  0D  == MAC
				if (0x0D == tmpc)
				{
					// MAC or DOS
					if (is.good())
					{
						stmpc = static_cast<char>(is.peek());
						tmpc = stmpc;
						if (0x0A == tmpc)
						{
							// DOS  -->  remove this char from stream
							is.get(stmpc);
						}
						// else: MAC  -->  nothing to do
					}
					// else: cannot read any more chars from input stream
					break;
				}
				if (0x0A == tmpc)
				{
					// DOS or UNIX
					// actually only UNIX because the case "DOS" should have been handled above when parsing 0x0D!
					// anyway: no following chars expected, so no read-ahead necessary
					break;
				}
				if (0x00 == tmpc)
				{
					break;
				}
				
				if (0x7F < tmpc)
				{
					found_utf8_encoding = true;
				}
				readdata.push_back(tmpc);
			} // while
			
			line.assign(readdata.begin(), readdata.end());
			if (append_line_break)
			{
				line.append(1, '\n');
			}
			return readdata.empty() ? EMPTY_LINE : found_utf8_encoding ? UTF8 : ASCII;
		}
};


} // namespace delovi
