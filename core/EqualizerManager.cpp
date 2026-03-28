#include "EqualizerManager.h"

EqualizerManager::EqualizerManager(QObject* parent)
    : QObject(parent)
{
    m_bandGains.resize(10, 0.0f);
}

EqualizerManager::~EqualizerManager() = default;

void EqualizerManager::setEnabled(bool enabled)
{
    m_enabled = enabled;
    emit enabledChanged(enabled);
}

bool EqualizerManager::isEnabled() const
{
    return m_enabled;
}

void EqualizerManager::setPreamp(float db)
{
    m_preamp = qBound(-12.0f, db, 12.0f);
    emit preampChanged(m_preamp);
}

void EqualizerManager::setBandGain(int band, float gain)
{
    if (band >= 0 && band < m_bandGains.size()) {
        m_bandGains[band] = qBound(-12.0f, gain, 12.0f);
        emit bandGainChanged(band, m_bandGains[band]);
    }
}

float EqualizerManager::bandGain(int band) const
{
    if (band >= 0 && band < m_bandGains.size()) return m_bandGains[band];
    return 0.0f;
}

void EqualizerManager::applyPreset(Preset preset)
{
    static const QList<QPair<int, float>> presets[] = {
        {},                                                        // Flat
        {{3, 3}, {4, 5}, {5, 4}},                                // Pop
        {{1, 4}, {2, 3}, {3, -2}, {5, 5}, {6, 4}},              // Rock
        {{0, 3}, {2, 2}, {4, 3}, {6, 2}},                        // Jazz
        {{0, 4}, {3, 5}, {5, 4}, {7, 3}},                        // Classical
        {{0, 6}, {1, 5}, {2, 3}},                                 // Bass
        {{6, 4}, {7, 5}, {8, 6}}                                 // Treble
    };
    const auto& p = presets[preset];
    for (const auto& pair : p) setBandGain(pair.first, pair.second);
}

QStringList EqualizerManager::presetNames() const
{
    return QStringList() << QStringLiteral("平坦") << QStringLiteral("流行")
                         << QStringLiteral("摇滚") << QStringLiteral("爵士")
                         << QStringLiteral("古典") << QStringLiteral("低音")
                         << QStringLiteral("高音");
}
