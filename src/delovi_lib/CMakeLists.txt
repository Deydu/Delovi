find_package(Boost)

include(WriteCompilerDetectionHeader)

file(GLOB DELOVI_LIB_SRCS
	*.cpp
	*.hpp
	*.h)

write_compiler_detection_header(
	FILE "${CMAKE_CURRENT_BINARY_DIR}/delovi_compiler_detection.h"
	PREFIX DELOVI
	COMPILERS GNU MSVC
	FEATURES
		cxx_attribute_deprecated
		cxx_constexpr
		cxx_decltype_auto
		cxx_generic_lambdas
		cxx_lambda_init_captures
		cxx_noexcept
		cxx_thread_local)

source_group("" FILES ${DELOVI_LIB_SRCS})

add_library(delovi_lib STATIC ${DELOVI_LIB_SRCS})
add_library(dlv::delovi_lib ALIAS delovi_lib)

set_property(TARGET delovi_lib PROPERTY CXX_STANDARD 11)

#include_directories(${DELOVI_SOURCE_DIR}/src/common)
target_include_directories(delovi_lib
	PUBLIC
		${DELOVI_SOURCE_DIR}/src/common
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR})
#if(WIN32)
#	include_directories(${Boost_INCLUDE_DIRS})
#else(WIN32)
#	include_directories(/usr/include/boost)
#endif(WIN32)
target_include_directories(delovi_lib SYSTEM
	PUBLIC ${Boost_INCLUDE_DIRS})

target_compile_features(delovi_lib
	PUBLIC
##		cxx_attribute_deprecated
##		cxx_constexpr
##		cxx_decltype_auto
##		cxx_generic_lambdas
##		cxx_lambda_init_captures
##		cxx_noexcept
##		cxx_thread_local
		cxx_auto_type
		cxx_decltype
		cxx_defaulted_functions
#		cxx_deleted_functions
		cxx_final
		cxx_lambdas
		cxx_nullptr
		cxx_override
		cxx_range_for
		cxx_right_angle_brackets
		cxx_rvalue_references
		cxx_static_assert
		cxx_uniform_initialization
		cxx_variadic_templates)

if(CMAKE_COMPILER_IS_GNUCXX)
	target_compile_definitions(delovi_lib
		PUBLIC LINUX
		PRIVATE
#			$<$<CONFIG:RelWithDebInfo>:NDEBUG>
#			$<$<CONFIG:MinSizeRel>:NDEBUG>
			$<$<CONFIG:Debug>:_DEBUG>
			$<$<CONFIG:Release>:NDEBUG>)
endif(CMAKE_COMPILER_IS_GNUCXX)
if(MSVC)
	target_compile_definitions(delovi_lib
		PUBLIC
			_WIN32_WINNT=0x0603
			BOOST_ALL_DYN_LINK
		PRIVATE
			$<$<CONFIG:Debug>:_CONSOLE>
			$<$<CONFIG:RelWithDebInfo>:_CONSOLE>)
endif(MSVC)

if(CMAKE_COMPILER_IS_GNUCXX)
	target_compile_options(delovi_lib
		PUBLIC
			-pipe
			-fPIC
			-Wfatal-errors
			-Wall
			-Weffc++
			-fno-diagnostics-show-caret
#			-std=c++11
		PUBLIC
			$<$<CONFIG:Debug>:-g -ggdb3>
			$<$<CONFIG:Release>:-march=native -O2>)
endif(CMAKE_COMPILER_IS_GNUCXX)
if(MSVC)
	target_compile_options(delovi_lib
		PUBLIC
#			$<$<CONFIG:RelWithDebInfo>:/arch:SSE>
			$<$<CONFIG:RelWithDebInfo>:/arch:SSE2>
#			$<$<CONFIG:MinSizeRel>:/arch:SSE>
			$<$<CONFIG:MinSizeRel>:/arch:SSE2>
#			$<$<CONFIG:Release>:/arch:SSE>
			$<$<CONFIG:Release>:-arch:SSE2>
		PUBLIC
#			/Zm1000
			/W4)
endif(MSVC)
