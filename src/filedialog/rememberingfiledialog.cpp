#include "RememberingFileDialog.h"

#include <QSettings>

QString RememberingFileDialog::getOpenFileName(
    QWidget* parent,
    QString const& key_,
    QString const& caption,
    QString const& filter,
    QString* selectedFilter,
    QFileDialog::Options options
) {
    QString key = "RememberingFileDialogDir";
    if (not key_.isNull() and not key_.isEmpty()) {
        key += " " + key_;
    }
    key.replace("/", "");
    key.replace("\\", "");

    QSettings settings;
    QString const dir = settings.value(key).toString();

    QString const file = QFileDialog::getOpenFileName(
        parent,
        caption,
        dir,
        filter,
        selectedFilter,
        options
    );
    if (file.isEmpty() or file.isNull()) {
        return file;
    }
    QDir d;
    settings.setValue(key, d.absoluteFilePath(file));
    return file;
}
