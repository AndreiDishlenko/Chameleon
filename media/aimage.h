#ifndef AIMAGE_H
#define AIMAGE_H

#include <QLabel>
#include <QVariant>
#include <QImage>
#include <QSize>

class AImage : public QLabel {
    Q_OBJECT
    Q_PROPERTY(QVariant image READ getImageFile WRITE setImage)
    Q_PROPERTY(QVariant scaleToSize READ getScaleToSize WRITE setScaleToSize)

private:
    QImage image_img;
    QString imagefile;
    bool scaleToSize;

public:
    explicit AImage(QWidget *parent = 0);

    void setScaleToSize (QVariant value) {scaleToSize = value.toBool(); this->setImage(imagefile);}
    QVariant getScaleToSize() {return scaleToSize;}

signals:

public slots:
    bool setImage(QVariant filename);
    QString getImageFile();
    QSize getOriginalSize();
    void applyPixmap();

};

#endif // AIMAGE_H
