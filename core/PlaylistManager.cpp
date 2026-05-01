#include "PlaylistManager.h"
#include <QTime>
#include <QRandomGenerator>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>

PlaylistManager::PlaylistManager(QObject* parent)
    : QObject(parent)
{
}

void PlaylistManager::save(const QString& path)
{
    QJsonArray arr;
    for (const PlaylistItem& item : m_items) {
        QJsonObject obj;
        obj[QStringLiteral("url")] = item.url.toString();
        obj[QStringLiteral("title")] = item.title;
        obj[QStringLiteral("artist")] = item.artist;
        obj[QStringLiteral("album")] = item.album;
        obj[QStringLiteral("duration")] = item.duration;
        arr.append(obj);
    }
    QJsonObject root;
    root[QStringLiteral("currentIndex")] = m_currentIndex;
    root[QStringLiteral("playbackMode")] = m_playbackMode;
    root[QStringLiteral("items")] = arr;
    QFile f(path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
        f.close();
    }
}

bool PlaylistManager::load(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (!doc.isObject()) return false;
    QJsonObject root = doc.object();
    m_items.clear();
    QJsonArray arr = root[QStringLiteral("items")].toArray();
    for (const QJsonValue& v : arr) {
        QJsonObject obj = v.toObject();
        PlaylistItem item;
        item.url = QUrl(obj[QStringLiteral("url")].toString());
        item.title = obj[QStringLiteral("title")].toString();
        item.artist = obj[QStringLiteral("artist")].toString();
        item.album = obj[QStringLiteral("album")].toString();
        item.duration = obj[QStringLiteral("duration")].toInteger();
        m_items.append(item);
    }
    m_currentIndex = root[QStringLiteral("currentIndex")].toInt(-1);
    m_playbackMode = root[QStringLiteral("playbackMode")].toInt(0);
    emit playlistChanged();
    return true;
}

PlaylistManager::~PlaylistManager() = default;

void PlaylistManager::addItem(const QUrl& url, const QString& title)
{
    PlaylistItem item(url, title);
    m_items.append(item);
    emit itemAdded(m_items.size() - 1, item);
    emit playlistChanged();
}

void PlaylistManager::addItems(const QList<QUrl>& urls)
{
    for (const QUrl& url : urls) addItem(url);
}

void PlaylistManager::insertItem(int index, const QUrl& url, const QString& title)
{
    if (index < 0 || index > m_items.size()) return;
    PlaylistItem item(url, title);
    m_items.insert(index, item);
    emit itemAdded(index, item);
    emit playlistChanged();
}

void PlaylistManager::removeItem(int index)
{
    if (index < 0 || index >= m_items.size()) return;
    m_items.removeAt(index);
    emit itemRemoved(index);
    emit playlistChanged();
}

void PlaylistManager::clearPlaylist()
{
    m_items.clear();
    m_currentIndex = -1;
    emit playlistChanged();
}

void PlaylistManager::playIndex(int index)
{
    if (index >= 0 && index < m_items.size()) {
        m_currentIndex = index;
        emit currentIndexChanged(index);
    }
}

void PlaylistManager::playNext()
{
    if (m_items.isEmpty()) return;
    if (m_playbackMode == 2) {
        // Random
        m_currentIndex = QRandomGenerator::global()->bounded(m_items.size());
    } else if (m_playbackMode == 1) {
        // Loop
        m_currentIndex = (m_currentIndex + 1) % m_items.size();
    } else {
        // Sequential
        if (m_currentIndex + 1 < m_items.size())
            ++m_currentIndex;
    }
    emit currentIndexChanged(m_currentIndex);
}

void PlaylistManager::playPrevious()
{
    if (m_items.isEmpty()) return;
    if (m_playbackMode == 2) {
        m_currentIndex = QRandomGenerator::global()->bounded(m_items.size());
    } else if (m_currentIndex > 0) {
        --m_currentIndex;
    }
    emit currentIndexChanged(m_currentIndex);
}

void PlaylistManager::setPlaybackMode(int mode)
{
    m_playbackMode = mode;
}

void PlaylistManager::setCurrentIndex(int index)
{
    if (index >= -1 && index < m_items.size())
        m_currentIndex = index;
}

int PlaylistManager::itemCount() const { return m_items.size(); }

PlaylistItem PlaylistManager::itemAt(int index) const
{
    if (index >= 0 && index < m_items.size()) return m_items[index];
    return {};
}

int PlaylistManager::currentIndex() const { return m_currentIndex; }

QUrl PlaylistManager::currentUrl() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_items.size())
        return m_items.value(m_currentIndex).url;
    return {};
}

int PlaylistManager::playbackMode() const { return m_playbackMode; }

void PlaylistManager::onCurrentIndexChanged(int index)
{
    emit currentIndexChanged(index);
}
