#include "adataset.h"
#include "aapplication.h"
#include "afunc.h"
#include "adatabase.h"
#include "adict.h"
#include "avariant.h"
#include "forms/atreeview.h"
#include "forms/atablewidget.h"
#include "forms/atreeview.h"
#include "forms/acombocheckbox.h"
#include "globals.h"
#include "forms/atablewidgetstatic.h"

#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QTableWidget>
#include <QTreeWidget>
#include <QDateEdit>
#include <QTreeView>
#include <QDirModel>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QRadioButton>
#include <QKeyEvent>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QGridLayout>

ADataset::ADataset(QObject *parent) : QObject(parent) {
    this->setObjectName("ADataset");
}

ADataset::~ADataset() {
//    qDebug() << "~ADataset";
}

////////////////////////////////////////////////////////////////////
//// ---- Read widget dataset params
////////////////////////////////////////////////////////////////////
QString ADataset::readDataSource(QObject *object) {
    QString sqlSource = ADataset::updateStringVariablesFromWidget(object->property("SqlSource").toString(), object);
    sqlSource = ADataset::updateStringVariablesFromWidget(sqlSource, object);
    //qDebug() << "ADataset::readDataSource" << object << sqlSource;
    //if (sqlSource=="") {return "";}
    //qDebug() << "ADataset::readDataSource " << object->objectName() << sqlSource;
    //bool ifDebug = object->property("ifDebug").toBool();
    if (sqlSource=="") {
        QString objectClass = object->metaObject()->className();
        QString dataSource = object->property("DataSource").toString();
        //qDebug() << "ADataset::readDataSource" << dataSource;
        if (dataSource!="") {
            QString filter = ADataset::updateStringVariablesFromWidget(object->property("Filter").toString(), object);
            if (filter!="") {filter.prepend(" WHERE ");}
            QString orderby = ADataset::updateStringVariablesFromWidget(object->property("OrderBy").toString(), object);
            QString keyfield = ADataset::updateStringVariablesFromWidget(object->property("KeyField").toString(), object);
            if (orderby!="") {orderby.prepend(" ORDER BY ");}
            else if (keyfield!="") { orderby = " ORDER BY "+keyfield;}
            if (objectClass == "QComboBox") {
                QString keyfield = object->property("FieldKey").toString();
                QString dataField = object->property("FieldData").toString();
                //                bool addNullRecord = object->property("addNullRecord").toBool();
                //                bool addAllRecord = object->property("addAllRecord").toBool();
                if (dataSource!="" && keyfield!="" && dataField!="") {
                    sqlSource = "SELECT "+keyfield+", "+dataField+" FROM "+dataSource+filter+orderby;
                    //                    if (addNullRecord) {
                    //                        QString nullRecordName;
                    //                        if (addAllRecord) {nullRecordName = "( Все )";}
                    //                            sqlSource.prepend("SELECT TOP 1 0 AS "+keyfield+", '"+nullRecordName+"' AS "+dataField+" FROM "+dataSource+" UNION ALL ");
                    //                    }
                }
            }
            else {
                QString fields = object->property("Fields").toString();
                    if (fields=="") {fields = "*";}
                sqlSource = "SELECT "+fields+" FROM "+dataSource+filter+orderby;
            }
        }
    }
    //qDebug() << "Debug SQL: " << sqlSource;
    return sqlSource;
}
QSqlQueryModel * ADataset::readSourceModel(QObject *object, QSqlQueryModel *qModel) {
    QString sqlSource = ADataset::readDataSource(object);
    ADatabase::selectSql(sqlSource, "", qModel);
    return qModel;
}


