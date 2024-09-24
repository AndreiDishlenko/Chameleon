#ifndef AENGINE_H
#define AENGINE_H

#include <QObject>
#include <QtScript>
//#include "fdataset.h"

class AEngine: public QScriptEngine {
Q_OBJECT
public:
    AEngine(QWidget *parent);
    ~AEngine();
    QWidget *widget;
//    AApplication *appwidget;
    void initEngine(QObjectList objList);
    void execFormScript();

    static QScriptValue debug(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue importExtension(QScriptContext *context, QScriptEngine *engine);
    QString getPteScripts(QWidget *widget);

    void addEngineBaseObject(QObject *obj);
    void addEngineObject(QObject *obj);
private slots:

public slots:
    void createConnects(QObject *object);
    void dropAllConnections(QObject *obj);

    void uniLoadSlot(QObject *obj=0);
    void uniCellChangeSlot(int curRow, int curColumn, int prevRow, int prevColumn);
    void tabWidgetClickSlot(int i=0);
    void uniClickSlot(QObject *obj=0);
    void uniDblClickSlot(QObject *obj=0);
    void uniUpdateSlot(QObject *obj=0);
    void uniChangeSlot(QObject *obj=0);
    void uniEditSlot(QObject *obj=0);
    void uniDeleteSlot(QObject *obj=0);
    void uniEditFinishSlot(QObject *obj=0);
    void uniLostFocusSlot(QObject *obj=0);
    void uniPopupMenuSlot();

    void include(QString filename);
    QObject * prototype(QString moduleName);

    bool exec(QString str, QString sender="");
};

#endif // AENGINE_H
