#include "PlaylistManager.h"
#include <QTime>
#include <QRandomGenerator>

PlaylistManager::PlaylistManager(QObject* parent)
    : QObject(parent)
{
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
