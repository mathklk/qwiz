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
        idle        = 1 << 0,
        waitForBuzz = 1 << 1,
        judging     = 1 << 2,
        estimation  = 1 << 3,
        answered    = 1 << 4,
        finished    = 1 << 5,
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
    Category* activeCategory() { return _activeCategory; }
    Rules rules() const { return _rules; }
    void setRules(Rules const& rules) { _rules = rules; }

public slots:
    void start(Board const&);
    void activateQuestion(int categoryIndex, int questionIndex);
    void buzz(int);
    void proceed();

    // Normal Question
    void judgeCorrect();
    void judgeWrong();
    void judgePass();

    // Estimation Question
    void chooseEsimationWinners(QList<int> const& winners);

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


inline Game::State operator|(Game::State a, Game::State b) {
    return static_cast<Game::State>(static_cast<int>(a) | static_cast<int>(b));
}

inline bool operator&(Game::State a, Game::State b) {
    return (static_cast<int>(a) & static_cast<int>(b)) != 0;
}

#endif // GAME_H
