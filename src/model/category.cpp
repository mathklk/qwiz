#include "Category.h"

Category::Category(QString const name, QList<Question> const questions):
    QList<Question>(questions),
    _name(name)
{}
