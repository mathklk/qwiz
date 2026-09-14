#include "Board.h"

#include <QDebug>

Board::Board(QString title, QList<Category> c):
    QList<Category>(c),
    _title(title)
{}

int Board::maxNumberOfQuestionsPerCategory() const {
    int count = 0;
    for (Category const& category : *this) {
        count = qMax(count, category.count());
    }
    return count;
}

//Question* Board::at(QPair<int, int> const& ciqi) {
//    return at(ciqi.first, ciqi.second);
//}

Question* Board::at(int ci, int qi) {
    if (ci > size()) {
        qCritical() << "Board::at called with ci=" << ci;
        return nullptr;
    }
    if (qi > (*this)[ci].size()) {
        qCritical() << "Board::at called with qi=" << qi;
        return nullptr;
    }
    return &(*this)[ci][qi];
}

Question* Board::activeQuestion() {
    for (Category& category : *this) {
        for (Question& question : category) {
            if (question.state() == Question::State::active) {
                return &question;
            }
        }
    }
    qCritical() << "Board::activeQuestion called but no active question found";
    return nullptr;
}
