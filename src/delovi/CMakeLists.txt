include(${QT_USE_FILE})

include_directories(${DELOVI_SOURCE_DIR}/src/delovi_lib)
include_directories(${DELOVI_SOURCE_DIR}/src/common)
if(WIN32)
	include_directories($ENV{BOOSTINCLUDE})
else(WIN32)
	include_directories(/usr/include/boost)
endif(WIN32)

if(WIN32)
	link_directories($ENV{BOOSTLIB})
else(WIN32)
	link_directories(/usr/lib/boost)
endif(WIN32)

set(DELOVI_MOC_HDRS LogfileDocument.hpp Mainwindow.hpp SimpleFilterDialog.hpp RegexFilterDialog.hpp)
set(DELOVI_UIS main_window.ui simple_filter_dialog.ui regex_filter_dialog.ui)
file(GLOB DELOVI_SRCS *.cpp *.hpp *.h)

#QT4_AUTOMOC(${DELOVI_SRCS})
QT4_WRAP_CPP(DELOVI_MOC_SRCS ${DELOVI_MOC_HDRS})
QT4_WRAP_UI(DELOVI_UIS_H ${DELOVI_UIS})
#QT4_ADD_RESOURCES(DELOVI_QRC ${DELOVI_RESRCS})

source_group("" FILES ${DELOVI_SRCS})
source_group("UI-Files" FILES ${DELOVI_UIS})
#source_group("Headers" FILES ${DELOVI_HEADERS})
source_group("MOC-Files" FILES ${DELOVI_MOC_SRCS})
source_group("UI-Files" FILES ${DELOVI_UIS_H})

#set(QT_USE_QTNETWORK, true)
#set(QT_USE_QTSQL, true)
#set(QT_USE_QTXML, true)

# Don't forget to include output directory, otherwise the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

#add_executable(delovi WIN32 ${DELOVI_MOC_SRCS} ${DELOVI_UIS_H} ${DELOVI_QRC} ${DELOVI_SRCS})
add_executable(delovi WIN32 ${DELOVI_MOC_SRCS} ${DELOVI_UIS_H} ${DELOVI_SRCS})

if(WIN32)
#	set_target_properties(delovi PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
#	set_target_properties(delovi PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
	set_target_properties(delovi PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
	set_target_properties(delovi PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
	set_target_properties(delovi PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	set_target_properties(delovi PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif(WIN32)

if(WIN32)
	target_link_libraries(delovi ${QT_LIBRARIES} delovi_lib)
else(WIN32)
	target_link_libraries(delovi ${QT_LIBRARIES} delovi_lib boost_serialization boost_regex)
endif(WIN32)

install(TARGETS delovi DESTINATION Delovi)
