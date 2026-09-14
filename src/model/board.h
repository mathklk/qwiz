#ifndef BOARD_H
#define BOARD_H

#include "Category.h"

#include <QPair>

class Board: public QList<Category> {
public:
    using QList::QList;
    explicit Board(QString title, QList<Category>);;

    static Board Empty(void) { return {}; }

    QString title() const { return _title; }
    int maxNumberOfQuestionsPerCategory() const;
    Question* at(int ci, int qi);
    //Question* at(QPair<int, int> const& ciqi);
    Question* activeQuestion();

private:
    QString _title;
};

#endif // BOARD_H
