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
    qDebug() << "ModeratorWindow::playerChanged(" << i << ")";
    Player const*const player = _game->players()[i];
    _statusBarLabels[i]->setText(QString("<b>%1</b> : %2").arg(player->name()).arg(player->points()));
    _statusBarLabels[i]->setStyleSheet(
        QString("QLabel { padding: 4px; background-color: %1; }").arg(player->color().name())
    );
}
