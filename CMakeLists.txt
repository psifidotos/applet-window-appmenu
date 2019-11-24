project(applet_window_appmenu)

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)
set(VERSION 0.6.0)
set(AUTHOR "Michail Vourlakos")
set(EMAIL "mvourlakos@gmail.com")

set(QT_MIN_VERSION "5.9.0")
set(KF5_MIN_VERSION "5.38.0")

set(KF5_LOCALE_PREFIX "")

find_package(ECM ${KF5_MIN_VER} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Quick Widgets DBus)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Plasma WindowSystem ConfigWidgets)
find_package(KDecoration2 REQUIRED)

include(ECMQtDeclareLoggingCategory)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(KDEInstallDirs)

add_subdirectory(lib)
add_subdirectory(plugin)

plasma_install_package(package org.kde.windowappmenu)

