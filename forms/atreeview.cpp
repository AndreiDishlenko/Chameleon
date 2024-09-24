#include <QTreeView>
#include <aform.h>
#include <QStandardItemModel>
#include "aengine.h"
#include "adataset.h"
#include "afunc.h"
#include "forms/atreeview.h"
#include "forms/adelegate.h"
#include "avariant.h"

ATreeView::ATreeView(QWidget *parent) {
    //qDebug() << "ATreeView::ATreeView";
    this->setObjectName("Tree");
    this->setParent(parent);
    widget = parent;

    engine = parent->findChild<AEngine*>("qs");
    formtype = widget->property("FormType").toString();
    dataobjlist = AForm::getFormDataObjects(widget);

    for (int i=0;i<dataobjlist.count()-1;i++){
        for (int j=0;j<dataobjlist.count()-1;j++){
            int to1=dataobjlist.at(j)->property("TableOrder").toInt();
            int to2=dataobjlist.at(j+1)->property("TableOrder").toInt();
            if (to1>to2) {dataobjlist.swap(j,j+1);}
        }
    }

    this->createTreeView();
}
ATreeView::~ATreeView(){
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Работа с одиночными TreeView (Статические методы)
/////////////////////////////////////////////////////////////////////////////////////////////////
void ATreeView::static_createTreeView(QTreeView *treeview) {
    //qDebug() << "ATreeView::static_createTreeView";
    QStandardItemModel *model = static_cast<QStandardItemModel*>(treeview->model());
    model->clear();
    int startpos=0;
    if (treeview->property("type").toString()=="isFlat") {treeview->setProperty("isFlat", true); startpos=1;}
    // Создание модели
    // Чтение источника данных
    QString sqlString = ADataset::readDataSource(treeview);
    if (sqlString=="") {return;}
    // Создание столбцов
    QList<QString> fieldsList = ADatabase::selectFieldsList(sqlString);
    if (fieldsList.count()==0) {return;}
    model->setColumnCount(fieldsList.count()+startpos);
    // Названия и ширина заголовков
    if (treeview->property("isFlat").toBool()) {
        model->setHeaderData(0, Qt::Horizontal, "");
        treeview->setColumnWidth(0, 60);
    }
    QStringList columnsWidthList = AFunc::strToList(treeview->property("ColumnsWidth").toString());

    QString header;
    int width;
    for (int i=0;i<fieldsList.count();i++) {
        header = fieldsList.at(i);
        model->setHeaderData(i+startpos, Qt::Horizontal, header);
        model->setHeaderData(i+startpos, Qt::Horizontal, Qt::AlignCenter, Qt::TextAlignmentRole);
        if (i<columnsWidthList.count()) {
            width = columnsWidthList[i].toInt();
            treeview->setColumnWidth(i+startpos, width);
            if (width==0) {treeview->hideColumn(i+startpos);}
        }
    }

    // Ширина столбцов
    QMap<QString, QVariant> temp;
    QList< QMap<QString, QVariant> > columnProps;
    for (int i1=0;i1<fieldsList.count();i1++) {
        temp["Data"] = fieldsList.at(i1);
        columnProps << temp;
    }
    // Свойства дерева
    int keyColumn = fieldsList.indexOf("id");
    if (keyColumn==-1) {qDebug() << "Warning (ADataset::createTreeView). For treeview "+treeview->objectName()+" id is not defined. Fields is not created"; return;}
    treeview->setProperty("keyColumn", keyColumn);

    // Создание корневого узла
    QStandardItem *parentItem = model->invisibleRootItem();
    QString rootItemName = treeview->property("rootItemName").toString();
    if (rootItemName != "") {
        int nameColumn = treeview->property("nameColumn").toInt();
        if (treeview->property("nameColumn").toString()!="") {
            QStandardItem *subNode = new QStandardItem("0");
            parentItem->setChild(0, keyColumn, subNode);
            subNode = new QStandardItem(rootItemName);
            parentItem->setChild(0, nameColumn+startpos, subNode);
            QString header = model->headerData(0, Qt::Horizontal).toString();

            if (treeview->property("isFlat").toBool() && header!="id" && header!="name") {
                QStandardItem *firstItem = new QStandardItem(" ");
                parentItem->setChild(0, 0, firstItem);
                parentItem = firstItem;
            } else {
                parentItem = parentItem->child(0,0);
            }
        }
    }

}
void ATreeView::static_update(QWidget *widget, QTreeView *treeview, int startId, QList< QMap<QString, QVariant> > columnProps) {
    //qDebug() << "ATreeView::static_update" << startId;
    QStandardItemModel *model = static_cast<QStandardItemModel*>(treeview->model());
    if (model->columnCount()==0) {ATreeView::static_createTreeView(treeview);}
    if (model->columnCount()==0) {return;}
    QStringList columnsList = ATreeView::getColumnsList(treeview);
    if (!columnsList.contains("id")) { qDebug() << "Warning (ATreeView::static_update). TreeView '"+treeview->objectName()+"' datasource doesn't contains id field. Update cancelled."; return;}
    if (!columnsList.contains("parId")) { qDebug() << "Warning (ATreeView::static_update). TreeView '"+treeview->objectName()+"' datasource doesn't contains parId field. Update cancelled."; return;}
    if (columnProps.isEmpty()) {columnProps = ATreeView::getColumnProps(treeview);}
    QString header = "";
    QString sqlString = ADataset::readDataSource(widget);
    //qDebug() << "ATreeView::static_update" << widget->property("ifDebug").toBool();
    if (widget->property("ifDebug").toBool()) {qDebug() << "[Debug SQL : ATreeView] " << sqlString;}

    QList< QMap<QString, QVariant> > tree = ADatabase::selectList(sqlString);
    //QMap<QString, QVariant> tempMap;
    //tempMap["id"]=-1;
    //tempMap["parId"]=0;
    //tempMap["name"]="( Не определено )";
    //tree.prepend(tempMap);
    QStandardItem *parentItem = model->invisibleRootItem();
    QModelIndex parindex = ATreeView::findIndex(treeview, startId);
    if (parindex.isValid()) {
        QStandardItemModel *model1 = static_cast<QStandardItemModel*>(model);
        parentItem = model1->itemFromIndex(parindex);
    }
    ATreeView::addTVNode(parentItem, startId, tree, columnProps, treeview->property("isFlat").toBool());
    //qDebug() << "ATreeView::static_update END";
}

void ATreeView::static_updateRecord(QWidget *widget, QTreeView *treeview, int id, QList< QMap<QString, QVariant> > columnProps) {
    //qDebug() << "ATreeView::static_updateRecord" << treeview->objectName() << id;
    QAbstractItemModel *model = treeview->model();
    if (model->columnCount()==0) {ATreeView::static_createTreeView(treeview);}
    if (model->columnCount()==0) {return;}

    // Чтение источника данных
    QString sqlString = ADataset::readDataSource(widget);
    QMap<int, QMap<QString, QVariant> > records = ADatabase::selectMap(sqlString, "", "ATV("+widget->objectName()+")");
    //qDebug() << records;
    // Поиск id в таблице
    QModelIndex startindex = ATreeView::findIndex(treeview, id);
    //qDebug() << "ATreeView::static_updateRecord id_index" << startindex;
    //qDebug() << "ATreeView::static_updateRecord parent_index" << startindex.parent();

    QList<int> ids;
    if (startindex!=QModelIndex()) {
        ids = ATreeView::findSubNodes(treeview, startindex);
    }
    //qDebug() << "sub nodes" << ids;
    QList<int> ids2 = ATreeView::findSubRecords(records, id);
//    qDebug() << "sub records" << ids2;
    for (int i=0;i<ids2.count();i++) {
        if (!ids.contains(ids2.at(i))) {ids.append(ids2.at(i)); }
    }
//    qDebug() << "id" << id;
    ids.append(id);
//    qDebug() << "finded rows" << ids;
    for (int i=0; i<ids.count();i++) {
        QMap<QString, QVariant> record = records[ids.at(i)];
        // Обработка модели
        QModelIndex index = ATreeView::findIndex(treeview, ids.at(i));

        if (columnProps.isEmpty()) {columnProps = ATreeView::getColumnProps(treeview);}
        int inc=0; if (treeview->property("isFlat").toBool()) {inc=1;}
//        if (ids.at(i)==0) {
//            model->setData(model->index(index.row(), i+inc, index.parent()), "data", Qt::EditRole);
//            continue;
//        }
        if (index.isValid() && record["id"].toInt()!=0) {
//            qDebug() << "1" << record;
            QString data;
            for (int i=0;i<columnProps.count();i++) {
                data = AVariant::convertVariantToString(record[columnProps.at(i)["Data"].toString()], columnProps.at(i)["Precision"].toString());
                model->setData(model->index(index.row(), i+inc, index.parent()), data, Qt::EditRole);
            }
            ATreeView::static_select(treeview, record["id"].toInt());

        } else if (index.isValid() && record["id"].toInt()==0) {
//            qDebug() << "2" << index << model->rowCount(index) << index.row() << index.parent();
//            int paridcolumn = ATreeView::getHeaderPosition(treeview, "parId");
//            int parId = model->data(model->index(index.row(), paridcolumn, index.parent())).toInt();
            model->removeRow(index.row(), index.parent());
//            ATreeView::static_select(treeview, parId);
        } else if (!index.isValid() && record["id"].toInt()!=0) {
//            qDebug() << "3";
            ATreeView::static_update(widget, treeview, record["parId"].toInt(), columnProps);
            ATreeView::static_select(treeview, id);
        }
    }
}

void ATreeView::static_select(QTreeView *treeview, int id, QList< QMap<QString, QVariant> > columnProps) {
//    qDebug() << "ATreeView::static_select" << id;
    if (columnProps.isEmpty()) {columnProps = ATreeView::getColumnProps(treeview);}
//    qDebug() << "ATreeView::static_select" << id << columnProps;
    QAbstractItemModel *model = treeview->model();
    QModelIndex index = ATreeView::findIndex(treeview, id);
    if (index.isValid()) {
        int inc=0; if (treeview->property("isFlat").toBool()) {inc=1;}
        treeview->selectionModel()->clear();
        for (int i=0;i<columnProps.count();i++) {
            treeview->selectionModel()->select(model->index(index.row(), i+inc, index.parent()), QItemSelectionModel::Select);
        }
        ATreeView::expandTVNode(treeview, index);
        treeview->scrollTo(index);
    }

}
void ATreeView::addTVNode(QStandardItem *parentNode, int node, QList< QMap<QString, QVariant> > tree, QList< QMap<QString, QVariant> > columnProps, bool isFlat) {
//    qDebug() << "ATreeView::addTVNode node " << node;
    int row=0;
    int id=0;
    QString field, precision;
    QString data;
    parentNode->removeRows(0, parentNode->rowCount());
    for (int i=0;i<tree.count();i++) {
        id=0;
//        qDebug() << "ATreeView::addTVNode parId" << tree.at(i)["parId"].toInt();
        if (node == tree.at(i)["parId"].toInt()) {
            QStandardItem *firstItem = new QStandardItem(" ");
            if (isFlat) {parentNode->setChild(row, 0, firstItem);}
            for (int i1=0;i1<columnProps.count();i1++) {
                field = columnProps.at(i1)["Data"].toString(); //objProperties[columnsList[i1]]["Data"].toString();
                if (field!="") {
                    precision = columnProps.at(i1)["Precision"].toString();
                    QStandardItem *subNode = new QStandardItem(AVariant::convertVariantToString(tree.at(i)[field], precision));
                    subNode->setData(data, Qt::UserRole);
                    if (isFlat){
                        parentNode->setChild(row, i1+1, subNode);
                    } else {
                        parentNode->setChild(row, i1, subNode);
                        if (i1==0) {firstItem = subNode;}
                    }
                }
            }
            row++;
            addTVNode(firstItem, tree.at(i)["id"].toInt(), tree, columnProps, isFlat);
        }
    }
}
QList<int> ATreeView::findSubNodes(QTreeView *treeview, QModelIndex parentIndex) {
         QList<int> result;
         QModelIndex index;
         QAbstractItemModel *model = treeview->model();
         int keyColumn = treeview->property("keyColumn").toInt();
     //    int row=-1;
//         qDebug() << "ATreeView::findSubNode" << model << id << model->rowCount(parentIndex);
//         qDebug() << "ATreeView::findSubNode" << parentIndex << model->rowCount(parentIndex);
         for (int i=0;i<model->rowCount(parentIndex);i++) {
             index = model->index(i, 0, parentIndex);
             result.append(ATreeView::findSubNodes(treeview, index));
             if (index.isValid()) {result.append(model->data(model->index(i, keyColumn, parentIndex)).toInt());}
         }
         return result;
}
QList<int> ATreeView::findSubRecords(QMap<int, QMap<QString, QVariant> > array, int id) {
//    qDebug() << "ATreeView::findSubRecords" << id << array;
         QList<int> result;
         for (int i=0;i<array.count();i++) {
             if (array.values().at(i)["parId"].toInt()==id) {
                 result.append(ATreeView::findSubRecords(array, array.keys().at(i)));
                 result.append(array.values().at(i)["id"].toInt());
             }
         }
         return result;
}

QModelIndex ATreeView::findIndex(QTreeView *treeview, int id, QModelIndex parentIndex) {
    QModelIndex index;
    QAbstractItemModel *model = treeview->model();
//    if (parentIndex==QModelIndex()) {
//        parentIndex = treeview->model()->index(0,0);
//    }
    int keyColumn = treeview->property("keyColumn").toInt();
//    int row=-1;
//    qDebug() << "ATreeView::findIndex" << model << id << model->rowCount(parentIndex);
    for (int i=0;i<model->rowCount(parentIndex);i++) {
        index = ATreeView::findIndex(treeview, id, model->index(i, 0, parentIndex));
        if (model->data(model->index(i, keyColumn, parentIndex)).toInt()==id && !index.isValid()) {index = model->index(i, 0, parentIndex);}
        if (index.isValid()) {return index;}
    }
    return index;
}

QList< QMap<QString, QVariant> > ATreeView::getColumnProps(QTreeView *treeview) {
    QList< QMap<QString, QVariant> > result;
    QAbstractItemModel *model = treeview->model();
    QMap<QString, QVariant> temp;
    for (int i=0;i<model->columnCount();i++) {
        temp["Data"] = model->headerData(i, Qt::Horizontal);
        result << temp;
    }
    return result;
}
int ATreeView::getColumnCount(QTreeView *treeview) {
    return treeview->model()->columnCount();
}

QStringList ATreeView::getColumnsList(QTreeView *treeview) {
    QStringList result;
    QAbstractItemModel *model = treeview->model();
    for (int i=0;i<model->columnCount();i++) {
        result << model->headerData(i, Qt::Horizontal).toString();
    }
    return result;
}
int ATreeView::getHeaderPosition(QTreeView *tv,QString headerName) { // Возвращает номер ключевого столбца
    int result = -1;
    QStandardItemModel *model = static_cast<QStandardItemModel*>(tv->model());
    for (int i=0;i<model->columnCount();i++) {
        QString header = model->headerData(i,Qt::Horizontal).toString();
        if (header==headerName) { result=i; }
    }
    return result;
}
void ATreeView::expandTVNode(QTreeView *tv, QModelIndex index) {
    tv->expand(index);
    QModelIndex parentIndex = index.parent();
    if (parentIndex.isValid()) {
        ATreeView::expandTVNode(tv, parentIndex);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Работа с ФОРМОЙ TreeView
/////////////////////////////////////////////////////////////////////////////////////////////////
void ATreeView::createTreeView(){
//    qDebug() << "ATreeView::createTreeView()";
    treeview = 0;
    QString objClassName;
    for (int i=0;i<dataobjlist.count();i++) {
        objClassName = dataobjlist.at(i)->metaObject()->className();
        if (objClassName=="QTreeView") {treeview = static_cast<QTreeView*>(dataobjlist.at(i));}
    }
    if (treeview==0) {
        treeview = new QTreeView(widget);
        treeview->setObjectName("treeview");
        treeview->resize(QSize(widget->width(),widget->height()));
        treeview->setSelectionBehavior(QAbstractItemView::SelectRows);
        treeview->show();
        treeview->installEventFilter(this);
        ADataset::prepareModelObject(treeview);
    }
    else {
        treeview->setVisible(true);
        treeview->move(QPoint(0,0));
        treeview->resize(widget->size());
        treeview->raise();
    }
    this->createTableConnects();
    treeview->setFocusPolicy(Qt::NoFocus);
    QList< QString > tableobjects; tableobjects << "QLineEdit" << "QCheckBox" << "QComboBox" << "QDateEdit" << "QTimeEdit";
    QList<int> tabOrder;
    // Создание Индексов
    QString header, data;
    QMap<QString, QVariant> tempMap;
    for (int i=0;i<dataobjlist.count();i++) {
        objClassName=dataobjlist.at(i)->metaObject()->className();
        header=dataobjlist.at(i)->property("Header").toString();
        data=dataobjlist.at(i)->property("Data").toString();
        if (tableobjects.contains(objClassName) && (!header.isEmpty() || !data.isEmpty())) {
            tempMap["Data"] = dataobjlist.at(i)->property("Data");
            tempMap["Header"] = dataobjlist.at(i)->property("Header");
            tempMap["ColumnWidth"] = dataobjlist.at(i)->property("ColumnWidth");
            tempMap["Precision"] = dataobjlist.at(i)->property("Precision");
            tempMap["objlistPos"] = i;
            columnProps.append(tempMap);
            tempMap.clear();
        }
    }
    // Обработка свойств
    keyColumn = this->getColumnPosition("id");
    treeview->setProperty("keyColumn", keyColumn);
    startpos = 0;
    if (treeview->property("type").toString()=="isFlat") {treeview->setProperty("isFlat", true); startpos=1;}
    // Создание столбцов
    QStandardItemModel *model = static_cast<QStandardItemModel*>(treeview->model());
    model->clear();
    model->setColumnCount(columnProps.count()+startpos);
    if (treeview->property("isFlat").toBool()) {
        model->setHeaderData(0, Qt::Horizontal, " ");
        treeview->setColumnWidth(0, 60);
    }
    int width;
    for (int i=0;i<columnProps.count();i++) {
        if (columnProps.at(i)["Header"].toString()!="") {header = columnProps.at(i)["Header"].toString();} else {header = columnProps.at(i)["Data"].toString();}
        model->setHeaderData(i+startpos, Qt::Horizontal, header);
        model->setHeaderData(i+startpos, Qt::Horizontal, Qt::AlignCenter, Qt::TextAlignmentRole);
        width = columnProps.at(i)["ColumnWidth"].toInt();
        treeview->setColumnWidth(i+startpos, width);
        if (width==0) {treeview->hideColumn(i+startpos);}
        ADelegate *delegate = this->createDelegate(dataobjlist.at(columnProps.at(i)["objlistPos"].toInt()));
        if (delegate!=0) {treeview->setItemDelegateForColumn(i+startpos, delegate);}
    }
}
void ATreeView::update(int startId) {
    ATreeView::static_update(widget, treeview, startId, columnProps);
}
void ATreeView::updateRecord(int id) {
    ATreeView::static_updateRecord(widget, treeview, id, columnProps);
}
void ATreeView::select(int id) {
    ATreeView::static_select(treeview, id, columnProps);
}

void ATreeView::createTableConnects() {
//    qDebug() << "ATreeView::createTableConnects()";
    engine->createConnects(treeview);
}
ADelegate * ATreeView::createDelegate(QObject *obj) {
    qDebug() << "ATreeView::createDelegate" << obj->metaObject()->className() << obj->objectName();
    QString objClassName = obj->metaObject()->className();
    if (objClassName=="QLineEdit" || objClassName=="QComboBox" || objClassName=="QCheckBox" || objClassName=="QPushButton" || objClassName=="QDateEdit" || objClassName=="QTimeEdit") {
        ADataset::updateObject(obj);
        ADelegate *delegate = new ADelegate(obj, this);
        return delegate;
        }
    return 0;
}

////////////////////////////////////////////
// РАЗНОЕ
////////////////////////////////////////////
int ATreeView::getColumnPosition(QString columnName) {
    int result = -1;
    for (int i=0;i<columnProps.count();i++) {
//        QString data = objProperties[objectsList[i]]["Data"].toString();
//        qDebug() << "ATreeView::getColumnPosition" << data;
        if (columnProps.at(i)["Data"]==columnName) {result = i;}
    }
    if (treeview->property("isFlat").toBool()) {result++;}
    return result;
}
int ATreeView::getSelected() {
//    qDebug() << "ATreeView::getSelected" << keyColumn;
    int result = ATreeView::getTVData(treeview, keyColumn).toInt();
    return result;
}
QVariant ATreeView::getTVData(QTreeView *tv, int column) {
    QVariant result;
    if (tv->selectionModel()->selectedIndexes().count()!=0) {
        const QItemSelection ranges = tv->selectionModel()->selection();
        QModelIndex index = ranges.indexes().at(column);
        result = tv->model()->data(index).toString();
    }
    return result;
}
double ATreeView::sum(QString fieldname) {
//    qDebug() << "ATreeView::sum";
    double result = 0;
    QAbstractItemModel *model = treeview->model();
    if (fieldname!="") {
        int fieldColumn = this->getColumnPosition(fieldname);
//        qDebug() << "ATreeView::sum" << fieldColumn << fieldname;
        if (fieldColumn!=-1) {
//            qDebug() << "ATreeView::sum rowcount" << model->rowCount() << model->columnCount();
            for (int i=0;i<model->rowCount();i++) {
                double value = AFunc::getModelValue(model, i, fieldColumn).toDouble();
//                qDebug() << "ATreeView::sum" << i << fieldColumn << AFunc::getModelValue(model, i, fieldColumn);
                result = result + value;
            }
        }
    }
    return result;
}
QMultiMap<int, bool> ATreeView::getTvState(QTreeView *tv, QStandardItem *parItem) {
//    QStandardItemModel *model = static_cast<QStandardItemModel*>(tv->model());
    QMultiMap<int, bool> result;
    int keyColumn = ATreeView::getHeaderPosition(tv, "id");
    if (keyColumn==-1) {return result;}
    QStandardItem *subitem;
    for (int i=0;i<parItem->rowCount();i++) {
        subitem = parItem->child(i);
        if (subitem->hasChildren()) {
            result.insert(parItem->child(i, keyColumn)->data(Qt::UserRole).toInt(), tv->isExpanded(subitem->index()));
            result.unite(ATreeView::getTvState(tv, subitem));
        }
    }
    return result;
}
void ATreeView::setTvState(QTreeView *tv, QStandardItem *parItem, QMultiMap<int, bool> stateMap) {
//    QStandardItemModel *model = static_cast<QStandardItemModel*>(tv->model());
    int keyColumn = ATreeView::getHeaderPosition(tv, "id");
    if (keyColumn==-1) {return;}
//    QModelIndex selIndex;
    int id;
    QStandardItem *subitem;
    for (int i=0;i<parItem->rowCount();i++) {
        subitem = parItem->child(i);
        if (subitem->hasChildren()) {
            id = parItem->child(i, keyColumn)->data(Qt::UserRole).toInt();
            if (stateMap.keys().contains(id)) {
                tv->setExpanded(subitem->index(), stateMap.value(id));
                ATreeView::setTvState(tv, subitem, stateMap);
            }
        }
    }
}
QVariant ATreeView::getTVSelParent(QTreeView *tv) {
    QVariant result = 0;
    int paridcolumn = ATreeView::getHeaderPosition(tv, "parId");
//    qDebug() << "ADataset::getTVSelParent" << paridcolumn;
    return ATreeView::getTVData(tv, paridcolumn);
}

#include "QClipboard"
#include "QApplication"
bool ATreeView::slot_copyToClipboard(QTreeView *treeview) {
    // Коннектитцо из ADataset::copyToClipboard_slot() а оно из ADataset::Preprocessing
    qDebug() << "ATreeView::slot_copyToClipboard" << treeview;
    QClipboard *clipboard=QApplication::clipboard();
    QString table_cells = "", tmp_cell_string;
    QItemSelection ranges = treeview->selectionModel()->selection();
    QStandardItemModel *model = static_cast<QStandardItemModel*>(treeview->model());
    bool b=false;
    for (int i=0; i < model->columnCount(); i++) {
        if (!treeview->isColumnHidden(i) && treeview->columnWidth(i)!=0) {
            if (b) { table_cells += "\t"; }
            table_cells += AVariant::convertVariantToString(model->headerData(i, Qt::Horizontal));
            b=true;
        }
    }
    table_cells += "\n";
    for (int i=0; i < model->columnCount(); i++) {
        b=false;
        if (!treeview->isColumnHidden(i) && treeview->columnWidth(i)!=0) {
            if (b) { table_cells += "\t"; }

            tmp_cell_string = ATreeView::getTVData(treeview, i).toString();
            if ( AVariant::ifNumber(tmp_cell_string) ) { tmp_cell_string.replace(".", ","); }
            table_cells += tmp_cell_string;

            b=true;
        }
    }
    clipboard->setText(table_cells); // заполнение буфера обмена}
    return true;
}
