#include "moderatorwindow.h"

#include <QApplication>

#include "domain/game.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Game game;
    ModeratorWindow w(&game);
    w.show();
    return a.exec();
}
