#include "game.h"

Game::Game() {
    for (int i = 0; i < nPlayers; ++i) {
        _players.append(Player("Player " + QString::number(i+1)));
    }
}
