#pragma once



#if defined __GNUC__
#	pragma GCC system_header
#elif defined __SUNPRO_CC
#	pragma disable_warn
#elif defined _MSC_VER
//#	pragma warning(push, 0)
#	pragma warning(push)
#	pragma warning(disable : 4127)	// conditional expression is constant
#	pragma warning(disable : 4251)	// class needs to have dll-interface to be used
//#	pragma warning(disable : 4365)	// 'argument' : conversion from 'uint' to 'int', signed/unsigned mismatch
//#	pragma warning(disable : 4571)	// Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
//#	pragma warning(disable : 4619)	// #pragma warning : there is no warning number '4660'
//#	pragma warning(disable : 4625)	// copy constructor could not be generated because a base class copy constructor is inaccessible
//#	pragma warning(disable : 4626)	// assignment operator could not be generated because a base class assignment operator is inaccessible
#	pragma warning(disable : 4800)	// forcing value to bool 'true' or 'false' (performance warning)
//#	pragma warning(disable : 4820)	// n bytes padding added after data member
//#	pragma warning(disable : 4986)	// 'operator new[]': exception specification does not match previous declaration
#endif



#include <boost/test/unit_test.hpp>
//#include <boost/chrono.hpp>
//#include <boost/filesystem.hpp>



#if defined __SUNPRO_CC
#	pragma enable_warn
#elif defined _MSC_VER
#	pragma warning(pop)
#endif
