#include <QApplication>
#include <QStyleFactory>
#include <QFont>
#include <QPalette>
#include <QColor>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName(QStringLiteral("WindyQtMediaPlayer"));
    app.setApplicationName(QStringLiteral(""));
    app.setApplicationVersion(QStringLiteral("1.0"));

    app.setStyle(QStyleFactory::create(QStringLiteral("fusion")));

    QPalette dark;
    dark.setColor(QPalette::Window,          QColor(22, 33, 62));
    dark.setColor(QPalette::WindowText,       QColor(224, 224, 224));
    dark.setColor(QPalette::Base,             QColor(13, 13, 26));
    dark.setColor(QPalette::AlternateBase,    QColor(26, 42, 78));
    dark.setColor(QPalette::ToolTipBase,      QColor(0, 0, 0));
    dark.setColor(QPalette::ToolTipText,      QColor(224, 224, 224));
    dark.setColor(QPalette::Text,             QColor(224, 224, 224));
    dark.setColor(QPalette::Button,           QColor(15, 52, 96));
    dark.setColor(QPalette::ButtonText,       QColor(224, 224, 224));
    dark.setColor(QPalette::BrightText,       QColor(233, 69, 96));
    dark.setColor(QPalette::Highlight,        QColor(15, 52, 96));
    dark.setColor(QPalette::HighlightedText,  QColor(255, 255, 255));
    dark.setColor(QPalette::Link,             QColor(42, 130, 218));
    app.setPalette(dark);

    app.setFont(QFont(QStringLiteral("Microsoft YaHei UI"), 9));

    MainWindow w;
    w.resize(1200, 700);
    w.show();

    return app.exec();
}
