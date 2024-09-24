#ifndef AMAP_H
#define AMAP_H

#include <QWidget>
#include "media/apush.h"
#include "aengine.h"

class AMap : public QWidget
{
    Q_OBJECT
private:
//    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *obj, QEvent *event);

    QWidget *widget;
    AEngine *engine;

    bool allowEdit;

    bool canResize;
    bool canMove;
    int resizeDirection;
    bool resizeMode;
    bool moveMode;
    APush *curPB;
    QPoint oldpos,oldmpos;
    QSize oldsize;
    QList<QObject*> objlist;
    QList<int> deletedlist;
    bool changed;

    void resizeOn(APush *pb);
    void resizeOff();

    void moveOn(APush *pb);
    void moveOff();

    void setCurrentPB(APush *pb);
public:
    AMap(QWidget *parent = 0);

    void clear();
    void load();
    bool save();
    int getSelected(int id=0);

signals:

public slots:
    int addObject(QString name = "Новый", QPoint point = QPoint(10,10), QSize size = QSize(100,100));
    void deleteObject(QObject *obj);

    QObject * getSelectedObject(int id=-1);

    void select(int id);

    void setChanged(bool flag);
    bool getChanged();
    void updateCurrentButton(APush *apush = 0, QString text = "");

    int count();
};

#endif // AMAP_H
