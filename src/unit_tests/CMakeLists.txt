include_directories (${DELOVI_SOURCE_DIR}/src/common)
include_directories (${DELOVI_SOURCE_DIR}/src/delovi_lib)
if(WIN32)
	include_directories ($ENV{BOOSTINCLUDE})
else(WIN32)
	include_directories (/usr/include/boost)
endif(WIN32)

if(WIN32)
	link_directories ($ENV{BOOSTLIB})
else(WIN32)
	link_directories (/usr/lib/boost)
endif(WIN32)

file (GLOB UT_MISC_SRCS *.cpp *.hpp *.h)

source_group("" FILES ${UT_MISC_SRCS})

add_executable (ut1 WIN32 ${UT_MISC_SRCS})

if(WIN32)
#	set_target_properties(ut1 PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
#	set_target_properties(ut1 PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
#	set_target_properties(ut1 PROPERTIES COMPILE_DEFINITIONS_RELEASE "_CONSOLE")
#	set_target_properties(ut1 PROPERTIES COMPILE_DEFINITIONS_MINSIZEREL "_CONSOLE")
	set_target_properties(ut1 PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
	set_target_properties(ut1 PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
	set_target_properties(ut1 PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")
	set_target_properties(ut1 PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:CONSOLE")
endif(WIN32)

if(WIN32)
	target_link_libraries (ut1 delovi_lib)
#	target_link_libraries (ut1 delovi_lib debug ... optimized ...)
else(WIN32)
	target_link_libraries (ut1 delovi_lib boost_random boost_unit_test_framework boost_system boost_filesystem boost_chrono)
endif(WIN32)

install(TARGETS ut1 DESTINATION Delovi)
