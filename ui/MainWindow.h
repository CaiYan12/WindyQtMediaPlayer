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

class MediaEngine;
class PlaylistManager;
class ControlBar;
class PlaylistWidget;
class VideoWidget;
class SpectrumWidget;

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
    void onToggleFullscreen();
    void onTogglePlaylist();
    void onToggleMiniMode();
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void onAlwaysOnTop(bool checked);
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
};
