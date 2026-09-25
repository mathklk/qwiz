#include "game.h"

#include <QDebug>

Game::Game():
    _board(Board::Empty()),
    _random(QRandomGenerator::securelySeeded()),
    _buzzerEffect(this)
{
    QList<QColor> const defaultColors = {
        QColor(0xd0, 0x35, 0x42),
        QColor(0x2b, 0x6b, 0xc7),
        QColor(0xd0, 0x9f, 0x36),
        QColor(0x46, 0x86, 0x29)}
    ;
    for (int i = 0; i < nPlayers; ++i) {
        _players.append(new Player(tr("Player %1").arg(i+1), defaultColors[i]));
    }
    _buzzerEffect.setSource(QUrl("qrc:/audio/buzzer.wav"));
}

void Game::start(Board const& board) {
    _board = board;
    _activeQuestion = nullptr;
    _state = State::idle;
    for (auto& player : _players) {
        player->setIsActive(false);
        player->setPoints(0);
    }
    nextPlayer()->setIsActive(true);
    emit changed();
}

void Game::activateQuestion(int categoryIndex, int questionIndex) {
    if (_state != State::idle) {
        return;
    }
    _chooserIndex = _players.indexOf(activePlayer());
    for (auto player : _players) {
        player->setIsActive(false);
        player->setHasAttempted(false);
    }
    _activeCategory = &(_board[categoryIndex]);
    _activeQuestion = _board.at(categoryIndex, questionIndex);
    _activeQuestion->activate();
    if (_activeQuestion->type() == Question::Type::normal) {
        _state = State::waitForBuzz;
    } else if (_activeQuestion->type() == Question::Type::estimate) {
        _state = State::estimation;
    } else {
        qCritical() << "Unknown question type" << int(_activeQuestion->type());
    }
    emit changed();
}

void Game::buzz(int const iPlayer) {
    if (_state != State::waitForBuzz) {
        return;
    }
    Player *const playerThatBuzzed = _players.at(iPlayer);
    if (playerThatBuzzed->hasAttempted()) {
        return;
    }
    _buzzerEffect.play();
    playerThatBuzzed->setIsActive(true);
    playerThatBuzzed->setHasAttempted(true);
    _state = State::judging;
    emit changed();
}

// Proceed or Skip
void Game::proceed() {
    if (not (_state & (State::answered | State::waitForBuzz | State::estimation))) {
        return;
    }
    if (_state != State::answered) {
        _activeQuestion->finish({});
    }

    bool gameIsFinished = true;
    for (Category const& category : _board) {
        for (Question const& question : category) {
            gameIsFinished &= question.state() == Question::State::finished;
        }
    }

    if (gameIsFinished) {
        float maxPoints = -INFINITY;
        for (auto player : _players) {
            player->setIsActive(false);
            player->setHasAttempted(false);
            maxPoints = qMax(maxPoints, float(player->points()));
        }
        // Mark winners active
        for (auto player : _players) {
            if (player->points() >= maxPoints) {
                player->setIsActive(true);
            }
        }
        _activeQuestion = nullptr;
        _state = State::finished;
    } else {
        Player *const chosenNextPlayer = nextPlayer(); // choose before resetting isActive bc nextPlayer may depend on it
        for (auto player : _players) {
            player->setIsActive(false);
            player->setHasAttempted(false);
        }
        chosenNextPlayer->setIsActive(true);
        _activeQuestion = nullptr;
        _state = State::idle;
    }
    emit changed();
}

void Game::judgeCorrect() {
    if (_state != State::judging) {
        return;
    }
    _activeQuestion->finish({activePlayer()});
    activePlayer()->addPoints(_activeQuestion->points());
    _state = State::answered;
    emit changed();
}

void Game::judgeWrong() {
    if (_state != State::judging) {
        return;
    }
    activePlayer()->addPoints(-1 * _activeQuestion->points() * _rules.wrongAnswerPointDeductionRatio);
    _activeQuestion->addFailer(activePlayer());
    activePlayer()->setIsActive(false);
    _state = State::waitForBuzz;
    emit changed();
}

void Game::judgePass() {
    if (_state != State::judging) {
        return;
    }
    _activeQuestion->addFailer(activePlayer());
    activePlayer()->setIsActive(false);
    _state = State::waitForBuzz;
    emit changed();
}

void Game::chooseEsimationWinners(QList<int> const& winners) {
    if (_state != State::estimation) {
        return;
    }
    QList<Player*> winnerPlayers;
    for (int const i : winners) {
        _players[i]->setIsActive(true);
        _players[i]->addPoints(activeQuestion()->points());
        winnerPlayers << _players[i];
    }
    activeQuestion()->finish(winnerPlayers);
    _state = State::answered;
    emit changed();
}

Player* Game::activePlayer() const {
    Player* player = nullptr;
    for (auto const& p : _players) {
        if (p->isActive()) {
            if (player != nullptr) {
                qCritical() << "Game::activePlayer was called but there are multiple active players!";
                return p;
            }
            player = p;
        }
    }
    return player;
}

Player *Game::nextPlayer() const {
    Player* activePlayer = nullptr;
    QList<Player*> inactivePlayers;
    for (auto const& player : _players) {
        if (player->isActive()) {
            activePlayer = player;
        } else {
            inactivePlayers.append(player);
        }
    }

    // No player active at beginning, chose random player to start
    if (activePlayer == nullptr) {
        int const chosenInitialPlayer = _random.bounded(_players.size());
        return _players[chosenInitialPlayer];
    }

    if (_rules.nextPlayerPolicy == Game::Rules::NextPlayerPolicy::random) {
        int const chosenNextPlayer = _random.bounded(inactivePlayers.size());
        return inactivePlayers[chosenNextPlayer];
    } else if (_rules.nextPlayerPolicy == Game::Rules::NextPlayerPolicy::cyclic) {
        _chooserIndex = (_chooserIndex + 1) % _players.size();
        return _players[_chooserIndex];
    } else if (_rules.nextPlayerPolicy == Game::Rules::NextPlayerPolicy::winner) {
        if (activePlayer != nullptr) {
            return activePlayer;
        }
        // Fallback: Choose random
        return _players.at(_random.bounded(nPlayers));
    } else if (_rules.nextPlayerPolicy == Game::Rules::NextPlayerPolicy::underdog) {
        float mostPoints = INFINITY;
        for (auto const& player : _players) {
            if (player->points() < mostPoints) {
                mostPoints = player->points();
            }
        }
        QList<Player*> underdogs;
        for (auto player : _players) {
            if (player->points() <= mostPoints) {
                underdogs << player;
            }
        }
        return underdogs.at(_random.bounded(underdogs.size()));
    } else {
        qCritical() << "Unknown next player policy" << int(_rules.nextPlayerPolicy);
        return nullptr;
    }
}
