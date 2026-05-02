#include "MainWindow.h"
#include "MediaEngine.h"
#include "EqualizerManager.h"
#include "PlaylistManager.h"
#include "ControlBar.h"
#include "PlaylistWidget.h"
#include "VideoWidget.h"
#include "SpectrumWidget.h"
#include "EqWidget.h"

#include <QMenuBar>
#include <QActionGroup>
#include <QStatusBar>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QCloseEvent>
#include <QTime>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_engine(new MediaEngine(this))
    , m_eq(new EqualizerManager(this))
    , m_playlist(new PlaylistManager(this))
    , m_seekTimer(new QTimer(this))
{
    setupUi();
    setupMenuBar();
    setupTray();
    setupConnections();
    loadStyleSheet();

    // Playback history persistence
    const QString appDataPath = QFile::decodeName(qgetenv("APPDATA")) + QStringLiteral("/WindyMediaPlayer");
    QDir(appDataPath).mkpath(appDataPath);
    m_playlistPath = appDataPath + QStringLiteral("/playlist.json");
    m_settingsPath = appDataPath + QStringLiteral("/settings.json");

    // Set base title BEFORE load — load() synchronously fires
    // currentIndexChanged -> engine.load() -> metaDataChanged which
    // would overwrite the title before we reach setWindowTitle below.
    m_engineReady = true;
    m_titleRestored = true;
    setWindowTitle(QStringLiteral("\u5a92\u4f53\u64ad\u653e\u5668"));

    m_playlist->load(m_playlistPath);
    m_playlistWidget->updateModeBtnText(m_playlist->playbackMode());
    m_videoWidget->attachToEngine(m_engine);

    // No auto-load on startup — title stays "媒体播放器" until user interaction.
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    m_videoWidget = new VideoWidget(this);
    m_spectrumWidget = new SpectrumWidget(this);

    m_stackWidget = new QStackedWidget;
    m_stackWidget->addWidget(m_videoWidget);
    m_stackWidget->addWidget(m_spectrumWidget);

    m_playlistWidget = new PlaylistWidget(m_playlist, this);
    m_playlistWidget->setMinimumWidth(280);
    m_playlistWidget->setMaximumWidth(380);

    m_controlBar = new ControlBar(this);
    m_controlBar->installEventFilter(this);
    connect(m_controlBar, &ControlBar::seekRequest, this, &MainWindow::onSeek);
    connect(m_controlBar, &ControlBar::playPauseClicked, this, &MainWindow::onPlayPause);
    connect(m_controlBar, &ControlBar::stopClicked, this, &MainWindow::onStop);
    connect(m_controlBar, &ControlBar::volumeChanged, this, &MainWindow::onVolumeChanged);
    connect(m_controlBar, &ControlBar::muteClicked, this, &MainWindow::onMuteToggled);
    connect(m_controlBar, &ControlBar::prevClicked, this, &MainWindow::onPlaylistPrevious);
    connect(m_controlBar, &ControlBar::nextClicked, this, &MainWindow::onPlaylistNext);
    connect(m_controlBar, &ControlBar::fullscreenClicked, this, &MainWindow::onFullscreen);
    connect(m_controlBar, &ControlBar::seeked, this, [this](qint64 ms) {
        m_engine->seek(ms);
        m_seeking = false;
    });

    auto* mainArea = new QVBoxLayout;
    mainArea->setContentsMargins(0, 0, 0, 0);
    mainArea->setSpacing(0);
    mainArea->addWidget(m_stackWidget, 1);
    mainArea->addWidget(m_controlBar);

    auto* center = new QWidget;
    center->setLayout(mainArea);

    auto* h = new QHBoxLayout;
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(2);
    h->addWidget(center, 1);
    h->addWidget(m_playlistWidget);

    auto* central = new QWidget;
    central->setLayout(h);
    setCentralWidget(central);
}

