#include "Question.h"

Question::Question(QString const& text, QString const& solution, int const points)
    : _text(text)
    , _solution(solution)
    , _points(points)
{}

void Question::activate() {
    if (_state != State::hidden) {
        return;
    }
    _state = State::active;
}

void Question::finish(QString player, bool solved) {
    if (_state != State::active) {
        return;
    }
    if (solved) {
        _solver = player;
    }
    _state = solved ? State::solved : State::unsolved;
}
