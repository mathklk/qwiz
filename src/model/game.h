#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QRandomGenerator>

#include "player.h"
#include "board.h"

class Game : public QObject
{
    Q_OBJECT
public:
    static constexpr int nPlayers = 4;

    enum class State {
        idle,
        activeQuestion,
    };

    struct Rules {
        enum class NextPlayerPolicy {
            random,
            cyclic
        } nextPlayerPolicy = NextPlayerPolicy::random;
        float wrongAnswerPointDeductionRatio = 0.5f;
    };

public:
    Game();

    State state() const { return _state; }
    QList<Player*> players() const { return _players; }
    Board const& board() const { return _board; }
    Board& board() { return _board; }
    Rules rules() const { return _rules; }
    void setRules(Rules const& rules) { _rules = rules; }

public slots:
    void start(Board const&);
    void buzz(int);
    void activateQuestion(int categoryIndex, int questionIndex);

signals:
    void changed();

private:
    Player* nextPlayer() const;

private:
    State _state = State::idle;
    Rules _rules;
    QList<Player*> _players;
    Board _board;
    mutable QRandomGenerator _random;
};

#endif // GAME_H
