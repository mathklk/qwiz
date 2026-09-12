#include "playerdialog.h"
#include "ui_playerdialog.h"

#include <QTimer>
#include <QColorDialog>

PlayerDialog::PlayerDialog(Game* game, QWidget *parent):
    QWidget(parent),
    ui(new Ui::PlayerDialog),
    _game(game)
{
    ui->setupUi(this);

    QAction* escAction = new QAction(parent);
    connect(escAction, &QAction::triggered, this, &QWidget::close);
    escAction->setShortcut(Qt::Key_Escape);
    addAction(escAction);

    _playerBoxes << PlayerBox{ui->frame_1, ui->lineEdit_1, ui->label_1, ui->buttonPickColor_1};
    _playerBoxes << PlayerBox{ui->frame_2, ui->lineEdit_2, ui->label_2, ui->buttonPickColor_2};
    _playerBoxes << PlayerBox{ui->frame_3, ui->lineEdit_3, ui->label_3, ui->buttonPickColor_3};
    _playerBoxes << PlayerBox{ui->frame_4, ui->lineEdit_4, ui->label_4, ui->buttonPickColor_4};
    for (int i = 0; i < Game::nPlayers; ++i) {
        Player* player = game->players()[i];
        setPlayerBoxBackgroundColor(i, player->color());
        _playerBoxes[i].lineEdit->setText(player->name());
        _playerBoxes[i].label->setText(QString::number(game->players()[i]->points()));
        connect(_playerBoxes[i].lineEdit, &QLineEdit::textEdited, this, [&player](QString const& newName){
            player->setName(newName);
        });
        connect(_playerBoxes[i].button, &QPushButton::clicked, this, [this, i, player](){
            QColor const newColor = QColorDialog::getColor(player->color());
            if (newColor.isValid()) {
                player->setColor(newColor);
                setPlayerBoxBackgroundColor(i, newColor);
            }
        });
    }
}

PlayerDialog::~PlayerDialog() {
    delete ui;
}

void PlayerDialog::triggerBuzzerVisualization(int iPlayer) {
    setPlayerBoxBackgroundColor(iPlayer, _game->players()[iPlayer]->color().lighter());
    QTimer::singleShot(200, this, [iPlayer, this](){
        setPlayerBoxBackgroundColor(iPlayer, _game->players()[iPlayer]->color());
    });
}

void PlayerDialog::setPlayerBoxBackgroundColor(int iPlayer, QColor const color) {
    _playerBoxes[iPlayer].frame->setStyleSheet(
        QString("QFrame { background-color: %1; }").arg(color.name())
    );
}
