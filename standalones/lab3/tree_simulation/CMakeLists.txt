cmake_minimum_required(VERSION 3.16)
project(tree_simulation VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(apptree_simulation
        src/main.cpp
        src/TreeConfig.hpp
        src/TreeConfig.cpp
        src/TreeSimulation.hpp
        src/TreeSimulation.cpp
        src/TreeController.hpp
        src/TreeController.cpp
)

qt_add_qml_module(apptree_simulation
        URI tree_simulation
        VERSION 1.0
        QML_FILES
        qml/Main.qml
        qml/TreeView.qml
        qml/HistogramView.qml
        qml/FilePickerDialog.qml
        qml/AppButton.qml
        qml/ParamSlider.qml
        qml/SectionLabel.qml
        qml/StatBadge.qml
        qml/StatDivider.qml
        qml/Theme.qml
        RESOURCES
        qml/qmldir
)

set_target_properties(apptree_simulation PROPERTIES
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

target_include_directories(apptree_simulation PRIVATE src)

target_link_libraries(apptree_simulation
        PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS apptree_simulation
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
