#include "SpectrumWidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QLinearGradient>
#include <QColor>

SpectrumWidget::SpectrumWidget(QWidget* parent)
    : QWidget(parent)
{
    setStyleSheet(QStringLiteral("background:#0d0d1a;"));
    setMinimumHeight(200);
    m_levels.resize(32, 0.0f);
}

SpectrumWidget::~SpectrumWidget() = default;

void SpectrumWidget::setLevels(const QVector<float>& levels)
{
    m_levels = levels;
    update();
}

void SpectrumWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int bars = m_levels.isEmpty() ? 32 : m_levels.size();
    const int gap = 3;
    const int w = bars > 0 ? (width() - (bars + 1) * gap) / bars : 0;
    const int h = height();

    QLinearGradient grad(0, h, 0, 0);
    grad.setColorAt(0.0, QColor(233, 69, 96));
    grad.setColorAt(0.5, QColor(233, 100, 96));
    grad.setColorAt(1.0, QColor(255, 160, 80));

    for (int i = 0; i < bars; ++i) {
        float level = m_levels.isEmpty() ? 0.0f : m_levels[i];
        int barH = int(level * h);
        QRect r(gap + i * (w + gap), h - barH, w, barH);
        p.fillRect(r, grad);
    }

    p.fillRect(0, h - 2, width(), 2, QColor(50, 50, 80));
}
