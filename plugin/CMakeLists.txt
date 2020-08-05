set(appmenuapplet_SRCS
    appmenumodel.cpp
    appmenuplugin.cpp
    commontools.cpp
    schemecolors.cpp
    schemesmodel.cpp
    wm/abstractwindowmanager.cpp
    wm/waylandwindowmanager.cpp
    wm/x11fallbackwindowmanager.cpp
)

add_library(appmenuplugin SHARED ${appmenuapplet_SRCS})

# load dbusmenuqt
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Widgets DBus)
cmake_policy(SET CMP0063 NEW) # this is very important otherwise some signals are not recognized
add_subdirectory(libdbusmenuqt)

target_link_libraries(appmenuplugin
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::Quick
                      KF5::Plasma
                      KF5::WaylandClient
                      KF5::WindowSystem
                      PW::LibTaskManager
                      dbusmenuqt)

if(HAVE_X11)
    find_package(XCB MODULE REQUIRED COMPONENTS XCB)
    set_package_properties(XCB PROPERTIES TYPE REQUIRED)
    target_link_libraries(appmenuplugin Qt5::X11Extras XCB::XCB)
endif()

install(TARGETS appmenuplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/private/windowAppMenu)

install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/private/windowAppMenu)
