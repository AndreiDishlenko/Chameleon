#include "aform.h"
#include "adataset.h"
#include "agui.h"
#include "adatabase.h"
#include "afunc.h"
#include <QStackedWidget>
#include <QObjectList>
#include <QTabWidget>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QObject>
#include <QApplication>
#include "globals.h"
#include <QStackedLayout>
#include "forms/atablewidgetstatic.h"
#include "forms/awebwidget.h"

AForm::AForm(QWidget *parent) : QObject(parent) {
    if (parent==0) {this->~AForm();}
    //qDebug() << "AForm::AForm" << this << parent;
    this->setParent(parent);
    this->setObjectName("Form");
    widget = parent;
    formType = widget->property("FormType").toString();   


    /// Установка для формы соединения по-умолчанию
    if (widget->property("Connection").toString()!="") {
        connection = widget->property("Connection").toString();
    } else {
        connection = globals["defConnection"];
        widget->setProperty("Connection", connection);
    }

    /// Создание списка ВСЕХ обьектов формы

    allobjlist = this->getFormAllObjects(widget);
    dataobjlist = this->getFormDataObjects(widget); //// Task: в список попадают элементы подчиненных форм

    QString objclassname;
    QWidget *parentwidget = static_cast<QWidget*>(widget->parent());
    if (widget->parent()==0) {
        widget->setProperty("mainwidget", true);
    } else {
        objclassname = parentwidget->metaObject()->className();
        if (objclassname=="QMdiSubWindow") {
            widget->setProperty("mainwidget", true);
        }
    }

    // make scalable objects list
    if (widget->property("mainwidget").toBool()) {
    QObjectList all_children_list = AForm::getWidgetAllObjects(widget);
        for (int i=0;i<all_children_list.count();i++) {
            objclassname = all_children_list.at(i)->metaObject()->className();
            if ((objclassname=="QTabWidget" || objclassname=="QPushButton" || objclassname=="QTableWidget" || objclassname=="QDateEdit" || objclassname=="QLabel") && all_children_list.at(i)->property("ScalableToMainWindowWidth").toBool()) {
                    scalable_objects << all_children_list.at(i);
                    //qDebug() << "scalable object" << all_children_list.at(i);
            }
        }
    }

    /// Формирование списка контейнеров формы (QWidget, QStackedWidget, ...)
    //qDebug() << "AForm::AForm form Widgets";
    widgetobjlist = AForm::getSubWidgets(widget);

//    widgetobjlist.clear();
//    for (int i=0;i<allobjlist.count();i++) {
//        QString objClassName = allobjlist.at(i)->metaObject()->className();
//        if (objClassName=="QWidget" || objClassName=="QStackedWidget") {widgetobjlist << allobjlist.at(i);}
//    }

    /// Подготавливает элементы формы, не являющиеся подчиненными формами данных, к работе
    for (int i=0;i<dataobjlist.count();i++) {
        // this->createEnterConnect(dataobjlist.at(i));
        objclassname = dataobjlist.at(i)->metaObject()->className();
        ADataset::prepareModelObject(dataobjlist.at(i));                             // QTreeView QTableView QScrollArea
        ADataset::updateObject(dataobjlist.at(i));
        ADataset::setDefault(dataobjlist.at(i));
    }
    //widget->installEventFilter(this);

    /// Создание AEngine, добавление элементов
    engine = new AEngine(widget);
    for (int i=0;i<widgetobjlist.count();i++) {
        engine->addEngineObject(widgetobjlist.at(i));
    }
    /// Создание форм данных ATableWidget, ATreeWidget, ATreeView
    if (formType=="tablewidget") {
        tablewidget = new ATableWidget(widget);
        //tablewidget->engine = engine;
        engine->addEngineBaseObject(tablewidget);
        this->setPopupMenu(tablewidget->tablewidget);
    }
    if (formType=="treewidget") {
        treewidget = new ATreeWidget(widget);
        engine->addEngineBaseObject(treewidget);
        this->setPopupMenu(treewidget->treewidget);
    }
    if (formType=="treeview") {
        treeview = new ATreeView(widget);
        engine->addEngineBaseObject(treeview);
        this->setPopupMenu(treeview->treeview);
    }
    if (formType=="map") {
        map = new AMap(widget);
        engine->addEngineBaseObject(map);
    }
    if (formType=="webwidget") {
        webwidget = new AWebWidget(widget);
        //return;
    }
    QString dataSource = widget->property("DataSource").toString();
    aDict = new ADict(dataSource, "", this, connection);
    engine->addEngineBaseObject(aDict);
    engine->addEngineBaseObject(this);

    /// Прорисовка элементов
    for (int i=0;i<allobjlist.count();i++) {
        AGui::setObjectView(allobjlist.at(i));
    }

    /// Локализация элементов формы, включая основной виджет
    AGui::setObjectLocalization(widget);
    for (int i=0;i<allobjlist.count();i++) {
        AGui::setObjectLocalization(allobjlist.at(i));
    }
    /// Создание порядка перехода по табуляции
    this->createTabOrder();
    widget->installEventFilter(this);
    for (int i=0;i<dataobjlist.count();i++) {
        dataobjlist.at(i)->installEventFilter(this);
        objclassname = dataobjlist.at(i)->metaObject()->className();
        //qDebug() << "AForm:AForm installEventFilter" << dataobjlist.at(i)->objectName() << objclassname;
        /// всплывающее меню табличных форм
        if (    objclassname=="QTreeView" ||
                objclassname=="QTreeWidget" ||
                objclassname=="QTableView" ||
                objclassname=="QTableWidget") {
            this->setPopupMenu(dataobjlist.at(i));
        }
    }
    aDict->newRecord();
    //qDebug() << "AForm::AForm end";
}

