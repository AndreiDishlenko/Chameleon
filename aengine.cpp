#include "aengine.h"

#include <QWidget>
#include <QtScript>
#include <QtScriptTools>
#include <QScriptValue>
#include <QObject>
#include <QFile>
#include <QDir>

#include "agui.h"
#include "adataset.h"
#include "adatabase.h"
#include "globals.h"
#include "afunc.h"
#include "adict.h"
#include "aexportxml.h"
#include "docs/abuild.h"
#include "secure/asecure.h"
#include "pos/apos.h"
#include "pos/adiscount.h"
#include "aprro.h"

AEngine::AEngine(QWidget *parent) : QScriptEngine() {
    //qDebug() << "AEngine:AEngine" << parent;
    this->setParent(parent);
    this->setObjectName("qs");
    widget = parent;

    this->globalObject().setProperty("qs", this->newQObject(this));
    this->globalObject().property("qs").setProperty("importExtension", this->newFunction(this->importExtension));
    this->globalObject().setProperty("ui", this->newQObject(widget));
    this->globalObject().property("ui").setProperty("parent", this->newQObject(widget->parent()));
    this->globalObject().setProperty("debug", this->newFunction(this->debug));
    //    AFunc::echoTimer("* AEngine::AEngine 0");

    AGui *gui = new AGui(this);
    this->globalObject().setProperty("AGui", this->newQObject(gui));

    ADataset *dataset = new ADataset(this);
    this->globalObject().setProperty("ADataset", this->newQObject(dataset));

    ADatabase *database = new ADatabase(this);
    this->globalObject().setProperty("ADatabase", this->newQObject(database));

    AFunc *func = new AFunc(this);
    this->globalObject().setProperty("AFunc", this->newQObject(func));


//    ADict *adict = new ADict("","",this);
//    this->globalObject().setProperty("ADict", this->newQObject(adict));


//    AFunc::echoTimer("* AEngine::AEngine 6");
//    QScriptEngineDebugger debugger;
//    debugger.attachTo(this);
//    debugger.action(QScriptEngineDebugger::InterruptAction)->trigger();
}

#include "media/aimage.h"
AEngine::~AEngine() {
    //    qDebug() << "~AEngine()";
}


//#include "aprinter.h"
#include "docs/adoc.h"
#include "igui.h"
#include "pos/acdisplay.h"
#include "media/aimage.h"
#include "media/aimagearea.h"
#include "media/avideo.h"
//AApplication *app = new AApplication();
#include <QTimer>
#include "forms/atreeview.h"
#include "avariant.h"

QObject * AEngine::prototype(QString moduleName) {
    QObject *obj;
        //    QMap<QString, QStringList> dllList;
        //    if (dllList[app->name()].contains(moduleName)) {
        // Globals
        if (moduleName=="ASecure") { obj = new ASecure(this); }
        if (moduleName=="ATimer") { obj = new QTimer(this); }
         // POS
        if (moduleName=="APos") { obj = new APos(this); }
        if (moduleName=="ADiscount") { obj = new ADiscount(this); }
        if (moduleName=="ACDisplay") { obj = new ACDisplay(this); }
        if (moduleName=="AExport") { obj = new AExportXml(this); }
        //if (moduleName=="APrinter") { obj = new APrinter(this); }
        // Documents
        if (moduleName=="ADoc") { obj = new ADoc(this, "AE:prototype("+widget->objectName()+")"); }
        // Info
        // if (moduleName=="AImage") { obj = new AImage(this); }
        if (moduleName=="AImageArea") { obj = new AImageArea(this); }
        if (moduleName=="AVideo") { obj = new AVideo(this); }
        if (moduleName=="iGui") { obj = new iGui(this); }
        if (moduleName=="Timer") { obj = new QTimer(this); }
        if (moduleName=="AVariant") { obj = new AVariant(this); }
        if (moduleName=="APrro") { obj = new APrro(this); }

    return obj;
}

