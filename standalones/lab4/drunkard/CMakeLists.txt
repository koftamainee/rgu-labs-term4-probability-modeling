cmake_minimum_required(VERSION 3.16)
project(drunk_walk VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Concurrent)
qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appdrunk_walk
    src/main.cpp
    src/drunk_walk.hpp
    src/drunk_walk.cpp
    src/drunk_walk_controller.hpp
    src/drunk_walk_controller.cpp
)

target_include_directories(appdrunk_walk PRIVATE src)

qt_add_qml_module(appdrunk_walk
    URI drunk_walk
    QML_FILES
        qml/Main.qml
        qml/Theme.qml
        qml/AppButton.qml
        qml/ParamSlider.qml
        qml/ModeTab.qml
        qml/StatBadge.qml
        qml/StatDivider.qml
        qml/SectionLabel.qml
        qml/StreetView.qml
        qml/ProbabilityChart.qml
)

set_target_properties(appdrunk_walk PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appdrunk_walk
    PRIVATE Qt6::Quick Qt6::Concurrent
)

include(GNUInstallDirs)
install(TARGETS appdrunk_walk
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
