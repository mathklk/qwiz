#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>

class ImageLabel : public QLabel
{
    Q_OBJECT
public:
    ImageLabel(QPixmap const& pixmap, QWidget* parent = nullptr);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateScaledPixmap();

private:
    QPixmap _src;
};

#endif // IMAGELABEL_H
