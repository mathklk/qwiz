#include "game.h"

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
}

void Game::start(Board const& board) {
    _board = board;
    _state = State::idle;
    emit changed();
}

void Game::buzz(int)
{

}
