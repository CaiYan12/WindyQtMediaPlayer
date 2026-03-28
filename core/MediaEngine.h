#pragma once
#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFileInfo>
#include <QPixmap>
#include <QImage>

class MediaEngine : public QObject
{
    Q_OBJECT

public:
    explicit MediaEngine(QObject* parent = nullptr);
    ~MediaEngine() override;

    // Playback
    void  load(const QUrl& url);
    void  load(const QString& path);
    void  play();
    void  pause();
    void  stop();
    void  seek(qint64 ms);
    void  setVolume(int volume);         // 0-100
    void  setPlaybackRate(qreal rate);  // 0.25-4.0
    void  setMuted(bool muted);

    // Getters
    QMediaPlayer::PlaybackState playbackState() const;
    qint64  duration() const;
    qint64  position() const;
    int     volume() const;
    bool    isMuted() const;
    qreal   playbackRate() const;
    QUrl    currentUrl() const;
    QString currentFileName() const;
    QString currentFilePath() const;

    // Meta
    QString title() const;
    QString artist() const;
    QString album() const;
    QPixmap coverArt() const;
    bool    isSeekable() const;
    bool    hasVideo() const;

    // Public access to raw player
    QMediaPlayer* player() const { return m_player; }
    QAudioOutput* audioOutput() const { return m_audioOutput; }

signals:
    void playbackStateChanged(QMediaPlayer::PlaybackState state);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void volumeChanged(int volume);
    void mutedChanged(bool muted);
    void seekableChanged(bool seekable);
    void metaDataChanged();
    void errorOccurred(const QString& errorString);
    void bufferProgressChanged(float progress);

private slots:
    void onPlayerStateChanged(QMediaPlayer::PlaybackState state);
    void onPlayerError(QMediaPlayer::Error error);
    void onVolumeChanged(float volume);

private:
    void initConnections();

    QMediaPlayer* m_player = nullptr;
    QAudioOutput* m_audioOutput = nullptr;
    qint64        m_duration = 0;
};
