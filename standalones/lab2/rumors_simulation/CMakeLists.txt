cmake_minimum_required(VERSION 3.16)
project(rumors_simulation VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt6 REQUIRED COMPONENTS Quick)
qt_standard_project_setup(REQUIRES 6.8)
qt_add_executable(apprumors_simulation
    src/main.cpp
)
qt_add_qml_module(apprumors_simulation
    URI rumors_simulation
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/ExperimentGraphView.qml
        qml/AppButton.qml
        qml/ParamSlider.qml
        qml/ModeTab.qml
        qml/SectionLabel.qml
        qml/StatBadge.qml
        qml/StatDivider.qml
        qml/Theme.qml
    SOURCES
        src/simulation.hpp
        src/simulation.cpp
        src/simulation_controller.hpp
        src/simulation_controller.cpp
)
set_target_properties(apprumors_simulation PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
target_link_libraries(apprumors_simulation
    PRIVATE Qt6::Quick
)
include(GNUInstallDirs)
install(TARGETS apprumors_simulation
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
