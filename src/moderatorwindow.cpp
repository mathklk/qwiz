#include "moderatorwindow.h"
#include "ui_moderatorwindow.h"

ModeratorWindow::ModeratorWindow(Game* game, QWidget* parent):
    QMainWindow(parent),
    ui(new Ui::ModeratorWindow),
    _playerDialog(new PlayerDialog(game)),
    _game(game)
{
    ui->setupUi(this);
    connect(ui->actionPlayers, &QAction::triggered, _playerDialog, &QWidget::show);
}

ModeratorWindow::~ModeratorWindow()
{
    delete ui;
}
