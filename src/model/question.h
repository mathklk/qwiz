#ifndef QUESTION_H
#define QUESTION_H

#include "Player.h"

#include <QList>
#include <QImage>

class Question {
public:
    enum class Type {
        invalid,
        normal,
        numeric
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
    Player         const* solver()           const { return _solver;                  }
    QList<Player*> const& failers()          const { return _failers;                 }


    void activate();
    void addFailer(Player* player) { _failers << player; }
    void finish(Player* solver);
    void setType(Type type) { _type = type; }
    void setImage(QImage const& img) { _image = img; }
    void setSolutionImage(QImage const& img) { _solutionImage = img; }

private:
    State _state = State::hidden;
    Player* _solver = nullptr;
    QList<Player*> _failers;

    Type _type = Type::normal;
    QString _text;
    QImage _image;
    QString _solution;
    QImage _solutionImage;
    int _points;
};

#endif // QUESTION_H
