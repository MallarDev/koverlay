cmake_minimum_required(VERSION 3.20)

# ---- Project ----
project(koverlay VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# ---- Deps ----
find_package(ECM 6.0 REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets WaylandClient DBus Qml Quick)
if(NOT TARGET Qt6::GuiPrivate)
    find_package(Qt6 REQUIRED COMPONENTS GuiPrivate)
endif()
find_package(LayerShellQt REQUIRED)
find_package(Wayland REQUIRED COMPONENTS Client)

# ---- Target ----
add_executable(koverlay
        src/main.cpp
        src/overlay_config.h
        src/overlay_view.h
        src/overlay_view.cpp
        src/overlay_adaptor.h
        src/overlay_adaptor.cpp
        src/resources.qrc
        src/Position.h
        src/monitor_settings.h
        src/single_instance.h
)

target_link_libraries(koverlay PRIVATE
        Qt6::Core Qt6::Gui Qt6::Widgets Qt6::WaylandClient Qt6::DBus
        Qt6::Qml Qt6::Quick Qt6::GuiPrivate
        LayerShellQt::Interface
        Wayland::Client
)

# ---- Dev convenience ----
add_custom_target(run
        COMMAND QT_QPA_PLATFORM=wayland $<TARGET_FILE:koverlay> --show
        DEPENDS koverlay
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)

# ---- Install ----
include(GNUInstallDirs)

install(TARGETS koverlay
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Optional desktop file (install only if present)
if (EXISTS "${CMAKE_SOURCE_DIR}/packaging/koverlay.desktop")
    install(FILES packaging/koverlay.desktop
            DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
endif ()

# ---- CPack (RPM) ----
set(CPACK_GENERATOR "RPM")
set(CPACK_PACKAGE_NAME "koverlay")
set(CPACK_PACKAGE_VENDOR "koverlay")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Click-through text and system-monitor overlay for KDE Wayland")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Utilities")

# Let RPM auto-detect exact .so deps
set(CPACK_RPM_PACKAGE_AUTOREQPROV ON)

# Also add human-friendly meta packages users should have
set(CPACK_RPM_PACKAGE_REQUIRES
        "layer-shell-qt, qt6-qtbase, qt6-qtdeclarative, qt6-qtwayland, libksysguard, kf6-kquickcharts, ksystemstats")

include(CPack)

include(CTest)
if(BUILD_TESTING)
    find_package(Qt6 REQUIRED COMPONENTS Test QuickTest)
    add_executable(single_instance_test tests/single_instance_test.cpp src/single_instance.h)
    target_include_directories(single_instance_test PRIVATE src)
    target_link_libraries(single_instance_test PRIVATE Qt6::Test Qt6::DBus)
    find_program(DBUS_RUN_SESSION dbus-run-session REQUIRED)
    add_test(NAME single_instance COMMAND ${DBUS_RUN_SESSION} -- $<TARGET_FILE:single_instance_test>)

    add_executable(settings_test tests/settings_test.cpp src/overlay_config.h src/monitor_settings.h)
    target_include_directories(settings_test PRIVATE src)
    target_link_libraries(settings_test PRIVATE Qt6::Test Qt6::Gui)
    add_test(NAME settings COMMAND settings_test)

    add_executable(charts_test tests/charts_test.cpp src/overlay_config.h)
    target_include_directories(charts_test PRIVATE src)
    target_link_libraries(charts_test PRIVATE Qt6::QuickTest Qt6::Quick)
    add_test(NAME charts COMMAND charts_test -input ${CMAKE_CURRENT_SOURCE_DIR}/tests/qml)
endif()
