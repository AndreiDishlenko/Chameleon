#include "aimage.h"
#include "qlabel.h"
#include <QDebug>

AImage::AImage(QWidget *parent) : QLabel(parent) {
    this->setBackgroundRole(QPalette::Base);
    this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    this->setScaledContents(true);
    this->setStyleSheet("border:1px solid black;");
    this->resize(QSize(100,100));
    this->setGeometry(QRect());
    scaleToSize = false;
//    QObject::connect(this)
}


void AImage::applyPixmap() {
    if (!scaleToSize) {
        this->setPixmap( QPixmap::fromImage(image_img.copy(0,0,this->width(), this->height())) );
    } else {
        this->setPixmap( QPixmap::fromImage(image_img) );
    }

}

bool AImage::setImage(QVariant filename) {
    if (image_img.load(filename.toString())) {
        applyPixmap();
        imagefile = filename.toString();
    } else {
        qDebug() << "AImage::setImage can't load '"+filename.toString()+"'";
        imagefile = "";
        return false;
    }
    return true;
}

QString AImage::getImageFile() {
    return imagefile;
}

QSize AImage::getOriginalSize() {
    return QSize(image_img.width(), image_img.height());
}