////////////////////////////////////////////////////////////////////
//// ---- Универсальные функции для работы с данными элементов формы
////////////////////////////////////////////////////////////////////
void ADataset::prepareModelObject(QObject *object) {
//    qDebug() << "ADataset::Preprocessing" << object->objectName() << object->metaObject()->className();
    QString objectClass = object->metaObject()->className();

    if (objectClass == "QTableView") {
        QTableView *tblView = static_cast<QTableView *>(object);
        tblView->setModel(new QStandardItemModel());
    }
    if (objectClass == "QTreeView") {
        QTreeView *treeView = static_cast<QTreeView *>(object);
        treeView->setModel(new QStandardItemModel());
        ATreeView::static_createTreeView(treeView);
    }
    if (objectClass=="QScrollArea") {
        QScrollArea *sa = static_cast<QScrollArea *>(object);
        sa->setWidget(new QWidget());
        QGridLayout *layout = new QGridLayout();
        sa->widget()->setLayout(layout);
    }
    if (objectClass == "QTableWidget") {
        //QTableWidget *tblWidget = static_cast<QTableWidget *>(object);
        //tblWidget->setModel(new QStandardItemModel);
    }
    if (objectClass == "QTreeWidget") {
        //QTreeWidget *treeWidget = static_cast<QTreeWidget *>(object);
        //treeWidget->setModel(new QStandardItemModel);
    }
}

void ADataset::updateObject(QObject *object, int id) {
    //qDebug() << "ADataset::updateObject " << object << object->objectName() << object->metaObject()->className();
    QString objectClass = object->metaObject()->className();
    if (objectClass == "QTreeView") {
        QTreeView *tv = static_cast<QTreeView *>(object);
        if (id!=0) {
            ATreeView::static_updateRecord(tv, tv, id);
        } else {
            ATreeView::static_update(tv, tv);
        }
    }
    if (objectClass == "QTableWidget") {
        //AFunc::echoTimer("ADataset::updateObject begin");
        QTableWidget *tablewidget = static_cast<QTableWidget *>(object);
        if (id!=0) {
            ATableWidgetStatic::static_updateRecord(tablewidget, tablewidget, id);
        } else {
            ATableWidgetStatic::static_update(tablewidget, tablewidget);
        }
        return;
    }
    if (objectClass == "QWebEngineView") {
        QWebEngineView *view = static_cast<QWebEngineView*>(object);
        QWebEnginePage *page = view->page();
        page->setHtml("abc");
    }
    if (objectClass == "QComboBox" || objectClass == "AComboCheckBox") {
        QComboBox *cBox = static_cast<QComboBox *>(object);//objectParent->findChild<QComboBox *>(objectName);
        //qDebug() << "ADataset::updateObject " << object << object->objectName() << id << cBox->property("Default").toInt();
        cBox->clear();
        cBox->setProperty("modelColumn", 0);
        //        int row = 0;
        QString key, data;
        if (cBox->property("addNullRow").toBool()) {
            key = ""; data = "";
            cBox->addItem(data, key);
        }
        if (cBox->property("addNullRecord").toBool()) {
            key = "0"; data = "( Не определено )";
            cBox->addItem(data, key);
        }
        if (cBox->property("addAllRecord").toBool()) {
            key = "-1"; data = "( Все )";
            cBox->addItem(data, key);
        }
        //if (cBox->property("Align")=="Left") {
        //    qDebug() << "ADataset::updateObject" << cBox;
        //    cBox->lineEdit()->setAlignment(Qt::AlignLeft);
        //}
        //cBox->lineEdit()->setReadOnly(true);
        //cBox->setStyleSheet("QComboBox{text-align:left;}");
        QString querystring = ADataset::readDataSource(cBox);
        //qDebug() << "ADataset::updateObject " << object << object->objectName() << id << cBox->property("Default").toInt() << querystring;
        if (cBox->property("ifDebug").toBool()) {
            qDebug() << "[Debug SQL : ADataset("+cBox->objectName()+"::updateObject]" << querystring;
        }
        ////////////////////////////
        //if (object->objectName()=="f_defLocation") {qDebug() << "ADataset::updateObject" << object << querystring << cBox->property("Connection").toString();}
        ////////////////////////////
        if (querystring!="") {
            QString connection = cBox->property("Connection").toString();
            QList<QMap<QString, QVariant> > result = ADatabase::selectList(querystring, connection);
            //qDebug() << "ADataset::updateObject" << object << querystring << cBox->property("Connection").toString() << result.count();
            if (result.count()==0) {return;}
            QString key_field = object->property("FieldKey").toString();
            QString data_field = object->property("FieldData").toString();
            //qDebug() << "result" <<  result;
            for (int i=0;i<result.count();i++) {
                key = result.at(i)[key_field].toString();
                data = result.at(i)[data_field].toString();
                ////////////////////////////
                //if (object->objectName()=="f_defLocation") {qDebug() << "ADataset::updateObject" << result.count() << key << data;;}
                ////////////////////////////
                cBox->addItem(data, key);
                cBox->setItemData(i, Qt::AlignLeft, Qt::TextAlignmentRole);
            }
            if (id != 0) {
                ADataset::setData(object, id);
            } else {
                ADataset::setDefault(cBox);
            }
            ////////////////////////////
            //if (object->objectName()=="f_defLocation") {AFunc::echoModel(cBox->model());}
            ////////////////////////////
        }
        return;
    }
}

