#include "amap.h"
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QStyleOption>
#include <QPainter>
#include <QSqlQueryModel>
#include "adataset.h"
#include "afunc.h"
#include "adict.h"

AMap::AMap(QWidget *parent) : QWidget(parent) {
//    this->setParent(parent->parentWidget());
    this->setObjectName("Map");
    widget = parent;
    engine = parent->findChild<AEngine*>("qs");
    canMove = false;
    canResize = false;
    resizeDirection=0;
    resizeMode=false;
    moveMode = false;
    changed = false;
    widget->setMouseTracking(true);
    if (widget->property("allowEdit").toBool()) {allowEdit=true;} else {allowEdit=false;}
    widget->installEventFilter(this);
    widget->setFocusPolicy(Qt::ClickFocus);
}
//void AMap::paintEvent(QPaintEvent *) {
//    QStyleOption opt;
//    opt.init(widget);
//    QPainter p(widget);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, widget);
//}

//////////////////////////////////////////////////
// RESIZE EVENTS
//////////////////////////////////////////////////
void AMap::setCurrentPB(APush *pb) {
//    qDebug() << "AMap::setCurrentPB" << pb;
    if (curPB!=0) {curPB->setStyleSheet("border:1px solid #cccccc;");}
    curPB = pb;
    widget->setFocus();
    curPB->setStyleSheet("border:1px solid #999999;");
}

void AMap::resizeOn(APush *pb) {
    oldpos = pb->pos();
    oldsize = pb->size();
    oldmpos = QCursor::pos();

    resizeMode=true;
    canResize=false;
}
void AMap::moveOn(APush *pb) {
    oldpos = pb->pos();
    oldmpos = QCursor::pos();

    moveMode=true;
}
void AMap::resizeOff() {
    resizeMode=false;
    if (curPB->pos()!=oldpos || curPB->size()!=oldsize){changed=true;}
}
void AMap::moveOff() {
    moveMode=false;
    if (curPB->pos()!=oldpos || curPB->size()!=oldsize){changed=true;}
}
bool AMap::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseMove && allowEdit) {
        if (resizeMode) {
            int mx = 0, my=0;
            if (resizeDirection==1) {mx = -1; my = -1;}
            if (resizeDirection==2 || resizeDirection==3) {my=-1;}
            if (resizeDirection==7 || resizeDirection==8) {mx=-1;}
                int movex = oldpos.x()-mx*(QCursor::pos().x()-oldmpos.x());
                int movey = oldpos.y()-my*(QCursor::pos().y()-oldmpos.y());
            if (resizeDirection==3 || resizeDirection==4) {mx=1;}
            if (resizeDirection==6 || resizeDirection==7) {my=1;}
            if (resizeDirection==5) {mx=1; my=1;}
                int deltax = oldsize.width()+mx*(QCursor::pos().x()-oldmpos.x());
                int deltay = oldsize.height()+my*(QCursor::pos().y()-oldmpos.y());
            if ((deltax>10) && (deltay>10)) {
                curPB->move(QPoint(movex, movey));
                curPB->resize(QSize(deltax, deltay));
            }
        }
        if (moveMode) {
            curPB->move(QPoint(oldpos.x()+QCursor::pos().x()-oldmpos.x(), oldpos.y()+QCursor::pos().y()-oldmpos.y()));
        }
    }
    if (event->type() == QEvent::MouseMove && objlist.contains(obj) && !resizeMode  && allowEdit) {
        APush *pb = static_cast<APush*>(obj);
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        // 1 LeftUp
        if (mouseEvent->x()<=4 && mouseEvent->y()<=4) {pb->setCursor(Qt::SizeFDiagCursor); canResize=true; canMove=false; resizeDirection=1;}
        // 2 Top
        if (mouseEvent->x()>4 && mouseEvent->x()<=pb->width()-4 && mouseEvent->y()<=4) {pb->setCursor(Qt::SizeVerCursor); canResize=true; canMove=false; resizeDirection=2;}
        // 3 RightTop
        if (mouseEvent->x()>pb->width()-4 && mouseEvent->y()<=4) {pb->setCursor(Qt::SizeBDiagCursor); canResize=true; canMove=false; resizeDirection=3;}
        // 4 Right
        if (mouseEvent->x()>pb->width()-4 && mouseEvent->y()>4 && mouseEvent->y()<=pb->height()-4) {pb->setCursor(Qt::SizeHorCursor); canResize=true; canMove=false; resizeDirection=4;}
        // 5 RightBottom
        if (mouseEvent->x()>pb->width()-4 && mouseEvent->y()>pb->height()-4) {pb->setCursor(Qt::SizeFDiagCursor); canResize=true; canMove=false; resizeDirection=5;}
        // 6 Bottom
        if (mouseEvent->x()>4 && mouseEvent->x()<=pb->width()-4 && mouseEvent->y()>pb->height()-4) {pb->setCursor(Qt::SizeVerCursor); canResize=true; canMove=false; resizeDirection=6;}
        // 7 LeftBottom
        if (mouseEvent->x()<=4 && mouseEvent->y()>pb->height()-4) {pb->setCursor(Qt::SizeBDiagCursor); canResize=true; canMove=false; resizeDirection=7;}
        // 8 Left
        if (mouseEvent->x()<=4 && mouseEvent->y()>4 && mouseEvent->y()<=pb->height()-4) {pb->setCursor(Qt::SizeHorCursor); canResize=true; canMove=false; resizeDirection=8;}

        if (mouseEvent->x()>4 && mouseEvent->x()<=pb->width()-4 && mouseEvent->y()>4 && mouseEvent->y()<=pb->height()-4) {
            pb->setCursor(Qt::PointingHandCursor);
            canResize=false;
            resizeDirection=0;
            canMove=true;
        }
    }
    if (event->type() == QEvent::MouseButtonPress && objlist.contains(obj)) {
//        qDebug() << "MousePress";
        APush *pb = static_cast<APush*>(obj);
        this->setCurrentPB(pb);
        if (canResize) {this->resizeOn(pb);}
        if (canMove) {this->moveOn(pb);}
    }
    if (event->type() == QEvent::MouseButtonRelease) {
        if (resizeMode) {this->resizeOff();}
        if (moveMode) {this->moveOff();}
    }
    if (event->type() == QEvent::Leave) {
        canMove = false;
    }
    if (event->type() == QEvent::KeyPress  && allowEdit) {

        QKeyEvent * keyevent = static_cast<QKeyEvent*>(event);
        qDebug() << keyevent->key();
        int x=0, y=0;
        if (keyevent->key()==Qt::Key_Left) {x=-1;}
        if (keyevent->key()==Qt::Key_Right) {x=1;}
        if (keyevent->key()==Qt::Key_Up) {y=-1;}
        if (keyevent->key()==Qt::Key_Down) {y=1;}
        if (x!=0 || y!=0) {
            curPB->move(QPoint(curPB->pos().x()+x, curPB->pos().y()+y));
        }
    }

    return false;
}

