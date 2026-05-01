# Windy 媒体播放器 (MediaPlayer)

基于 **Qt 6.10 MSVC2022** 的高性能本地/网络媒体播放器，采用 **Windows 11 Fluent Design** 设计风格。

## 支持格式

| 类型 | 格式 |
|------|------|
| 音频 | MP3, WAV, FLAC, AAC, OGG, M4A |
| 视频 | MP4, MKV, AVI, WMV, WebM |

## 环境要求

- **Qt 6.10** (MSVC2022 64-bit)
- **CMake 3.20+** + **Ninja**
- **Visual Studio 2022** (MSVC, C++17)
- **Windows 10/11**

## 项目结构

```
MediaPlayer/
├── CMakeLists.txt        # CMake 构建配置
├── main.cpp              # 程序入口
├── README.md
├── core/                 # 核心业务层（无 UI 依赖）
│   ├── MediaEngine       # QMediaPlayer 封装、播放控制
│   ├── PlaylistManager   # 播放列表管理
│   └── EqualizerManager  # 均衡器
├── ui/                   # UI 层
│   ├── MainWindow        # 主窗口
│   ├── ControlBar        # 底部控制栏
│   ├── PlaylistWidget    # 播放列表面板
│   ├── VideoWidget       # 视频渲染
│   └── SpectrumWidget    # 音频频谱可视化
├── res/                  # 资源文件
│   ├── fonts/            # 字体
│   ├── icons/            # 图标
│   ├── styles/           # QSS 样式表
│   └── resources.qrc
├── scripts/              # 构建/工具脚本
│   ├── build.bat         # CMD 构建
│   ├── build-ps.ps1      # PowerShell 构建（Debug）
│   ├── build-release.ps1 # Release 构建 + 打包
│   ├── setup-msvc.bat    # MSVC 环境初始化 (CMD)
│   └── setup-msvc.ps1    # MSVC 环境初始化 (PS)
├── build/                # Debug 构建产物
├── build-release/        # Release 构建产物
├── release/              # 打包发布文件
└── .vscode/              # VS Code 配置
    ├── tasks.json        # Ctrl+Shift+B 任务
    ├── launch.json       # F5 调试启动
    └── c_cpp_properties.json
```

## 构建步骤

### VS Code（推荐）

```bash
# Ctrl+Shift+B → 选择 "2. Build Debug"
# 或 F5 直接调试运行
```

所有构建任务通过 `.vscode/tasks.json` 定义，自动检测 MSVC 环境。也可手动执行脚本：

```bash
# PowerShell 构建 + 运行
pwsh -NoProfile -File scripts/build-ps.ps1        # Debug 构建
pwsh -NoProfile -File scripts/build-ps.ps1 run    # 构建后运行
pwsh -NoProfile -File scripts/build-ps.ps1 release # Release 构建
```

### 命令行 (Ninja)

```bash
# 配置
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug ^
    -DQt6_DIR="C:/Qt/6.10.0/msvc2022_64/lib/cmake/Qt6"

# 编译
cmake --build build --config Debug --parallel
```

### Qt Creator

1. Qt Creator → 文件 → 打开文件/项目
2. 选择 `CMakeLists.txt`
3. Kit 自动识别，点击 **Configure Project**
4. 运行

## windeployqt（打包）

```bash
C:\Qt\6.10.0\msvc2022_64\bin\windeployqt.exe build\MediaPlayer.exe --no-translations
```

## 快捷键

| 快捷键 | 功能 |
|--------|------|
| `Space` | 播放 / 暂停 |
| `Ctrl+S` | 停止 |
| `←` / `→` | 快退 / 快进 10 秒 |
| `↑` / `↓` | 音量 ±5% |
| `Ctrl+←` / `Ctrl+→` | 上一首 / 下一首 |
| `F11` | 全屏切换 |
| `Ctrl+L` | 显示/隐藏播放列表 |
| `Ctrl+M` | 迷你模式 |
| `Ctrl+O` | 打开文件 |
| `Ctrl+U` | 打开 URL |
| `Ctrl+Q` | 退出 |
| `M` | 静音切换 |

## 设计

- **主题**：Windows 11 Fluent Design 暗色主题
- **配色**：深色表面 `#202020`，辅色 `#2D2D2D`，强调色 `#60CDFF`
- **图标**：调用 Windows 系统图标 (`QStyle::standardIcon`)，确保与系统外观一致
- **圆角**：控件 4px，面板 8px

## 常见问题

**Q: CMake 找不到 Qt6？**
> 通过 `-DQt6_DIR=C:/Qt/6.10.0/msvc2022_64/lib/cmake/Qt6` 指定 Qt6 CMake 模块路径。

**Q: MSVC 编译报错 `rc.exe not found`？**
> 确保安装了"使用 C++ 的桌面开发"工作负载，SDK 完整。

**Q: F5 调试报 Invalid cwd？**
> 检查 `launch.json` 中 `program` 和 `cwd` 的路径指向 `build/`（而非 `build/Debug/`）。
