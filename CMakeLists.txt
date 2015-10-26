cmake_minimum_required (VERSION 2.8)
project (DELOVI)

find_package (Qt4 REQUIRED) # find and setup Qt4 for this project

add_subdirectory (src)

if(WIN32)
	set (CMAKE_INSTALL_PREFIX "D:/Tools/")
else(WIN32)
	set (CMAKE_INSTALL_PREFIX $ENV{HOME}/bin/)
endif(WIN32)
