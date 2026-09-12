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

Question* Board::at(QPair<int, int> const& ciqi) {
    // category index, question index
    auto const& [ci, qi] = ciqi;
    if (ci > size()) {
        qWarning() << "Board::at called with ci=" << ci;
        return nullptr;
    }
    if (qi > (*this)[ci].size()) {
        qWarning() << "Board::at called with qi=" << qi;
        return nullptr;
    }
    return &(*this)[ci][qi];
}
