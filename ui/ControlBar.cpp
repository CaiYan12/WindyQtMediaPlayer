#include "ControlBar.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QFont>

ControlBar::ControlBar(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

ControlBar::~ControlBar() = default;

void ControlBar::setupUi()
{
    setMinimumHeight(90);
    setObjectName(QStringLiteral("controlBarWidget"));

    // Seek slider + time labels
    m_seekSlider = new QSlider(Qt::Horizontal, this);
    m_seekSlider->setRange(0, 0);
    m_seekSlider->setMinimumHeight(20);
    connect(m_seekSlider, &QSlider::sliderMoved, this, [this](int val) {
        emit seekRequest(val);
    });
    connect(m_seekSlider, &QSlider::sliderPressed, this, [this]() {
        emit seeked(m_seekSlider->value());
    });

    m_currentTimeLabel = new QLabel(QStringLiteral("00:00"), this);
    m_totalTimeLabel = new QLabel(QStringLiteral("00:00"), this);
    QFont mono(QStringLiteral("Consolas"), 9);
    m_currentTimeLabel->setFont(mono);
    m_totalTimeLabel->setFont(mono);

    // Transport buttons - use plain text icons for reliability
    m_prevBtn      = new QPushButton("<|", this);
    m_playPauseBtn = new QPushButton("▶", this);
    m_stopBtn      = new QPushButton("■", this);
    m_nextBtn      = new QPushButton("|>", this);

    m_prevBtn->setObjectName(QStringLiteral("prevBtn"));
    m_playPauseBtn->setObjectName(QStringLiteral("playPauseBtn"));
    m_stopBtn->setObjectName(QStringLiteral("stopBtn"));
    m_nextBtn->setObjectName(QStringLiteral("nextBtn"));

    m_prevBtn->setFixedSize(36, 36);
    m_playPauseBtn->setFixedSize(36, 36);
    m_stopBtn->setFixedSize(36, 36);
    m_nextBtn->setFixedSize(36, 36);

    QFont btnFont(QStringLiteral("Segoe UI Symbol"), 11);
    m_prevBtn->setFont(btnFont);
    m_playPauseBtn->setFont(btnFont);
    m_stopBtn->setFont(btnFont);
    m_nextBtn->setFont(btnFont);

    connect(m_prevBtn,      &QPushButton::clicked, this, &ControlBar::prevClicked);
    connect(m_playPauseBtn, &QPushButton::clicked, this, &ControlBar::playPauseClicked);
    connect(m_stopBtn,      &QPushButton::clicked, this, &ControlBar::stopClicked);
    connect(m_nextBtn,      &QPushButton::clicked, this, &ControlBar::nextClicked);

    // Volume
    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(80);
    m_volumeSlider->setFixedWidth(90);
    connect(m_volumeSlider, &QSlider::valueChanged, this, &ControlBar::volumeChanged);

    m_volumeBtn = new QPushButton("Vol", this);
    m_volumeBtn->setFixedSize(36, 36);
    connect(m_volumeBtn, &QPushButton::clicked, this, &ControlBar::muteClicked);

    m_fullscreenBtn = new QPushButton("⛶", this);
    m_fullscreenBtn->setFixedSize(30, 30);
    m_fullscreenBtn->setFont(btnFont);

    // Layout: seek row
    auto* seekRow = new QHBoxLayout;
    seekRow->setContentsMargins(12, 0, 12, 0);
    seekRow->addWidget(m_currentTimeLabel);
    seekRow->addWidget(m_seekSlider, 1);
    seekRow->addWidget(m_totalTimeLabel);

    // Layout: button row
    auto* btnRow = new QHBoxLayout;
    btnRow->setContentsMargins(10, 4, 10, 6);
    btnRow->addStretch();
    btnRow->addWidget(m_prevBtn);
    btnRow->addWidget(m_playPauseBtn);
    btnRow->addWidget(m_stopBtn);
    btnRow->addWidget(m_nextBtn);
    btnRow->addSpacing(20);
    btnRow->addWidget(m_volumeBtn);
    btnRow->addWidget(m_volumeSlider);
    btnRow->addSpacing(8);
    btnRow->addWidget(m_fullscreenBtn);
    btnRow->addStretch();

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 4, 0, 4);
    layout->setSpacing(2);
    layout->addLayout(seekRow);
    layout->addLayout(btnRow);
}

void ControlBar::setPlaying(bool playing)
{
    m_playPauseBtn->setText(playing ? "❚❚" : "▶");
}

void ControlBar::setPosition(qint64 ms)
{
    m_seekSlider->setValue(static_cast<int>(qMax(0LL, ms)));
}

void ControlBar::setDuration(qint64 ms)
{
    m_seekSlider->setRange(0, static_cast<int>(qMax(0LL, ms)));
}

void ControlBar::setVolume(int volume)
{
    m_volumeSlider->setValue(volume);
    m_volumeBtn->setText(volume == 0 ? "X" : "Vol");
}

void ControlBar::setTimeLabels(const QString& current, const QString& total)
{
    m_currentTimeLabel->setText(current);
    m_totalTimeLabel->setText(total);
}
