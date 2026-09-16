#include "imagelabel.h"

ImageLabel::ImageLabel(const QPixmap &pixmap, QWidget* parent):
    QLabel(parent),
    _src(pixmap)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    updateScaledPixmap();
    setAlignment(Qt::AlignCenter);
}

void ImageLabel::resizeEvent(QResizeEvent *event) {
    QLabel::resizeEvent(event);
    updateScaledPixmap();
}

void ImageLabel::updateScaledPixmap() {
    if (!_src.isNull()) {
        QPixmap const scaled = _src.scaled(
            this->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );
        setPixmap(scaled);
    }
}
