#ifndef CATEGORY_H
#define CATEGORY_H

#include "Question.h"

#include <QList>

class Category: public QList<Question> {
public:
    using QList::QList;
    explicit Category(QString const name, QList<Question> const questions);

    QString name(void) const { return _name; }

private:
    QString _name;
};

#endif // CATEGORY_H
