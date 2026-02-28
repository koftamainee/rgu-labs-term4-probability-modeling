cmake_minimum_required(VERSION 3.16)

project(infection_simulation VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appinfection_simulation
    src/main.cpp
)

qt_add_qml_module(appinfection_simulation
    URI infection_simulation
    QML_FILES
        qml/Main.qml
        qml/SectionLabel.qml
        qml/AppButton.qml
        qml/ModeTab.qml
        qml/ParamSlider.qml
        qml/StatBadge.qml
        qml/StatDivider.qml
        qml/SearchButton.qml
        qml/GraphView.qml
        qml/FilePickerDialog.qml
    SOURCES src/simulation.hpp src/simulation.cpp
    SOURCES src/simulation_controller.hpp src/simulation_controller.cpp
    QML_FILES qml/Theme.qml
)



# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appinfection_simulation PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appinfection_simulation
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appinfection_simulation
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appinfection_simulation
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