//////////////////////////////////////////////////
// Work with Dataset
//////////////////////////////////////////////////
void AMap::clear() {
    int count = objlist.count();
    for (int i=0; i<count; i++) {
        objlist.at(i)->~QObject();
    }
    objlist.clear();
    changed=false;
    deletedlist.clear();
}
void AMap::load() {
    QSqlQueryModel *model = ADataset::readSourceModel(widget);
    int pos=0;
    QString name;
    this->clear();
    for (int i=0;i<model->rowCount();i++) {
        name = AFunc::getQueryModelValue(model,i,"name").toString();
        QPoint point = QPoint(AFunc::getQueryModelValue(model,i,"x").toInt(), AFunc::getQueryModelValue(model,i,"y").toInt());
        QSize size = QSize(AFunc::getQueryModelValue(model,i,"width").toInt(), AFunc::getQueryModelValue(model,i,"height").toInt());
        pos = this->addObject(name, point, size);
        APush *pb = static_cast<APush*>(objlist.at(pos));
        pb->setProperty("id", AFunc::getQueryModelValue(model,i,"id").toInt());
        pb->setProperty("name", AFunc::getQueryModelValue(model,i,"name").toString());
        pb->setProperty("img", AFunc::getQueryModelValue(model,i,"img").toString());
        pb->setProperty("idhall", AFunc::getQueryModelValue(model,i,"idhall"));
        this->updateCurrentButton(pb);
        //        pb->move(QPoint(x, y));
//        pb->resize(QSize(width, height));
//        pb->setProperty("onClick", widget->property("onClick"));
//        engine->createConnects(pb);
        curPB = pb;
    }
    changed=false;
    deletedlist.clear();
    model->~QSqlQueryModel();
}

