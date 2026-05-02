#pragma once
#include <QMainWindow>
#include <QMediaPlayer>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QMenu>
#include <QActionGroup>
#include <QAbstractItemView>
#include <QStackedWidget>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QDir>

class MediaEngine;
class EqualizerManager;
class PlaylistManager;
class ControlBar;
class PlaylistWidget;
class VideoWidget;
class SpectrumWidget;
class EqWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onOpenFile();
    void onOpenUrl();
    void onOpenFolder();
    void onPlayPause();
    void onStop();
    void onSeek(int ms);
    void onVolumeChanged(int vol);
    void onMuteToggled();
    void onSpeedChanged(qreal speed);
    void onPlaylistItemDoubleClicked(int index);
    void onPlaylistNext();
    void onPlaylistPrevious();
    void onFullscreen();
    void onToggleFullscreen();
    void onTogglePlaylist();
    void onToggleMiniMode();
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void onAlwaysOnTop(bool checked);
    void onOpenEq();
    void onPlaybackModeChanged(int mode);
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
    void onPositionChanged(qint64 pos);
    void onDurationChanged(qint64 dur);
    void onMetaDataChanged();
    void onEngineError(const QString& err);

private:
    void setupUi();
    void setupMenuBar();
    void setupTray();
    void setupConnections();
    void loadStyleSheet();
    void updateWindowTitle();
    void updatePlayPauseButton();
    void updateTimeLabel();
    QString formatTime(qint64 ms) const;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

    MediaEngine*       m_engine = nullptr;
    EqualizerManager*  m_eq = nullptr;
    PlaylistManager*    m_playlist = nullptr;

    ControlBar*         m_controlBar = nullptr;
    PlaylistWidget*     m_playlistWidget = nullptr;
    VideoWidget*        m_videoWidget = nullptr;
    SpectrumWidget*     m_spectrumWidget = nullptr;

    QWidget*            m_centerWidget = nullptr;
    QStackedWidget*     m_stackWidget = nullptr;

    QTimer*             m_seekTimer = nullptr;
    bool                m_seeking = false;
    bool                m_playlistVisible = true;
    bool                m_miniMode = false;
    QSystemTrayIcon*    m_tray = nullptr;
    QMenu*              m_speedMenu = nullptr;
    QActionGroup*       m_speedGroup = nullptr;
    QMenu*              m_modeMenu = nullptr;
    QActionGroup*       m_modeGroup = nullptr;
    bool                m_titleRestored = false;
    bool                m_engineReady = false;
    QString              m_playlistPath;
    QString              m_settingsPath;
    QAction*             m_actFullscreen = nullptr;
    QAction*             m_actPlaylist = nullptr;
    QAction*             m_actMiniMode = nullptr;
    QAction*             m_actAlwaysOnTop = nullptr;
    EqWidget*            m_eqWidget = nullptr;
};