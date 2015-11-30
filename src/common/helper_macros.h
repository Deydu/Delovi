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



#ifdef _MSC_VER
#	if !defined AFXASSUME && !defined _ASSERT
#		include <cassert>
#	endif // !AFXASSUME && !_ASSERT
#	if !defined _ASSERT_EXPR && !defined BOOST_ASSERT_MSG
#		include <cassert>
#	endif // _ASSERT_EXPR
#else
#	include <cassert>
#endif // _MSC_VER



#define HELPER_MACRO_DO_STRINGIZE(X) #X
#define HELPER_MACRO_STRINGIZE(X) HELPER_MACRO_DO_STRINGIZE(X)
// best practice: use BOOST_STRINGIZE defined in boost/config/suffix.hpp

#define HELPER_MACRO_DO_WIDEN_STRING(x) L ## x
#define HELPER_MACRO_WIDEN_STRING(x) HELPER_MACRO_DO_WIDEN_STRING(x)

#define HELPER_MACRO_DO_STRING_JOIN2(arg1, arg2) arg1 ## arg2
#define HELPER_MACRO_STRING_JOIN2(arg1, arg2) HELPER_MACRO_DO_STRING_JOIN2(arg1, arg2)


#ifdef __GNUC__
#	define DO_PRINT_COMPILE_TODO(x)   _Pragma (#x)
#	define PRINT_COMPILE_MAYBE(msg)   DO_PRINT_COMPILE_TODO(message ("MAYBE: " msg))
#	define PRINT_COMPILE_TODO(msg)    DO_PRINT_COMPILE_TODO(message ("TODO: "  msg))
//#	define PRINT_COMPILE_WARNING(msg) <-- dunno yet
#endif // __GNUC__

#ifdef _MSC_VER
#	define PRINT_COMPILE_DEPRECATED(msg) __pragma(message(__FILE__ "(" HELPER_MACRO_STRINGIZE(__LINE__) "): warning: DEPRECATED: " msg));
#	define PRINT_COMPILE_MAYBE(msg)      __pragma(message(__FILE__ "(" HELPER_MACRO_STRINGIZE(__LINE__) "): warning: MAYBE: "      msg));
#	define PRINT_COMPILE_INFO(msg)       __pragma(message(__FILE__ "(" HELPER_MACRO_STRINGIZE(__LINE__) "): warning: INFO: "       msg));
#	define PRINT_COMPILE_TODO(msg)       __pragma(message(__FILE__ "(" HELPER_MACRO_STRINGIZE(__LINE__) "): warning: TODO: "       msg));
#	define PRINT_COMPILE_WARNING(msg)    __pragma(message(__FILE__ "(" HELPER_MACRO_STRINGIZE(__LINE__) "): warning: "             msg));
#	define PRINT_COMPILE_ERROR(msg)      __pragma(message(__FILE__ "(" HELPER_MACRO_STRINGIZE(__LINE__) "): error: "               msg));
#endif // _MSC_VER



#ifdef __GNUC__
#	define COMMON_ASSERT(condition) assert(condition)
#	define COMMON_ASSERT_MSG(condition, msg) COMMON_ASSERT(condition)
#endif // __GNUC__

#ifdef _MSC_VER
#	ifdef AFXASSUME
#		define COMMON_ASSERT(condition) AFXASSUME(condition)
#	elif defined _ASSERT
#		define COMMON_ASSERT(condition) _ASSERT(condition)
#	else
#		define COMMON_ASSERT(condition) assert(condition)
#	endif // AFXASSUME
#	ifdef _ASSERT_EXPR
#		ifdef CT2CW
#			define COMMON_ASSERT_MSG(condition, msg) _ASSERT_EXPR(condition, CT2CW(msg))
#		else
#			define COMMON_ASSERT_MSG(condition, msg) _ASSERT_EXPR(condition, HELPER_MACRO_WIDEN_STRING(msg))
#		endif // CT2CW
#	elif defined BOOST_ASSERT_MSG
#		define COMMON_ASSERT_MSG(condition, msg) BOOST_ASSERT_MSG(condition, msg)
#	else
#		define COMMON_ASSERT_MSG(condition, msg) COMMON_ASSERT(condition)
#	endif // _ASSERT_EXPR
#else
#	define COMMON_ASSERT(condition) assert(condition)
#	define COMMON_ASSERT_MSG(condition, msg) COMMON_ASSERT(condition)
#endif // _MSC_VER



#define COMMON_LOCK_GUARD(lock_class, mutex) lock_class<decltype(mutex)> HELPER_MACRO_STRING_JOIN2(lock_, __LINE__)(mutex);



namespace common {
	template <typename F>
	struct ScopeExit
	{
		ScopeExit(F f) : f(f) {}
		~ScopeExit() { f(); }
		F f;
	};
	template <typename F>
	ScopeExit<F> MakeScopeExit(F f)
	{
		return ScopeExit<F>(f);
	};
} // namespace common

#define COMMON_SCOPE_EXIT(code) auto HELPER_MACRO_STRING_JOIN2(scope_exit_, __LINE__) = common::MakeScopeExit((code))
