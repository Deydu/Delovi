find_package(Boost COMPONENTS
	random
	unit_test_framework
	system
	filesystem
	chrono
	REQUIRED)

include(WriteCompilerDetectionHeader)

file(GLOB UT_MISC_SRCS
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

source_group("" FILES ${UT_MISC_SRCS})

add_executable(ut1 WIN32 ${UT_MISC_SRCS})
add_executable(dlv::ut1 ALIAS ut1)

set_property(TARGET ut1 PROPERTY CXX_STANDARD 11)

#include_directories(${DELOVI_SOURCE_DIR}/src/common)
#include_directories(${DELOVI_SOURCE_DIR}/src/delovi_lib)
target_include_directories(ut1
	PUBLIC
#		${DELOVI_SOURCE_DIR}/src/common
		${DELOVI_SOURCE_DIR}/src/delovi_lib
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR})
#if(WIN32)
#	include_directories(${Boost_INCLUDE_DIRS})
#else(WIN32)
#	include_directories(/usr/include/boost)
#endif(WIN32)
#target_include_directories(ut1 SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})

target_compile_features(ut1
	PUBLIC
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

#if(WIN32)
#	link_directories(${Boost_LIBRARY_DIRS})
#else(WIN32)
#	link_directories(/usr/lib/boost)
#endif(WIN32)

if(MSVC)
#	set_target_properties(ut1 PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
#	set_target_properties(ut1 PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
#	set_target_properties(ut1 PROPERTIES COMPILE_DEFINITIONS_RELEASE "_CONSOLE")
#	set_target_properties(ut1 PROPERTIES COMPILE_DEFINITIONS_MINSIZEREL "_CONSOLE")
	set_target_properties(ut1 PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
	set_target_properties(ut1 PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
	set_target_properties(ut1 PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")
	set_target_properties(ut1 PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:CONSOLE")
endif(MSVC)

#target_link_libraries(ut1 PUBLIC delovi_lib debug ... optimized ...)
target_link_libraries(ut1
	PUBLIC
		dlv::delovi_lib
		${Boost_LIBRARIES})

if(MSVC)
	foreach(Boost_library ${Boost_LIBRARIES})
		get_filename_component(Boost_library_name ${Boost_library} NAME_WE)
		string(REGEX REPLACE "^lib(.*)" "\\1" Boost_library_name ${Boost_library_name})
		set(Boost_shared_library ${Boost_LIBRARY_DIRS}/${Boost_library_name}.dll)
		if(EXISTS ${Boost_shared_library})
			message("Boost shared library to be installed:  ${Boost_shared_library}")
			list(APPEND Boost_Install_Libraries ${Boost_shared_library})
		endif(EXISTS ${Boost_shared_library})
	endforeach(Boost_library ${Boost_LIBRARIES})
	list(REMOVE_DUPLICATES Boost_Install_Libraries)
	install(FILES ${Boost_Install_Libraries} DESTINATION Delovi)
endif(MSVC)
install(TARGETS ut1 DESTINATION Delovi)
