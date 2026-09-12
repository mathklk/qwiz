#include "KeyboardBuzzer.h"

#include <QAction>

KeyboardBuzzer::KeyboardBuzzer(QWidget* parent, QList<QKeySequence> keys):
    BuzzerBase(parent)
{
    int n = 0;
    for (auto const& key : keys) {
        QAction* action = new QAction(parent);
        connect(action, &QAction::triggered, this, [this, n](){
            emit buzz(n);
        });
        action->setShortcut(key);
        parent->addAction(action);
        ++n;
    }
}
