cmake_minimum_required(VERSION 3.20)
project(MediaPlayer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core Gui Widgets Multimedia MultimediaWidgets
)

qt_standard_project_setup()

add_executable(${PROJECT_NAME} WIN32)

target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
    core/MediaEngine.cpp
    core/PlaylistManager.cpp
    core/EqualizerManager.cpp
    ui/MainWindow.cpp
    ui/ControlBar.cpp
    ui/PlaylistWidget.cpp
    ui/VideoWidget.cpp
    ui/SpectrumWidget.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/core
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets
    Qt6::Multimedia Qt6::MultimediaWidgets
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
