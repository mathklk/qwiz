#ifndef PLAYERDIALOG_H
#define PLAYERDIALOG_H

#include "model/game.h"

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

namespace Ui {
class PlayerDialog;
}

class PlayerDialog : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerDialog(Game* game, QWidget *parent = nullptr);
    ~PlayerDialog();

public slots:
    void triggerBuzzerVisualization(int);

protected:
    void changeEvent(QEvent*) override;

private:
    void setPlayerBoxBackgroundColor(int, QColor const);

private:
    Ui::PlayerDialog *ui;
    Game* _game;

    struct PlayerBox {
        QFrame* frame;
        QLineEdit* lineEditName;
        QLabel* labelPoints;
        QPushButton* buttonColor;
        QPushButton* buttonActive;
        QLineEdit* lineEditPoints;
    };
    QList<PlayerBox> _playerBoxes;
};

#endif // PLAYERDIALOG_H
