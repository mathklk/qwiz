#include "player.h"

Player::Player(QString const name, QColor const color):
    _name(name),
    _color(color),
    _points(0),
    _isActive(false),
    _hasAttempted(false)
{}
