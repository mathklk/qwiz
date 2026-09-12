#ifndef GAME_H
#define GAME_H

#include <QObject>

#include "player.h"

class Game: QObject
{
    Q_OBJECT
public:
    Game();

    QList<Player>& players() { return _players; }

private:
    static constexpr int nPlayers = 4;
    QList<Player> _players;
};

#endif // GAME_H
