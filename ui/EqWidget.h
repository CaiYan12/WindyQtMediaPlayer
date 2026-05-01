#pragma once
#include <QDialog>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>

class EqualizerManager;

class EqWidget : public QDialog
{
    Q_OBJECT

public:
    explicit EqWidget(EqualizerManager* eq, QWidget* parent = nullptr);
    ~EqWidget() override;

private:
    void setupUi();
    void syncFromEngine();
    void syncToEngine();
    void refreshSliders();

    EqualizerManager* m_eq;
    QCheckBox* m_enableCheck = nullptr;
    QSlider* m_preampSlider = nullptr;
    QLabel* m_preampLabel = nullptr;
    QList<QSlider*> m_bandSliders;
    QList<QLabel*> m_bandLabels;
    QComboBox* m_presetCombo = nullptr;

private slots:
    void onEnableChanged(bool checked);
    void onPreampChanged(int value);
    void onBandChanged(int index);
    void onPresetChanged(int index);
};