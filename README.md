# Windy媒体播放器 (WindyQtMediaPlayer)

基于 **Qt6 Multimedia** 的高性能本地/网络媒体播放器，界面风格参考 QQ 影音。

## 支持格式

| 类型 | 格式 |
|------|------|
| 音频 | MP3, WAV, FLAC, AAC, OGG, M4A |
| 视频 | MP4, MKV, AVI, WMV, WebM |

## 环境要求

- **Qt 6.x**（任意版本，建议 6.8+）
- **CMake 3.20+**
- **Ninja** 或 **Make**
- **支持 C++17 的编译器**（MSVC / GCC / Clang 均可）


## 项目结构

```
MediaPlayer/
├── CMakeLists.txt       # CMake 构建配置（无硬编码路径）
├── build-ps.ps1        # Windows 构建脚本（含 MSVC 环境检测）
├── main.cpp            # 程序入口
├── core/               # 核心业务层（无 UI 依赖）
│   ├── MediaEngine     # QMediaPlayer 封装
│   ├── PlaylistManager # 播放列表
│   └── EqualizerManager # 均衡器
├── ui/                 # UI 层
│   ├── MainWindow      # 主窗口
│   ├── ControlBar      # 底部控制栏
│   ├── PlaylistWidget  # 播放列表面板
│   ├── VideoWidget     # 视频显示
│   └── SpectrumWidget  # 音频频谱
└── res/
    └── resources.qrc   # Qt 资源
```

## 构建步骤

### Windows + MSVC + VS Code

```bash
# 1. 用 PowerShell 运行构建脚本（自动检测 MSVC + Qt）
pwsh -File build-ps.ps1

# 2. 编译后运行
pwsh -File build-ps.ps1 run
```

`build-ps.ps1` 会自动查找本机安装的 MSVC 2022 和 Qt6，不需要手动配置路径。

### Windows + MSVC + 命令行

```bash
# 配置（需指定 Qt 路径）
cmake -S . -B build ^
    -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_PREFIX_PATH="C:/Qt/6.10.0/msvc2022_64/lib/cmake"

cmake --build build --config Debug --parallel
```

### Linux / macOS + CMake

```bash
# Qt 通常安装到标准路径，find_package 自动找到
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel

# 如果找不到 Qt，手动指定：
cmake -S . -B build \
    -DCMAKE_PREFIX_PATH="$HOME/Qt/6.8.3/gcc_64/lib/cmake"
```

### Qt Creator（跨平台最简单）

1. Qt Creator → 文件 → 打开文件/项目
2. 选择 `CMakeLists.txt`
3. Kit 自动识别，点击 **Configure Project**
4. 运行

## windeployqt（打包）

Debug 构建后部署 Qt 依赖：

```bash
# Windows
C:\Qt\6.10.0\msvc2022_64\bin\windeployqt.exe build\MediaPlayer.exe --no-translations

# Linux
$ Qt/6.8.3/gcc_64/bin/windeployqt build/MediaPlayer
```

## 快捷键

| 快捷键 | 功能 |
|--------|------|
| `Space` | 播放 / 暂停 |
| `←` / `→` | 快退 / 快进 10 秒 |
| `↑` / `↓` | 音量 ±5% |
| `F11` | 全屏切换 |
| `Ctrl+L` | 显示/隐藏播放列表 |
| `Ctrl+M` | 迷你模式 |
| `Ctrl+O` | 打开文件 |
| `M` | 静音切换 |

## 常见问题

**Q: CMake 找不到 Qt6？**
> 确保安装了 Qt6，并在 cmake 命令中加 `-DCMAKE_PREFIX_PATH` 指向 Qt 的 cmake 目录，例如：
> `C:/Qt/6.10.0/msvc2022_64/lib/cmake`

**Q: MSVC 编译报错 `rc.exe not found`？**
> 确保安装了 "使用 C++ 的桌面开发" 工作负载，且 Windows SDK 正确安装。

---


