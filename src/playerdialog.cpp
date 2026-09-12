#include "playerdialog.h"
#include "ui_playerdialog.h"

PlayerDialog::PlayerDialog(Game* game, QWidget *parent):
    QWidget(parent),
    ui(new Ui::PlayerDialog),
    _game(game)
{
    ui->setupUi(this);
}

PlayerDialog::~PlayerDialog()
{
    delete ui;
}