bool AMap::save() {
    ADict *dict = new ADict(widget->property("DataSource").toString());
    int id=0;
    QMap<QString, QVariant> updateMap;
    for (int i=0; i<objlist.count();i++) {
        APush *pb = static_cast<APush*>(objlist.at(i));
        updateMap.clear();
        id = pb->property("id").toInt();
        updateMap["name"] = pb->property("name").toString();
        updateMap["img"] = pb->property("img").toString();
        updateMap["idhall"] = pb->property("idhall").toInt();
        updateMap["x"] = pb->pos().x();
        updateMap["y"] = pb->pos().y();
        updateMap["width"] = pb->size().width();
        updateMap["height"] = pb->size().height();
        if (id!=0) {
            dict->openRecord(id);
            dict->updateRecord(updateMap);
            dict->saveChanges();
        } else {
            dict->clearStack();
            dict->newRecord();
            dict->updateRecord(updateMap);
            dict->saveChanges();
            pb->setProperty("id", dict->getCRValue("id", 0));
//            qDebug() << "AMap::save()" << dict->getCRValue("id", 0);
        }
    }
//    qDebug() << "deletelist" << deletedlist;
    for (int i=0;i<deletedlist.count();i++) {
        dict->openRecord(deletedlist.at(i));
        dict->setCRValue("ifDeleted", "1");
        dict->saveChanges();
    }
    deletedlist.clear();
    changed=false;
    return true;
}

//////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////
int AMap::addObject(QString name, QPoint point, QSize size) {
//    qDebug() << "AMap::addObject" << name;
    APush *pb = new APush(widget);
    objlist << pb;
    pb->move(point);
    pb->resize(size);
//    pb->setHtml(name);
    pb->setMouseTracking(true);
    pb->installEventFilter(this);
    pb->setProperty("onClick", widget->property("onClick"));
    pb->setProperty("id","");
    pb->setProperty("img","");
    pb->setProperty("idhall","");
    pb->setProperty("type", "map");

    engine->createConnects(pb);
    pb->show();
    curPB = pb;
    changed = true;
    return objlist.count()-1;
}
void AMap::deleteObject(QObject *obj) {
//    int pos = objlist.indexOf(obj);
    objlist.removeAt(objlist.indexOf(obj));
    int id = obj->property("id").toInt();
    qDebug() << "deleteObject" << id;
    if (id!=0) {deletedlist << id;}
    if (curPB==obj) {curPB = 0;}
    obj->~QObject();
    changed=true;
}

QObject * AMap::getSelectedObject(int id) {
//    qDebug() << "AMap::getSelectedObject" << id;
    QObject *result = 0;
    if (id==-1) {
        if (curPB) {result = curPB;}
    } else {
        if (id < objlist.count()) {result = objlist.at(id);}
    }
    return result;
}

int AMap::getSelected(int id) {
    return 0;
}

void AMap::select(int objid) {
    int id=0;
    for (int i=0;i<objlist.count();i++) {
        id = objlist.at(i)->property("id").toInt();
        if (id==objid) {
           this->setCurrentPB(static_cast<APush*>(objlist.at(i)));
        }
    }
}

void AMap::setChanged(bool flag) {
    changed = flag;
}
bool AMap::getChanged() {
    return changed;
}

void AMap::updateCurrentButton(APush *apush, QString text) {
//    qDebug() << "AMap::updateCurrentButton";
    APush *pb;
    if (apush==0) {pb=curPB;}
    else {pb = apush;}
    QSize size = pb->size();
    QString name = pb->property("name").toString();
    QString img = pb->property("img").toString();
    QString background, image;
    if (!img.isEmpty()) {
        background = "background-image: url(\""+img+"\");";
    } else {
        background = "background-image: url(./data/ui/restaurant/pic/table.png);";
    }
    QString content;
    QString sizes;
    sizes.append(" width=").append(QString::number(size.width())).append(" height=").append(QString::number(size.height()));
    if (text=="") {text = name;}
    content.append("<TABLE STYLE=").append("'").append(background).append("'").append(sizes).append(" ><tr><td ALIGN=CENTER style='padding-top:10px; font: 11px Tahoma;' >").append(text).append("</td></tr></TABLE>");
//    qDebug() << "AMap::updateCurrentButton" << content << background;
    pb->setHtml(content);
}

int AMap::count() {
    return objlist.count();
}
