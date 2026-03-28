# 媒体播放器 (MediaPlayer)

基于 **Qt6 Multimedia** 的高性能本地/网络媒体播放器，界面风格参考 QQ 影音。

## 支持格式

| 类型 | 格式 |
|------|------|
| 音频 | MP3, WAV, FLAC, AAC, OGG, M4A |
| 视频 | MP4, MKV, AVI, WMV, WebM |

## 项目结构

```
MediaPlayer/
├── CMakeLists.txt           # CMake 构建配置
├── build-ps.ps1             # 构建脚本（Ctrl+Shift+B）
├── main.cpp                 # 程序入口
├── core/                    # 核心业务层
│   ├── MediaEngine.h/cpp    # 媒体播放引擎（QMediaPlayer 封装）
│   ├── PlaylistManager.h/cpp # 播放列表管理
│   └── EqualizerManager.h/cpp # 均衡器
├── ui/                      # UI 层
│   ├── MainWindow.h/cpp/.ui # 主窗口
│   ├── ControlBar.h/cpp     # 底部播放控制栏
│   ├── PlaylistWidget.h/cpp  # 播放列表面板
│   ├── VideoWidget.h/cpp    # 视频显示窗口
│   └── SpectrumWidget.h/cpp  # 音频频谱显示
└── res/
    └── resources.qrc        # Qt 资源
```

## 快速开始

### 构建

```bash
# 在 VS Code 中
Ctrl+Shift+B   # 编译 + 自动部署 Qt DLL
```

或手动：

```powershell
pwsh -File build-ps.ps1        # Debug 构建
pwsh -File build-ps.ps1 full   # 清理 + 重构建
pwsh -File build-ps.ps1 run    # 运行
```

### 运行

```
pwsh -File build-ps.ps1 run
```

可执行文件位于：`build/MediaPlayer.exe`

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
| `Ctrl+U` | 打开 URL |
| `M` | 静音切换 |

## 环境要求

- Qt 6.10.0 (MSVC 2022 64-bit)
- Visual Studio 2022 Community
- CMake 3.20+
- Ninja Build

## 开发说明

- **core/** 层不依赖任何 UI，可单独测试
- **ui/** 层通过信号槽与 core 层通信
- 编译后自动调用 `windeployqt` 部署 Qt 依赖到 `build/`
- `build/` 目录已加入 `.gitignore`，无需提交

## 后续扩展方向

- [ ] 拖放文件到窗口打开
- [ ] 均衡器面板（10 段 EQ）
- [ ] 字幕加载（ASS/SRT）
- [ ] 截图功能
- [ ] 播放历史记录
- [ ] 窗口尺寸记忆

---

*基于 Qt6 + C++17 构建 | OpenClaw AI 助手生成*
