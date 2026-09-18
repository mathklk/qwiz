#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QSettings>
#include <QDir>

namespace Translation {

static QString const SETTINGS_KEY_SYSTEM_LOCALE = "system";

bool load(QTranslator *const t, QString const& locale);
QTranslator* buildTranslator();
QStringList availableLocales();
QString localeNativeName(QString const& locale);

} // namespace Translation

#endif // TRANSLATION_H
