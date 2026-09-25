#ifndef QUESTION_H
#define QUESTION_H

#include "Player.h"

#include <QList>
#include <QImage>
#include <QMap>

class Question {
public:
    enum class Type {
        invalid,
        normal,
        estimate
    };

    enum class State {
        hidden,
        active,
        finished
    };
public:
    Question(QString const& text, QString const& solution, int const points);

    Type                  type()             const { return _type;                    }
    QString        const& text()             const { return _text;                    }
    bool                  hasImage()         const { return !_image.isNull();         }
    QImage         const& image()            const { return _image;                   }
    QString        const& solution()         const { return _solution;                }
    bool                  hasSolutionImage() const { return !_solutionImage.isNull(); }
    QImage         const& solutionImage()    const { return _solutionImage;           }
    int                   points()           const { return _points;                  }
    State                 state()            const { return _state;                   }
    QList<Player*> const& solvers()          const { return _solvers;                 }
    QList<Player*> const& failers()          const { return _failers;                 }


    void activate();
    void addFailer(Player* player) { _failers << player; }
    void finish(QList<Player*> solvers);
    void setType(Type type) { _type = type; }
    void setImage(QImage const& img) { _image = img; }
    void setSolutionImage(QImage const& img) { _solutionImage = img; }

private:
    State _state = State::hidden;
    QList<Player*> _solvers;
    QList<Player*> _failers;

    Type _type = Type::normal;
    QString _text;
    QImage _image;
    QString _solution;
    QImage _solutionImage;
    int _points;
};

#endif // QUESTION_H
