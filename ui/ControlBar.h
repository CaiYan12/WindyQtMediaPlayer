#pragma once
#include <QWidget>
#include <qmediaplayer.h>
#include <QSlider>
#include <QLabel>
#include <QPushButton>

class ControlBar : public QWidget
{
    Q_OBJECT

public:
    explicit ControlBar(QWidget* parent = nullptr);
    ~ControlBar() override;

    void setPlaying(bool playing);
    void setPosition(qint64 ms);
    void setDuration(qint64 ms);
    void setVolume(int volume);
    void setTimeLabels(const QString& current, const QString& total);

signals:
    void playPauseClicked();
    void stopClicked();
    void seekRequest(qint64 ms);
    void seeked(qint64 ms);
    void volumeChanged(int volume);
    void muteClicked();
    void prevClicked();
    void nextClicked();

private:
    void setupUi();

    QPushButton*  m_prevBtn = nullptr;
    QPushButton*  m_playPauseBtn = nullptr;
    QPushButton*  m_stopBtn = nullptr;
    QPushButton*  m_nextBtn = nullptr;
    QPushButton*  m_volumeBtn = nullptr;
    QPushButton*  m_fullscreenBtn = nullptr;

    QSlider*      m_seekSlider = nullptr;
    QSlider*      m_volumeSlider = nullptr;

    QLabel*       m_currentTimeLabel = nullptr;
    QLabel*       m_totalTimeLabel = nullptr;

    qint64        m_duration = 0;
};
