#ifndef AFORM_H
#define AFORM_H

#include <QObject>
#include "aengine.h"
#include "adict.h"

#include "forms/atreeview.h"
#include "forms/atreewidget.h"
#include "forms/atablewidget.h"
#include "pos/amap.h"
#include <forms/awebwidget.h>

//2#include "media/acontent.h"

class AForm : public QObject
{
    Q_OBJECT
public:
    explicit AForm(QWidget *parent = 0);
    ~AForm();

    QWidget *widget;
    AEngine *engine;

    ATreeView *treeview;
    ATreeWidget *treewidget;   
    ATableWidget *tablewidget;
    AWebWidget *webwidget;


    AMap *map;
    QString connection;
    //2AContent *content;


    void createObjectsConnects();

    static QObjectList getFormAllObjects(QObject *obj);         // Получить ВСЕ объекты формы (кроме системных qt_, AForm, FEngine) "
    static QObjectList getWidgetAllObjects(QObject *obj);
    static QObjectList getFormDataObjects(QObject *obj);        // Получить объекты формы (кроме widget'ов)
    static QObjectList getSubWidgets(QObject *obj);             // Формирование списка контейнеров формы (QWidget, QStackedWidget, ...)
    static QObjectList getSubForms(QObject *obj);

    void setPopupMenu (QObject *obj);

    QString getPteScripts();

private:
    QString formType;

    ADict *aDict;
    QObjectList allobjlist;
    QObjectList dataobjlist;
    QObjectList widgetobjlist;
    QObjectList tablist;
    QObjectList scalable_objects;


    QMap<QString, QVariant> record;

    bool eventFilter(QObject *obj, QEvent *event);

    void createTabOrder();
    //    void createEnterConnect(QObject *obj);

    bool loadFormData();

    bool saveFormData();

public slots:

    void updateAllObjects();


    void updateSubForms();
    void saveSubForms();
    void load();

    QObject * getParentForm(QWidget *wgt);

    void update();
    bool save();
    void saveData(); ///////// ЗАМЕНИТЬ ВЕЗДЕ НА save();

    QVariant getSelected();
    void select(int id);

    void clear();
    void reset();


    QMap<QString, QVariant> getFormData();
    bool updateFormData(QMap<QString, QVariant> updateMap);

    void setRecordValue(QString fieldName, QVariant data);

    bool changed();
    bool checkNull();
    void script(QString script);

    void popupMenuSlot(QPoint point);

    void setDonsaveFalse(int i);
    void switchNextItem(QObject *obj);
    void setTabEnabled(QTabWidget *tab, QString widgetname, bool state=true);

    void scaleSubObjects(float scale);

signals:
    void mouseclick();

};

#endif // AFORM_H
