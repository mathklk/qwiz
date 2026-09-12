#ifndef PLAYER_H
#define PLAYER_H

#include <QString>
#include <QObject>
#include <QColor>

class Player: public QObject
{
    Q_OBJECT
public:
    Player(QString const name, QColor const);

    QString const& name() const { return _name; }
    void setName(QString const name) { _name = name; emit changed(); }

    QColor const& color() const { return _color; }
    void setColor(QColor const color) { _color = color; emit changed(); }

    qint64 points() const { return _points; }
    void setPoints(qint64 const points)  { _points = points; emit changed(); }

signals:
    void changed();

private:
    QString _name;
    QColor _color;
    qint64 _points;
};

#endif // PLAYER_H