void AForm::updateAllObjects() {
    for (int i=0;i<dataobjlist.count();i++) {
        ADataset::updateObject(dataobjlist.at(i));
    }
}

AForm::~AForm(){
//    qDebug("~AForm");
}

bool AForm::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key()==Qt::Key_Enter || keyEvent->key() == 16777220) {
            QString classname = obj->metaObject()->className();
            if (classname=="QLineEdit" || classname=="QDateTime" || classname=="QComboBox") {
                this->switchNextItem(obj);
            }
        }
        if (keyEvent->modifiers() == Qt::CTRL && keyEvent->key() == Qt::Key_C) {

            QString classname = obj->metaObject()->className();
//            qDebug() << "AForm::eventFilter Ctrl+C" << classname << obj;
            if (classname =="QTableWidget") {
                return ATableWidget::slot_copyToClipboard(static_cast<QTableWidget*>(obj));
            } else if (classname=="QTreeView") {
                return ATreeWidget::slot_copyToClipboard(static_cast<QTreeWidget*>(obj));
            }else if (classname=="QTreeView") {
                return ATreeView::slot_copyToClipboard(static_cast<QTreeView*>(obj));
            }
        }
    }

    if (event->type()==QEvent::Resize) {
        //qDebug() << "AForm::eventFilter" << obj;
        QString classname = obj->metaObject()->className();
        if (obj!=0) {
            if (obj->property("mainwidget").toBool()) {

            }
        }
        if (classname =="QTableWidget") {
            ATableWidgetStatic::resizeColumns(static_cast<QTableWidget*>(obj));
        }

    }


    if (event->type() == QEvent::MouseButtonPress) {
        this->mouseclick();
    }

    return false;
}

void AForm::scaleSubObjects(float scale) {
    //qDebug() << "AForm::scaleSubObjects" << widget ;
    for (int i=0;i<scalable_objects.count();i++) {
        QString classname1 = scalable_objects.at(i)->metaObject()->className();
        QWidget *wgt = static_cast<QWidget*>(scalable_objects.at(i));
        QFont fnt = wgt->font();
        int scalable_font_pt = wgt->property("ScalableFontPt").toInt();
        if (scalable_font_pt==0) {
            wgt->setProperty("ScalableFontPt", fnt.pointSize());
            scalable_font_pt = fnt.pointSize();
        }
        fnt.setPointSize(scalable_font_pt/2+scalable_font_pt*scale/2);
        wgt->setFont(fnt);
        //qDebug() << "AForm::scaleSubObjects" << widget << wgt << scalable_font_pt/2+scalable_font_pt*scale/2 << wgt->font().pointSize();
        if (classname1=="QDateEdit") {
            wgt->setStyleSheet("font-size:"+QString::number(scalable_font_pt/2+scalable_font_pt*scale/2)+";");
        }
        if (classname1=="QTableWidget") {
            //qDebug() << "AForm::eventFilter resize" << scalable_objects.at(i);
            QTableWidget *tw = static_cast<QTableWidget*>(scalable_objects.at(i));
            fnt = tw->horizontalHeader()->font();
            fnt.setPointSize(scalable_font_pt/2+scalable_font_pt*scale/2);
            tw->horizontalHeader()->setFont(fnt);
        }
        if (classname1=="QLabel") {
            //qDebug() << "AForm::eventFilter resize" << scalable_objects.at(i);
            QLabel *label = static_cast<QLabel*>(scalable_objects.at(i));
            fnt = label->font();
            fnt.setPointSize(scalable_font_pt/2+scalable_font_pt*scale/2);
            label->setFont(fnt);
        }

    }
}

