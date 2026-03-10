cmake_minimum_required(VERSION 3.16)
project(staircase VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Concurrent)
qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appstaircase
    src/main.cpp
    src/staircase_sim.hpp
    src/staircase_sim.cpp
    src/staircase_controller.hpp
    src/staircase_controller.cpp
)

target_include_directories(appstaircase PRIVATE src)

qt_add_qml_module(appstaircase
    URI staircase
    QML_FILES
        qml/Main.qml
        qml/Theme.qml
        qml/AppButton.qml
        qml/ParamSlider.qml
        qml/ModeTab.qml
        qml/StatBadge.qml
        qml/StatDivider.qml
        qml/SectionLabel.qml
        qml/StaircaseChart.qml
        qml/FilePickerDialog.qml
)

set_target_properties(appstaircase PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appstaircase
    PRIVATE Qt6::Quick Qt6::Concurrent
)

include(GNUInstallDirs)
install(TARGETS appstaircase
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
