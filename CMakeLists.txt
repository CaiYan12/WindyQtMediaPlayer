cmake_minimum_required(VERSION 3.20)
project(MediaPlayer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================
# MSVC 标准库路径（供 CMake 的 Ninja 生成器找到 type_traits 等）
# CMake 的 Ninja 生成器不会自动从 %INCLUDE% 环境变量读取，
# 所以这里显式注入 MSVC 2022 标准库和 Windows SDK 的 include
# ============================================================
if(MSVC)
    # 从 MSVC 安装目录自动探测最新版本
    file(GLOB MSVC_TOOLCHAINS
        "C:/Program Files/Microsoft Visual Studio/*/Community/VC/Tools/MSVC/*"
    )
    list(SORT MSVC_TOOLCHAINS)
    list(REVERSE MSVC_TOOLCHAINS)
    list(GET MSVC_TOOLCHAINS 0 MSVC_TOOLCHAIN_DIR)

    # MSVC C++ 标准库头文件
    list(APPEND CMAKE_MSVC_RESOLVE_SYMLINKS 1)
    include_directories("${MSVC_TOOLCHAIN_DIR}/include")

    # Windows SDK ucrt 和 um
    file(GLOB WINSDK_INCLUDES
        "C:/Program Files (x86)/Windows Kits/10/Include/*/ucrt"
        "C:/Program Files (x86)/Windows Kits/10/Include/*/shared"
        "C:/Program Files (x86)/Windows Kits/10/Include/*/um"
        "C:/Program Files (x86)/Windows Kits/10/Include/*/winrt"
    )
    if(WINSDK_INCLUDES)
        include_directories(${WINSDK_INCLUDES})
    endif()

    # 修复 MSVC 的 __cplusplus 宏默认为 C++14 的问题
    # Qt6 检测 C++17 需要这个标志
    add_compile_options(/Zc:__cplusplus)
endif()

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