void AForm::createObjectsConnects() {
    //qDebug() << "AForm::createObjectsConnects" << allobjlist;
    if (formType!="tablewidget" && formType!="treeview") {
        for (int i=0;i<allobjlist.count();i++) {
            QString classname = allobjlist.at(i)->metaObject()->className();
            engine->createConnects(allobjlist.at(i));
        }
    }
}

/// Установка порядка перехода курсора между элементами по табуляции (параметр [TabOrder]).
void AForm::createTabOrder() {
    tablist.clear();
    QObjectList tablist2;
    QList<int> tablistindex;
    QVariant tabOrder;
    bool dontTabStop;
    for (int i=0;i<dataobjlist.count();i++) {
        QString objClassName = dataobjlist.at(i)->metaObject()->className();
        tabOrder = dataobjlist.at(i)->property("TabOrder");
        dontTabStop = dataobjlist.at(i)->property("dontTabStop").toBool();
        if (objClassName=="QLineEdit" || objClassName=="QComboBox" || objClassName=="QDateEdit") {
            if ((tabOrder.toString()=="0" || tabOrder.toInt()!=0) && tabOrder.toInt()!=-1) {tablist.append(dataobjlist.at(i)); tablistindex.append(tabOrder.toInt());}
            else if (((tabOrder.toString()!="0" && tabOrder.toInt()==0) || tabOrder.toString()=="") && tabOrder.toInt()!=-1) {tablist2.append(dataobjlist.at(i));}
        }
    }
    for(int i=0;i<tablistindex.count()-1;i++) {
        for(int i1=0;i1<tablistindex.count()-1;i1++) {
            if (tablistindex.at(i1)>tablistindex.at(i1+1)) {
                tablistindex.insert(i1-1, tablistindex.at(i1+1));
                tablistindex.removeAt(i1+2);
                tablist.insert(i1-1, tablist.at(i1+1));
                tablist.removeAt(i1+2);
            }
        }
    }
    tablist.append(tablist2);
}
//void AForm::createEnterConnect(QObject *obj) {
//    QString objClassName = obj->metaObject()->className();
//    if (objClassName=="QLineEdit" || objClassName=="QComboBox" || objClassName=="QDateEdit") {
//        obj->installEventFilter(this);
//    }
//}
void AForm::switchNextItem(QObject *obj) {
    int newindex=-1;
    int oldindex =-1;

    int index = tablist.indexOf(obj);
    if (index==-1) {oldindex=tablist.count(); index=0;}
    else {
        oldindex = index;
        index++;
        if (index>=tablist.count()) {index=0;}
    }
    if (index!=0) {
        for (int i=index;i<tablist.count();i++) {
            if (tablist.at(i)->property("enabled").toBool()) {newindex=i; i=tablist.count();}
        }
    }
    if (newindex==-1) {
        for (int i=0;i<oldindex;i++) {
            if (tablist.at(i)->property("enabled").toBool()) {newindex=i; i=oldindex;}
        }
    }
//    qDebug() << "AForm::switchNextItem" << newindex;
    if (newindex!=-1) {
        QWidget *wgt = static_cast<QWidget*>(tablist.at(newindex));
        AGui::setFocus(wgt);
    }
}

