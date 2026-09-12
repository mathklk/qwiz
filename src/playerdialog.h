#ifndef PLAYERDIALOG_H
#define PLAYERDIALOG_H

#include "domain/game.h"

#include <QWidget>

namespace Ui {
class PlayerDialog;
}

class PlayerDialog : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerDialog(Game* game, QWidget *parent = nullptr);
    ~PlayerDialog();

private:
    Ui::PlayerDialog *ui;
    Game* _game;
};

#endif // PLAYERDIALOG_H
