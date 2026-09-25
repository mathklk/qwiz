#ifndef COLOR_H
#define COLOR_H

#include <QColor>

namespace Color {

static inline QColor mix(QList<QColor> const& colors) {
    if (colors.isEmpty()) {
        return QColor(); // invalid
    }
    int r = 0, g = 0, b = 0, a = 0;
    for (const QColor& color : colors) {
        r += color.red();
        g += color.green();
        b += color.blue();
        a += color.alpha();
    }
    return QColor(
        r / colors.size(),
        g / colors.size(),
        b / colors.size(),
        a / colors.size()
    );
}

}; // namespace Color

#endif // COLOR_H
