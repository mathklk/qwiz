#include "moderatorwindow.h"
#include "ui_moderatorwindow.h"

#include "filedialog/rememberingfiledialog.h"
#include "model/jsonBoard.h"

#include <QMessageBox>
#include <QHBoxLayout>

ModeratorWindow::ModeratorWindow(Game* game, PlayerDialog* playerDialog, QWidget* parent):
    QMainWindow(parent),
    ui(new Ui::ModeratorWindow),
    _playerDialog(playerDialog),
    _game(game)
{
    ui->setupUi(this);
    connect(ui->actionPlayers, &QAction::triggered, _playerDialog, &QWidget::show);
    connect(ui->actionNew,     &QAction::triggered, this, &ModeratorWindow::newGameClicked);
    connect(ui->actionIncrease_Font_Size, &QAction::triggered, this, &ModeratorWindow::signalIncreaseFont);
    connect(ui->actionDecrease_Font_Size, &QAction::triggered, this, &ModeratorWindow::signalDecreaseFont);
    connect(ui->actionAlways_Show_Categories, &QAction::toggled, this, &ModeratorWindow::signalAlwaysShowCategoriesChanged);

    // Player Info in Status Bar
    ui->statusbar->setContentsMargins(0, 0, 0, 0);
    _statusBarLabels = {
        new QLabel(),
        new QLabel(),
        new QLabel(),
        new QLabel()
    };
    for (auto label : _statusBarLabels) {
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        ui->statusbar->addWidget(label, 1);
    }
    for (int i = 0; i < Game::nPlayers; ++i) {
        connect(_game->players()[i], &Player::changed, this, [this, i](){ playerChanged(i); });
        playerChanged(i);
    }

    // Board raster
    ui->raster->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->raster->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->raster->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui->raster->setFocusPolicy(Qt::NoFocus);
    ui->raster->setFrameStyle(QFrame::NoFrame);

    connect(_game, &Game::changed, this, [this](){
        updateBoard(_game->board());
    });
}

ModeratorWindow::~ModeratorWindow() {
    delete ui;
}

void ModeratorWindow::newGameClicked() {
    QString const filePath = RememberingFileDialog::getOpenFileName(
        this,
        "boardJson",
        "Load Board-JSON",
        "Board-JSON (*.json)"
    );
    if (filePath.isNull() or filePath.isEmpty()) {
        return;
    }

    try {
        Board const board = JsonBoard::fromJsonFile(QFile(filePath));
        _game->start(board);
        setWindowTitle("qwiz | " + board.title());
    } catch (JsonBoard::JsonException const& jsonException) {
        QMessageBox::critical(this, "File Error", jsonException.detail());
    }
}

void ModeratorWindow::playerChanged(int const i) {
    Player const*const player = _game->players()[i];
    _statusBarLabels[i]->setText(QString("<b>%1</b> : %2").arg(player->name()).arg(player->points()));
    QString boxStyleSheet;
    if (player->isActive()) {
        boxStyleSheet = QString("QFrame { padding: 2px; background-color: %1; }").arg(player->color().name());
    } else {
        // Frame not filled, but border colored
        boxStyleSheet = QString("QFrame { padding: 2px; background-color: transparent; border: 6px solid %1; }").arg(player->color().name());
    }
    _statusBarLabels[i]->setStyleSheet(boxStyleSheet);
}

void ModeratorWindow::updateBoard(Board const& board) {
    ui->raster->clear();
    ui->raster->show();
    ui->raster->setColumnCount(board.count());
    ui->raster->setRowCount(1 + board.maxNumberOfQuestionsPerCategory());
    for (int iCategory = 0; iCategory < board.count(); ++iCategory) {
        // Add Category Names / Header
        Category const& category = board[iCategory];
        auto categoryNameItem = new QTableWidgetItem(category.name());
        auto categoryFont = categoryNameItem->font();
        categoryFont.setBold(true);
        categoryNameItem->setFont(categoryFont);
        categoryNameItem->setTextAlignment(Qt::AlignCenter);
        ui->raster->setItem(0, iCategory, categoryNameItem);

        // Add Question Boxes
        for (int iQuestion = 0; iQuestion < category.count(); ++iQuestion) {
            if (iQuestion > category.count()) {
                continue;
            }
            Question const& question = category[iQuestion];
            QWidget* cellWidget;
            QPushButton* cellButton = new QPushButton(QString::number(question.points()), ui->raster);
            connect(cellButton, &QPushButton::clicked, this, [this, iCategory, iQuestion](){
                _game->activateQuestion(iCategory, iQuestion);
            });
            cellWidget = cellButton;
            ui->raster->setCellWidget(1 + iQuestion, iCategory, cellWidget);
        }
    }
}
