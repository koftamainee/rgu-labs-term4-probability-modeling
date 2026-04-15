cmake_minimum_required(VERSION 3.16)
project(random_walk_simulator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Concurrent Widgets)
find_package(nlohmann_json REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(app_random_walk
        src/main.cpp
        src/discrete_random_variable.cpp
        src/random_walk_simulation.cpp
        src/random_walk_controller.cpp
)

target_include_directories(app_random_walk PRIVATE src)
target_link_libraries(app_random_walk PRIVATE Qt6::Quick Qt6::Concurrent Qt6::Widgets nlohmann_json::nlohmann_json)

qt_add_qml_module(app_random_walk
        URI RandomWalkSimulator
        QML_FILES
        qml/Main.qml
        qml/Theme.qml
        qml/AppButton.qml
        qml/ParamSlider.qml
        qml/ModeTab.qml
        qml/StatBadge.qml
        qml/StatDivider.qml
        qml/SectionLabel.qml
        qml/WalkVisualization.qml
        qml/DistributionChart.qml
)

set_target_properties(app_random_walk PROPERTIES
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS app_random_walk
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)