void MainWindow::setupMenuBar()
{
    auto* menuFile = menuBar()->addMenu(QStringLiteral("\u6587\u4ef6(F)"));
    menuFile->addAction(QStringLiteral("\u6253\u5f00\u6587\u4ef6..."), this, &MainWindow::onOpenFile);
    menuFile->actions().last()->setShortcut(Qt::CTRL | Qt::Key_O);
    menuFile->addAction(QStringLiteral("\u6253\u5f00URL..."), this, &MainWindow::onOpenUrl);
    menuFile->actions().last()->setShortcut(Qt::CTRL | Qt::Key_U);
    menuFile->addAction(QStringLiteral("\u6253\u5f00\u6587\u4ef6\u5939..."), this, &MainWindow::onOpenFolder);
    menuFile->actions().last()->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_O);
    menuFile->addSeparator();
    menuFile->addAction(QStringLiteral("\u9000\u51fa"), this, &QWidget::close);
    menuFile->actions().last()->setShortcut(Qt::CTRL | Qt::Key_Q);

    auto* menuPlay = menuBar()->addMenu(QStringLiteral("\u64ad\u653e(P)"));
    menuPlay->addAction(QStringLiteral("\u64ad\u653e/\u6682\u505c"), this, &MainWindow::onPlayPause);
    menuPlay->actions().last()->setShortcut(Qt::Key_Space);
    menuPlay->addAction(QStringLiteral("\u505c\u6b62"), this, &MainWindow::onStop);
    menuPlay->actions().last()->setShortcut(Qt::CTRL | Qt::Key_S);
    menuPlay->addSeparator();
    menuPlay->addAction(QStringLiteral("\u4e0a\u4e00\u9996"), this, &MainWindow::onPlaylistPrevious);
    menuPlay->actions().last()->setShortcut(Qt::CTRL | Qt::Key_Left);
    menuPlay->addAction(QStringLiteral("\u4e0b\u4e00\u9996"), this, &MainWindow::onPlaylistNext);
    menuPlay->actions().last()->setShortcut(Qt::CTRL | Qt::Key_Right);

    m_speedMenu = menuPlay->addMenu(QStringLiteral("\u64ad\u653e\u901f\u5ea6"));
    m_speedGroup = new QActionGroup(this);
    QList<QPair<QString, qreal>> speeds = {
        {QStringLiteral("0.5x"), 0.5},
        {QStringLiteral("0.75x"), 0.75},
        {QStringLiteral("1.0x (\u6b63\u5e38)"), 1.0},
        {QStringLiteral("1.25x"), 1.25},
        {QStringLiteral("1.5x"), 1.5},
        {QStringLiteral("2.0x"), 2.0}
    };
    for (const auto& pair : speeds) {
        auto* a = m_speedMenu->addAction(pair.first, this, [this, pair]() { onSpeedChanged(pair.second); });
        a->setCheckable(true);
        m_speedGroup->addAction(a);
        if (qFuzzyCompare(pair.second, 1.0)) a->setChecked(true);
    }

    menuPlay->addSeparator();
    menuPlay->addAction(QStringLiteral("EQ \u8c03节"), this, &MainWindow::onOpenEq);

    m_modeMenu = menuPlay->addMenu(QStringLiteral("\u64ad\u653e\u6a21\u5f0f"));
    m_modeGroup = new QActionGroup(this);
    QStringList modeNames = {
        QStringLiteral("\u5355\u4e2a\u64ad\u653e"),
        QStringLiteral("\u987a\u5e8f\u64ad\u653e"),
        QStringLiteral("\u5355\u66f2\u5faa\u73af"),
        QStringLiteral("\u5217\u8868\u5faa\u73af"),
        QStringLiteral("\u968f\u673a\u64ad\u653e")
    };
    for (int i = 0; i < modeNames.size(); ++i) {
        auto* a = m_modeMenu->addAction(modeNames[i], this, [this, i]() { onPlaybackModeChanged(i); });
        a->setCheckable(true);
        m_modeGroup->addAction(a);
    }
    // NOTE: do NOT setChecked here — playlist load already synced in setupConnections()

    auto* menuView = menuBar()->addMenu(QStringLiteral("\u89c6\u56fe(V)"));
    m_actFullscreen = menuView->addAction(QStringLiteral("\u5168\u5c4f"), this, &MainWindow::onToggleFullscreen);
    m_actFullscreen->setShortcut(Qt::Key_F11);
    m_actPlaylist = menuView->addAction(QStringLiteral("\u64ad\u653e\u5217\u8868"), this, &MainWindow::onTogglePlaylist);
    m_actPlaylist->setShortcut(Qt::CTRL | Qt::Key_L);
    m_actPlaylist->setCheckable(true);
    m_actPlaylist->setChecked(true);
    m_actMiniMode = menuView->addAction(QStringLiteral("\u8ff7\u4f60\u6a21\u5f0f"), this, &MainWindow::onToggleMiniMode);
    m_actMiniMode->setShortcut(Qt::CTRL | Qt::Key_M);
    m_actMiniMode->setCheckable(true);
    menuView->addSeparator();
    m_actAlwaysOnTop = menuView->addAction(QStringLiteral("\u603b\u5728\u6700\u524d"), this, &MainWindow::onAlwaysOnTop);
    m_actAlwaysOnTop->setCheckable(true);

    auto* menuHelp = menuBar()->addMenu(QStringLiteral("\u5e2e\u52a9(H)"));
    menuHelp->addAction(QStringLiteral("\u5173\u4e8e"), []() {
        QMessageBox::about(nullptr, QStringLiteral("\u5173\u4e8e\u5a92\u4f53\u64ad\u653e\u5668"),
            QStringLiteral("<b>\u5a92\u4f53\u64ad\u653e\u5668 v1.0</b><br>\u57fa\u4e8e Qt6 Multimedia \u6784\u5efa\uff0c\u652f\u6301 MP3/MP4/WAV \u7b49\u4e3b\u6d41\u683c\u5f0f\u3002"));
    });
}