void AEngine::include(QString filename) {
    filename = AFunc::unionFolderNames(globals["configurationPath"], filename);
    QFile file(filename);
    if (!file.open(QFile::ReadOnly)) {
        qDebug() << "Error AEngine::include. Can't open "+filename+". "+file.errorString();
    }
    QString script = file.readAll();
    file.close();
    this->exec(script);
}

void AEngine::createConnects(QObject *object) {
    //qDebug() << "AEngine::createConnects " << object->objectName();
    QString objClassName = object->metaObject()->className();
    //qDebug() << "AEngine::createConnets " << object->property("onClick") << objClassName;
    if (object->property("onCellChange").isValid()) {
        if (objClassName=="QTableWidget") {QObject::connect(object,SIGNAL(currentCellChanged(int, int, int, int)),this,SLOT(uniCellChangeSlot(int, int, int, int))); }
    }
    if (object->property("onClick").isValid()) {
        if (objClassName=="QPushButton" || objClassName=="APush") {QObject::connect(object,SIGNAL(clicked()),this,SLOT(uniClickSlot())); }
        if (objClassName=="QTableWidget") {QObject::connect(object,SIGNAL(cellClicked(int, int)),this,SLOT(uniClickSlot())); }
        if (objClassName=="QTreeWidget") {QObject::connect(object,SIGNAL(itemClicked ( QTreeWidgetItem * , int )),this,SLOT(uniClickSlot())); }
        if (objClassName=="QTableView") {QObject::connect(object,SIGNAL(clicked(QModelIndex)),this,SLOT(uniClickSlot()));}
        if (objClassName=="QTreeView") {QObject::connect(object,SIGNAL(clicked(QModelIndex)),this,SLOT(uniClickSlot()));}
        if (objClassName=="QRadioButton") {QObject::connect(object,SIGNAL(clicked(bool)),this,SLOT(uniClickSlot()));}
        if (objClassName=="QCheckBox") {QObject::connect(object,SIGNAL(clicked(bool)),this,SLOT(uniClickSlot()));}
        if (objClassName=="QTabWidget") {QObject::connect(object,SIGNAL(tabBarClicked(int)),this,SLOT(tabWidgetClickSlot(int)));}

    }
    if (object->property("onDblClick").isValid()) {
        if (objClassName=="QTableWidget") {QObject::connect(object,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(uniDblClickSlot()));}
        if (objClassName=="QTreeWidget") {QObject::connect(object,SIGNAL(itemDoubleClicked ( QTreeWidgetItem *, int )),this,SLOT(uniDblClickSlot()));}
        if (objClassName=="QTableView") {QObject::connect(object,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(uniDblClickSlot()));}
        if (objClassName=="QTreeView") {QObject::connect(object,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(uniDblClickSlot()));}
    }
    if (object->property("onUpdate").isValid()) {
        if (objClassName=="QComboBox") {QObject::connect(object,SIGNAL(currentIndexChanged(int)),this,SLOT(uniUpdateSlot()));}
        if (objClassName=="AComboCheckBox") {QObject::connect(object,SIGNAL(closed()),this,SLOT(uniUpdateSlot()));}
        if (objClassName=="QLineEdit") {QObject::connect(object,SIGNAL(textChanged(QString)),this,SLOT(uniUpdateSlot()));}
        if (objClassName=="QDateEdit") {QObject::connect(object,SIGNAL(dateChanged(QDate)),this,SLOT(uniUpdateSlot()));}
        if (objClassName=="QCheckBox") {QObject::connect(object,SIGNAL(stateChanged(int)),this,SLOT(uniUpdateSlot()));}
        if (objClassName=="QTabWidget") {QObject::connect(object,SIGNAL(currentChanged(int)),this,SLOT(uniUpdateSlot()));}
    }
    if (object->property("onActivated").isValid()) {
        if (objClassName=="QTreeView") {QObject::connect(object,SIGNAL(activated(QModelIndex)),this,SLOT(uniClickSlot()));}
        if (objClassName=="QTableView") {QObject::connect(object,SIGNAL(activated(QModelIndex)),this,SLOT(uniClickSlot()));}

    }
    if (object->property("onTextEdited").isValid()) {
        if (objClassName=="QLineEdit") {QObject::connect(object,SIGNAL(textEdited(QString)),this,SLOT(uniEditSlot()));}
    }
    if (object->property("onEditFinish").isValid()) {
        if (objClassName=="QLineEdit") {QObject::connect(object,SIGNAL(returnPressed()),this,SLOT(uniEditFinishSlot()));}
    }
    if (object->property("onLostFocus").isValid()) {
        if (objClassName=="QComboBox") {QObject::connect(object,SIGNAL(currentIndexChanged(int)),this,SLOT(uniUpdateSlot()));}
        if (objClassName=="QLineEdit") {QObject::connect(object,SIGNAL(editingFinished()),this,SLOT(uniLostFocusSlot()));}
    }
}

