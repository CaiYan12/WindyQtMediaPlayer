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

PlaylistWidget::PlaylistWidget(PlaylistManager* playlist, QWidget* parent)
    : QWidget(parent)
    , m_playlist(playlist)
{
    setObjectName(QStringLiteral("playlistPanel"));
    setMinimumWidth(280);
    setMaximumWidth(380);

    // Header
    auto* header = new QHBoxLayout;
    header->setContentsMargins(12, 8, 12, 4);
    auto* title = new QLabel(QStringLiteral("\u25b6 \u64ad\u653e\u5217\u8868"), this);
    title->setStyleSheet(QStringLiteral("font-size:13px; font-weight:bold; color:#e0e0e0;"));
    header->addWidget(title);
    header->addStretch();

    auto* clearBtn = new QPushButton(QStringLiteral("\u6e05\u7a7a"), this);
    clearBtn->setFixedHeight(22);
    clearBtn->setStyleSheet(QStringLiteral("font-size:11px; background:#0f3460; color:#e0e0e0; border-radius:3px; padding:0 8px;"));
    connect(clearBtn, &QPushButton::clicked, m_playlist, &PlaylistManager::clearPlaylist);
    header->addWidget(clearBtn);

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
    layout->addLayout(header);
    layout->addWidget(m_table, 1);
}

PlaylistWidget::~PlaylistWidget() = default;

void PlaylistWidget::onCurrentRowChanged(int row)
{
    if (row >= 0) {
        m_table->selectRow(row);
        m_table->scrollToItem(m_table->item(row, 0));
    }
}
