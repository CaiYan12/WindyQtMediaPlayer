#pragma once
#include <QObject>

class EqualizerManager : public QObject
{
    Q_OBJECT
public:
    explicit EqualizerManager(QObject* parent = nullptr);
    ~EqualizerManager() override;

    void setEnabled(bool enabled);
    bool isEnabled() const;

    void setPreamp(float db);
    void setBandGain(int band, float gain);
    float bandGain(int band) const;

    enum Preset { Flat, Pop, Rock, Jazz, Classical, Bass, Treble };
    void applyPreset(Preset preset);

    QStringList presetNames() const;

signals:
    void enabledChanged(bool enabled);
    void preampChanged(float db);
    void bandGainChanged(int band, float gain);

private:
    bool   m_enabled = false;
    float  m_preamp  = 0.0f;
    QList<float> m_bandGains;
};
