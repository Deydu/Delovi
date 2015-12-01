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



#define COMPILERVERSION_CPP11
#ifdef __GNUC__
#	if (199711L >= __cplusplus)
#		undef COMPILERVERSION_CPP11
#	endif // __cplusplus
#endif // __GNUC__
#ifdef _MSC_VER
#	if (1800 > _MSC_VER)
#		undef COMPILERVERSION_CPP11
#	endif // 1800 > _MSC_VER
#endif // _MSC_VER

#define COMPILERVERSION_CPP11_HAS_MAKE_UNIQUE
#ifdef __GNUC__
#	if (201103L >= __cplusplus)
#		undef COMPILERVERSION_CPP11_HAS_MAKE_UNIQUE
#	endif // __cplusplus
#endif // __GNUC__
#ifdef _MSC_VER
#	if (1800 > _MSC_VER)
#		undef COMPILERVERSION_CPP11_HAS_MAKE_UNIQUE
#	endif // 1800 > _MSC_VER
#endif // _MSC_VER
