#pragma once
#include <QObject>
#include <QFileInfo>
#include <QUrl>

struct PlaylistItem
{
    QUrl    url;
    QString title;
    QString artist;
    QString album;
    qint64  duration = 0;

    PlaylistItem() = default;
    explicit PlaylistItem(const QUrl& u, const QString& t = QString())
        : url(u), title(t.isEmpty() ? QFileInfo(u.toString()).fileName() : t) {}
};

class PlaylistManager : public QObject
{
    Q_OBJECT

public:
    explicit PlaylistManager(QObject* parent = nullptr);
    ~PlaylistManager() override;

    enum PlaybackMode { Single = 0, Sequential = 1, SingleLoop = 2, ListLoop = 3, Random = 4 };

    void addItem(const QUrl& url, const QString& title = QString());
    void addItems(const QList<QUrl>& urls);
    void insertItem(int index, const QUrl& url, const QString& title = QString());
    void removeItem(int index);
    void clearPlaylist();

    void playIndex(int index);
    void playNext();
    void playPrevious();
    void setPlaybackMode(PlaybackMode mode);
    void setCurrentIndex(int index);
    void onEngineStopped(); // called when engine reaches end of media

    int itemCount() const;
    PlaylistItem itemAt(int index) const;
    int currentIndex() const;
    QUrl currentUrl() const;
    PlaybackMode playbackMode() const;
    QString playbackModeName() const;

    void save(const QString& path);
    bool load(const QString& path);

signals:
    void currentIndexChanged(int index);
    void playlistChanged();
    void itemAdded(int index, const PlaylistItem& item);
    void itemRemoved(int index);

private slots:
    void onCurrentIndexChanged(int index);

private:
    QList<PlaylistItem> m_items;
    int                 m_currentIndex = -1;
    PlaybackMode       m_playbackMode = Sequential;
};
