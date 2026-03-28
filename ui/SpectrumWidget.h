#pragma once
#include <QWidget>
#include <QVector>
#include <QColor>

class SpectrumWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SpectrumWidget(QWidget* parent = nullptr);
    ~SpectrumWidget() override;

    void setLevels(const QVector<float>& levels);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QVector<float>   m_levels;
};