QVariant ADataset::getData(QObject *object, int column) {
    QVariant result;
    QString objectClass = object->metaObject()->className();
    if (objectClass == "QTableView") {
        QTableView *tblView = static_cast<QTableView *>(object);
        int keycolumn;
        if (column!=-1) {keycolumn=column;}
        else if (tblView->property("KeyColumn").toInt()==0 && tblView->property("KeyColumn").toString()!="0") {keycolumn = -1;}
        else {
            keycolumn = tblView->property("KeyColumn").toInt();
            if (keycolumn<0 || keycolumn >= tblView->model()->columnCount()) {keycolumn=-1;}
        }
        if (objectClass == "QTreeView") {
            QTreeView *tv = static_cast<QTreeView *>(object);
    //        QStandardItemModel *model = static_cast<QStandardItemModel*>(tv->model());
                int keyColumn = ATreeView::getHeaderPosition(tv, "id");
                result = ATreeView::getTVData(tv, keyColumn);
        }
        QString keyfield = tblView->property("KeyField").toString();
        for (int i=0;i<tblView->model()->columnCount();i++) {if (tblView->model()->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString()==keyfield) {keycolumn = i;}}
            if (keycolumn==-1) {return 0;}
        if (tblView->selectionModel()->selectedIndexes().count()!=0) {
            int row = tblView->selectionModel()->selectedIndexes().at(0).row();
            result = AFunc::getModelValue(tblView->model(), row, keycolumn);
            //            qDebug() << "ADataset::getData " << result;
        }
        else {result = 0;}
    }
    if (objectClass == "QTreeView") {
        QTreeView *tv = static_cast<QTreeView *>(object);
//        QStandardItemModel *model = static_cast<QStandardItemModel*>(tv->model());
            int keyColumn = ATreeView::getHeaderPosition(tv, "id");
            result = ATreeView::getTVData(tv, keyColumn);
    }
    if (objectClass == "QTableWidget") {            // Предает значение выделенного элемента QTableWidget либо из указанного столбца, либо из keyColumn
        QTableWidget *tblWidget = static_cast<QTableWidget *>(object);
        if (tblWidget->selectedItems().count()>0) {
            int keyColumn = 0;
            if (column==-1) {keyColumn = tblWidget->property("KeyColumn").toInt();}
            else {keyColumn = column;}

            int selectedrow = tblWidget->selectedItems().at(0)->row();
            result = AFunc::getModelValue(tblWidget->model(), selectedrow, keyColumn);
        }
    }
    if (objectClass == "QLineEdit") {
        QLineEdit *le = static_cast<QLineEdit *>(object);
        result = AVariant::convertVariantToString(le->text(), le->property("Precision").toString());
        //qDebug() << "ADataset::getData" << le->text() << result;
        //        result = ADataset::processNumber(le->text(), le);
        //        if (le->property("Data").toString()=="sum") {qDebug() << "ADataset::getData" << le->text() << result;}
        //        le->setText(result.toString());
        }
    if (objectClass == "QComboBox") {
        QComboBox *cb = static_cast<QComboBox *>(object);
        int row = cb->currentIndex();
        if (column==-1 && !cb->property("withoutID").toBool()) {result = cb->itemData(row);}
        else {result = result = cb->itemText(row);}
        //qDebug() << "ADataset::getData cb - " << row << result;// << AFunc::getModelValue(cb->model(), row, keycolumn);
    }
    if (objectClass == "AComboCheckBox") {
        AComboCheckBox *cb = static_cast<AComboCheckBox *>(object);
        QList<QVariant> res = cb->getSelected();
        result = QVariant(res);
        //        qDebug() << "ADataset::getData cb - " << cb->objectName() << AFunc::getModelValue(cb->model(), row, keycolumn);
    }
    if (objectClass == "QCheckBox") {
        QCheckBox *chb = static_cast<QCheckBox *>(object);
        if (chb->isChecked()) {return 1;}
        else {return 0;}
        //        result = chb->isChecked();
        }
    if (objectClass == "QDateEdit") {
        QDateEdit *de = static_cast<QDateEdit *>(object);
        result = de->date().toString("yyyy-MM-dd"); // MS SQL
    }
    if (objectClass == "QTimeEdit") {
        QTimeEdit *te = static_cast<QTimeEdit *>(object);
        result = te->time().toString("hh:mm"); // MS SQL
    }
    if (objectClass == "QRadioButton") {
        QRadioButton *rb = static_cast<QRadioButton*>(object);
        result = rb->isChecked();
    }
    if (result.isNull()) {result = 0;}
    return result;
}
bool ADataset::setData(QObject *object, QVariant data) {
    //qDebug() << "ADataset::setData" << object->objectName() << data;
    QString objectClass = object->metaObject()->className();
    if (objectClass == "QTableView") {
        QTableView *tview = static_cast<QTableView *>(object);
        tview->selectRow(ADataset::findKeyIndex(tview, data.toInt()));
    }
    if (objectClass == "QTreeView") {
        QTreeView *tv = static_cast<QTreeView *>(object);
        ATreeView::static_select(tv, data.toInt());
    }
    if (objectClass == "QWebEngineView") {
        QWebEngineView *vw = static_cast<QWebEngineView *>(object);
        //55vw->setHtml(data.toString());
    }
    if (objectClass == "QTableWidget") {
        QTableWidget *twidget = static_cast<QTableWidget *>(object);
        twidget->selectRow(ADataset::findKeyIndex(twidget, data.toInt()));
    }
    if (objectClass == "QLineEdit") {
        QLineEdit *lEdit = static_cast<QLineEdit *>(object);
        lEdit->setText(AVariant::convertVariantToString(data, lEdit->property("Precision").toString()));
    }
    if (objectClass == "QComboBox") {
        QComboBox *cBox = static_cast<QComboBox *>(object);
        int index = cBox->findData(data);//ADataset::findKeyIndex(cBox, data.toInt());
        //qDebug() << "ADataset::setData " << object << data.toString() << index;
        if (index==-1) {return false;}
        cBox->setCurrentIndex(index);
        return true;
    }
    if (objectClass == "QCheckBox") {
        QCheckBox *chBox = static_cast<QCheckBox *>(object);
        if (data.toBool()) {chBox->setCheckState(Qt::Checked);} else {chBox->setCheckState(Qt::Unchecked);}
    }
    if (objectClass == "QDateEdit") {
        QDateEdit *de = static_cast<QDateEdit *>(object);
        //qDebug() << "setData" << object->objectName() << data << AVariant::convertVariantToDate(data);
        de->setDate(AVariant::convertVariantToDate(data));
    }
    if (objectClass == "QTimeEdit") {
        QTimeEdit *te = static_cast<QTimeEdit *>(object);
        te->setTime(data.toTime());
        te->setTime(QTime::fromString(data.toString(), "hh:mm"));
    }

    if (objectClass == "QRadioButton") {
        QRadioButton *rb = static_cast<QRadioButton*>(object);
        rb->setChecked(data.toBool());
    }
    return true;
}

