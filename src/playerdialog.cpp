#include "playerdialog.h"
#include "ui_playerdialog.h"

#include <QTimer>
#include <QColorDialog>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

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

    _playerBoxes << PlayerBox{ui->frame_1, ui->lineEdit_1, ui->label_1, ui->buttonPickColor_1, ui->pushButtonActive_1, ui->lineEditPoints_1};
    _playerBoxes << PlayerBox{ui->frame_2, ui->lineEdit_2, ui->label_2, ui->buttonPickColor_2, ui->pushButtonActive_2, ui->lineEditPoints_2};
    _playerBoxes << PlayerBox{ui->frame_3, ui->lineEdit_3, ui->label_3, ui->buttonPickColor_3, ui->pushButtonActive_3, ui->lineEditPoints_3};
    _playerBoxes << PlayerBox{ui->frame_4, ui->lineEdit_4, ui->label_4, ui->buttonPickColor_4, ui->pushButtonActive_4, ui->lineEditPoints_4};
    for (int i = 0; i < Game::nPlayers; ++i) {
        Player *const player = game->players()[i];

        // Init with current values
        setPlayerBoxBackgroundColor(i, player->color());
        _playerBoxes[i].lineEditName->setText(player->name());
        _playerBoxes[i].labelPoints->setText("<h3>" + QString::number(_game->players()[i]->points()) + "</h3>");

        // Name Change
        connect(_playerBoxes[i].lineEditName, &QLineEdit::textEdited, this, [player](QString const& newName){
            player->setName(newName);
        });
        // Color Change
        connect(_playerBoxes[i].buttonColor, &QPushButton::clicked, this, [this, i, player](){
            QColor const newColor = QColorDialog::getColor(player->color());
            if (newColor.isValid()) {
                player->setColor(newColor);
                setPlayerBoxBackgroundColor(i, newColor);
            }
        });
        // Make Active
        connect(_playerBoxes[i].buttonActive, &QPushButton::clicked, this, [this, i, player](){
            for (int j = 0; j < Game::nPlayers; ++j) {
                _game->players()[j]->setIsActive(j == i);
            }
        });
        // Update on Player change
        connect(player, &Player::changed, this, [this, i](){
            _playerBoxes[i].labelPoints->setText("<h3>" + QString::number(_game->players()[i]->points()) + "</h3>");
            _playerBoxes[i].buttonActive->setChecked(_game->players()[i]->isActive());
            _playerBoxes[i].buttonActive->setEnabled(not _game->players()[i]->isActive());
        });
        // Points Adjust
        QRegularExpression pmNumberRegex("^[+-]\\d{1,6}$");
        QRegularExpressionValidator const*const validator = new QRegularExpressionValidator(pmNumberRegex);
        _playerBoxes[i].lineEditPoints->setValidator(validator);
        connect(_playerBoxes[i].lineEditPoints, &QLineEdit::returnPressed, this, [this, i, player](){
            QString const input = _playerBoxes[i].lineEditPoints->text();
            _playerBoxes[i].lineEditPoints->clear();
            signed int const points = input.toInt();
            player->setPoints(player->points() + points);
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

void PlayerDialog::changeEvent(QEvent* ev) {
    QWidget::changeEvent(ev);
    if (ev->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}

void PlayerDialog::setPlayerBoxBackgroundColor(int iPlayer, QColor const color) {
    _playerBoxes[iPlayer].frame->setStyleSheet(
        QString("QFrame { background-color: %1; }").arg(color.name())
    );
}
