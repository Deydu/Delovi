#pragma once



#include "cpp_version_check.hpp"

#include <memory>



#ifdef COMPILERVERSION_CPP11
#ifndef COMPILERVERSION_CPP11_HAS_MAKE_UNIQUE
namespace std {


template<typename _Tp>
struct _MakeUniq
{
	typedef unique_ptr<_Tp> __single_object;
};
template<typename _Tp>
struct _MakeUniq<_Tp[]>
{
	typedef unique_ptr<_Tp[]> __array;
};
template<typename _Tp, size_t _Bound>
struct _MakeUniq<_Tp[_Bound]>
{
	struct __invalid_type { };
};

// std::make_unique for single objects
template<typename _Tp, typename... _Args>
typename _MakeUniq<_Tp>::__single_object make_unique(_Args&&... __args)
{
	return unique_ptr<_Tp>(new _Tp(std::forward<_Args>(__args)...));
}

// std::make_unique for arrays of unknown bound
template<typename _Tp>
typename _MakeUniq<_Tp>::__array make_unique(size_t __num)
{
	return unique_ptr<_Tp>(new typename remove_extent<_Tp>::type[__num]());
}

// Disable std::make_unique for arrays of known bound
template<typename _Tp, typename... _Args>
typename _MakeUniq<_Tp>::__invalid_type make_unique(_Args&&...) = delete;


} // namespace std
#endif // COMPILERVERSION_CPP11_HAS_MAKE_UNIQUE
#endif // COMPILERVERSION_CPP11
