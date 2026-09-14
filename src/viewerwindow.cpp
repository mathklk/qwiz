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
        connect(game->players()[i], &Player::changed, this, [this, i](){ updatePlayerBox(i); });
    }

    connect(_game, &Game::changed, this, &ViewerWindow::updateGame);

    /*
    for (QTableWidget* table: {ui->raster, ui->categories}) {
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        table->setFocusPolicy(Qt::NoFocus);
        table->setFrameStyle(QFrame::NoFrame);
        table->sizeHint()
    }
    */

    updateGame();
}

ViewerWindow::~ViewerWindow()
{
    delete ui;
}

void ViewerWindow::updateGame() {
    setWindowTitle(_game->board().title());

    for (int i = 0; i < Game::nPlayers; ++i) {
        updatePlayerBox(i);
    }

    // Clear Raster


    QLayout* oldLayout = ui->raster->layout();
    if (oldLayout != nullptr) {
    QLayoutItem* item;
        while ((item = oldLayout->takeAt(0))) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
        delete oldLayout;
    }
    QGridLayout* rasterLayout = new QGridLayout;
    ui->raster->setLayout(rasterLayout);
    // Reset Layout size

    Board const& board = _game->board();
    if (_game->state() == Game::State::idle or _alwaysShowCategories) {
        // Draw Board
        if (board.empty()) {
            rasterLayout->addWidget(new QLabel());
        }
        for (int iColumn = 0; iColumn < board.count(); ++iColumn) {
            // Add Categories
            Category const& category = board[iColumn];
            auto categoryNameLabel = new QLabel("<h3>" + category.name() + "</h3>");
            auto categoryFont = categoryNameLabel->font();
            categoryFont.setBold(true);
            categoryNameLabel->setFont(categoryFont);
            categoryNameLabel->setAlignment(Qt::AlignCenter);
            categoryNameLabel->setWordWrap(true);
            categoryNameLabel->setContentsMargins(16, 16, 16, 16);
            rasterLayout->addWidget(categoryNameLabel, 0, iColumn);

            // Add Question Boxes
            if (_game->state() != Game::State::idle) {
                continue;
            }
            for (int iRow = 0; iRow < category.count(); ++iRow) {
                Question const& question = category[iRow];
                auto questionLabel = new QLabel("<h2>" + QString::number(question.points()) + "</h2>");
                questionLabel->setAlignment(Qt::AlignCenter);
                rasterLayout->addWidget(questionLabel, 1 + iRow, iColumn);
            }
        }
        for (int iRow = 1; iRow < board.maxNumberOfQuestionsPerCategory() + 1; ++iRow) {
            rasterLayout->setRowStretch(iRow, 1);
        }
    }
    if (_game->state() == Game::State::activeQuestion) {
        Question const*const activeQuestion = _game->board().activeQuestion();
        QLabel *const questionLabel = new QLabel(activeQuestion ? "<h1>" + activeQuestion->text() + "</h1>" : "");
        questionLabel->setAlignment(Qt::AlignCenter);
        questionLabel->setWordWrap(true);
        if (_alwaysShowCategories) {
            rasterLayout->addWidget(questionLabel, 1, 0, board.maxNumberOfQuestionsPerCategory(), board.count());
            rasterLayout->setRowStretch(1, 1);
        } else {
            rasterLayout->addWidget(questionLabel, 0, 0);
            rasterLayout->setRowStretch(0, 1);
        }
    }
    mainLayout()->setStretchFactor(ui->horizontalLayout, 0);
}

void ViewerWindow::updatePlayerBox(int i) {
    PlayerBox& box = _playerBoxes[i];
    Player const*const player = _game->players().at(i);
    QString boxStyleSheet;
    if (player->isActive()) {
        boxStyleSheet = QString("QFrame { background-color: %1; }").arg(player->color().name());
    } else {
        // Frame not filled, but border colored
        boxStyleSheet = QString("QFrame { background-color: transparent; border: 6px solid %1; }").arg(player->color().name());
    }
    box.frame->setStyleSheet(boxStyleSheet);
    box.nameLabel->setText("<b>" + player->name() + "</b>");
    box.nameLabel->setStyleSheet("QFrame {border: 0px; background-color: transparent;}");
    box.pointsLabel->setText(QString::number(player->points()));
    box.pointsLabel->setStyleSheet("QFrame {border: 0px; background-color: transparent;}");
}