///////////////////////////////////////////////////////////////////////////
/// РЕКУРСИВНАЯ обработка текущей формы:
/// - Собирает элементы запрашиваемого виджета в один список, не включая главный виджет
/// - включает элементы подчиненных QWidget, QTabWidget, QStackedWidget, QGroupBox
/// - не включает подчиненные виджеты форм данных
/// - не включает системные обьекты AForm, AEngine
///////////////////////////////////////////////////////////////////////////
QObjectList AForm::getFormAllObjects(QObject *obj) { // Сбор всех объектов данной формы
    //qDebug() << "AGui::getFormAllObjs " << objtemp;
    QList<QString> dataformtypes_list;
    QList<QString> sysobjecttypes_list;
    dataformtypes_list << "dataform" << "tablewidget" <<  "treeview" << "treewidget" << "map" << "content" << "webwidget";
    sysobjecttypes_list << "AForm" << "AEngine";

    QObjectList result;
    QObjectList selfitems_list;
    QObjectList children_list = obj->children();
    for (int i=0;i<children_list.count();i++) {
        QObject *objtemp = children_list.at(i);
        QString objName = objtemp->objectName();

        QString objClassName = objtemp->metaObject()->className();
        if (objClassName=="QWidget") {
            if (!dataformtypes_list.contains(objtemp->property("FormType").toString())) {result.append(getFormAllObjects(objtemp));}
        }
        if (objClassName == "QTabWidget") {
            QTabWidget *qtab = static_cast<QTabWidget*>(objtemp);
            for (int i=0;i<qtab->count();i++) {
                QWidget *wgt = qtab->widget(i);
                result.append(wgt);
                if (!dataformtypes_list.contains(wgt->property("FormType").toString())) {result.append(getFormAllObjects(wgt));}
            }
        }
        if (objClassName == "QStackedWidget") {
            QStackedWidget *sw = static_cast<QStackedWidget*>(objtemp);
            for (int i=0;i<sw->count();i++) {
                QWidget *wgt = sw->widget(i);
                result.append(wgt);
                if (!dataformtypes_list.contains(wgt->property("FormType").toString())) {result.append(getFormAllObjects(wgt));}
            }
        }
        if (objClassName == "QGroupBox") {
            result.append(getFormAllObjects(objtemp));
        }
//        if (objClassName=="QScrollArea") {
//            if (!dataformtypes_list.contains(objtemp->property("FormType").toString())) {result.append(getFormAllObjects(objtemp));}
//        }
        if (objName!="" && objName.left(3) != "qt_" && !sysobjecttypes_list.contains(objClassName)) {
            selfitems_list.append(objtemp);
        }
    }
    result.append(selfitems_list);
    return result;
}

QObjectList AForm::getWidgetAllObjects(QObject *obj) { // Сбор всех объектов данной формы
        QList<QString> sysobjecttypes_list;
    sysobjecttypes_list << "AForm" << "AEngine";

    QObjectList result;
    QObjectList selfitems_list;
    QObjectList children_list = obj->children();
    for (int i=0;i<children_list.count();i++) {
        QObject *objtemp = children_list.at(i);
        QString objName = objtemp->objectName();

        QString objClassName = objtemp->metaObject()->className();
        if (objClassName=="QWidget") {
            result.append(getWidgetAllObjects(objtemp));
        }
        if (objClassName == "QTabWidget") {
            QTabWidget *qtab = static_cast<QTabWidget*>(objtemp);
            for (int i=0;i<qtab->count();i++) {
                QWidget *wgt = qtab->widget(i);
                result.append(wgt);
                result.append(getWidgetAllObjects(wgt));
            }
        }
        if (objClassName == "QStackedWidget") {
            QStackedWidget *sw = static_cast<QStackedWidget*>(objtemp);
            for (int i=0;i<sw->count();i++) {
                QWidget *wgt = sw->widget(i);
                result.append(wgt);
                result.append(getWidgetAllObjects(wgt));
            }
        }
        if (objClassName == "QGroupBox") {
            result.append(getWidgetAllObjects(objtemp));
        }
//        if (objClassName == "QGridLayout") {
//            QGridLayout *gl = static_cast<QGridLayout*>(objtemp);
//            result.append(gl);
//            result.append(getWidgetAllObjects(gl));
//        }
//        if (objClassName=="QScrollArea") {
//            if (!dataformtypes_list.contains(objtemp->property("FormType").toString())) {result.append(getFormAllObjects(objtemp));}
//        }
        if (objName!="" && objName.left(3) != "qt_" && !sysobjecttypes_list.contains(objClassName)) {
            selfitems_list.append(objtemp);
        }
    }
    result.append(selfitems_list);
    return result;
}

// Сбор всех объектов формы, способных содержать сведения о данных формы
QObjectList AForm::getFormDataObjects(QObject *obj) {
    //qDebug() << "-- AGui::getFormObjs " << obj;
    QList<QString> excludeWidgets;
        excludeWidgets << "QWidget" << "QTabWidget" << "QStackedWidget" << "QLabel";
    QObjectList result;
    result = AForm::getFormAllObjects(obj);
    for (int i=0;i<result.count();i++) {       
        if (excludeWidgets.contains(result.at(i)->metaObject()->className())) {result.removeAt(i); i--;}

    }
    return result;
}

