#include "game.h"

#include <QDebug>

Game::Game() {
    QList<QColor> const defaultColors = {
        QColor(0xd0, 0x35, 0x42),
        QColor(0x2b, 0x6b, 0xc7),
        QColor(0xd0, 0x9f, 0x36),
        QColor(0x46, 0x86, 0x29)}
    ;
    for (int i = 0; i < nPlayers; ++i) {
        _players.append(new Player("Player " + QString::number(i+1), defaultColors[i]));
    }
    _board = Board::Empty();
    _random = QRandomGenerator::securelySeeded();
}

void Game::start(Board const& board) {
    _board = board;
    _state = State::idle;
    for (auto& player : _players) {
        player->setIsActive(false);
    }
    nextPlayer()->setIsActive(true);
    emit changed();
}

void Game::buzz(int)
{

}

void Game::activateQuestion(int categoryIndex, int questionIndex) {
    _state = State::activeQuestion;
    _board.at(categoryIndex, questionIndex)->activate();

    emit changed();
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

    // No player active, chose random player to start
    if (activePlayer == nullptr) {
        int const chosenInitialPlayer = _random.bounded(_players.size());
        return _players[chosenInitialPlayer];
    }

    if (_rules.nextPlayerPolicy == Game::Rules::NextPlayerPolicy::random) {
        int const chosenNextPlayer = _random.bounded(inactivePlayers.size());
        return inactivePlayers[chosenNextPlayer];
    } else if (_rules.nextPlayerPolicy == Game::Rules::NextPlayerPolicy::cyclic) {
        int const activeIndex = _players.indexOf(activePlayer);
        int const nextIndex = (activeIndex + 1) % _players.size();
        return _players[nextIndex];
    } else {
        qCritical() << "Unknown next player policy " << int(_rules.nextPlayerPolicy);
        return nullptr;
    }
}