void MainWindow::setupTray()
{
    m_tray = new QSystemTrayIcon(this);
    m_tray->setToolTip(QStringLiteral("\u5a92\u4f53\u64ad\u653e\u5668"));
    connect(m_tray, &QSystemTrayIcon::activated, this, &MainWindow::onTrayActivated);
}

void MainWindow::setupConnections()
{
    connect(m_engine, &MediaEngine::playbackStateChanged, this, &MainWindow::onPlaybackStateChanged);
    connect(m_engine, &MediaEngine::positionChanged, this, &MainWindow::onPositionChanged);
    connect(m_engine, &MediaEngine::durationChanged, this, &MainWindow::onDurationChanged);
    connect(m_engine, &MediaEngine::metaDataChanged, this, &MainWindow::onMetaDataChanged);
    connect(m_engine, &MediaEngine::errorOccurred, this, &MainWindow::onEngineError);

    connect(m_playlist, &PlaylistManager::currentIndexChanged, this, [this](int idx) {
        if (idx >= 0 && idx < m_playlist->itemCount()) {
            auto item = m_playlist->itemAt(idx);
            m_engine->load(item.url);
            m_engine->play();
        }
    });
    connect(m_engine, &MediaEngine::playbackEnded, m_playlist, &PlaylistManager::onEngineStopped);

    connect(m_playlistWidget, &PlaylistWidget::itemDoubleClicked, this, &MainWindow::onPlaylistItemDoubleClicked);
    connect(m_playlistWidget, &PlaylistWidget::nextRequested, this, &MainWindow::onPlaylistNext);
    connect(m_playlistWidget, &PlaylistWidget::prevRequested, this, &MainWindow::onPlaylistPrevious);

    // Sync menu checkmark when mode is changed externally (e.g. playlist widget)
    // Also sync immediately: playlist.load() ran before this, so signal already fired
    connect(m_playlist, &PlaylistManager::playlistChanged, this, [this]() {
        m_modeGroup->actions().at(m_playlist->playbackMode())->setChecked(true);
    });
    m_modeGroup->actions().at(m_playlist->playbackMode())->setChecked(true);

    m_controlBar->setVolume(m_engine->volume());
}

void MainWindow::loadStyleSheet()
{
    setStyleSheet(
        "QMainWindow { background: #1a1a2e; color: #e0e0e0; }"
        "QMenuBar { background: #16213e; color: #e0e0e0; border-bottom: 1px solid #0f3460; }"
        "QMenuBar::item:selected { background: #0f3460; }"
        "QMenu { background: #16213e; color: #e0e0e0; border: 1px solid #0f3460; }"
        "QMenu::item:selected { background: #0f3460; }"
        "QStatusBar { background: #16213e; color: #888; }"
    );
}