QObjectList AForm::getSubWidgets(QObject *obj) {
    //qDebug() << "-- AForm::getSubwidget" << obj;
    QObjectList dataobjlist;
    QObjectList objlisttemp = obj->children();
    QString objClassName = obj->metaObject()->className();
    // Добавление к списку
    for (int i=0;i<objlisttemp.count();i++) {
        QObject *objtemp = objlisttemp.at(i);
        objClassName = objtemp->metaObject()->className();
        if (objClassName=="QWidget") {
            dataobjlist.append(objtemp);
            dataobjlist.append(getSubWidgets(objtemp));
        }
        if (objClassName == "QTabWidget") {
            QTabWidget *qtab = static_cast<QTabWidget*>(objtemp);
            dataobjlist.append(qtab);
            for (int i=0;i<qtab->count();i++) {
                QWidget *wgt = qtab->widget(i);
                dataobjlist.append(wgt);
                dataobjlist.append(getSubWidgets(wgt));
            }
        }
        if (objClassName == "QStackedWidget") {
            QStackedWidget *sw = static_cast<QStackedWidget*>(objtemp);
            dataobjlist.append(sw);
            for (int i=0;i<sw->count();i++) {
                QWidget *wgt = sw->widget(i);
                dataobjlist.append(wgt);
                dataobjlist.append(getSubWidgets(wgt));
            }
        }
        if (objClassName == "QGridLayout") {
            QGridLayout *gl = static_cast<QGridLayout*>(objtemp);
            dataobjlist.append(gl);
            dataobjlist.append(getSubWidgets(gl));
//            for (int i=0;i<gl->count();i++) {
//                QWidget *wgt = gl->widget();
//                dataobjlist.append(wgt);
//                qDebug() << wgt;
//                dataobjlist.append(getSubWidgets(wgt));
//            }
        }
        if (objClassName == "QGroupBox") {dataobjlist.append(getSubWidgets(objtemp));}
    }
    return dataobjlist;
}
QObjectList AForm::getSubForms(QObject *obj) {
    QObjectList result;
    QObjectList dataobjlist = AForm::getSubWidgets(obj);
    QList<QString> dataformtypes_list;
    dataformtypes_list << "dataform" << "tablewidget" <<  "treeview" << "treewidget" << "map" << "content" << "webwidget";
    //qDebug() << "AForm::getSubForms" << dataobjlist;
    for (int i=0;i<dataobjlist.count();i++) {
        QString name = dataobjlist.at(i)->objectName();
        //        14if (name=="checkViewCont") {qDebug() << "AForm::getSubForms" << dataobjlist.at(i) << dataobjlist.at(i)->parent();}
        if (dataformtypes_list.contains(dataobjlist.at(i)->property("FormType").toString())) {
            result.append(dataobjlist.at(i));
        }
    }
    return result;
}

QObject * AForm::getParentForm(QWidget *wgt) {
//    qDebug() << "AForm::getParentForm 1" << wgt;
    QList<QString> dataformtypes_list;
        dataformtypes_list << "dataform" << "tablewidget" <<  "treeview" << "treewidget" << "map" << "content";
//    qDebug() << wgt << wgt->findChild<AForm*>("Form") << wgt->findChild<AForm*>("Form")->parent();
    while ((wgt->findChild<AForm*>("Form")==0 || wgt->findChild<AForm*>("Form")->parent()!=wgt) && wgt!=0) {
        wgt = wgt->parentWidget();
//        qDebug() << "AForm::getParentForm 2" << wgt;
    }

//    while ((wgt->findChild<AForm*>("Form")==0 || wgt->findChild<AForm*>("Form")->parent()!=wgt) && wgt!=0) {
//        qDebug() << "AForm::getParentForm" << wgt;
//    }
//    qDebug() << "selft form" << wgt->findChild<AForm*>("Form");;
    wgt = wgt->parentWidget();
//    qDebug() << "AForm::getParentForm 3" << wgt << wgt->findChild<AForm*>("Form");
    while ((wgt->findChild<AForm*>("Form")==0 || wgt->findChild<AForm*>("Form")->parent()!=wgt) && wgt!=0) {
        wgt = wgt->parentWidget();
//        qDebug() << "AForm::getParentForm 4" << wgt;
    }
////    qDebug() << wgt->findChild<AForm*>("Form") << wgt->findChild<AForm*>("Form")->parent() << wgt;
//    while ((wgt->findChild<AForm*>("Form")==0 || wgt->findChild<AForm*>("Form")->parent()!=wgt) && wgt!=0) {
//        wgt = wgt->parentWidget();
//    }
    if (wgt!=0) {return wgt->findChild<AForm*>("Form");}
    return 0;
}

