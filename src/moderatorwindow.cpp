#include "moderatorwindow.h"
#include "ui_moderatorwindow.h"

#include "filedialog/rememberingfiledialog.h"
#include "model/jsonBoard.h"

#include <QMessageBox>

ModeratorWindow::ModeratorWindow(Game* game, PlayerDialog* playerDialog, QWidget* parent):
    QMainWindow(parent),
    ui(new Ui::ModeratorWindow),
    _playerDialog(playerDialog),
    _game(game)
{
    ui->setupUi(this);
    connect(ui->actionPlayers, &QAction::triggered, _playerDialog, &QWidget::show);
    connect(ui->actionNew,     &QAction::triggered, this, &ModeratorWindow::newGameClicked);
}

ModeratorWindow::~ModeratorWindow() {
    delete ui;
}

void ModeratorWindow::newGameClicked() {
    QString const fileName = RememberingFileDialog::getOpenFileName(
        this,
        "boardJson",
        "Load Board-JSON",
        "Board-JSON (*.json)"
    );
    if (fileName.isNull() or fileName.isEmpty()) {
        return;
    }

    try {
        Board board = JsonBoard::fromJsonFile(QFile(fileName));
        _game->start(board);
    } catch (JsonBoard::JsonException const& jsonException) {
        QMessageBox::critical(this, "File Error", jsonException.detail());
    }
}
