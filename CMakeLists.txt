project(applet_window_appmenu)

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)
set(VERSION 0.7.0)
set(AUTHOR "Michail Vourlakos")
set(EMAIL "mvourlakos@gmail.com")

set(QT_MIN_VERSION "5.9.0")
set(KF5_MIN_VERSION "5.38.0")

set(KF5_LOCALE_PREFIX "")

find_package(ECM ${KF5_MIN_VER} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Quick Widgets DBus)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Plasma Wayland WindowSystem ConfigWidgets)
find_package(KDecoration2 REQUIRED)
find_package(LibTaskManager)

include(ECMQtDeclareLoggingCategory)
include(KDECMakeSettings)
include(KDECompilerSettings)

# load X11 libraries
find_package(X11)
set_package_properties(X11 PROPERTIES DESCRIPTION "X11 libraries"
                        URL "http://www.x.org"
                        TYPE REQUIRED
                        PURPOSE "Required for building the X11 based workspace")

if(X11_FOUND)
  find_package(XCB MODULE REQUIRED COMPONENTS XCB RANDR)
  set_package_properties(XCB PROPERTIES TYPE REQUIRED)
  if(NOT X11_SM_FOUND)
    message(FATAL_ERROR "\nThe X11 Session Management (SM) development package could not be found.\nPlease install libSM.\n")
  endif(NOT X11_SM_FOUND)

  find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS X11Extras)
endif()

if(X11_FOUND AND XCB_XCB_FOUND)
  set(HAVE_X11 ON)
endif()

#discover KF5 version
string(REGEX MATCH "\\.([^]]+)\\." KF5_CURRENTMINOR_VERSION ${KF5_VERSION})
string(REGEX REPLACE "\\." "" KF5_CURRENTMINOR_VERSION ${KF5_CURRENTMINOR_VERSION})

message(STATUS "KF5 VERSION : ${KF5_VERSION}")
message(STATUS "KF5 VERSION CURRENT MINOR : ${KF5_CURRENTMINOR_VERSION}")

if(LibTaskManager_FOUND)
    #discover Plasma LibTaskManager version
    string(REGEX MATCH "\\.([^]]+)\\." LibTaskManager_CURRENTMINOR_VERSION ${LibTaskManager_VERSION})
    string(REGEX REPLACE "\\." "" LibTaskManager_CURRENTMINOR_VERSION ${LibTaskManager_CURRENTMINOR_VERSION})
    message(STATUS "LibTaskManager VERSION : ${LibTaskManager_VERSION}")
else()
    #initialize to not found
    set(LibTaskManager_CURRENTMINOR_VERSION -1)
endif()

message(STATUS "LibTaskManager CURRENT VERSION : ${LibTaskManager_CURRENTMINOR_VERSION}")

configure_file(config-appmenu.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/plugin/config-appmenu.h)
configure_file(config-appmenu.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/lib/config-appmenu.h)

include(KDEInstallDirs)

add_subdirectory(lib)
add_subdirectory(plugin)
plasma_install_package(package org.kde.windowappmenu)

