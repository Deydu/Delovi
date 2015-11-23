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



#include <algorithm>



namespace delovi {


//template <typename ITER> bool does_text_contain_a_utf8_sequence(ITER begin, ITER end)
//{
//	int could_be_utf8 = 0;
//	bool is_utf8 = false;
////	for (const unsigned char ch : entire_file_contents)
//	for (ITER iter = begin; end != iter; ++iter)
//	{
//		const unsigned char ch = *iter;
//		if (0 < could_be_utf8)
//		{
////			if (0x80 <= ch && 0xC0 > ch)
//			if (0xBF == (0xBF | ch))
//			{
//				--could_be_utf8;
//				if (0 == could_be_utf8)
//				{
//					// valid UTF8-sequence found
//					is_utf8 = true;
//					break;
//				}
//			}
//			else
//			{
//				// not a valid UTF8-sequence, could be anything
//				could_be_utf8 = 0;
//			}
//		}
//		else if (0xC0 <= ch)
//		{
//			if (0xE0 > ch)
//			{
//				// expecting one more UTF8-char following this one
//				could_be_utf8 = 1;
//			}
//			else // 0xE0 <= ch
//			{
//				if (0xF0 > ch)
//				{
//					// expecting two more UTF8-chars following this one
//					could_be_utf8 = 2;
//				}
//				else // 0xF0 <= ch
//				{
//					if (0xF8 > ch)
//					{
//						// expecting three more UTF8-chars following this one
//						could_be_utf8 = 3;
//					}
//					else
//					{
//						// not a valid UTF8-sequence, could be anything
//						could_be_utf8 = 0;
//					}
//				}
//			}
//		}
//		else
//		{
//			// not a valid UTF8-sequence, could be anything
//			could_be_utf8 = 0;
//		}
//	}
//	// if (0 < could_be_utf8) -> still not a valid UTF8-sequence
//	return is_utf8;
//}

template <typename ITER> bool does_text_contain_a_utf8_sequence(const ITER begin, const ITER end)
{
	auto there_are_utf8_followers = [] (ITER iter2, const ITER end, const unsigned int n) -> bool
	{
		if (end == iter2)
		{
			return false;
		}
		for (unsigned int i = 0; n > i; ++i)
		{
			++iter2;
			if (end == iter2)
			{
				return false;
			}
//			const unsigned char ch = static_cast<unsigned char>(*iter2);
//			if (!(0x80 <= ch && 0xC0 > ch))
//			if (0x80 > ch || 0xC0 <= ch)
			if (0x80 != (0xC0 & *iter2))
			{
				return false;
			}
		}
		return true;
	};
	auto find_first_utf8_char = [] (const char ch) -> bool { return 0xC0 <= static_cast<unsigned char>(ch); };
	for (ITER iter = begin; end != iter; ++iter)
	{
		iter = std::find_if(iter, end, find_first_utf8_char);
		if (end == iter)
		{
			return false;
		}
//		if (0xE0 > static_cast<unsigned char>(*iter))
//		{
//			// expecting one more UTF8-char following this one
//			if (there_are_utf8_followers(iter, end, 1))
//			{
//				return true;
//			}
//		}
//		else // 0xE0 <= *iter
//		{
//			if (0xF0 > static_cast<unsigned char>(*iter))
//			{
//				// expecting two more UTF8-chars following this one
//				if (there_are_utf8_followers(iter, end, 2))
//				{
//					return true;
//				}
//			}
//			else // 0xF0 <= *iter
//			{
//				if (0xF8 > static_cast<unsigned char>(*iter))
//				{
//					// expecting three more UTF8-chars following this one
//					if (there_are_utf8_followers(iter, end, 3))
//					{
//						return true;
//					}
//				}
//				// else: // not a valid UTF8-sequence, could be anything
//			}
//		}
		if (0xC0 == (0xC0 & *iter))
		{
			if (0xC0 == (0xE0 & *iter))
			{
				// expecting one more UTF8-char following this one
				if (there_are_utf8_followers(iter, end, 1))
				{
					return true;
				}
			}
			else if (0xE0 == (0xF0 & *iter))
			{
				// expecting two more UTF8-chars following this one
				if (there_are_utf8_followers(iter, end, 2))
				{
					return true;
				}
			}
			else if (0xF0 == (0xF8 & *iter))
			{
				// expecting three more UTF8-chars following this one
				if (there_are_utf8_followers(iter, end, 3))
				{
					return true;
				}
			}
		}
		// at this point no valid UTF8-sequence detected, could be anything
		// continueing loop
	}
	return false;
}


} // namespace delovi
