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