QString AForm::getPteScripts(){
//    qDebug() << "AForm::getPteScripts" << dataobjlist;
    QString result = "";
    for (int i=0;i<dataobjlist.count();i++) {
        QObject *objtemp = dataobjlist.at(i);
        QString name = objtemp->objectName();
        if (name.contains("pteWidgetScript")) {
            QString objClassName = objtemp->metaObject()->className();
            if (objClassName=="QPlainTextEdit") {
                QPlainTextEdit *pte = static_cast<QPlainTextEdit*>(objtemp);
                pte->setVisible(false);
                result.append(pte->toPlainText());
            }
        }
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////
// -- Public секция для объединения работы с AForm и ATableWidget
///////////////////////////////////////////////////////////////////////////
void AForm::load() {
    this->update();
}
void AForm::update() {
    //qDebug() << "AForm::update() " << formType << widget->objectName();
    if (formType=="tablewidget") {
        tablewidget->update();
    } else if (formType=="treeview") {
        treeview->update();
    } else if (formType=="treewidget") {
        treewidget->update();
    } else if (formType=="map") {
        map->load();
    } else if (formType=="webwidget") {
        webwidget->update();
    //} else if (formType=="content") {
    //    content->load();
    } else {
        this->reset();
        this->loadFormData();
    }
}

bool AForm::save() {
    if (formType=="tablewidget") {
        return tablewidget->saveTableWidget();
    } else if (formType=="map") {
        return map->save();
    } else {
        return this->saveFormData();
    }
}
void AForm::saveData() {
//    qDebug() << "AForm::saveData()";
    this->save();
}
void AForm::updateSubForms() {
//    qDebug() << "AForm::updateSubForms() " << widget;
    QObjectList dataobjlist = AForm::getSubForms(widget);
    for (int i=0;i<dataobjlist.count();i++) {
        AForm *form =dataobjlist.at(i)->findChild<AForm*>("Form");
        //qDebug() << "AForm::updateSubForms() " << form << dataobjlist.at(i);
        form->update();
    }
}
void AForm::saveSubForms() {
    //qDebug() << "AForm::saveSubForms() " << widget;
    QObjectList dataobjlist = AForm::getSubForms(widget);
    for (int i=0;i<dataobjlist.count();i++) {
        AForm *form =dataobjlist.at(i)->findChild<AForm*>("Form");
        //qDebug() << "AForm::saveSubForms()"<< dataobjlist.at(i)->objectName();
        form->saveData();
    }
}

///////////////////////////////////////////////////////////////////////////
// -- Работа с данными формы
///////////////////////////////////////////////////////////////////////////
bool AForm::loadFormData() {
    //qDebug() << "AForm::loadFormData()";
    bool result = true;
    int currentRecord = widget->property("currentRecord").toInt();
    //qDebug() << "AForm::loadFormData" << widget->objectName() << currentRecord;
    if (currentRecord==0) {this->clear(); return false;}
    if (!aDict->openRecord(currentRecord)) {return false;}
    QMap<QString, QVariant> record = aDict->getCurrentRecord();
    //    qDebug() << "AForm::loadFormData " << record;
    for (int i=0;i<dataobjlist.count();i++) {
        QObject *objtemp = dataobjlist.at(i);
        QString objfield = objtemp->property("Data").toString();
        if (objfield != "") {
            QVariant modeldata = record[objfield];
            if (!ADataset::setData(objtemp, modeldata)) {
                objtemp->setProperty("dontsave", "true");
                QString objClassName = objtemp->metaObject()->className();
                if (objClassName=="QComboBox") {
                    QComboBox *cBox = static_cast<QComboBox *>(objtemp);
                    QObject::connect(cBox,SIGNAL(currentIndexChanged(int)), this, SLOT(setDonsaveFalse(int)));
                }
                result = false;
                qDebug() << "Warning. AForm::loadFormData Can't set" << widget->objectName() << objtemp->objectName() << modeldata;
            } else {

                objtemp->setProperty("dontsave", "false");
                engine->uniLoadSlot(objtemp);
            }
        }
    }
    return result;
}

bool AForm::saveFormData() {
    //qDebug() << "AForm::saveFormData() 1" << widget << aDict->getCRValue("id");
    for (int i=0;i<dataobjlist.count();i++) {
        QObject *objtemp = dataobjlist.at(i);
        QString fieldName = objtemp->property("Data").toString();
        QString dontsave = objtemp->property("dontsave").toString();
        //        qDebug() << "AForm::saveFormData" << fieldName << dontsave << ADataset::getData(objtemp);
        if (fieldName != "" && dontsave!="true") {
            record[fieldName] = ADataset::getData(objtemp);
        }
    }
    QMap<QString, QString> filter = AFunc::getStrParam(widget->property("Filter").toString());
    for (int i=0;i<filter.keys().count();i++) {
        QString data = widget->property(filter.values().at(i).toLocal8Bit().data()).toString();
        record[filter.keys().at(i)] = data;
    }
    aDict->updateRecord(record);
    if (!aDict->saveChanges()) {return false;}
    //qDebug() << "AForm::saveFormData() 2" << aDict->getCRValue("id");
    QVariant lastInsertedKey = aDict->getCRValue("id");

    //    if (lastInsertedKey.isNull()) {
    //        QString dataSource = widget->property("DataSource").toString();
    //        lastInsertedKey = ADatabase::lastInsertedKey(dataSource);
    //    }

    widget->setProperty("currentRecord", lastInsertedKey);
    return true;
}
void AForm::clear() {
    if (formType=="tablewidget") {
        tablewidget->clear();
    } else {
        for (int i=0;i<dataobjlist.count();i++) {
            QObject *objtemp = dataobjlist.at(i);
            ADataset::setDefault(objtemp);
        }
    }
}
void AForm::reset() {
//    qDebug() << "AForm::reset";
    for (int i=0;i<dataobjlist.count();i++) {
        ADataset::setDefault(dataobjlist.at(i));
    }
}

QVariant AForm::getSelected() {
    QVariant result;
    if (formType=="tablewidget") {result = tablewidget->getSelected();}
    else if (formType=="treewidget") {result = treewidget->getSelected();}
    else if (formType=="treeview") {result = treeview->getSelected();}
    else if (formType=="map") {result = map->getSelected();}
    else {result = widget->property("currentRecord").toInt();}
    return result;
}
void AForm::select(int id) {
    if (formType=="tablewidget") {tablewidget->select(id);}
    else if (formType=="treeview") {treeview->select(id);}
    else if (formType=="map") {map->select(id);}
    else {widget->setProperty("currentRecord", id); this->load();}
}

QMap<QString, QVariant> AForm::getFormData() {
    //qDebug() << "getFormData";
    QMap<QString, QVariant> result;
    //result["currentRecord"] = widget->property("currentRecord");
    for (int i=0;i<dataobjlist.count();i++) {
        QObject *objtemp = dataobjlist.at(i);
        QString fieldName = objtemp->property("Data").toString();
        QString dontsave = objtemp->property("dontsave").toString();
        if (fieldName != "" && dontsave!="true") {
            result[fieldName] = ADataset::getData(objtemp);
            //if (fieldName=="sum") {
            //  qDebug() << "AForm::getFormData" << result[fieldName];
            //}
            //qDebug() << "aa" << fieldName << result[fieldName];
        }
    }
    //qDebug() << "aaa" << result;
    return result;
}

bool AForm::updateFormData(QMap<QString, QVariant> updateMap) {
    //qDebug() << "AForm::updateFormData" << updateMap;
    QMap<QString, QVariant> result;
    for (int i=0;i<dataobjlist.count();i++) {
        QObject *objtemp = dataobjlist.at(i);
        QString fieldName = objtemp->property("Data").toString();
        if (fieldName!="") {
            ADataset::setData(objtemp, updateMap[fieldName]);
        }
    }
    return true;
}
void AForm::setRecordValue(QString fieldName, QVariant data) { //a11
    for (int i=0;i<dataobjlist.count();i++) {
        QString objField = dataobjlist.at(i)->property("Data").toString();//->objectName();
        if (objField==fieldName) {ADataset::setData(dataobjlist.at(i),data);}
    }
}

#include <QMenu>
#include <QAction>
#include <QHeaderView>
void AForm::setPopupMenu (QObject *obj) {
    //qDebug() << "AForm::setPopupMenu" << obj << obj->property("cMenuNames").toString().isEmpty() << obj->property("cMenuActions").toStringList().count();
    if (!obj->property("cMenuNames").toString().isEmpty() && obj->property("cMenuActions").toStringList().count()>0)   {
        //qDebug() << "AForm::setPopupMenu" << obj;
        QWidget *wgt = static_cast<QWidget*>(obj);
        wgt->setContextMenuPolicy( Qt::CustomContextMenu );
        QObject::connect(wgt, SIGNAL( customContextMenuRequested ( const QPoint & ) ), this,  SLOT( popupMenuSlot( const QPoint & ) ) );
    }
}
void AForm::popupMenuSlot( QPoint point) {
    QWidget *sender = static_cast<QWidget*>(QObject::sender());
    QString objClassName = QObject::sender()->metaObject()->className();
    QMenu *menu = new QMenu("Меню"); menu->clear();
    QList<QString> cMenuNames = AFunc::strToList(sender->property("cMenuNames").toString(), QString(";"));
    QList<QString> cMenuActions = sender->property("cMenuActions").toStringList();
    qDebug() << "AForm::popupMenuSlot" << sender;

    for (int i=0;i<cMenuNames.count();i++) {
        if (cMenuNames[i]=="sep") {
            menu->addSeparator();
        } else {
            QAction *newAction = new QAction(QIcon(":/images/table/table_row_delete.png"), cMenuNames[i], menu);
            if (i<cMenuActions.count() && !cMenuActions[i].isNull()) {
                newAction->setProperty("script", cMenuActions[i]);
                QObject::connect(newAction,SIGNAL(triggered()), engine, SLOT(uniPopupMenuSlot()));
            }
            menu->addAction(newAction);
        }
    }
//    qDebug() << "ATreeView::popupMenuSlot" << cMenuNames << cMenuNames.count() << cMenuNames[0];
    int incy = 0;
    if (objClassName=="QTreeView") {
        QTreeView *tv = static_cast<QTreeView*>(QObject::sender());
        incy = tv->header()->height();
    }
    QPoint pnt = sender->mapToGlobal(QPoint(point.x(), point.y()+incy));
    menu->exec(pnt);
    delete menu;
}

bool AForm::changed() {

    bool result=false;
    if (formType=="tablewidget") {
        qDebug() << "AForm::changed()" << tablewidget << tablewidget->property("ifChanged");
        return tablewidget->tablewidget->property("ifChanged").toBool();
    }
//    for (int i=0;i<dataobjlist.count();i++) {
//        QObject *objtemp = dataobjlist.at(i);
//        QString fieldName = objtemp->property("Data").toString();
//        if (fieldName != "") {
//            QString modeldata = ds[fieldName].toString();//AFunc::getQueryModelValue(ds,0,fieldName).toString();
//            QString data = ADataset::getData(objtemp).toString();
////                qDebug() << "changed() " << fieldName << data << modeldata << ds[fieldName];
//            if (data != modeldata) {return true;}
//        }
//    }
    return false;
}
bool AForm::checkNull() {
    QStringList nulllist;
    for (int i=0;i<dataobjlist.count();i++) {
        QObject *objtemp = dataobjlist.at(i);
//        QString fieldName = objtemp->property("field").toString();
        bool notNull = objtemp->property("notNull").toBool();
        QVariant data = ADataset::getData(objtemp);
//        qDebug() << "AForm::checkNull()" << objtemp->objectName() << notNull << data;
        if (notNull && (data.isNull() || data.toString()=="")) {nulllist.append(objtemp->objectName());}
    }
    if (nulllist.count()>0) {qDebug() << "AFormOld::checkNull:: " << nulllist << " objects is null!"; return false;}
    return true;
}
void AForm::script(QString script) {
    //qDebug() << "AForm::script" << engine;
    engine->evaluate(script);
}
void AForm::setDonsaveFalse(int i) {
    QObject::sender()->setProperty("dontsave", "false");
}

void AForm::setTabEnabled(QTabWidget *tab, QString widgetname, bool state) {
    if (tab==0) {qDebug() << "[WARNING] AForm::setTabEnabled widget is null."; return;}
    QString name;
    for (int i=0;i<tab->count();i++) {
        name = tab->widget(i)->objectName();
        if (name==widgetname) {
            //qDebug() << "AForm::setTabEnabled" << tab << widgetname << state << i;
            tab->setTabEnabled(i, state);
        }
    }
}
