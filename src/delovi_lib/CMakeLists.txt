find_package(Boost)

include_directories(${DELOVI_SOURCE_DIR}/src/common)
if(WIN32)
	include_directories(${Boost_INCLUDE_DIRS})
else(WIN32)
	include_directories(/usr/include/boost)
endif(WIN32)

#file(GLOB DELOVI_LIB_SRCS *.cpp)
file(GLOB DELOVI_LIB_SRCS *.cpp *.hpp *.h)

source_group("" FILES ${DELOVI_LIB_SRCS})

#if(NOT WIN32)
##	add_definitions(-DSOMETHING)
#	add_definitions(-Weffc++)
#endif(NOT WIN32)

add_library(delovi_lib STATIC ${DELOVI_LIB_SRCS})