// Установка значения "по умолчанию" (параметр Default)
void ADataset::setDefault(QObject *object) {
    // qDebug() << "ADataset::setDefault";
    QString objectClass = object->metaObject()->className();
    QVariant defvalue = object->property("Default");
    if (objectClass == "QDateEdit") {
        if (defvalue.toString()=="") {defvalue=QDate::currentDate().toString(Qt::SystemLocaleDate);}
        ADataset::setData(object, defvalue);
    } else if (objectClass == "QLineEdit" || objectClass == "QComboBox" || objectClass == "QCheckBox") {
        ADataset::setData(object, ADataset::updateStringVariablesFromWidget(defvalue.toString(), object));
        //qDebug() << " ADataset::setDefault" << object << defvalue << ADataset::updateStringVariablesFromWidget(defvalue.toString(), object);
    }

}



////////////////////////////////////////////////////////////////////
//// ---- Разное
////////////////////////////////////////////////////////////////////

#include <QCalendarWidget>
// Заполняет comboBox произвольными данными
void ADataset::fillComboBox(QObject *obj, QStringList stringList) {
//    qDebug() << "ADataset::fillComboBox" << stringList.count();
    QComboBox *cb = static_cast<QComboBox*>(obj);
    if (!cb) return;    
    if (cb->property("addNullRecord").toBool()) {cb->addItem("");}
    cb->addItems(stringList);
    for (int i=0;i<cb->count();i++) {
        cb->setItemData(i, i);
    }
    ADataset::setDefault(cb);
}
// Выбирает элемент в произвольном ComboBox (текстовом)
void ADataset::setComboBoxData(QObject *obj, QString text) {
//    qDebug() << "ADataset::fillComboBox" << stringList.count();
    QComboBox *cb = static_cast<QComboBox*>(obj);
    if (!cb) return;
    int index = cb->findText(text);
    cb->setCurrentIndex(index);
}