void AEngine::dropAllConnections(QObject *obj) {
    obj->disconnect();
}

void AEngine::addEngineBaseObject(QObject *obj) {
    this->globalObject().setProperty(obj->objectName(), this->newQObject(obj));
}
void AEngine::addEngineObject(QObject *obj) {
    this->globalObject().property("ui").setProperty(obj->objectName(), this->newQObject(obj));
}

// Отладочный Debug для engine
QScriptValue AEngine::debug(QScriptContext *context, QScriptEngine *engine) {
    qDebug() << context->argument(0).toString();
    return engine->undefinedValue();
}
QScriptValue AEngine::importExtension(QScriptContext *context, QScriptEngine *engine) {
    return engine->importExtension(context->argument(0).toString());
}

// Универсальные слоты
void AEngine::uniLoadSlot(QObject *obj) {
    //qDebug() << "AEngine::uniLoadSlot" << obj;
    if (obj==0) {obj = QObject::sender();}
    QString onLoad = ADataset::updateStringVariablesFromWidget(obj->property("onLoad").toString(), obj);
        if (onLoad=="") {return;}
    QScriptValue result = this->evaluate(onLoad);
    if (result.isError()) qDebug() << "Script error:" << result.toString();
}

void AEngine::uniCellChangeSlot(int curRow, int curColumn, int prevRow, int prevColumn) {
//    qDebug() << "AEngine::uniCellChangeSlot";// << onClick;
    QObject *obj = obj = QObject::sender();
    obj->setProperty("prevRow", prevRow);
    obj->setProperty("prevColumn", prevColumn);
    QString onCellChange = ADataset::updateStringVariablesFromWidget(obj->property("onCellChange").toString(), obj);
    if (onCellChange=="") {return;}
    QScriptValue result = this->evaluate(onCellChange);
    if (result.isError()) qDebug() << "Script error:" << result.toString();
}
void AEngine::tabWidgetClickSlot(int i) {
    //qDebug() << "tabWidgetClickSlot" << obj;
    QObject * obj = QObject::sender();
    QTabWidget *tw = static_cast<QTabWidget*>(obj);
    uniClickSlot(tw->widget(i));
}

