#ifndef PLAYER_H
#define PLAYER_H

#include <QString>

class Player
{
public:
    Player(QString const name);

    QString const& name() { return _name; }
    void setName(QString const name) { _name = name; }

    qint64 points() { return _points; }
    void setPoints(qint64 const points) { _points = points; }

private:
    QString _name;
    qint64 _points;
};

#endif // PLAYER_H
