#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "board.h"

#include <QObject>
#include <QRandomGenerator>
#include <QSoundEffect>

class Game : public QObject
{
    Q_OBJECT
public:
    static constexpr int nPlayers = 4;

    enum class State {
        idle,
        activeQuestion,
        judging,
        answered,
        finished
    };

    struct Rules {
        enum class NextPlayerPolicy {
            random,
            cyclic,
            winner,
            underdog
        } nextPlayerPolicy = NextPlayerPolicy::random;
        float wrongAnswerPointDeductionRatio = 0.0f;
    };

public:
    Game();

    State state() const { return _state; }
    QList<Player*> players() const { return _players; }
    Board const& board() const { return _board; }
    Board& board() { return _board; }
    Question* activeQuestion() { return _activeQuestion; }
    Rules rules() const { return _rules; }
    void setRules(Rules const& rules) { _rules = rules; }

public slots:
    void start(Board const&);
    void activateQuestion(int categoryIndex, int questionIndex);
    void buzz(int);
    void judgeCorrect();
    void judgeWrong();
    void judgePass();
    void proceed();

signals:
    void changed();

private:
    Player* activePlayer() const;
    Player* nextPlayer() const;

private:
    State _state = State::idle;
    Rules _rules;
    QList<Player*> _players;
    Board _board;
    Category* _activeCategory = nullptr;
    Question* _activeQuestion = nullptr;
    mutable int _chooserIndex = 0;
    mutable QRandomGenerator _random;

    QSoundEffect _buzzerEffect;
};

#endif // GAME_H