/////////////////////////////////////////////
//// ---- Work with String variables
////////////////////////////////////////////
// Update all %% local variables
QString ADataset::updateLocalStringVariables(QString var, QObject *object) {
    QString result="";
    if (var.contains("lang_")) {
        var = var.remove(0,5);
        if (globals["Lang"]=="RU") {
            var = "ru_"+var;
        } else if (globals["Lang"]=="EN") {
            var = "en_"+var;
        }
    }
    result = object->property(var.toLocal8Bit().data()).toString();
    if (var.contains("reg_")) {
        var = var.remove(0, 4);
        result = AApplication::getRegValue(var).toString();
    }
    if (var.contains("local_")) {
        var = var.remove(0, 6);
        result = AApplication::getLocalSetting(var).toString();
    }
    if (var.contains("glob_")) {
        var = var.remove(0, 5);
        result = AApplication::getGlobalVar(var);
    }
    return result;
}

// Update all %% widget parameters
QString ADataset::updateStringVariablesFromWidget(QString str, QObject *object) {   
    //qDebug() << "ADataset::updateStringVariablesFromWidget" << object << str;
    QStringList varlist;
    int x=0;
    int y=0;
    for (int i=0;i<str.count("%")/2;i++) {
        x = str.indexOf("%",x)+1;
        y = str.indexOf("%",x);
        if (!varlist.contains(str.mid(x,y-x))) {
            varlist << str.mid(x,y-x);
            //qDebug() << "ADataset::updateStringVariablesFromWidget " << str.mid(x,y-x);
        }
    x = y+1;
    }
    //qDebug() << "ADataset::updateStringVariablesFromWidget" << varlist;
    for (int i=0;i<varlist.count();i++) {
        //// Превращаем строковую переменную %х% в значение динамического параметра элемента или подставляем значение реестра
        str.replace("%"+varlist.at(i)+"%", ADataset::updateLocalStringVariables(varlist.at(i), object));
        //qDebug() << "ADataset::updateStringVariablesFromWidget" << object << varlist.at(i) << ADataset::updateLocalStringVariables(varlist.at(i), object);
    }
    return str;
}

