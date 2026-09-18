#include "JsonBoard.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QBuffer>
#include <QImageReader>
#include <QDir>
#include <QObject>

QImage imgFromB64(QString const& str) {
    QByteArray imgData = QByteArray::fromBase64(str.toUtf8());
    QBuffer buffer(&imgData);
    buffer.open(QIODevice::ReadOnly);
    QImageReader reader(&buffer);
    return reader.read();
}

QImage loadImage(QString const& str, QDir const& dir) {
    if (str.startsWith("b64:")) {
        // str contains b64 data
        return imgFromB64(str.sliced(4));
    } else {
        // str contains a filepath relative to dir
        return QImage(dir.absoluteFilePath(str));
    }
}

Board fromJson(QJsonObject const& root, QDir const& dir) {
    int version = 1;
    if (root.contains("version")) {
        version = root["version"].toInt();
    }
    int const mult = root["mult"].toInt();
    QList<Category> categories;
    for (QJsonValue const& cVal : root["categories"].toArray()) {
        QJsonObject const& c = cVal.toObject();
        QList<Question> questions;
        int i = 1;
        for (QJsonValue const& qVal : c["questions"].toArray()) {
            QJsonObject const& q = qVal.toObject();
            Question question(q["text"].toString(), q["solution"].toString(), i++*mult);
            if (version >= 2 and q.contains("type")) {
                QMap<QString, Question::Type> const typeMap = {
                    { "normal",  Question::Type::normal  },
                    { "numeric", Question::Type::numeric }
                };
                Question::Type type = typeMap.value(q["type"].toString(), Question::Type::invalid);
                if (type == Question::Type::invalid) {
                    throw JsonBoard::JsonException(QObject::tr("Invalid question type \"%1\", possible values are: %2")
                        .arg(q["type"].toString(), typeMap.keys().join(", "))
                    );
                }
                question.setType(type);
            }
            if (q.contains("image")) {
                question.setImage(loadImage(q["image"].toString(), dir));
            }
            if (q.contains("solutionImage")) {
                question.setSolutionImage(loadImage(q["solutionImage"].toString(), dir));
            }
            questions << question;
        }
        categories << Category(c["name"].toString(), questions);
    }

    return Board(root["name"].toString(), categories);
}

Board JsonBoard::fromJsonFile(QFile file) {
    if (!file.open(QIODevice::ReadOnly)) {
        throw JsonException(QObject::tr("File \"%1\" couldn't be opened for reading").arg(file.fileName()));
    }

    QByteArray const data = file.readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        // Determine Line/Column to tell user where the error is
        int line = -1, column = -1;
        for (int i = 0; i < parseError.offset && i < data.size(); ++i) {
            if (data[i] == '\n') {
                ++line;
                column = -1;
            }
            ++column;
        }
        throw JsonException(QObject::tr("JSON parse error \"%1\" @ line %2, column %3")
            .arg(parseError.errorString(), QString::number(line), QString::number(column))
        );
    }

    // Json can contain paths relative to json itself
    QDir dir(file.fileName());
    dir.cdUp(); // remove filename, just base directory

    return fromJson(doc.object(), dir);
}
