#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include "model/game.h"

#include <QWidget>
#include <QLabel>

namespace Ui {
class ViewerWindow;
}

class ViewerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ViewerWindow(Game*, QWidget *parent = nullptr);
    ~ViewerWindow();

private:
    void updateGame();
    void updatePlayerBox(int);

private:
    Ui::ViewerWindow *ui;
    Game* _game;

    struct PlayerBox {
        QFrame* frame;
        QLabel* nameLabel;
        QLabel* pointsLabel;
    };
    QList<PlayerBox> _playerBoxes;
};

#endif // VIEWERWINDOW_H