// Update all %% from
QString ADataset::parseHtml(QString html, QMap <QString, QVariant> mapname, QString querystring) {
    html.replace("%currentpath%", QDir::currentPath());

    QRegExp rx_begin("(%begin%)");
    QRegExp rx_end("(%end%)");
    QRegExp pattern("([^\\][.][%]{1}[0-9a-zA-Z_]+[%]{1})");
    QString wildcard_p1 = html.left(html.indexOf(rx_begin));
    QString wildcard_p2 = html.left(html.indexOf(rx_end)).remove(0, html.indexOf(rx_begin)+7);
    QString wildcard_p3 = html.remove(0, html.indexOf(rx_end)+5);
    QString check_p1, check_p2,check_p3, name_T, value_T, wildcard_Tp2;

    int length = 0;
    // Шапка
    int pos = 0;
    while ((pos = pattern.indexIn(wildcard_p1, pos)) != -1) {
        name_T = pattern.cap(0).remove(0, 1);
        //qDebug() << "ADataset::parseHtml" << pos << name_T << name_T.length();
        pos++;
        //length = ;
        name_T = name_T.remove('%');
        if (mapname[name_T].toString()!="") {
            value_T = AVariant::convertVariantToString(mapname.take(name_T), "");
            //if (value_T == "") {value_T = AApplication::getRegValue(name_T).toString();}

            wildcard_p1.remove(pos, name_T.length()+2);
            wildcard_p1.insert(pos, value_T);
            //qDebug() << "ADataset::parseHtml" << value_T;
        } else {
            value_T = "%"+name_T+"%";
        }
        //qDebug() << "ADataset::parseHtml" << value_T.length();

        pos += value_T.length();
    }

    QSqlQueryModel *qModel = new QSqlQueryModel();
    ADatabase::selectSql(querystring, "", qModel);
    QAbstractItemModel * model = static_cast<QAbstractItemModel*>(qModel);

    QMap<QString, int> columnIndex;
    for (int i=0;i<model->columnCount();i++) {
        columnIndex[model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString()] = i;
    }

    //qDebug() << "ADataset::parseHtml" << querystring;
    for (int i=0; i<model->rowCount(); i++) {
        wildcard_Tp2 = wildcard_p2;
        int pos = 0;
        while ((pos = pattern.indexIn(wildcard_Tp2, pos)) != -1) {
            name_T = pattern.cap(0).remove(0, 1);
            pos++;
            length = name_T.length();
            name_T = name_T.remove('%');

            value_T = AVariant::convertVariantToString(AFunc::getModelValue(model, i, columnIndex[name_T]), "2");
            wildcard_Tp2.remove(pos, length);
            wildcard_Tp2.insert(pos, value_T);
            pos += value_T.length();
        }
        //qDebug() << "ADataset::parseHtml" << wildcard_Tp2;
        check_p2.append(wildcard_Tp2);
    }

    // FOOTER
    pos=0;
    while ((pos = pattern.indexIn(wildcard_p3, pos)) != -1) {
        name_T = pattern.cap(1).remove(0, 1);
        pos++;
        length = name_T.length();
        name_T = name_T.remove('%');
        value_T = AVariant::convertVariantToString(mapname.take(name_T), "2");
        if (value_T == ""){
            if (name_T.mid(0, 4)=="sum_") {
                QString field = name_T.mid(4, name_T.length()-4);
                if (columnIndex.keys().contains(field)) {
                    float sum=0;
                    for (int i=0;i<model->rowCount();i++) {
                        sum = sum + AFunc::getModelValue(model, i, columnIndex[field]).toFloat();
                    }
                    value_T = QString::number(sum);
                }
            } else {
                value_T = AApplication::getRegValue(name_T).toString();
            }
        }
        wildcard_p3.remove(pos, length);
        wildcard_p3.insert(pos, value_T);
        pos += value_T.length();
    }
    QString result = wildcard_p1 + check_p2 + wildcard_p3;
    //qDebug() << "ADataset::parseHtml 1" << wildcard_p1 << "\n";
    //qDebug() << "ADataset::parseHtml 2" << check_p2 << "\n";
    //qDebug() << "ADataset::parseHtml 3" << wildcard_p3 << "\n";

    result = result.replace("\\%", "%");
//    qDebug() << result.indexOf("\%") << result.indexOf("\\  %");

    qModel->~QSqlQueryModel();
    return result;
}

