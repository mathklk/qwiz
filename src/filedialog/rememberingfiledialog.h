#ifndef REMEMBERINGFILEDIALOG_H
#define REMEMBERINGFILEDIALOG_H

#include <QFileDialog>

namespace RememberingFileDialog {
    QString getOpenFileName(
        QWidget* parent = nullptr,
        QString const& key = QString(),
        QString const& caption = QString(),
        QString const& filter = QString(),
        QString* selectedFilter = nullptr,
        QFileDialog::Options options = QFileDialog::Options()
    );
};

#endif // REMEMBERINGFILEDIALOG_H
