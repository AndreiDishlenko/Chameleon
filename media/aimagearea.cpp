#include "aimagearea.h"

#include <adataset.h>
#include <adatabase.h>
#include <media/aimage.h>
#include <media/apush.h>
#include <globals.h>

#include <QScrollArea>
#include <QGridLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QtWebEngineWidgets/QtWebEngineWidgets>

AImageArea::AImageArea(QObject *parent) : QObject(parent) {
}

bool AImageArea::createImageScroll(QScrollArea *sa, QString imgStorePath, bool adminMode) {
    qDebug() << "AImageArea::createImageScroll";
    sa->setWidget(new QWidget());
    QLayout * oldLayout = sa->widget()->layout();
    sa->widget()->setLayout(new QGridLayout());
    if (oldLayout) oldLayout->~QLayout();
    QGridLayout *layout = static_cast<QGridLayout*>(sa->widget()->layout());
//    layout->setAlignment(Qt::AlignTop|Qt::AlignLeft);

    int imgWidth = sa->property("imgWidth").toInt();
    int imgHeight = sa->property("imgHeight").toInt();
    QString orientation = sa->property("orientation").toString();

    if (imgWidth==0 && imgHeight==0) {return false;}

    QString sqlString = ADataset::readDataSource(sa);
    if (sqlString=="") {return false;}
    QList< QMap<QString, QVariant> > records = ADatabase::selectList(sqlString);
//    QImage *img = new QImage();

    int width=0;
    int height=0;
    QSize size;
    QString id;
    int pos=0;
    if (adminMode) { pos=1; }

    for (int i=0;i<records.count();i++) {
        AImage *ai = new AImage(sa);
        ai->setProperty("scaleToSize", true);
        ai->setObjectName("obj"+id);
        qDebug() << imgStorePath+records.at(i).value("filename").toString();
        ai->setImage(imgStorePath+records.at(i).value("filename").toString());
//        img->load(glob_vars["ServerFileStore"]+records.at(i).value("filename").toString());
//        size = img->size();
        size = ai->getOriginalSize();
        if (imgHeight==0) {
            width = imgWidth;
            height = width*size.height()/size.width();
        } else if (imgWidth==0) {
            height = imgHeight;
            width = height*size.width()/size.height();
        } else {
            width = imgWidth;
            height = imgHeight;
        }
        if (adminMode) {
            QLineEdit *le = new QLineEdit();
            le->setStyleSheet("background-color:#f0f0f0;");
            le->setText(records.at(i).value("filename").toString());
            le->setMinimumSize(width,25);
            le->setMaximumSize(width,25);
            le->resize(QSize(width,25));
            if (orientation=="horizontal") {
                layout->addWidget(le, 0, i);
            } else if (orientation=="vertical") {
                layout->addWidget(le, i, 0);
            }
            le->show();
        }
//                APush *wv = new APush(sa);
//                QWebView *wv = new QWebView();
//                wv->setUrl(glob_vars["ServerFileStore"]+records.at(i).value("filename").toString());
//                wv->setHtml("<img width="+QString::number(width)+" height="+QString::number(height)+" vspace=0 hspace=0 style='padding:0px margin:0px;' src='"+glob_vars["ServerFileStore"]+records.at(i).value("filename").toString()+"' />");
//                wv->setProperty("imgname", records.at(i).value("filename").toString());
//                wv->setHtml("<img width="+QString::number(width)+" height="+QString::number(height)+" vspace=0 hspace=0 style='padding:0px margin:0px;' src='d:/action.png' />");
        ai->setMinimumSize(width,height);
        ai->setMaximumSize(width,height);
        ai->resize(QSize(width,height));

        if (orientation=="horizontal") {
            layout->addWidget(ai, pos, i);
        } else if (orientation=="vertical") {
            layout->addWidget(ai, i, pos);
        }
        ai->show();
//        QObject::connect(wv, SIGNAL(clicked()), this, SLOT(preview()));
        if (adminMode) {
            QCheckBox *chb = new QCheckBox();
            chb->setObjectName("chb"+records.at(i).value("id").toString());
            chb->setText("Изображение "+QString::number(i));
            if (orientation=="horizontal") {
                layout->addWidget(chb, 2, i);
            } else if (orientation=="vertical") {
                layout->addWidget(chb, i, 2);
            }
        chb->show();
        }
    }
    QSpacerItem * vSpacer = new QSpacerItem(width, height, QSizePolicy::Expanding, QSizePolicy::Expanding);
    if (orientation=="horizontal") {
        layout->addItem(vSpacer, pos, records.count());
    } else if (orientation=="vertical") {
        layout->addItem(vSpacer, records.count(), pos);
    }
//    qDebug() << "AImageArea::createImageScroll end";
    return true;
}


