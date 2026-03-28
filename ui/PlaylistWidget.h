#pragma once
#include <QWidget>
class PlaylistManager;
class QTableWidget;

class PlaylistWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlaylistWidget(PlaylistManager* playlist, QWidget* parent = nullptr);
    ~PlaylistWidget() override;

signals:
    void itemDoubleClicked(int index);
    void nextRequested();
    void prevRequested();

private slots:
    void onCurrentRowChanged(int row);

private:
    PlaylistManager* m_playlist = nullptr;
    QTableWidget*   m_table = nullptr;
};
