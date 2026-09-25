#ifndef WIDGETLIST_H
#define WIDGETLIST_H

#include <QList>
#include <QWidget>

template <typename T = QWidget>
class WidgetList : public QList<T*>
{
public:
    using QList<T*>::QList;

    void setEnabled(bool enabled) {
        for (QWidget *const widget : *this) widget->setEnabled(enabled);
    }
    void setVisible(bool enabled) {
        for (QWidget *const widget : *this) widget->setVisible(enabled);
    }
    void setSizePolicy(QSizePolicy::Policy h, QSizePolicy::Policy v) {
        for (QWidget *const widget : *this) widget->setSizePolicy(h, v);
    }

};

#endif // WIDGETLIST_H