void AEngine::uniClickSlot(QObject *obj) {
    //qDebug() << "AEngine::uniClickSlot";// << onClick;
    if (obj==0) {obj = QObject::sender();}
    QString onClick = ADataset::updateStringVariablesFromWidget(obj->property("onClick").toString(), obj);
    if (onClick=="") {return;}
    QScriptValue result = this->evaluate(onClick);
    if (result.isError()) qDebug() << "Script error:" << result.toString();
}
void AEngine::uniDblClickSlot(QObject *obj) {
    if (obj==0) {obj = QObject::sender();}
    QString onDblClick = ADataset::updateStringVariablesFromWidget(obj->property("onDblClick").toString(), obj);
//    qDebug() << "AEngine::uniDblClickSlot" << obj->objectName() << onDblClick;
    if (onDblClick=="") {return;}
        QScriptValue result = this->evaluate(onDblClick);
    if (result.isError()) qDebug() << "Script error:" << result.toString();
}
void AEngine::uniUpdateSlot(QObject *obj) {
//    qDebug() << "AEngine::uniUpdateSlot";
    //qDebug() << "AEngine::uniUpdateSlot " << obj;
    if (obj==0) {obj = QObject::sender();}
    QString onUpdate = ADataset::updateStringVariablesFromWidget(obj->property("onUpdate").toString(), obj);
        if (onUpdate=="") {return;}
    QScriptValue result = this->evaluate(onUpdate);
    if (result.isError()) qDebug() << "Script error:" << result.toString();
}
void AEngine::uniChangeSlot(QObject *obj) {
//    qDebug() << "AEngine::uniChangeSlot";
//    qDebug() << "AEngine::uniChangeSlot " << obj;
    if (obj==0) {obj = QObject::sender();}
    QString onChange = ADataset::updateStringVariablesFromWidget(obj->property("onChange").toString(), obj);
        if (onChange=="") {return;}
    QScriptValue result = this->evaluate(onChange);
    if (result.isError()) qDebug() << "Script error:" << result.toString();
}
void AEngine::uniEditSlot(QObject *obj) {
    if (obj==0) {obj = QObject::sender();}
    QString onEdit = ADataset::updateStringVariablesFromWidget(obj->property("onTextEdited").toString(), obj);
//    qDebug() << "AEngine::uniEditSlot " << obj << onEdit;
        if (onEdit=="") {return;}
    QScriptValue result = this->evaluate(onEdit);
    if (result.isError()) qDebug() << "Script error:" << result.toString();
}
void AEngine::uniEditFinishSlot(QObject *obj) {
//    qDebug() << "AEngine::uniEditSlot";
//    qDebug() << "AEngine::uniEditSlot " << obj;
    if (obj==0) {obj = QObject::sender();}
    QString onEdit = ADataset::updateStringVariablesFromWidget(obj->property("onEditFinish").toString(), obj);
        if (onEdit=="") {return;}
    QScriptValue result = this->evaluate(onEdit);
    if (result.isError()) qDebug() << "Script error:" << result.toString();
}
void AEngine::uniDeleteSlot(QObject *obj) {
//    qDebug() << "AEngine::uniDeleteSlot";
    if (obj==0) {obj = QObject::sender();}
    QString onEdit = ADataset::updateStringVariablesFromWidget(obj->property("onDelete").toString(), obj);
        if (onEdit=="") {return;}
    QScriptValue result = this->evaluate(onEdit);
    if (result.isError()) qDebug() << "Script error:" << result.toString();
}

void AEngine::uniPopupMenuSlot() {
    QAction *action = static_cast<QAction*>(QObject::sender());
    QString script = ADataset::updateStringVariablesFromWidget(action->property("script").toString(), action);
//      qDebug() << "AEngine::uniPopupMenuSlot()" << script;
    if (!script.isEmpty()) {

        QScriptValue result = this->evaluate(script);
        if (result.isError()) qDebug() << "Script error:" << result.toString();
    }
}
void AEngine::uniLostFocusSlot(QObject *obj) {
    if (obj==0) {obj = QObject::sender();}
    QString onLostFocus = ADataset::updateStringVariablesFromWidget(obj->property("onLostFocus").toString(), obj);
        if (onLostFocus=="") {return;}
    QScriptValue result = this->evaluate(onLostFocus);
    if (result.isError()) qDebug() << "Script error:" << result.toString();
}


bool AEngine::exec(QString str, QString sender) {
    if (str=="") {return false;}
    QScriptValue result = this->evaluate(str.append(";"));
    if (result.isError()) qDebug() << "[Error] "+sender+" Script error:" << result.toString();
    if (result.isError()) {
        return false;
    } else {
        return true;
    }
}