/////////////////////////////////////////////
//// ---- Работа с произвольными данными
////////////////////////////////////////////
// Опознавание числа
//QVariant ADataset::detectNumber(QVariant data, int precision) {
//    //    qDebug() << "ADataset::detectNumber" << data << precision << data.typeName();
//    QString datatype = data.typeName();
//    if (datatype=="QString") {

//    } else {return data;}
//}


/////////////////////////////////////////////
//// ---- Функции QTableWidget
////////////////////////////////////////////
QVariant ADataset::getTableCellData(QTableWidget *tablewidget, int row, int column) {
    QTableWidgetItem *item = tablewidget->item(row, column);
    if (item) {
        return item->data(Qt::EditRole);
    }
    return "";
}

void ADataset::setTableCellData(QTableWidget *tablewidget, int row, int column, QVariant data) {
    QTableWidgetItem *item = tablewidget->item(row, column);
    if (item) {
        item->setData(Qt::EditRole, data);
        //tablewidget->setItem(row, column, item);
    }
}
int ADataset::findTableCellData(QTableWidget *tablewidget, int column, QVariant data) {
    QTableWidgetItem *item;
    for (int row=0;row<tablewidget->rowCount();row++) {
        item = tablewidget->item(row, column);
        if (item->data(Qt::EditRole).toString()==data.toString()) {
            return row;
        }
    }
    return -1;
}
int ADataset::getTableSelectedRow(QTableWidget *tablewidget) {
    int result=-1;
    int count = tablewidget->selectionModel()->selectedRows().count();
    if (count != 0) {
        result = tablewidget->selectionModel()->selectedRows().at(0).row();
    }
    return result;
}

// ---- Поиск значения в поле модели - возврат индекса
int ADataset::findModelIndex(QAbstractItemModel *model, int column, QVariant data) {
    int result = 0;
    for (int i=0;i<model->rowCount();i++) {
        QModelIndex index = model->index(i, column, QModelIndex());
        if (model->data(index, Qt::DisplayRole) == data) {result = i;}
    }
    return result;
}
int ADataset::findKeyIndex(QObject *object, QVariant data, int column){
    QAbstractItemModel *model = 0;
    int keycolumn = object->property("KeyColumn").toInt();
    if (column!=-1) {keycolumn = column;}
    if (keycolumn==-1) {return 0;}
//    qDebug() << "ADataset::findKeyIndex " << keycolumn;
    QString objectClass = object->metaObject()->className();
    if (objectClass=="QTableWidget") {QTableWidget *obj = static_cast<QTableWidget*>(object); model = obj->model();}
    if (objectClass=="QTableView") {QTableView *obj = static_cast<QTableView*>(object); model = obj->model();}
    if (objectClass=="QComboBox") {QComboBox *obj = static_cast<QComboBox*>(object); model = obj->model();}
    if (model==0) {return 0;}
    for (int row=0;row<model->rowCount();row++) {
        QVariant result = AFunc::getModelValue(model, row, keycolumn);
//        qDebug() << "ADataset::findKeyIndex result-data " << result << data;
        if (result==data) {return row;}
    }
    return -1;
}


QString ADataset::convertToScriptDate(QString srcdate) {
    QDate date = QDate::fromString(srcdate, "yyyy-MM-dd");
    return date.toString("yyyy/MM/dd");
}
QDate ADataset::convertToDate(QString srcdate) {
    QDate date = QDate::fromString(srcdate, "dd.MM.yyyy");
    //qDebug() << "ADataset::convertToDate" << date.isValid() << date.isNull();
    if (date.isNull()) return QDate();
    return date;
}
QList<int> ADataset::findTreeTableSubRecords(QString dictName, int id) {
    QList<int> result;
    QList<QMap<QString, QVariant> > list = ADatabase::selectList("SELECT id, parId FROM "+dictName+" WHERE parId='"+QString::number(id)+"'");
//    QMap temp;
    for (int i=0;i<list.count();i++) {
//        temp = QVariant();
        result.append(list.at(i)["id"].toInt());
        result.append(ADataset::findTreeTableSubRecords(dictName, list.at(i)["id"].toInt()));
    }
    return result;
}





