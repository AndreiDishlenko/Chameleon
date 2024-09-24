// QT declaration
#include <qmessagebox.h>
#include <QUiLoader>
#include <QLayout>
#include <QAbstractItemModel>
#include <QTableView>
#include <QInputDialog>
#include <QDateEdit>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStackedWidget>
#include <QTableWidget>
#include <QKeyEvent>
#include <QApplication>
#include <QDir>
#include <QHeaderView>
#include <QListView>
//#include <QWebEngineView>
#include <QtWebEngineWidgets/QtWebEngineWidgets>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include "aapplication.h"

// CLASSES declaration
#include "globals.h"
#include "afunc.h"
// SELF declaration
#include "agui.h"
#include "aform.h"

#include "QGroupBox"
#include "QGraphicsDropShadowEffect"

AGui::AGui(QObject *parent) : QObject(parent){
//    qDebug() << "AGui parent" << this->parent();
}
AGui::~AGui() {
//    qDebug() << "~AGui()";
}

void AGui::msgBox(QString text){
    QMessageBox *msg = new QMessageBox(QMessageBox::Information,"Уведомление", text, QMessageBox::Ok);
    msg->exec();
    msg->~QMessageBox();
}
bool AGui::dlgAccept(QString label) {
    QMessageBox *msg = new QMessageBox(QMessageBox::Question,"Подтверждение", label, QMessageBox::Yes | QMessageBox::No);
    int n = msg->exec();
//    msg->setDefaultButton();
//    qDebug() << "AGui::dlgAccept" << msg->children();
    msg->~QMessageBox();
    if (n==QMessageBox::Yes) {return true;}
    return false;
}
int AGui::dlgAcceptWide(QString label) {
    int result;
    QMessageBox *msg = new QMessageBox(QMessageBox::Question,"Подтверждение", label, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    int n = msg->exec();
//    msg->setDefaultButton();
//    qDebug() << "AGui::dlgAccept" << msg->children();
    msg->~QMessageBox();
    if (n==QMessageBox::Yes) {return 1;}
    if (n==QMessageBox::No) {return 0;}
    if (n==QMessageBox::Cancel) {return -1;}
    return n;
}

QString AGui::dlgInput(QString title, QString label, QString text) {
    //qDebug() << "AGui::dlgInput" << label << text;
    QInputDialog inputDialog;

    inputDialog.setMinimumWidth(300);
    inputDialog.resize(QSize(300,100));
    inputDialog.setWindowTitle(title);
    inputDialog.setLabelText(label);
    inputDialog.setTextValue(text);
    inputDialog.setTextEchoMode(QLineEdit::Normal);
    bool ok = !!inputDialog.exec();
    if (ok)
        return inputDialog.textValue();
    else
        return QString();
}

QString AGui::dlgInputPassword(QString title, QString label, QString text) {
    QInputDialog inputDialog;

    inputDialog.setMinimumWidth(300);
    inputDialog.resize(QSize(300,100));
    inputDialog.setWindowTitle(title);
    inputDialog.setLabelText(label);
    inputDialog.setTextValue(text);
    inputDialog.setTextEchoMode(QLineEdit::Password);
    bool ok = !!inputDialog.exec();
    if (ok)
        return inputDialog.textValue();
    else
        return QString();
}

#include <QBuffer>
#include <secure/asecure.h>
#include <QStringList>
QWidget* AGui::openUi(QString uiName, QWidget * parent) {
    //qDebug() << "AGui::openUi" << uiName;
    QWidget *widget;
    if (!uiName.contains(".ui")) {qDebug() << "Error AGui::openUi Can't open "+uiName; return 0;}
    QBuffer buffer;
    QByteArray byarr;
    QUiLoader uiLoader;
    if (uiName.contains("$")) {
        uiName.remove("$");
        uiName = AFunc::unionFolderNames(globals["uiPath"], uiName);
    } else {
         uiName = AFunc::unionFolderNames(globals["configurationPath"], uiName);
    }
    //qDebug() << "AGui::openUi" << uiName;

    QString chName = uiName;
    chName.replace(".ui", ".ch");
    //qDebug() << "AGui::openUi" << uiName;
    if (QFile::exists(uiName)) {
        QFile file(uiName);
        //if (glob_warncounter==0) {AGui::msgBox("Ошибка проверки User interface!"); glob_warncounter++;}
        if (!file.open(QIODevice::ReadOnly)) {qDebug() << "AGui::openUi Can't open '"+uiName+"'."; file.close(); return 0;}
        byarr = file.readAll();
        buffer.setBuffer(&byarr);
        file.close();
        widget = uiLoader.load(&buffer, parent);
    } else
    if (QFile::exists(chName)) {
        //qDebug() << "AGui::openUi" << chName;
        QFile file(chName);
        if (!file.open(QIODevice::ReadOnly)) {qDebug() << "AGui::openUi Can't open '"+chName+"'."; file.close(); return 0;}
        byarr = ASecure::decodeStr(file.readAll(), globals["uikey"]);
        buffer.setBuffer(&byarr);
        file.close();
        widget = uiLoader.load(&buffer, parent);
    } else {
        AGui::msgBox("AGui::openUi не найдена форма "+uiName.replace(".ui", "")+" ("+uiName+")");
        return 0;
    }

    return widget;
}

#include <QScrollArea>
void AGui::setObjectView(QObject *object) { // Вид объекта в соответствии с параметрами
    //qDebug() << "AGui::setObjectView " << object;
    QString objectClass = object->metaObject()->className();
    if (globals["showShadows"]=="1") {
        int shadowLength = object->property("shadowLength").toInt();
        int shadowBlur = object->property("shadowBlur").toInt();
        QString shadowColor = object->property("shadowColor").toString();
        if (shadowLength>0 || shadowBlur>0) {
            QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(object);
            shadow->setOffset(shadowLength);
            shadow->setBlurRadius(shadowBlur);
            if (shadowColor!="") {shadow->setColor(shadowColor);}
            QWidget *wgt = static_cast<QWidget *>(object);
            wgt->setGraphicsEffect(shadow);
//            qDebug() << "AGui::setObjectView create shadow " << object->objectName() << shadowLength << shadowBlur;
        }
    }
    QWidget *widget = static_cast<QWidget*>(object);

    if (objectClass == "QTableWidget") {
        QTableWidget *tblWidget = static_cast<QTableWidget *>(object);
        tblWidget->setStyle(QStyleFactory::create("Fusion"));

        // Tourn on scroll by finger on touch screens
        bool fingerScroll = widget->property("FingerScroll").toBool();
        if (fingerScroll) {
            //QScroller::grabGesture(tblWidget, QScroller::TouchGesture);
            QScroller::grabGesture(tblWidget, QScroller::LeftMouseButtonGesture);
            QScroller* scroller = QScroller::scroller( tblWidget );
            QScrollerProperties props = scroller ->scrollerProperties();
            props.setScrollMetric( QScrollerProperties::AxisLockThreshold, 0);
            props.setScrollMetric(QScrollerProperties::DragVelocitySmoothingFactor, 0.6);
            props.setScrollMetric(QScrollerProperties::MinimumVelocity, 0.0);
            props.setScrollMetric(QScrollerProperties::MaximumVelocity, 0.5);
            props.setScrollMetric(QScrollerProperties::AcceleratingFlickMaximumTime, 0.4);
            props.setScrollMetric(QScrollerProperties::AcceleratingFlickSpeedupFactor, 1.2);
            props.setScrollMetric(QScrollerProperties::SnapPositionRatio, 0.2);
            props.setScrollMetric(QScrollerProperties::MaximumClickThroughVelocity, 0);
            props.setScrollMetric(QScrollerProperties::DragStartDistance, 0.001);
            props.setScrollMetric(QScrollerProperties::MousePressEventDelay, 0.5);
            props.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
            props.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
            scroller->setScrollerProperties( props );
        }

        //int rowHeight = tblWidget->property("rowHeight").toInt();
        //if (rowHeight!=0 && tblWidget->model()!=0) {
        //  for (int i=0;i<tblWidget->model()->rowCount();i++) {
        //    tblWidget->setRowHeight(i, rowHeight);
        //  }
        //}
    }
    if (objectClass == "QTableView") {
        QTableView *tblView = static_cast<QTableView *>(object);
        tblView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
        tblView->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
        QString columnsWidth = object->property("ColumnsWidth").toString();
        if (columnsWidth!="") {
            QStringList columnsWidthList = AFunc::strToList(columnsWidth);
          for (int i=0;i<columnsWidthList.size();i++) {
                QString temp = columnsWidthList.at(i);
                tblView->setColumnWidth(i, temp.toInt());
          }
       }
        int rowHeight = tblView->property("rowHeight").toInt();
        if (rowHeight!=0 && tblView->model()!=0) {
            for (int i=0;i<tblView->model()->rowCount();i++) {
                tblView->setRowHeight(i, rowHeight);
//                tblView->setC
            }
        }
    }
    if (objectClass == "QComboBox") {
        QComboBox *cb = static_cast<QComboBox *>(object);
//        qDebug() << "modelColumn" << cb->modelColumn();
        cb->setModelColumn(cb->modelColumn());
        cb->setView(new QListView());
    }
    if (objectClass == "QDateEdit") {
        QDateEdit *de = static_cast<QDateEdit *>(object);
        //        qDebug() << "AGui::setObjectView " << object << de;
        de->setDisplayFormat("dd.MM.yyyy");
    }
    if (objectClass == "QTreeView") {
        QTreeView *tw = static_cast<QTreeView *>(object);
        if (tw->property("ifOpenAllNodes").toBool()) {
            tw->expandAll();
        }
        //        qDebug() << "AGui::setObjectView " << object << de;
        //de->setDisplayFormat("dd.MM.yyyy");
    }
    if (objectClass == "QWebEngineView") {
        //        QWebEngineView *wv = static_cast<QWebEngineView *>(object);
        ////        QPalette palette = wv->palette();
        ////        palette.setBrush(QPalette::Base, Qt::transparent);
        ////        //55wv->page()->setPalette(palette);
        ////        wv->setAttribute(Qt::WA_OpaquePaintEvent, false);
        //        bool fingerScroll = widget->property("FingerScroll").toBool();
        //        if (fingerScroll) {
        //            qDebug() << "AGui::setObjectView" << object->objectName();
        //            QScroller::grabGesture(wv->page(), QScroller::LeftMouseButtonGesture);
        //            //QScroller::grabGesture(tblWidget, QScroller::LeftMouseButtonGesture);
        //            QScroller* scroller = QScroller::scroller( wv->page() );
        //            QScrollerProperties props = scroller ->scrollerProperties();
        //            props.setScrollMetric( QScrollerProperties::AxisLockThreshold, 0);
        //            props.setScrollMetric(QScrollerProperties::DragVelocitySmoothingFactor, 0.6);
        //            props.setScrollMetric(QScrollerProperties::MinimumVelocity, 0.0);
        //            props.setScrollMetric(QScrollerProperties::MaximumVelocity, 0.5);
        //            props.setScrollMetric(QScrollerProperties::AcceleratingFlickMaximumTime, 0.4);
        //            props.setScrollMetric(QScrollerProperties::AcceleratingFlickSpeedupFactor, 1.2);
        //            props.setScrollMetric(QScrollerProperties::SnapPositionRatio, 0.2);
        //            props.setScrollMetric(QScrollerProperties::MaximumClickThroughVelocity, 0);
        //            props.setScrollMetric(QScrollerProperties::DragStartDistance, 0.001);
        //            props.setScrollMetric(QScrollerProperties::MousePressEventDelay, 0.5);
        //            props.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
        //            props.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
        //            scroller->setScrollerProperties( props );
        //        }
    }
}

void AGui::startWidgetLocalization(QWidget *wgt) {
    QObjectList allobjectslist = AForm::getFormAllObjects(wgt);
    AGui::setObjectLocalization(wgt);
    for (int i=0;i<allobjectslist.count();i++) {
        //qDebug() << i << objlist.at(i);
        AGui::setObjectLocalization(allobjectslist.at(i));
        //qDebug() << "AForm::AForm" << allobjlist.at(i)->metaObject()->className();
    }
}

// Локализация объекта, перевод на системный язык (параметр виджета Ru,En)
void AGui::setObjectLocalization(QObject *object) {
    //qDebug() << "AGui::setObjectLocalization" << object;
    QString objectClass = object->metaObject()->className();
    if (objectClass == "QTabWidget") {
        QTabWidget *qtw = static_cast<QTabWidget *>(object);
        int i;
        for (i=0;i<qtw->count();i++) {
            if (globals["Lang"] == "RU" && qtw->widget(i)->property("Ru").toString()!="") {
                qtw->setTabText(i, qtw->widget(i)->property("Ru").toString());
            }
            if (globals["Lang"] == "EN" && qtw->widget(i)->property("En").toString()!="") {
                qtw->setTabText(i, qtw->widget(i)->property("En").toString());
            }
        }
    }
    if (objectClass == "QAction") {
        QAction *qa = static_cast<QAction *>(object);
        //qDebug() << "AGui::setObjectLocalization" << qa << object->property("Ru").toString() << object->property("En").toString();
        if (globals["Lang"] == "RU" && object->property("Ru").toString()!="") {
            qa->setText(object->property("Ru").toString());
        }
        if (globals["Lang"] == "EN" && object->property("En").toString()!="") {
            qa->setText(object->property("En").toString());
        }
    }
    if (objectClass == "QMenuBar") {
        QMenuBar *qmb = static_cast<QMenuBar *>(object);
        //qDebug() << "AGui::setObjectLocalization" << qmb->children().count();
        for (int i=0;i<qmb->children().count();i++) {
            //qDebug() << "AGui::setObjectLocalization" << object << object->property("Ru").toString();
            QMenu *qm = static_cast<QMenu *>(qmb->children().at(i));
            if (globals["Lang"] == "RU" && qm->property("Ru").toString()!="") {
                qm->setTitle(qm->property("Ru").toString());
            }
            if (globals["Lang"] == "EN" && qm->property("En").toString()!="") {
                qm->setTitle(qm->property("En").toString());
            }
        }
    }
    if (objectClass == "QLabel") {
        QLabel *ql = static_cast<QLabel *>(object);
        if (globals["Lang"] == "RU" && object->property("Ru").toString()!="") {
            ql->setText(object->property("Ru").toString());
        }
        if (globals["Lang"] == "EN" && object->property("En").toString()!="") {
            ql->setText(object->property("En").toString());
        }
    }
    if (objectClass == "QDialog" || objectClass == "QInputDialog") {
        if (globals["Lang"] == "RU" && object->property("Ru").toString()!="") {
            QDialog *qd = static_cast<QDialog *>(object);
            qd->setWindowTitle(object->property("Ru").toString());
        }
        if (globals["Lang"] == "EN" && object->property("En").toString()!="") {
            QDialog *qd = static_cast<QDialog *>(object);
            qd->setWindowTitle(object->property("En").toString());
        }
    }
    if (objectClass == "QPushButton") {
        if (globals["Lang"] == "RU" && object->property("Ru").toString()!="") {
            QPushButton *qpb = static_cast<QPushButton *>(object);
            qpb->setText(object->property("Ru").toString());
        }
        if (globals["Lang"] == "EN" && object->property("En").toString()!="") {
            QPushButton *qd = static_cast<QPushButton *>(object);
            qd->setText(object->property("En").toString());
        }
    }
    if (objectClass == "QWidget") {
        QWidget *qwi = static_cast<QWidget *>(object);
        if (globals["Lang"] == "RU" && object->property("Ru").toString()!="") {            
            qwi->setWindowTitle(object->property("Ru").toString());
        }
        if (globals["Lang"] == "EN" && object->property("En").toString()!="") {
            qwi->setWindowTitle(object->property("En").toString());
        }
    }

}

void AGui::moveWidget(QWidget *widget, int x, int y) {
    widget->move(x, y);
    widget->show();
}

#include <QApplication>
void AGui::smoothMoveWidget(QWidget *widget, int x_offset, int y_offset) {
    int acceleration = 5;  // acceleration - pixel per second
    int v = 5;              // speed - pixel per second
    int pause;
    int timer=0;

    int start_pos = widget->x();
    int offset=0;
    if (x_offset>0) {offset = 1;} else {offset=-1; x_offset = -x_offset;}

//    x_offset = widget->x() + x_offset;
    for (int i=0; i<x_offset; i=i+5)   {
        widget->move(start_pos+offset*i, widget->y());
//        QApplication::processEvents();
        widget->repaint();
        pause = 100/v;
        AApplication::pause(pause);

        timer = timer+pause;
        v = timer*acceleration/100;
//        qDebug() << "pause" << pause << "timer" << timer << "v" << v;

    }
}

void AGui::resize(QWidget *widget, int width, int height) {
//    qDebug() << "AGui::resize" << widget << width << height;
    if (width==-1) {width=widget->width();}
    if (height==-1) {height=widget->height();}
    widget->resize(width, height);
 }

// SendKeys functions
void AGui::selectAllText(QObject *obj) {
//    qDebug() << "AGui::selectAllText" << obj << obj->metaObject()->className();
//    QString objclass = obj->metaObject()->className();
//    if (objclass=="QLineEdit") {
//        QLineEdit *le = static_cast<QLineEdit*>(obj);
////        qDebug() << "selectAll" << le;
//        le->selectAll();
//    } else {
//        QKeyEvent event(QEvent::KeyPress, Qt::Key_A, Qt::ControlModifier);
//        QApplication::sendEvent(obj, &event);
//    }
}
void AGui::openComboBox(QObject *obj) {
    QKeyEvent event(QEvent::KeyPress, Qt::Key_Down, Qt::AltModifier);
    QApplication::sendEvent(obj, &event);
}

void AGui::setFocus(QWidget *wgt) {
    wgt->setFocus(Qt::TabFocusReason);
    QString objClassName = wgt->metaObject()->className();
    //if (objClassName=="QLineEdit") {AGui::selectAllText(wgt);}
    if (objClassName=="QComboBox") {
        if (wgt->property("ifDropDown").toBool()) {AGui::openComboBox(wgt);}
        else {AGui::selectAllText(wgt);}
    }
}

void AGui::hideWidget(QWidget *wgt) {
    wgt->hide();
}
void AGui::showWidget(QWidget *wgt) {
    wgt->show();
}

QWidget * AGui::getParentWidget(QWidget*widget) {
    return widget->parentWidget();
}

bool AGui::setProperty(QWidget *wgt, QString name, QVariant value) {
    QByteArray ba;
        for (int i=0;i<name.length();i++) {
            ba.append(name.at(i));
        }
    const char* temp = ba.constData();
    //    qDebug() << "AGui::setProperty" << name << value;
    return wgt->setProperty(temp, value);
}

#include <QLineEdit>
#include <QImage>
#include "media/aimage.h"
QObject * AGui::createWidget(QWidget *parent, QString type, int x, int y, int width, int height, QString name) {
    QObject *result;
    QWidget *obj;
    if (name=="") {name="autoCreate";}
    if (type=="APush") {
        obj = new APush(parent);
        obj->setObjectName(name);
        obj->resize(QSize(width, height));
        obj->move(QPoint(x,y));
        obj->show();
    }
    if (type=="AImage") {
        obj = new AImage(parent);
        obj->setObjectName(name);
        obj->resize(QSize(width, height));
        obj->move(QPoint(x,y));
        obj->show();
    }
    if (type=="QPush") {
        obj = new QPushButton(parent);
        obj->setObjectName(name);
        obj->resize(QSize(width, height));
        obj->move(QPoint(x,y));
        obj->show();
    }
    if (type=="QLineEdit") {
        obj = new QLineEdit(parent);
        obj->setObjectName(name);
        obj->resize(QSize(width, height));
        obj->move(QPoint(x,y));
        obj->show();
    }
//    if (type=="QImage") {
//        QImage * img = new QImage();
//        img->setParent(parent);
//        img->setObjectName(name);
//        img->resize(QSize(width, height));
//        img->move(QPoint(x,y));
//        img->show();
//        result = img;
//    }
    return obj;
}
void AGui::destroyWidget(QWidget *widget) {
    if (widget) {widget->~QWidget();}
}

QObjectList AGui::getChildren(QWidget *wgt, QString datatype) {
    qDebug() << "AGui::getChildren";
    QObjectList result;
    if (wgt) {
        result = wgt->children();
        for (int i=0;i<result.count();i++) {
            if (result.at(i)->metaObject()->className()!=datatype && datatype!="") {result.removeAt(i); i--;}
        }
    }
    qDebug() << "AGui::getChildren end "<<result;
    return result;
}

QObjectList AGui::getAllChildrens(QObject *obj) { // Сбор всех объектов данной формы
//    qDebug() << "AGui::getFormAllObjs " << objtemp;
    QObjectList objlist;
    QObjectList objlisttemp = obj->children();
    QObjectList objlisttemp2;
    for (int i=0;i<objlisttemp.count();i++) {
        QObject *objtemp = objlisttemp.at(i);
        QString objClassName = objtemp->metaObject()->className();
        QString objName = objtemp->objectName();
        objlist.append(getAllChildrens(objtemp));
        if (objName!="") {
            objlisttemp2.append(objtemp);
        }
    }
    objlist.append(objlisttemp2);
    return objlist;
}

bool AGui::checkObjectContains(QObject *wgt, QObject *obj) {
    QObjectList childrenList = AGui::getAllChildrens(wgt);
    return childrenList.contains(obj);
}

void AGui::scrollToCurrent(QTableWidget *tw) {
//    qDebug() << "AGui::scrollToCurrent";
    tw->scrollToItem(tw->currentItem(), QAbstractItemView::EnsureVisible);
}

void AGui::setTabEnabled(QTabWidget *tab, int index, bool state) {
    tab->setTabEnabled(index, state);
}



void AGui::setPBText(QPushButton *pb, QString text) {
    pb->setText(text);
}
void AGui::setLeAlign(QLineEdit *le, QString align) {
    if (align=="left") {le->setAlignment(Qt::AlignLeft);}
    if (align=="center") {le->setAlignment(Qt::AlignHCenter);}
    if (align=="right") {le->setAlignment(Qt::AlignRight);}
}
void AGui::LeBackspace(QObject *obj) {
    QLineEdit *le = static_cast<QLineEdit*>(obj);
    if (le) le->backspace();
}

void AGui::setLeCursorPosition(QLineEdit *le, int pos) {
    qDebug() << "AGui::setLeCursorPosition" << le << pos;
    le->setCursorPosition(pos);
}
void AGui::insertLeText(QObject *obj, QString text) {
        QLineEdit *le = static_cast<QLineEdit*>(obj);
        if (le) le->insert(text);
}

#include "adataset.h"
#include "adatabase.h"
#include "QSpacerItem"

void AGui::setHtml(QObject *obj, QString html) {
    QString metatype = obj->metaObject()->className();
    if (metatype=="QWebEngineView") {
        QWebEngineView * wv = static_cast<QWebEngineView*>(obj);
        QUrl baseUrl =  QUrl::fromLocalFile( QDir::currentPath()+"/");
        //55wv->setHtml(html, baseUrl);
        //55wv->page()->mainFrame()->evaluateJavaScript("document.body.style.margin='0';");
    } else {
        qDebug() << "Warning! AGui::setHtml - object have not QWebEngineView type";
    }
}

void AGui::setTWHeader(QTableWidget *table, QString orientation, bool state) {
    if (orientation=="horizontal") {table->horizontalHeader()->setVisible(state);}
    if (orientation=="vertical") {table->verticalHeader()->setVisible(state);}
}








//void AGui::setNoSelection(QObject *obj) {
//    QTableWidget *table = static_cast<QTableWidget *>(obj);
//    table->setSelectionMode(QAbstractItemView::NoSelection);
//}


//void AGui::table_highlightRow(QTableWidget *tablewidget, int row, QString color) {                  // Раскрашивает фон строки TableWidget
//    if (tablewidget->rowCount()>row) {
//        for (int column=0;column<tablewidget->columnCount();column++) {
////            qDebug() << "ATableWidget::highlightRow" << column;
//            QTableWidgetItem *item = tablewidget->item(row, column);
//            item->setBackgroundColor(QColor(color));
//        }
//    }
//}


void AGui::table_highlightRowText(QTableWidget *tablewidget, int row, QString color, bool ifBold) {              // Раскрашивает текст рядока TableWidget
//    qDebug() << "ATableWidget::highlightRow" << highlightedRows << tablewidget->columnCount();
    if (row < tablewidget->rowCount()) {
        for (int column=0;column<tablewidget->columnCount();column++) {
//            qDebug() << "ATableWidget::highlightRow" << column;
            QTableWidgetItem *item = tablewidget->item(row, column);
            if (item) {
                if (color!="") {item->setTextColor(QColor(color));}
                QFont font = item->font();
                font.setBold(ifBold);
                item->setFont(font);
                //highlightedRows.replace(row, color);
            }
            //            ADelegate *delegate = static_cast<ADelegate*>(tablewidget->itemDelegate());
//            delegate->setHiglightColor("#aa3148");
        }
    }
}

//void AGui::table_highlightColumn(QTableWidget *tablewidget, int column, QColor color) {        // Раскрашивает столбец TableWidget
//    if (tablewidget->columnCount()>column) {
//        for (int row=0;row<tablewidget->rowCount();row++) {
//            QTableWidgetItem *item = tablewidget->item(row, column);
//            item->setBackgroundColor(color);
//        }
//    }
//}
//void AGui::table_highlightCell(QTableWidget *tablewidget, int row, int column, QColor color) { // Раскрашивает ячейку TableWidget
//    if (tablewidget->rowCount()>row && tablewidget->columnCount()>column) {
//        QTableWidgetItem *item = tablewidget->item(row, column);
//        item->setBackgroundColor(color);
//    }
//}
//void AGui::table_cellTextColor(QTableWidget *tablewidget, int row, int column, QColor color) { // Меняет цвет текста в ячейке TableWidget
//    if (tablewidget->rowCount()>row && tablewidget->columnCount()>column) {
//        QTableWidgetItem *item = tablewidget->item(row, column);
//        item->setTextColor(color);
//    }
//}
