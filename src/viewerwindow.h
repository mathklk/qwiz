#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include "model/game.h"

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

namespace Ui {
class ViewerWindow;
}

class ViewerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ViewerWindow(Game*, QWidget *parent = nullptr);
    ~ViewerWindow();

public slots:
    void setFont(QFont const& font) {
        QWidget::setFont(font);
        for (PlayerBox const& box : _playerBoxes) {
            box.nameLabel->setFont(font);
            box.pointsLabel->setFont(font);
        }
    }
    void setAlwaysShowCategories(bool show) {
        _alwaysShowCategories = show;
        updateGame();
    }

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent* ev) override {
        QWidget::closeEvent(ev);
        emit closed();
    }

private:
    QVBoxLayout* mainLayout() { return qobject_cast<QVBoxLayout*>(layout()); }
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
    bool _alwaysShowCategories = true;
};

#endif // VIEWERWINDOW_H
