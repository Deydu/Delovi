cmake_minimum_required(VERSION 2.8)

project(DELOVI)

add_subdirectory(src)

if(WIN32)
	set(CMAKE_INSTALL_PREFIX "D:/Tools/")
else(WIN32)
	set(CMAKE_INSTALL_PREFIX $ENV{HOME}/bin/)
endif(WIN32)
