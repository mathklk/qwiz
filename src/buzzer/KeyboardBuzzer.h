#ifndef KEYBOARDBUZZER_H
#define KEYBOARDBUZZER_H

#include "BuzzerBase.h"

#include <QWidget>

class KeyboardBuzzer: public BuzzerBase
{
public:
    KeyboardBuzzer(QWidget* parent, QList<QKeySequence> keys);
};

#endif // KEYBOARDBUZZER_H
