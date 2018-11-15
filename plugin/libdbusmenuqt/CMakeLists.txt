set(libdbusmenu_SRCS
dbusmenuimporter.cpp
dbusmenushortcut_p.cpp
dbusmenutypes_p.cpp
utils.cpp
)

ecm_qt_declare_logging_category(libdbusmenu_SRCS HEADER debug.h
                                            IDENTIFIER DBUSMENUQT
                                            CATEGORY_NAME org.kde.libdbusmenuqt
                                            DEFAULT_SEVERITY Info)

set_source_files_properties(com.canonical.dbusmenu.xml PROPERTIES
   NO_NAMESPACE true
   INCLUDE "dbusmenutypes_p.h"
   CLASSNAME DBusMenuInterface
)
qt5_add_dbus_interface(libdbusmenu_SRCS com.canonical.dbusmenu.xml dbusmenu_interface)


add_library(dbusmenuqt STATIC ${libdbusmenu_SRCS})
target_link_libraries(dbusmenuqt
    Qt5::DBus
    Qt5::Widgets
)

add_subdirectory(test)