bool AImageArea::scrollToWV(QScrollArea *sa, QString id) {
    QWebEngineView *wv = sa->findChild<QWebEngineView*>("obj"+id);
    if (!wv) {return false;}
    qDebug() << "AApplication::scrollToWV" << id << wv->pos().x() << wv->x() << wv->pos().y();
    sa->ensureVisible(wv->pos().x(), wv->pos().y());
    sa->ensureVisible(wv->pos().x() + wv->size().width(), wv->pos().y() + wv->size().height());
    return true;
}

#include <QScrollBar>;
bool AImageArea::scrollFor(QScrollArea *sa, int horizontal, int vertical) {
    QScrollBar *vsb = sa->verticalScrollBar();
    vsb->setValue(vsb->value()+vertical);
//    QScrollBar *hsb = sa->verticalScrollBar();
    vsb->setValue(vsb->value()+horizontal);
//    int curpos = sa->property("curpos").toInt();
//    height
//    sa->widget()->scroll(horizontal, vertical);
    return true;
}

#include <QDialog>
#include <QDesktopWidget>
#include <QApplication>
#include <QScreen>
void AImageArea::preview() {
    QObject *obj = QObject::sender();
    QString imgname = obj->property("imgname").toString();

    QDesktopWidget desktop;
    QRect rect = desktop.availableGeometry(desktop.primaryScreen());

    int dialog_width = rect.width();
    int dialog_height = rect.height()+40;

    QImage *img = new QImage();
    if (!img->load(globals["ServerFileStore"]+imgname)) {
        qWarning() << "AImage::preview - can't open image" << globals["ServerFileStore"]+imgname;
        return;
    }

    QSize size = img->size();

    int width = dialog_width - 40;
    int height = dialog_height - 40;
    float ratio_img = size.width() * 100 /size.height();
    float ratio_area = width * 100 / height;
    int img_width=0, img_height=0;

    if (ratio_img > ratio_area) {
        img_width = width;
        img_height = size.height() * width / size.width();
    } else if (ratio_img < ratio_area) {
        img_width = size.width() * height / size.height();
        img_height = height;

    } else {
        img_width = width;
        img_height = height;
    }

//    qDebug() << "AImage::preview()" << imgname << "size" << dialog_width << dialog_height;
    QDialog *dlg = new QDialog();
    dlg->setStyleSheet("background-color:black;");
    dlg->setModal(true);
    dlg->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    dlg->resize(QSize(dialog_width, dialog_height));

    APush *wv = new APush(dlg);
    wv->setStyleSheet("background-color:black;");
    wv->resize(QSize(width, height));
    wv->move(QPoint(20, 20));
    wv->setHtml("<img width="+QString::number(img_width)+" height="+QString::number(img_height)+" vspace=0 hspace=0 style='padding:0px margin:0px;' src='"+globals["ServerFileStore"]+imgname+"' />");
    QObject::connect(wv, SIGNAL(clicked()), dlg, SLOT(accept()));
    dlg->exec();
}

