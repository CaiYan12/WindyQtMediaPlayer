#pragma once
#include <QWidget>
class MediaEngine;

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWidget(QWidget* parent = nullptr);
    ~VideoWidget() override;

    void attachToEngine(MediaEngine* engine);

protected:
    bool event(QEvent* event) override;

private:
    MediaEngine* m_engine = nullptr;
};
