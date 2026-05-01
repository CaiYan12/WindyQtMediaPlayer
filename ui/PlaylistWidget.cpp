#include "PlaylistWidget.h"
#include "PlaylistManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QLabel>
#include <QFileInfo>
#include <QToolButton>
#include <QMenu>
#include <QAction>

PlaylistWidget::PlaylistWidget(PlaylistManager* playlist, QWidget* parent)
    : QWidget(parent)
    , m_playlist(playlist)
{
    setObjectName(QStringLiteral("playlistPanel"));
    setMinimumWidth(280);
    setMaximumWidth(380);

    // Title bar
    auto* titleBar = new QHBoxLayout;
    titleBar->setContentsMargins(12, 6, 12, 4);
    auto* title = new QLabel(QStringLiteral("\u25b6 \u64ad\u653e\u5217\u8868"), this);
    title->setStyleSheet(QStringLiteral("font-size:13px; font-weight:bold; color:#e0e0e0;"));
    titleBar->addWidget(title);
    titleBar->addStretch();

    // Mode toolbar
    auto* toolbar = new QHBoxLayout;
    toolbar->setContentsMargins(12, 0, 12, 4);
    auto* modeLabel = new QLabel(QStringLiteral("\u64ad\u653e\u6a21\u5f0f:"), this);
    modeLabel->setStyleSheet(QStringLiteral("font-size:11px; color:#a0a0a0;"));
    toolbar->addWidget(modeLabel);

    // 播放模式菜单
    m_modeMenu = new QMenu(this);
    QAction* acts[5];
    acts[0] = m_modeMenu->addAction(QStringLiteral("\u5355\u4e2a\u64ad\u653e"));
    acts[1] = m_modeMenu->addAction(QStringLiteral("\u987a\u5e8f\u64ad\u653e"));
    acts[2] = m_modeMenu->addAction(QStringLiteral("\u5355\u66f2\u5f80\u590d"));
    acts[3] = m_modeMenu->addAction(QStringLiteral("\u5217\u8868\u5f80\u590d"));
    acts[4] = m_modeMenu->addAction(QStringLiteral("\u968f\u673a\u64ad\u653e"));
    for (int i = 0; i < 5; ++i) {
        acts[i]->setData(i);
        acts[i]->setCheckable(true);
    }
    acts[m_playlist->playbackMode()]->setChecked(true);
    connect(m_modeMenu, &QMenu::triggered, this, &PlaylistWidget::onModeChanged);

    m_modeBtn = new QToolButton(this);
    m_modeBtn->setFixedHeight(22);
    m_modeBtn->setMenu(m_modeMenu);
    m_modeBtn->setPopupMode(QToolButton::InstantPopup);
    m_modeBtn->setStyleSheet(QStringLiteral(
        "QToolButton { background:#1a3a6e; color:#e0e0e0; border:1px solid #2a4a7e; border-radius:3px; padding:2px 14px 2px 6px; font-size:11px; min-width:90px; }"
        "QToolButton:hover { background:#2a4a8e; }"
        "QToolButton::menu-indicator { subcontrol-origin:padding; subcontrol-position:right center; }"
    ));
    updateModeBtnText(m_playlist->playbackMode());
    toolbar->addWidget(m_modeBtn);

    auto* clearBtn = new QPushButton(QStringLiteral("\u6e05\u7a7a"), this);
    clearBtn->setFixedHeight(22);
    clearBtn->setStyleSheet(QStringLiteral("font-size:11px; background:#0f3460; color:#e0e0e0; border-radius:3px; padding:0 8px;"));
    connect(clearBtn, &QPushButton::clicked, m_playlist, &PlaylistManager::clearPlaylist);
    toolbar->addWidget(clearBtn);

    // Table
    m_table = new QTableWidget(0, 3, this);
    m_table->setObjectName(QStringLiteral("playlistTable"));
    m_table->setAlternatingRowColors(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setShowGrid(false);
    m_table->verticalHeader()->setVisible(false);
    m_table->horizontalHeader()->setVisible(false);
    m_table->setColumnWidth(0, 30);
    m_table->setColumnWidth(1, 160);
    m_table->setColumnWidth(2, 70);
    m_table->setStyleSheet(
        "QTableWidget { background: #16213e; color: #e0e0e0; border: none; font-size: 12px; }"
        "QTableWidget::item { padding: 4px 6px; }"
        "QTableWidget::item:selected { background: #0f3460; color: #fff; }"
        "QTableWidget::item:hover { background: #1a3a6e; }"
        "QTableWidget::item:alternate { background: #1a2a4e; }"
        "QScrollBar:vertical { background: #16213e; width: 6px; }"
        "QScrollBar::handle:vertical { background: #0f3460; border-radius: 3px; }"
    );

    connect(m_table, &QTableWidget::itemDoubleClicked,
            this, [this]() { emit itemDoubleClicked(m_table->currentRow()); });

    connect(m_playlist, &PlaylistManager::playlistChanged, this, [this]() {
        m_table->setRowCount(m_playlist->itemCount());
        for (int i = 0; i < m_playlist->itemCount(); ++i) {
            auto item = m_playlist->itemAt(i);
            QString name = item.title.isEmpty()
                ? QFileInfo(item.url.toString()).fileName()
                : item.title;

            auto* idxItem = new QTableWidgetItem(QString::number(i + 1));
            idxItem->setTextAlignment(Qt::AlignCenter);
            idxItem->setFlags(idxItem->flags() & ~Qt::ItemIsSelectable);

            auto* nameItem = new QTableWidgetItem(name);
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsSelectable);
            nameItem->setToolTip(name);

            auto* durItem = new QTableWidgetItem(QStringLiteral("-"));
            durItem->setTextAlignment(Qt::AlignCenter);
            durItem->setFlags(durItem->flags() & ~Qt::ItemIsSelectable);

            m_table->setItem(i, 0, idxItem);
            m_table->setItem(i, 1, nameItem);
            m_table->setItem(i, 2, durItem);
        }
    });

    connect(m_playlist, &PlaylistManager::currentIndexChanged, this,
            &PlaylistWidget::onCurrentRowChanged);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addLayout(titleBar);
    layout->addLayout(toolbar);
    layout->addWidget(m_table, 1);
}

PlaylistWidget::~PlaylistWidget() = default;

void PlaylistWidget::updateModeBtnText(int mode)
{
    const QString names[5] = {
        QStringLiteral("\u5355\u4e2a\u64ad\u653e"),
        QStringLiteral("\u987a\u5e8f\u64ad\u653e"),
        QStringLiteral("\u5355\u66f2\u5f80\u590d"),
        QStringLiteral("\u5217\u8868\u5f80\u590d"),
        QStringLiteral("\u968f\u673a\u64ad\u653e")
    };
    m_modeBtn->setText(names[mode]);
}

void PlaylistWidget::onCurrentRowChanged(int row)
{
    if (row >= 0) {
        m_table->selectRow(row);
        m_table->scrollToItem(m_table->item(row, 0));
    }
}

void PlaylistWidget::onModeChanged(QAction* action)
{
    int mode = action->data().toInt();
    m_playlist->setPlaybackMode(static_cast<PlaylistManager::PlaybackMode>(mode));
    updateModeBtnText(mode);

    // 同步勾选状态
    for (QAction* a : m_modeMenu->actions()) {
        a->setChecked(a == action);
    }
}