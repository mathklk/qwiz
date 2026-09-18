#include "translation.h"


bool Translation::load(QTranslator *const t, QString const& locale) {
    return t->load(":/i18n/qwiz_" + QLocale(locale).name());
}

QTranslator* Translation::buildTranslator() {
    auto translator = new QTranslator;
    QString const settingsLocale = QSettings().value("locale", SETTINGS_KEY_SYSTEM_LOCALE).toString();
    if (settingsLocale == SETTINGS_KEY_SYSTEM_LOCALE) {
        // Use system ui langues
        for (QString const& locale : QLocale::system().uiLanguages()) {
            if (load(translator, locale)) break;
        }
    } else {
        load(translator, settingsLocale);
    }
    return translator;
}

QStringList Translation::availableLocales() {
    // iterate through i18n dir to determine available locales
    QDir const i18nDir(":/i18n");
    QStringList availableLocales = {
        SETTINGS_KEY_SYSTEM_LOCALE, // option to use the system locale, i.e. dont specify an explicit locale
        "en", // English always available, it's the apps default language and doesnt have a translation file
    };
    for (QString const& fileName : i18nDir.entryList(QStringList("qwiz_*.qm"))) {
        QString const locale = fileName.mid(5, fileName.length() - 8); // remove "qwiz_" and ".qm"
        availableLocales.append(locale);
    }
    return availableLocales;
}

QString Translation::localeNativeName(QString const& locale) {
    if (locale == SETTINGS_KEY_SYSTEM_LOCALE) {
        return QObject::tr("System");
    }
    return QLocale(locale).nativeLanguageName();
}
