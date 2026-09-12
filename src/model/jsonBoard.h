#ifndef JSONBOARD_H
#define JSONBOARD_H

#include "Board.h"
#include <QFile>
#include <exception>

namespace JsonBoard {
    class JsonException: public std::exception {
    public:
        JsonException(QString const& detail) : _detail(detail) {}
        QString const& detail() const { return _detail; }
    private:
        QString _detail;
    };

    Board fromJsonFile(QFile);
};

#endif // JSONBOARD_H
