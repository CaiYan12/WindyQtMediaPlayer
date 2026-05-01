#include "EqWidget.h"
#include "EqualizerManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QFrame>
#include <QFont>
#include <QPalette>

// 10-band EQ center frequencies (Hz)
static const int BAND_COUNT = 10;
static const int FREQS[BAND_COUNT] = {
    31, 62, 125, 250, 500, 1000, 2000, 4000, 8000, 16000
};

EqWidget::EqWidget(EqualizerManager* eq, QWidget* parent)
    : QDialog(parent)
    , m_eq(eq)
{
    setWindowTitle(QStringLiteral("EQ 均衡器"));
    setMinimumSize(560, 320);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setupUi();
    syncFromEngine();
}

EqWidget::~EqWidget() = default;

void EqWidget::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    // Top row: enable + preset
    auto* topRow = new QHBoxLayout;
    m_enableCheck = new QCheckBox(QStringLiteral("启用均衡器"), this);
    m_enableCheck->setChecked(m_eq->isEnabled());
    connect(m_enableCheck, &QCheckBox::toggled, this, &EqWidget::onEnableChanged);

    auto* presetLabel = new QLabel(QStringLiteral("预设:"), this);
    m_presetCombo = new QComboBox(this);
    m_presetCombo->addItems(m_eq->presetNames());
    connect(m_presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &EqWidget::onPresetChanged);

    topRow->addWidget(m_enableCheck);
    topRow->addStretch();
    topRow->addWidget(presetLabel);
    topRow->addWidget(m_presetCombo);
    mainLayout->addLayout(topRow);

    // Separator
    auto* sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(sep);

    // Preamp row
    auto* preampRow = new QHBoxLayout;
    m_preampSlider = new QSlider(Qt::Horizontal, this);
    m_preampSlider->setRange(-120, 120); // in 0.1 dB steps
    m_preampSlider->setValue(qRound(m_eq->preamp() * 10));
    m_preampSlider->setMinimumWidth(200);
    connect(m_preampSlider, &QSlider::valueChanged, this, &EqWidget::onPreampChanged);

    m_preampLabel = new QLabel(this);
    m_preampLabel->setMinimumWidth(50);
    m_preampLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    auto* preampTitle = new QLabel(QStringLiteral("预加重"), this);
    preampRow->addWidget(preampTitle);
    preampRow->addWidget(m_preampSlider, 1);
    preampRow->addWidget(m_preampLabel, 0);
    mainLayout->addLayout(preampRow);

    // Band sliders row
    auto* bandsContainer = new QWidget(this);
    auto* bandsLayout = new QHBoxLayout(bandsContainer);
    bandsLayout->setContentsMargins(0, 8, 0, 8);
    bandsLayout->setSpacing(6);

    for (int i = 0; i < BAND_COUNT; ++i) {
        auto* bandWidget = new QWidget(this);
        auto* vLayout = new QVBoxLayout(bandWidget);
        vLayout->setContentsMargins(0, 0, 0, 0);
        vLayout->setSpacing(4);

        // dB label above slider
        auto* dbLabel = new QLabel(QStringLiteral("0 dB"), bandWidget);
        dbLabel->setAlignment(Qt::AlignHCenter);
        dbLabel->setStyleSheet(QStringLiteral("color: #888; font-size: 9pt;"));

        // Vertical slider
        auto* slider = new QSlider(Qt::Vertical, bandWidget);
        slider->setRange(-120, 120); // 0.1 dB steps
        slider->setMinimumHeight(160);
        slider->setTickPosition(QSlider::TicksRight);
        slider->setTickInterval(60);  // 6 dB ticks

        // Band label below
        auto* freqLabel = new QLabel;
        if (FREQS[i] >= 1000) freqLabel->setText(QString::number(FREQS[i] / 1000) + QStringLiteral("k"));
        else freqLabel->setText(QString::number(FREQS[i]));
        freqLabel->setAlignment(Qt::AlignHCenter);
        freqLabel->setStyleSheet(QStringLiteral("color: #aaa; font-size: 9pt;"));

        // Store references
        m_bandSliders.append(slider);
        m_bandLabels.append(dbLabel);

        vLayout->addWidget(dbLabel, 0, Qt::AlignHCenter);
        vLayout->addWidget(slider, 1, Qt::AlignHCenter);
        vLayout->addWidget(freqLabel, 0, Qt::AlignHCenter);

        bandsLayout->addWidget(bandWidget);

        connect(slider, &QSlider::valueChanged,
                this, [this, i](int) { onBandChanged(i); });
    }

    mainLayout->addWidget(bandsContainer, 1);

    refreshSliders();
}

void EqWidget::refreshSliders()
{
    bool enabled = m_eq->isEnabled();

    for (int i = 0; i < BAND_COUNT; ++i) {
        float gain = m_eq->bandGain(i);
        m_bandSliders[i]->blockSignals(true);
        m_bandSliders[i]->setValue(qRound(gain * 10));
        m_bandSliders[i]->setEnabled(enabled);
        m_bandLabels[i]->setText(QString::number(gain, 'f', 1) + QStringLiteral(" dB"));
        m_bandLabels[i]->setStyleSheet(
            enabled ? QStringLiteral("color: #e0e0e0; font-size: 9pt;")
                    : QStringLiteral("color: #555; font-size: 9pt;"));
    }

    m_preampSlider->blockSignals(true);
    m_preampSlider->setValue(qRound(m_eq->preamp() * 10));
    m_preampSlider->setEnabled(enabled);
    float preamp = m_eq->preamp();
    m_preampLabel->setText(QString::number(preamp, 'f', 1) + QStringLiteral(" dB"));
    m_preampSlider->blockSignals(false);
}

void EqWidget::syncFromEngine()
{
    m_preampSlider->blockSignals(true);
    m_preampSlider->setValue(qRound(m_eq->preamp() * 10));
    m_preampSlider->blockSignals(false);
    float preamp = m_eq->preamp();
    m_preampLabel->setText(QString::number(preamp, 'f', 1) + QStringLiteral(" dB"));

    for (int i = 0; i < BAND_COUNT; ++i) {
        m_bandSliders[i]->blockSignals(true);
        m_bandSliders[i]->setValue(qRound(m_eq->bandGain(i) * 10));
        m_bandSliders[i]->blockSignals(false);
        float g = m_eq->bandGain(i);
        m_bandLabels[i]->setText(QString::number(g, 'f', 1) + QStringLiteral(" dB"));
    }
}

void EqWidget::onEnableChanged(bool checked)
{
    m_eq->setEnabled(checked);
    refreshSliders();
}

void EqWidget::onPreampChanged(int value)
{
    float db = value / 10.0f;
    m_eq->setPreamp(db);
    m_preampLabel->setText(QString::number(db, 'f', 1) + QStringLiteral(" dB"));
}

void EqWidget::onBandChanged(int index)
{
    if (index < 0 || index >= BAND_COUNT) return;
    float gain = m_bandSliders[index]->value() / 10.0f;
    m_eq->setBandGain(index, gain);
    m_bandLabels[index]->setText(QString::number(gain, 'f', 1) + QStringLiteral(" dB"));
}

void EqWidget::onPresetChanged(int index)
{
    if (index < 0) return;
    m_eq->applyPreset(static_cast<EqualizerManager::Preset>(index));
    refreshSliders();
}