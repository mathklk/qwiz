#ifndef QUESTION_H
#define QUESTION_H

#include "Player.h"

#include <QList>
#include <QImage>

class Question {
public:
    enum class State {
        hidden,
        active,
        solved,
        unsolved
    };
public:
    Question(QString const& text, QString const& solution, int const points);

    QString     const& text()             const { return _text;                    }
    bool               hasImage()         const { return !_image.isNull();         }
    QImage      const& image()            const { return _image;                   }
    QString     const& solution()         const { return _solution;                }
    bool               hasSolutionImage() const { return !_solutionImage.isNull(); }
    QImage      const& solutionImage()    const { return _solutionImage;           }
    int         const& points()           const { return _points;                  }
    State       const& state()            const { return _state;                   }
    QString     const& solver()           const { return _solver;                  }
    QStringList const& failers()          const { return _failers;                 }


    void activate();
    void addFailer(QString name) { _failers << name; }
    void finish(QString player, bool solved);
    void setImage(QImage const& img) { _image = img; }
    void setSolutionImage(QImage const& img) { _solutionImage = img; }

private:
    State _state = State::hidden;

    QString _text;
    QImage _image;
    QString _solution;
    QImage _solutionImage;
    int _points;
    QString _solver;
    QStringList _failers;
};

#endif // QUESTION_H
