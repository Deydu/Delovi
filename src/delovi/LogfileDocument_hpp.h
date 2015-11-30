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



#if defined __GNUC__
#	pragma GCC system_header
#elif defined __SUNPRO_CC
#	pragma disable_warn
#elif defined _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4127)	// conditional expression is constant
//#	pragma warning(disable : 4244)	// 'return' : type conversion, possible loss of data
#	pragma warning(disable : 4251)	// class needs to have dll-interface to be used
//#	pragma warning(disable : 4311)	// 'reinterpret_cast' : pointer truncation
//#	pragma warning(disable : 4312)	// 'reinterpret_cast' : conversion to <type> of greater size
//#	pragma warning(disable : 4365)	// 'argument' : conversion from 'uint' to 'int', signed/unsigned mismatch
//#	pragma warning(disable : 4512)	// assignment operator could not be generated
//#	pragma warning(disable : 4514)	//  unreferenced inline function has been removed
//#	pragma warning(disable : 4619)	// there is no warning number '4660'
//#	pragma warning(disable : 4625)	// copy constructor could not be generated because a base class copy constructor is inaccessible
//#	pragma warning(disable : 4626)	// assignment operator could not be generated because a base class assignment operator is inaccessible
#	pragma warning(disable : 4800)	// forcing value to bool 'true' or 'false' (performance warning)
//#	pragma warning(disable : 4820)	// n bytes padding added after data member
//#	pragma warning(disable : 4986)	// 'operator new[]': exception specification does not match previous declaration
#endif



#if 1 == DELOVI_DOCUMENT_USES_TEXTEDIT
#	include <QTextEdit>
#elif 2 == DELOVI_DOCUMENT_USES_TEXTEDIT
#	include <QPlainTextEdit>
#endif



#if defined __SUNPRO_CC
#	pragma enable_warn
#elif defined _MSC_VER
#	pragma warning(pop)
#endif
