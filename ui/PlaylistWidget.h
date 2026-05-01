#pragma once
#include <QWidget>
class PlaylistManager;
class QTableWidget;
class QToolButton;
class QMenu;

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
    void onModeChanged(QAction* action);

private:
    void loadPlaylist();
    void updateModeBtnText(int mode);

private:
    PlaylistManager* m_playlist = nullptr;
    QTableWidget*   m_table = nullptr;
    QToolButton*    m_modeBtn = nullptr;
    QMenu*          m_modeMenu = nullptr;
};