void MainWindow::onOpenFile()
{
    QStringList files = QFileDialog::getOpenFileNames(this,
        QStringLiteral("\u6253\u5f00\u5a92\u4f53\u6587\u4ef6"),
        QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).first(),
        QStringLiteral("\u5a92\u4f53\u6587\u4ef6 (*.mp3 *.mp4 *.wav *.flac *.aac *.ogg *.m4a *.mkv *.avi *.wmv *.webm);;"
                      "\u97f3\u9891 (*.mp3 *.wav *.flac *.aac *.ogg *.m4a);;"
                      "\u89c6\u9891 (*.mp4 *.mkv *.avi *.wmv *.webm);;"
                      "\u6240\u6709\u6587\u4ef6 (*.*)"));

    if (files.isEmpty()) return;
    for (const QString& f : files) {
        QUrl url = QUrl::fromLocalFile(QFileInfo(f).absoluteFilePath());
        int existingIndex = -1;
        for (int i = 0; i < m_playlist->itemCount(); ++i) {
            if (m_playlist->itemAt(i).url == url) { existingIndex = i; break; }
        }
        if (existingIndex >= 0) {
            m_playlist->playIndex(existingIndex);
            m_engine->load(url);
            m_engine->play();
        } else {
            m_playlist->addItem(url, QFileInfo(f).fileName());
            m_playlist->playIndex(m_playlist->itemCount() - 1);
            m_engine->load(url);
            m_engine->play();
        }
    }
}

void MainWindow::onOpenUrl()
{
    bool ok = false;
    QString urlStr = QInputDialog::getText(this,
        QStringLiteral("\u6253\u5f00URL"), QStringLiteral("\u8bf7\u8f93\u5165\u5a92\u4f53URL:"),
        QLineEdit::Normal, QString(), &ok);
    if (ok && !urlStr.isEmpty()) {
        QUrl url(urlStr);
        if (url.isValid()) {
            m_playlist->addItem(url, urlStr);
            m_engine->load(url);
            m_engine->play();
        }
    }
}

void MainWindow::onOpenFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this,
        QStringLiteral("\u6253\u5f00\u6587\u4ef6\u5939"));
    if (dir.isEmpty()) return;

    QStringList filters;
    filters << QStringLiteral("*.mp3") << QStringLiteral("*.mp4") << QStringLiteral("*.wav")
            << QStringLiteral("*.flac") << QStringLiteral("*.aac") << QStringLiteral("*.ogg")
            << QStringLiteral("*.m4a") << QStringLiteral("*.mkv") << QStringLiteral("*.avi")
            << QStringLiteral("*.wmv") << QStringLiteral("*.webm");

    QDir qdir(dir);
    QFileInfoList files = qdir.entryInfoList(filters, QDir::Files | QDir::Readable, QDir::Name);
    QList<QUrl> urls;
    for (const QFileInfo& fi : files)
        urls.append(QUrl::fromLocalFile(fi.absoluteFilePath()));
    if (!urls.isEmpty()) {
        m_playlist->addItems(urls);
        m_engine->load(urls.first());
        m_engine->play();
    }
}

void MainWindow::onPlayPause()
{
    switch (m_engine->playbackState()) {
        case QMediaPlayer::PlayingState: m_engine->pause(); break;
        case QMediaPlayer::PausedState:  m_engine->play();  break;
        case QMediaPlayer::StoppedState:
            if (m_playlist->currentIndex() < 0 && m_playlist->itemCount() > 0) {
                m_playlist->playIndex(0);
            } else {
                m_engine->play();
            }
            break;
    }
}

void MainWindow::onStop()
{
    m_engine->stop();
}

void MainWindow::onSeek(int ms)
{
    m_engine->seek(ms);
}

void MainWindow::onVolumeChanged(int vol)
{
    m_engine->setVolume(vol);
    m_controlBar->setVolume(vol);
}

void MainWindow::onMuteToggled()
{
    m_engine->setMuted(!m_engine->isMuted());
}

void MainWindow::onSpeedChanged(qreal speed)
{
    m_engine->setPlaybackRate(speed);
}

