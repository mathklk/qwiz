#include "viewerwindow.h"
#include "ui_viewerwindow.h"

ViewerWindow::ViewerWindow(Game* game, QWidget *parent):
    QWidget(parent),
    ui(new Ui::ViewerWindow),
    _game(game)
{
    ui->setupUi(this);

    _playerBoxes << PlayerBox{ui->frame_1, ui->p1_name, ui->p1_points};
    _playerBoxes << PlayerBox{ui->frame_2, ui->p2_name, ui->p2_points};
    _playerBoxes << PlayerBox{ui->frame_3, ui->p3_name, ui->p3_points};
    _playerBoxes << PlayerBox{ui->frame_4, ui->p4_name, ui->p4_points};

    for (int i = 0; i < Game::nPlayers; ++i) {
        updatePlayerBox(i);
        connect(game->players()[i], &Player::changed, this, [this, i](){
            updatePlayerBox(i);
        });
    }

    connect(_game, &Game::changed, this, &ViewerWindow::updateGame);

    ui->raster->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->raster->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->raster->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui->raster->setFocusPolicy(Qt::NoFocus);
}

ViewerWindow::~ViewerWindow()
{
    delete ui;
}

void ViewerWindow::updateGame() {
    for (int i = 0; i < Game::nPlayers; ++i) {
        updatePlayerBox(i);
    }

    if (_game->state() == Game::State::idle) {
        // Draw Board
        Board const& board = _game->board();
        ui->raster->clear();
        ui->raster->show();
        ui->raster->setColumnCount(board.count());
        ui->raster->setRowCount(1 + board.maxNumberOfQuestionsPerCategory());
        for (int iColumn = 0; iColumn < board.count(); ++iColumn) {
            // Add Category Names / Header
            Category const& category = board[iColumn];
            auto categoryNameItem = new QTableWidgetItem(category.name());
            categoryNameItem->setTextAlignment(Qt::AlignCenter);
            ui->raster->setItem(0, iColumn, categoryNameItem);

            // Add Question Boxes
            for (int iRow = 0; iRow < category.count(); ++iRow) {
                if (iRow > category.count()) {
                    continue;
                }
                Question const& question = category[iRow];
                auto questionItem = new QTableWidgetItem(QString::number(question.points()));
                questionItem->setTextAlignment(Qt::AlignCenter);
                ui->raster->setItem(1 +iRow, iColumn, questionItem);
            }
        }
    }
}

void ViewerWindow::updatePlayerBox(int i) {
    PlayerBox& box = _playerBoxes[i];
    _playerBoxes[i].frame->setStyleSheet(
        QString("QFrame { background-color: %1; }").arg(_game->players()[i]->color().name())
    );
    box.nameLabel->setText(_game->players()[i]->name());
    QFont nameFont = box.nameLabel->font();
    nameFont.setBold(true);
    box.nameLabel->setFont(nameFont);
    box.pointsLabel->setText(QString::number(_game->players()[i]->points()));
}
