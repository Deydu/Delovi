find_package(Boost COMPONENTS
	serialization
	regex
	thread
	system
	date_time
	chrono
	REQUIRED)
find_package(Qt4 4.8 COMPONENTS
	QtCore
	QtGui
	REQUIRED)

include(${QT_USE_FILE})
include(WriteCompilerDetectionHeader)

set(DELOVI_MOC_HDRS
	LogfileDocument.hpp
	Mainwindow.hpp
	SimpleFilterDialog.hpp
	RegexFilterDialog.hpp)
set(DELOVI_UIS
	main_window.ui
	simple_filter_dialog.ui
	regex_filter_dialog.ui)
file(GLOB DELOVI_SRCS
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

source_group("" FILES ${DELOVI_SRCS})
source_group("UI-Files" FILES ${DELOVI_UIS})
#source_group("Headers" FILES ${DELOVI_HEADERS})
source_group("MOC-Files" FILES ${DELOVI_MOC_SRCS})
source_group("UI-Files" FILES ${DELOVI_UIS_H})

#QT4_AUTOMOC(${DELOVI_SRCS})
QT4_WRAP_CPP(DELOVI_MOC_SRCS ${DELOVI_MOC_HDRS})
QT4_WRAP_UI(DELOVI_UIS_H ${DELOVI_UIS})
#QT4_ADD_RESOURCES(DELOVI_QRC ${DELOVI_RESRCS})

add_executable(delovi WIN32
	${DELOVI_MOC_SRCS}
	${DELOVI_UIS_H}
#	${DELOVI_QRC}
	${DELOVI_SRCS})
add_executable(dlv::delovi ALIAS delovi)

set_property(TARGET delovi PROPERTY CXX_STANDARD 11)

#include_directories(${DELOVI_SOURCE_DIR}/src/delovi_lib)
#include_directories(${DELOVI_SOURCE_DIR}/src/common)
target_include_directories(delovi
	PUBLIC
#		${DELOVI_SOURCE_DIR}/src/common
		${DELOVI_SOURCE_DIR}/src/delovi_lib
	PRIVATE
		# Don't forget to include output directory, otherwise the UI file won't be wrapped!
		${CMAKE_CURRENT_BINARY_DIR})
#if(WIN32)
#	include_directories(${Boost_INCLUDE_DIRS})
#else(WIN32)
#	include_directories(/usr/include/boost)
#endif(WIN32)
#target_include_directories(delovi SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
# Don't forget to include output directory, otherwise the UI file won't be wrapped!
#include_directories(${CMAKE_CURRENT_BINARY_DIR})

target_compile_features(delovi
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

#if(MSVC)
#	target_compile_definitions(delovi
#		PUBLIC BOOST_ALL_DYN_LINK)
#endif(MSVC)

#if(WIN32)
#	link_directories(${Boost_LIBRARY_DIRS})
#else(WIN32)
#	link_directories(/usr/lib/boost)
#endif(WIN32)

#set(QT_USE_QTNETWORK, true)
#set(QT_USE_QTSQL, true)
#set(QT_USE_QTXML, true)

if(MSVC)
#	set_target_properties(delovi PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
#	set_target_properties(delovi PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
	set_target_properties(delovi PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
	set_target_properties(delovi PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
	set_target_properties(delovi PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	set_target_properties(delovi PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif(MSVC)

#target_link_libraries(delovi PUBLIC dlv::delovi_lib debug ... optimized ...)
target_link_libraries(delovi
	PUBLIC
		dlv::delovi_lib
		${QT_LIBRARIES}
		${Boost_LIBRARIES})

if(MSVC)
	# source: http://stackoverflow.com/questions/6534660/cmake-qt-libraries-binary-equivilent
	foreach(Qt_library ${QT_LIBRARIES})
		get_filename_component(Qt_library_name ${Qt_library} NAME_WE)
		string(REGEX REPLACE "^lib(.*)" "\\1" Qt_library_name ${Qt_library_name})
		set(Qt_shared_library ${QT_BINARY_DIR}/${Qt_library_name}.dll)
		if(EXISTS ${Qt_shared_library})
			message("Qt shared library to be installed:  ${Qt_shared_library}")
			list(APPEND Qt_Install_Libraries ${Qt_shared_library})
		endif(EXISTS ${Qt_shared_library})
	endforeach(Qt_library ${QT_LIBRARIES})
	list(REMOVE_DUPLICATES Qt_Install_Libraries)
	install(FILES ${Qt_Install_Libraries} DESTINATION Delovi)
	# same for Boost libraries
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
install(TARGETS delovi DESTINATION Delovi)