void MainWindow::onPlaylistItemDoubleClicked(int index)
{
    m_playlist->playIndex(index);
}

void MainWindow::onPlaylistNext()
{
    m_playlist->playNext();
}

void MainWindow::onPlaylistPrevious()
{
    m_playlist->playPrevious();
}

void MainWindow::onToggleFullscreen()
{
    if (isFullScreen()) { showNormal(); m_actFullscreen->setChecked(false); }
    else { showFullScreen(); m_actFullscreen->setChecked(true); }
}

void MainWindow::onTogglePlaylist()
{
    m_playlistVisible = !m_playlistVisible;
    m_playlistWidget->setVisible(m_playlistVisible);
    m_actPlaylist->setChecked(m_playlistVisible);
}

void MainWindow::onToggleMiniMode()
{
    if (m_miniMode) {
        resize(900, 600);
        m_miniMode = false;
        m_actMiniMode->setChecked(false);
    } else {
        resize(400, 120);
        m_miniMode = true;
        m_actMiniMode->setChecked(true);
    }
}

void MainWindow::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        show();
        raise();
        activateWindow();
    }
}

void MainWindow::onAlwaysOnTop(bool checked)
{
    Qt::WindowFlags flags = windowFlags();
    if (checked) setWindowFlags(flags | Qt::WindowStaysOnTopHint);
    else         setWindowFlags(flags & ~Qt::WindowStaysOnTopHint);
    show();
}

void MainWindow::onPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    updatePlayPauseButton();
    QString msg;
    switch (state) {
        case QMediaPlayer::PlayingState: msg = QStringLiteral("\u25b6 \u64ad\u653e\u4e2d"); break;
        case QMediaPlayer::PausedState:  msg = QStringLiteral("\u23f8 \u5df2\u6682\u505c"); break;
        case QMediaPlayer::StoppedState: msg = QStringLiteral("\u23f9 \u5df2\u505c\u6b62"); break;
    }
    statusBar()->showMessage(msg, 3000);
}

void MainWindow::onPositionChanged(qint64 pos)
{
    if (!m_seeking) {
        m_controlBar->setPosition(pos);
        updateTimeLabel();
    }
}

void MainWindow::onDurationChanged(qint64 dur)
{
    m_controlBar->setDuration(dur);
}

void MainWindow::onMetaDataChanged()
{
    if (m_titleRestored) updateWindowTitle();
}

void MainWindow::onFullscreen()
{
    if (isFullScreen()) { showNormal(); m_actFullscreen->setChecked(false); }
    else { showFullScreen(); m_actFullscreen->setChecked(true); }
}

void MainWindow::onOpenEq()
{
    EqWidget eq(m_eq, this);
    eq.setWindowModality(Qt::ApplicationModal);
    eq.exec();
}
void MainWindow::updateTimeLabel()
{
    // handled by ControlBar
}
void MainWindow::onEngineError(const QString& err)
{
    QMessageBox::warning(this, QStringLiteral("\u64ad\u653e\u9519\u8bef"), err);
}

void MainWindow::updateWindowTitle()
{
    QString title = m_engine->title();
    if (title.isEmpty()) title = m_engine->currentFileName();
    if (!title.isEmpty()) setWindowTitle(title + QStringLiteral(" \u2014 \u5a92\u4f53\u64ad\u653e\u5668"));
    else setWindowTitle(QStringLiteral("\u5a92\u4f53\u64ad\u653e\u5668"));
}

void MainWindow::updatePlayPauseButton()
{
    // Play/pause button icon is managed by ControlBar internally
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        auto* ke = static_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_Escape && ke->modifiers() == Qt::NoModifier) {
            if (isFullScreen()) {
                showNormal();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    qDebug() << "[DEBUG closeEvent] playbackMode:" << m_playlist->playbackMode() << "path:" << m_playlistPath;
    m_playlist->save(m_playlistPath);
    event->accept();
}

void MainWindow::onPlaybackModeChanged(int mode)
{
    qDebug() << "[DEBUG onPlaybackModeChanged] mode:" << mode;
    m_playlist->setPlaybackMode(static_cast<PlaylistManager::PlaybackMode>(mode));
    m_playlistWidget->updateModeBtnText(mode);
}
