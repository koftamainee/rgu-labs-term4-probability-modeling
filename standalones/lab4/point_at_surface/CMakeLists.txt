cmake_minimum_required(VERSION 3.16)
project(point_walk VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Concurrent)
qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(apppoint_walk
    src/main.cpp
    src/point_walk_sim.hpp
    src/point_walk_sim.cpp
    src/point_walk_controller.hpp
    src/point_walk_controller.cpp
)

target_include_directories(apppoint_walk PRIVATE src)

qt_add_qml_module(apppoint_walk
    URI point_walk
    QML_FILES
        qml/Main.qml
        qml/Theme.qml
        qml/AppButton.qml
        qml/ParamSlider.qml
        qml/ModeTab.qml
        qml/StatBadge.qml
        qml/StatDivider.qml
        qml/SectionLabel.qml
        qml/FilePickerDialog.qml
        qml/TrajectoryChart.qml
)

set_target_properties(apppoint_walk PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(apppoint_walk
    PRIVATE Qt6::Quick Qt6::Concurrent
)

include(GNUInstallDirs)
install(TARGETS apppoint_walk
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
