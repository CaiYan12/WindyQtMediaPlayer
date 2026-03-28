#include "VideoWidget.h"
#include "MediaEngine.h"
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QEvent>

VideoWidget::VideoWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setStyleSheet(QStringLiteral("background:#0d0d1a;"));

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto* placeholder = new QLabel(
        QStringLiteral("[ Audio Mode ]\nDrag files here or use File -> Open to load media"),
        this);
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet(
        QStringLiteral("color:#555; font-size:16px; font-family:'Microsoft YaHei UI';"));
    layout->addWidget(placeholder, 1, Qt::AlignHCenter | Qt::AlignVCenter);
}

VideoWidget::~VideoWidget() = default;

void VideoWidget::attachToEngine(MediaEngine* engine)
{
    if (!engine || m_engine) return;
    m_engine = engine;

    // Remove placeholder
    QLayoutItem* item = layout()->takeAt(0);
    if (item) {
        delete item->widget();
        delete item;
    }

    auto* videoWidget = new QVideoWidget(this);
    videoWidget->setAttribute(Qt::WA_OpaquePaintEvent);
    videoWidget->setAttribute(Qt::WA_NoSystemBackground);
    videoWidget->setStyleSheet(QStringLiteral("border:none; background:#0d0d1a;"));
    videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);

    layout()->addWidget(videoWidget);
    videoWidget->show();

    QMediaPlayer* player = engine->player();
    if (player) {
        player->setVideoOutput(videoWidget);
    }
}

bool VideoWidget::event(QEvent* event)
{
    return QWidget::event(event);
}
