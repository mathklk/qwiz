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

signals:
    void closed();
    void signalIncreaseFont();
    void signalDecreaseFont();
    void signalAlwaysShowCategoriesChanged(bool);

protected:
    void closeEvent(QCloseEvent* ev) override {
        QMainWindow::closeEvent(ev);
        emit closed();
    }

private:
    void newGameClicked();
    void playerChanged(int);
    void updateBoard(Board const&);

private:
    Ui::ModeratorWindow* ui;
    PlayerDialog* _playerDialog;
    Game* _game;

    QList<QLabel*> _statusBarLabels;

};
#endif // MODERATORWINDOW_H
