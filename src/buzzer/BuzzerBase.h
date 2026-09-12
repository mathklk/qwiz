#ifndef BUZZERBASE_H
#define BUZZERBASE_H

#include <QObject>

class BuzzerBase : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

signals:
    void buzz(int);
};

#endif // BUZZERBASE_H
