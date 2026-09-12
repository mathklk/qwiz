#ifndef GAME_H
#define GAME_H

#include <QObject>

#include "player.h"
#include "board.h"

class Game : public QObject
{
    Q_OBJECT
public:
    static constexpr int nPlayers = 4;

    enum class State {
        idle
    };

public:
    Game();

    State state() const { return _state; }
    QList<Player*>& players() { return _players; }
    Board const& board() { return _board; }

public slots:
    void start(Board const&);
    void buzz(int);

signals:
    void changed();

private:
    State _state = State::idle;
    QList<Player*> _players;
    Board _board;
};

#endif // GAME_H
