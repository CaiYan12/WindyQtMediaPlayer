#include "MediaEngine.h"
#include <QMediaMetaData>
#include <QDebug>

MediaEngine::MediaEngine(QObject* parent)
    : QObject(parent)
    , m_player(new QMediaPlayer(this))
    , m_audioOutput(new QAudioOutput(this))
    , m_userStopped(false)
{
    m_player->setAudioOutput(m_audioOutput);
    initConnections();
}

MediaEngine::~MediaEngine() = default;

void MediaEngine::initConnections()
{
    connect(m_player, &QMediaPlayer::playbackStateChanged,
            this, &MediaEngine::onPlayerStateChanged);
    connect(m_player, &QMediaPlayer::positionChanged,
            this, &MediaEngine::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged,
            this, [this](qint64 d) { m_duration = d; emit durationChanged(d); });

    // volumeChanged lives on QAudioOutput
    connect(m_audioOutput, &QAudioOutput::volumeChanged,
            this, &MediaEngine::onVolumeChanged);
    connect(m_audioOutput, &QAudioOutput::mutedChanged,
            this, &MediaEngine::mutedChanged);

    connect(m_player, &QMediaPlayer::seekableChanged,
            this, &MediaEngine::seekableChanged);
    connect(m_player, &QMediaPlayer::metaDataChanged,
            this, &MediaEngine::metaDataChanged);
    connect(m_player, &QMediaPlayer::bufferProgressChanged,
            this, &MediaEngine::bufferProgressChanged);
    connect(m_player, &QMediaPlayer::errorOccurred,
            this, &MediaEngine::onPlayerError);
}

void MediaEngine::onPlayerStateChanged(QMediaPlayer::PlaybackState state)
{
    emit playbackStateChanged(state);
    // Detect natural end-of-media stop (vs user-initiated stop)
    if (state == QMediaPlayer::StoppedState && !m_userStopped)
        emit playbackEnded();
}

void MediaEngine::onVolumeChanged(float volume)
{
    emit volumeChanged(qRound(volume * 100));
}

void MediaEngine::onPlayerError(QMediaPlayer::Error error)
{
    QString msg;
    switch (error) {
        case QMediaPlayer::NoError:            msg = QString(); break;
        case QMediaPlayer::ResourceError:      msg = QStringLiteral("\u8d44\u6e90\u52a0\u8f7d\u5931\u8d25"); break;
        case QMediaPlayer::FormatError:         msg = QStringLiteral("\u4e0d\u652f\u6301\u7684\u5a92\u4f53\u683c\u5f0f"); break;
        case QMediaPlayer::NetworkError:        msg = QStringLiteral("\u7f51\u7edc\u9519\u8bef"); break;
        case QMediaPlayer::AccessDeniedError:   msg = QStringLiteral("\u8bbf\u95ee\u88ab\u62d2\u7edd"); break;
        default:                               msg = QStringLiteral("\u672a\u77e5\u9519\u8bef"); break;
    }
    if (!msg.isEmpty()) {
        qWarning() << "[MediaEngine]" << msg;
        emit errorOccurred(msg);
    }
}

void MediaEngine::load(const QUrl& url)
{
    m_userStopped = false;
    m_player->setSource(url);
}

void MediaEngine::load(const QString& path)
{
    load(QUrl::fromLocalFile(path));
}

void MediaEngine::play()
{
    m_player->play();
}

void MediaEngine::pause()
{
    m_player->pause();
}

void MediaEngine::stop()
{
    m_userStopped = true;
    m_player->stop();
}

void MediaEngine::seek(qint64 ms)
{
    m_player->setPosition(ms);
}

void MediaEngine::setVolume(int volume)
{
    m_audioOutput->setVolume(volume / 100.0f);
}

void MediaEngine::setPlaybackRate(qreal rate)
{
    m_player->setPlaybackRate(rate);
}

void MediaEngine::setMuted(bool muted)
{
    m_audioOutput->setMuted(muted);
}

QMediaPlayer::PlaybackState MediaEngine::playbackState() const
{
    return m_player->playbackState();
}

qint64 MediaEngine::duration() const
{
    return m_duration;
}

qint64 MediaEngine::position() const
{
    return m_player->position();
}

int MediaEngine::volume() const
{
    return qRound(m_audioOutput->volume() * 100);
}

bool MediaEngine::isMuted() const
{
    return m_audioOutput->isMuted();
}

qreal MediaEngine::playbackRate() const
{
    return m_player->playbackRate();
}

QUrl MediaEngine::currentUrl() const
{
    return m_player->source();
}

QString MediaEngine::currentFileName() const
{
    QUrl url = currentUrl();
    if (url.isLocalFile())
        return QFileInfo(url.toLocalFile()).fileName();
    return url.fileName();
}

QString MediaEngine::currentFilePath() const
{
    QUrl url = currentUrl();
    return url.isLocalFile() ? url.toLocalFile() : QString();
}

QString MediaEngine::title() const
{
    return m_player->metaData().stringValue(QMediaMetaData::Title);
}

QString MediaEngine::artist() const
{
    return m_player->metaData().stringValue(QMediaMetaData::Author);
}

QString MediaEngine::album() const
{
    return m_player->metaData().stringValue(QMediaMetaData::AlbumTitle);
}

QPixmap MediaEngine::coverArt() const
{
    QVariant cover = m_player->metaData().value(QMediaMetaData::CoverArtImage);
    if (cover.canConvert<QPixmap>())
        return cover.value<QPixmap>();
    return QPixmap();
}

bool MediaEngine::isSeekable() const
{
    return m_player->isSeekable();
}

bool MediaEngine::hasVideo() const
{
    return m_player->videoOutput() != nullptr;
}
