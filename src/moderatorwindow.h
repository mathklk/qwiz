#ifndef MODERATORWINDOW_H
#define MODERATORWINDOW_H

#include "model/game.h"
#include "playerdialog.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class ModeratorWindow;
}
QT_END_NAMESPACE

class ModeratorWindow : public QMainWindow
{
    Q_OBJECT

public:
    ModeratorWindow(Game* game, PlayerDialog* playerDialog, QWidget* parent = nullptr);
    ~ModeratorWindow();

private:
    void newGameClicked();

private:
    Ui::ModeratorWindow* ui;
    PlayerDialog* _playerDialog;

    Game* _game;

};
#endif // MODERATORWINDOW_H
