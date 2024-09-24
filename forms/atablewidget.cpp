#include <QScrollBar>
#include <QStatusBar>
#include <QKeyEvent>
#include <QPainter>
#include <QHeaderView>

#include "forms/atablewidget.h"
#include "aform.h"
#include "agui.h"
#include "adataset.h"
#include "adatabase.h"
#include "afunc.h"
#include "avariant.h"
#include "aengine.h"
#include "globals.h"
#include "atablewidgetstatic.h"
#include <QAction>

ATableWidget :: ATableWidget(QWidget *parent) {
    //qDebug() << "constructor ATableWidget, parent:";// << parent->objectName();
    this->setObjectName("Table");
    this->setParent(parent);
    created=false;
    widget = parent;
    engine = parent->findChild<AEngine*>("qs");
    flag_allowInsert = widget->property("allowInsert").toBool();
    dataobjlist = AForm::getFormDataObjects(widget);
    for (int i=0;i<dataobjlist.count()-1;i++){
        for (int j=0;j<dataobjlist.count()-1;j++){
            int to1=dataobjlist.at(j)->property("TableOrder").toInt();
            int to2=dataobjlist.at(j+1)->property("TableOrder").toInt();
            if (to1>to2) {dataobjlist.swap(j,j+1);}
        }
    }

    // SET default db connection of TableWidget
    if (widget->property("Connection").toString()!="") {
        connection = widget->property("Connection").toString();
    } else {
        connection = globals["defConnection"];
    }
    this->createTableWidget();
    //qDebug() << "constructor ATableWidget end";
}
ATableWidget::~ATableWidget() {
    QObject::disconnect(tablewidget, 0, 0 , 0);
}

void ATableWidget::enableWidget() {
    tablewidget->setEnabled(true);
}
void ATableWidget::disableWidget() {
    tablewidget->setEnabled(false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Работа с одиночными таблицами (не являющимися формами данных)
/////////////////////////////////////////////////////////////////////////////////////////////////

void ATableWidget::updateFieldDelegats(QString fieldName) {
    qDebug() << "ATableWidget::updateFieldDelegats()";
    int column = ColumnFieldIndex.keys().at(ColumnFieldIndex.values().indexOf(fieldName));
    //    qDebug() << "ATableWidget::updateFieldDelegats" << fieldName << ColumnHeaderIndex.values().indexOf(fieldName) << ColumnHeaderIndex.keys().at(ColumnHeaderIndex.values().indexOf(fieldName)) << column;
    //    qDebug() << ColumnHeaderIndex;
    ADelegate *delegate = static_cast<ADelegate*>(tablewidget->itemDelegateForColumn(column));
    //    qDebug() << i << column << delegate;
    if (delegate) {
        delegate->update();
        tablewidget->setItemDelegateForColumn(column, delegate);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Работа с ФОРМОЙ TableWidget
/////////////////////////////////////////////////////////////////////////////////////////////////

void ATableWidget::createTableWidget( ){
    //qDebug() << "ATableWidget::createTableWidget()" << widget;
    //    AFunc::echoTimer("ATableWidget::createTableWidget()");
    //    tablewidget = widget->findChild<QTableWidget*>("tableWidget");
    highlightedRows.clear();
    tablewidget = 0;
    QString objClassName;
    for (int i=0;i<dataobjlist.count();i++) {
        objClassName = dataobjlist.at(i)->metaObject()->className();
        if (objClassName=="QTableWidget") {
            tablewidget = static_cast<QTableWidget*>(dataobjlist.at(i));
            //            AGui::setObjectView(tablewidget);
        }
    }
    //* Задание свойств таблицы
    if (tablewidget==0) {
        tablewidget = new QTableWidget(widget);       
        tablewidget->show();
        ADataset::prepareModelObject(tablewidget);
    }
    AGui::setObjectView(tablewidget);
    //tablewidget->setObjectName("tableWidget");
    if (!widget->property("allowEdit").toBool()) {
        tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    } else {
        tablewidget->setEditTriggers(QAbstractItemView::AnyKeyPressed | QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked); // Мгновенное редактирование
    }

    tablewidget->move(QPoint(0,0));
    tablewidget->resize( QSize(widget->size()) );
    tablewidget->setVisible(true);
    tablewidget->setWordWrap(true);
    tablewidget->verticalScrollBar()->setVisible(true);
    tablewidget->raise();
    tablewidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    tablewidget->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);

    // Создание столбцов таблицы и необходимых индексов
    QList<QString> l_tableElements; l_tableElements << "QLineEdit" << "QComboBox" << "QCheckBox" << "QPushButton" << "QDateEdit" << "QTimeEdit";
    int column = 0;
    QMap<QString, QVariant> temp;
    QString header2; /*int x,y;*/

    //QHeaderView *header = tablewidget->horizontalHeader();
    //header->setResizeMode( QHeaderView::Stretch);

    // Перебор всех элементов, сбор сведений о столбцах
    //

    for (int i=0;i<dataobjlist.count();i++) {
        if (l_tableElements.contains(dataobjlist.at(i)->metaObject()->className())) {
            QObject *objtemp = dataobjlist.at(i);
            tablewidget->insertColumn(column);
            QString header = objtemp->property("Header").toString();
            if (header=="") {header = objtemp->property("Data").toString();}
            ColumnHeaderIndex[column] = header;
            header.replace("\\n","\n");

            tablewidget->setHorizontalHeaderItem(column, new QTableWidgetItem(header));
            // qDebug() << header+"\n" +header;
            ColumnFieldIndex[column] = objtemp->property("Data").toString();
            columnProps[column]["Data"] = objtemp->property("Data").toString();
            columnProps[column]["Precision"] = objtemp->property("Precision").toString();
            columnProps[column]["Default"] = objtemp->property("Default").toString();
            columnProps[column]["pushNullOnEmpty"] = objtemp->property("pushNullOnEmpty").toString();
            columnProps[column]["flag_dontsave_column"] = objtemp->property("flag_dontsave_column").toString();
            columnProps[column]["ColumnWidth"] = objtemp->property("ColumnWidth").toString();
            columnProps[column]["Hidden"] = objtemp->property("Hidden");
            columnProps[column]["enabled"] = objtemp->property("enabled");
            columnProps[column]["notNull"] = objtemp->property("notNull");

            //qDebug() << "ATableWidget::createTableWidget" << column << header << objtemp->metaObject()->className() << columnProps[column]["enabled"].toBool() << columnProps[column]["Hidden"].toBool();

            //
            //            qDebug() << "ATableWidget::createTableWidget" << temp["Data"] << temp["Precision"];
            //            columnProps[column] << temp;

            QString lfield = objtemp->property("LData").toString();
            if (lfield!="") {ColumnLDFieldIndex[column] = lfield;}
            bool notNull = columnProps[column]["notNull"].toBool();
            if (notNull) {NotNullColumns.append(column);}

            ColumnObjectIndex[column] = i;
            column++;
        }
    }

    this->createTableDelegats();
    tablewidget->installEventFilter(this);
    widget->installEventFilter(this);
    if (flag_allowInsert) {
        this->insertRow(tablewidget->rowCount());
    } else {setNRI(-1);}
    engine->createConnects(tablewidget);
    created=true;
    defaultColor = "#000000";
    //tablewidget->horizontalHeader() ->setWindowTitle("needtopaint");
    //QString s = QString("QHeaderView[windowTitle='needtopaint']{background: grey;}");
    //tablewidget->viewport()->setStyleSheet(s);
    //tablewidget->setStyleSheet("");
    //    tablewidget->horizontalHeader()->setResizeMode(tablewidget->horizontalHeader()->ResizeToContents);//->resize(QSize(tablewidget->horizontalHeader()->width(), 90));
    //    AFunc::echoTimer("ATableWidget::createTableWidget() end");

//    for (int i=0; i<tablewidget->columnCount(); i++) {
//        qDebug() << "ATableWidget::createTableWidget" << i << tablewidget->item(0, i) ;
//    }


    this->resizeColumnsToWidth();
    //qDebug() << "ATW:createTableWidget" << ColumnObjectIndex;

}
void ATableWidget::execFunction(QString script) {
    engine->evaluate(script);
}

void ATableWidget::resizeColumnsToWidth() {
    //qDebug() << "ATableWidget::resizeColumnsToWidth()" << tablewidget << tablewidget->parentWidget() << tablewidget->parentWidget()->parentWidget();
    /// SET widget and treeWidget height. Round it to fit height of the rows
    int tableheight = ( (widget->size().height()-40)/25 )*25+40; //55qRound( (widget->size().height()-40)/25 )*25+40;
    if (!widget->property("ifTotals").toBool()) {
        tablewidget->resize(QSize(widget->size().width(), widget->size().height()));
    } else {
        tablewidget->resize(QSize(widget->size().width(), widget->size().height()-25));
    }

    //qDebug() << "columnwidth" << treewidget->width() << treewidget->verticalScrollBar()->geometry().height();
    int columnwidth;
    for (int i=0;i<columnProps.count();i++) {
        columnwidth = columnProps[i]["ColumnWidth"].toInt();
        if (columnProps[i]["ColumnWidth"].toString().contains('%')) {
            int vscroll_width=0;
            int vheader_width=0;
            if (tablewidget->verticalScrollBar()->isVisible()) { vscroll_width=tablewidget->verticalScrollBar()->width();}
            if (tablewidget->verticalHeader()->isVisible()) { vheader_width=tablewidget->verticalHeader()->width();}
            //qDebug() << "ATableWidget::resizeColumnsToWidth" << tablewidget->width() << vscroll_width << vheader_width;
            columnwidth = columnProps[i]["ColumnWidth"].toString().remove("%").toInt()*(tablewidget->width() - vscroll_width - vheader_width)/100;
        }

        bool hidden = columnProps[i]["Hidden"].toBool();
        //qDebug() << "ATableWidget::resizeColumnsToWidth() hidden" << i << hidden << columnwidth;
        if (hidden) {tablewidget->setColumnHidden(i, true);}
        if (!hidden && columnwidth!=0) {
            tablewidget->setColumnWidth(i, columnwidth);
            //qDebug() << "resizeColumnsToWidth" << tablewidget->columnWidth(i);
        }
        //qDebug() << "columnwidth" << i << columnwidth;

    }
}

// Обновление делегатов
void ATableWidget::updateDelegats() {
    //qDebug() << "ATableWidget::updateDelegats()";
    for (int i=0;i<tablewidget->columnCount();i++) {
        ADelegate *delegate = static_cast<ADelegate*>(tablewidget->itemDelegateForColumn(i));
        if (delegate) {
            delegate->update();
            tablewidget->setItemDelegateForColumn(i, delegate);
        }

    }
}
void ATableWidget::update() {
    //qDebug() << "ATableWidget::update";
    // Обнуление таблицы
    QString connection = widget->property("SaveConnection").toString();
    if (connection=="") {connection = widget->property("Connection").toString();}

    int rowcount = tablewidget->rowCount();
    for (int i=0;i<rowcount;i++) {tablewidget->removeRow(0); highlightedRows.removeAt(0);}
    this->updateDelegats();
    // Заполнение таблицы данными
    QString keyField = this->getKeyField();
        if (keyField=="") {qDebug() << "ATableWidget::loadTableWidget("+widget->objectName()+"). Error: Can't get KeyField."; return; }
    QString data;
    QString querystring = ADataset::readDataSource(widget);
    //qDebug() << "ATableWidget::update" << this << widget;// << querystring;
    if (widget->property("ifDebug").toBool()) {qDebug() << "[Debug SQL : ATableWidget("+widget->objectName()+")::update()] " << querystring << connection;}
    QList<QMap<QString, QVariant> > result = ADatabase::selectList(querystring, connection);
    this->disableTableConnects();
    QMap<QString, QVariant> record;


    // Debug columns structure
    //qDebug() << "ATableWidget::update()" << ColumnFieldIndex;
    //QStringList temp;
    //for (int i=0;i<columnProps.count();i++) {
        //temp << columnProps[i]["Data"].toString();
    //}
    //qDebug() << "ATableWidget::update()" << temp;

    QString field;
    QString def_value="";
    for (int row=0;row<result.count();row++) {
        tablewidget->insertRow(row);
        highlightedRows.insert(row, defaultColor);
        record = result.at(row);
        //for (int i=0;i<ColumnFieldIndex.count();i++) {
        for (int i=0;i<columnProps.count();i++) {
            field = columnProps[i]["Data"].toString();
            //if (i==11) {
                //qDebug() << "ATableWidget::update()" << row << i << field << record.keys().contains(field) << record[field] << record[field].isNull() << ColumnFieldIndex.keys().at(i) << columnProps[i]["Default"].toString() << AVariant::convertVariantToString(record[field], columnProps[i]["Precision"].toString());
                //qDebug() << "ATableWidget::update()" << row << i << field << record.keys().contains(field)  ;
            //}
            if (record[field].isNull()) {
                this->setCellData(row, i, columnProps[i]["Default"].toString());
            } else {
                this->setCellData(row, i, AVariant::convertVariantToString(record[field], columnProps[i]["Precision"].toString()));
            }
            if (flag_loaded_color!="") {this->highlightRow(i, flag_loaded_color);}
        }
    }
    if (flag_allowInsert)
        {this->insertRow(tablewidget->rowCount());}
    else
        {setNRI(-1);}

    this->createTableConnects();

    AGui::setObjectView(tablewidget);
    this->procProps();

    this->resizeColumnsToWidth();
    tablewidget->setCurrentCell(0,0);

    List_DeletedId.clear();
    List_DeletedLDId.clear();

}
void ATableWidget::updateRecord(int id) {
    //qDebug() << "ATableWidget::updateRecord" << columnProps;
    if (tablewidget->columnCount()==0) {this->createTableWidget();}
    if (tablewidget->columnCount()==0) {return;}
    QString sqlString = ADataset::readDataSource(widget);
    QString connection = widget->property("Connection").toString();
    QList< QMap<QString, QVariant> > records = ADatabase::selectList(sqlString, connection);
    QMap<QString, QVariant> record; int position;
    for (int i=0;i<records.count();i++) { if (records.at(i)["id"]==id) { record = records.at(i); position = i; } }
    QAbstractItemModel *model = tablewidget->model();
    QModelIndex index = ATableWidgetStatic::findIndex(tablewidget, id);
    if (columnProps.isEmpty()) {columnProps = ATableWidgetStatic::getColumnProps(tablewidget);}
    if (!index.isValid() && !record.isEmpty()) {
            this->insertRow(position);
            index = model->index(position, tablewidget->property("keyColumn").toInt());
        }
    if (index.isValid() && !record.isEmpty()) {
        QString data;
        for (int i=0;i<columnProps.count();i++) {
            data = AVariant::convertVariantToString(record[columnProps[i]["Data"].toString()], columnProps[i]["Precision"].toString());
            model->setData(model->index(index.row(), i), data, Qt::EditRole);
        }
    } else if (index.isValid() && record.isEmpty()) {
        model->removeRow(index.row());
    }
    ATableWidgetStatic::static_select(tablewidget, id);
}

void ATableWidget::clear() {
    ATableWidgetStatic::static_clear(tablewidget);
}

bool ATableWidget::saveTableWidget(QString param) {
    //qDebug() << "ATableWidget::saveTableWidget idDoc" << widget->property("idDoc");
    //qDebug() << "ATableWidget::saveTableWidget::ADict" << tablewidget;
    // Удаление из БД удаленных записей
    QAbstractItemModel *tableModel(tablewidget->model());

    QString dataSource = widget->property("DataSource").toString();
    if (dataSource=="") {return 0;}
    //QString connection = widget->property("Connection").toString();
    //if (connection=="") {connection = widget->property("Connection").toString();}
    //qDebug() << "ATableWidget::saveTableWidget idDoc" << connection  << dataSource;

    QString keyField="";
    QList<int> *ListDel;
    QMap<int, QString> *MapColumnField;
    if (param=="") {
        keyField = widget->property("KeyField").toString();
        ListDel = &List_DeletedId;
        MapColumnField = &ColumnFieldIndex;
    } else {
        keyField = widget->property("LKeyField").toString();
        ListDel = &List_DeletedLDId;
        MapColumnField = &ColumnLDFieldIndex;
    }
    if (keyField=="") {return 0;}
    for (int i=0;i<ListDel->count();i++) {
        ADatabase::deleteParam(dataSource, keyField+"='"+QString::number(ListDel->at(i))+"'", connection);
    }
    ListDel->clear();

    // Создание класса ADict, заполнение его из таблицы, сохранение, уничтожение
    int NRI = getNRI();
    QMap<QString, QVariant> updateMapFilter;
    QMap<QString, QString> filter = AFunc::getStrParam(widget->property("Filter").toString());
    //qDebug() << "ATableWidget::saveTableWidget NRI" << filter;
    for (int i=0;i<filter.keys().count();i++) {
        QString data = widget->property(filter.values().at(i).toLocal8Bit().data()).toString();
        updateMapFilter[filter.keys().at(i)] = data;
    }

    // Заполнение dictObj, сохранение
    ADict *dictObj = new ADict(dataSource, "", this, connection);
    //    dictObj->setCurrentConnection(connection);
    //if (connection!="") {
        //dictObj->setCurrentConnection(connection);
        //dictObj->setCurrentDict(dataSource);
    //}

    QMap<QString, QVariant> updateMap;
    QVariant temp;
    int column;
    QString fieldName;
    bool flag_dontsave_row = false; // Флаг НЕ сохранения рядка
    QMap<int, int> dictObjIndex;

    //qDebug() << "---------";
    //qDebug() << "MapColumnField:" << *MapColumnField;
    //qDebug() << "---------";
    //qDebug() << "ATableWidget::saveTableWidget" << NotNullColumns;
    for (int row=0;row<tablewidget->rowCount();row++) {
        if (row!=NRI) {
            updateMap.clear();
            for (int i=0;i<MapColumnField->count();i++) {
                column = MapColumnField->keys().at(i);
                fieldName = MapColumnField->values().at(i);
                // Если столбец без указания поля БД или с флагом flag_dontsave_column - не сохраняем
                if (fieldName!="" && !columnProps[i]["flag_dontsave_column"].toBool()) {
                    temp = AFunc::getModelValue(tableModel, row, column);
                    //qDebug() << "ATableWidget:save" << row << column << temp;
                    // Проверка не нулевых полей. Если обнаруживается не заполненное, строка не сохраняется
                    if ((temp.toString()=="" && columnProps[i]["pushNullOnEmpty"].toBool())) {
                        //qDebug() << "[ATableWidget::saveTableWidget] обнаружено не заполненное поле" << fieldName << ". Запись не сохранена.";
                    } else {
                        updateMap[fieldName] = temp;
                    }
                    if (updateMap[fieldName].isNull() && NotNullColumns.contains(column)) {flag_dontsave_row = true;}
                }
            }
            //qDebug() << "ATableWidget::saveTableWidget" << flag_dontsave_row << updateMap;
            if (!flag_dontsave_row) {
                updateMap.unite(updateMapFilter);
                //qDebug() << "updateMap" << updateMap;
                dictObj->newRecord();
                //qDebug() << dictObj->count();
                dictObj->updateRecord(updateMap);
                //dictObj->echoCR();
                dictObjIndex[row] = dictObj->currentRecordI;
            }
            flag_dontsave_row=false;
        }
    }
    dictObj->echoDict();
    if (dictObj->count()>0) {
        //qDebug() << "ATableWidget::saveTableWidget  dictObj->saveChanges";
        dictObj->saveChanges();
        // Заполнение ключевого поля (id) для новых рядков (где id пустое)
        for (int row=0;row<dictObj->count();row++) {
            int keyValue = this->getRecordValue(keyField, row).toInt();
            //            qDebug() << keyValue << row << NRI << dictObjIndex[row] << dictObj->getCRValue(keyField, dictObjIndex[row]).toInt();
            if (keyValue==0 && row!=NRI) {
                this->setRecordValue(keyField, dictObj->getCRValue(keyField, dictObjIndex[row]), dictObjIndex[row]);
            }
        }
    }
    dictObj->~ADict();
    dictObjIndex.clear();
    //qDebug() << "ATableWidget::saveTableWidget end";
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Делегаты
/////////////////////////////////////////////////////////////////////////////////////////////////
void ATableWidget::createTableDelegats() {
    //qDebug() << "ATableWidget::createTableDelegats()" << tablewidget->columnCount() << dataobjlist;
    //qDebug() << "ATableWidget::createTableDelegats()" << ColumnObjectIndex;
    // Создание делегатов
    for (int i=0;i<tablewidget->columnCount();i++) {
        QObject *objtemp = dataobjlist.at(ColumnObjectIndex[i]);
        //qDebug() << "ATableWidget::createTableDelegats()" << i << objtemp;
        ADelegate *delegate = 0;     //this->createDelegate(objtemp);
        QString objClassName = objtemp->metaObject()->className();
        if (objClassName=="QLineEdit" || objClassName=="QComboBox" || objClassName=="QCheckBox" || objClassName=="QPushButton" || objClassName=="QDateEdit" || objClassName=="QTimeEdit") {
            ADataset::updateObject(objtemp);
            delegate = new ADelegate(objtemp, this);
        }
        //qDebug() << "ATableWidget::createTableDelegats()" << i << delegate;
        if (delegate!=0) {
            tablewidget->setItemDelegateForColumn(i, delegate);
            //qDebug() << "ATableWidget::createTableDelegat()" << i << objtemp;
        }
    }
}

void ATableWidget::createTableConnects() {
    //qDebug() << "ATableWidget::createTableConnects cell" << tablewidget;
    if (!flag_allowActions) {
        //// double click
        // QObject::connect(tablewidget,SIGNAL(cellActivated(int, int)),this,SLOT(onCellActivated(int, int)));
        //// This signal is emitted whenever a cell in the table is clicked
        QObject::connect(tablewidget,SIGNAL(cellClicked(int, int)),this,SLOT(onCellClick(int, int)));
        QObject::connect(tablewidget,SIGNAL(cellDoubleClicked(int, int)),this,SLOT(onCellDblClick(int, int)));
        //// This signal is emitted whenever a cell in the table is pressed
        //QObject::connect(tablewidget,SIGNAL(cellPressed(int, int)),this,SLOT(onCellActivated(int, int))); // click
        ////This signal is emitted whenever the data of the item in the cell specified by row and column has changed
        // QObject::connect(tablewidget,SIGNAL(cellChanged),this,SLOT(cellUpdateAction(int, int)));
        //// Focus In
        QObject::connect(tablewidget,SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),this,SLOT(onCellFocusOut(QTableWidgetItem*,QTableWidgetItem*))); // double click
        QObject::connect(tablewidget,SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),this,SLOT(onCellFocusIn(QTableWidgetItem*,QTableWidgetItem*))); // double click
        //QObject::connect(tablewidget,SIGNAL(itemSelectionChanged()),this,SLOT(onSelChange()));
        flag_allowActions = true;
    }
}
void ATableWidget::disableTableConnects() {
    //qDebug() << "ATableWidget::disableTableConnects";
    if (flag_allowActions) {
        //qDebug() << "ATableWidget::disableTableConnects cell" << tablewidget;

        QObject::disconnect(tablewidget,SIGNAL(cellClicked(int, int)),this,SLOT(onCellClick(int, int)));
        QObject::disconnect(tablewidget,SIGNAL(cellDoubleClicked(int, int)),this,SLOT(onCellDblClick(int, int)));
        QObject::disconnect(tablewidget,SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)),this,SLOT(onCellFocusOut(QTableWidgetItem*,QTableWidgetItem*)));
        QObject::disconnect(tablewidget,SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)),this,SLOT(onCellFocusIn(QTableWidgetItem*, QTableWidgetItem*)));

        //QObject::disconnect(tablewidget,SIGNAL(cellChanged(int, int)),this,SLOT(cellUpdateAction(int, int)));       
        //QObject::disconnect(tablewidget,SIGNAL(itemSelectionChanged()),this,SLOT(onSelChange()));
        //QObject::disconnect(tablewidget,SIGNAL(cellPressed(int,int)),this,SLOT(onCellActivated(int, int)));
        //QObject::disconnect(tablewidget,SIGNAL(cellActivated(int, int)),this,SLOT(onCellActivated(int, int)));
        flag_allowActions = false;
    }
}

void ATableWidget::cellChangeAction(int row, int column) {
    tablewidget->setProperty("ifChanged", true);
    //qDebug() << "ATableWidget::cellChangeAction" << tablewidget << tablewidget->property("ifChanged");
    //Изменение состояние индекса измененных загруженых рядков
    //QString keyField = this->getKeyField();
    //int id = AFunc::getModelValue(tablewidget->model(), row, ColumnFieldIndex.keys().at(ColumnFieldIndex.values().indexOf(keyField))).toInt();
    //LoadedStateIndex[id] = true;

    int NRI = getNRI();
    //qDebug() << "ATableWidget:cellChangeAction" << row << column << "engine.uniChangeSlot";
    if (NRI==row && flag_allowInsert) {this->insertRow(row+1);}

    QObject *obj = dataobjlist.at(ColumnObjectIndex[column]);
    if (flag_allowActions==true) {engine->uniChangeSlot(obj);}
    //qDebug() << "//ATW:cellChangeAction";
}
void ATableWidget::cellUpdateAction(int row, int column){
    //qDebug() << "ATableWidget::cellUpdateAction" << tablewidget;
    // Изменение состояние индекса измененных загруженых рядков
        //LoadedStateIndex[id] = true;
    // Create new row if CURRENT newrow is changed
    //int NRI = getNRI();
    //if (NRI==row && flag_allowInsert) {this->insertRow(row+1);}
    //this->updateTotals();
    tablewidget->setProperty("ifChanged", true);

    QObject *obj = dataobjlist.at(ColumnObjectIndex[column]);
    QString classname = obj->metaObject()->className();
    // next cell after combobox is changed. ONLY for combobox
    //if (classname=="QComboBox") {nextCell();}
    //qDebug() << "ATableWidget::cellUpdateAction "
    //            << row
    //            << column
    //            << columnProps[column]["Default"] //default value
    //            << AFunc::getModelValue(tablewidget->model(), row, column);

    if (flag_allowActions==true) {engine->uniUpdateSlot(obj);}
}
void ATableWidget::onCellClick(int row, int column) {
    //qDebug() << "onCellClick" << row << column;
    //qDebug() << "ATableWidget::onCellClick" << tablewidget->currentIndex().row() << tablewidget->currentIndex().column();

    bool enabled = columnProps[column]["enabled"].toBool();
    bool hidden = columnProps[column]["Hidden"].toBool();
    if (!enabled || hidden) {
        nextCell();
    } else {
        QObject *obj = dataobjlist.at(ColumnObjectIndex[column]);
        QString objClass = obj->metaObject()->className();
        if (objClass=="QPushButton") {engine->uniClickSlot(obj);}
    }
    //engine->uniClickSlot(tablewidget);
}
void ATableWidget::onCellDblClick(int row, int column) {
    //qDebug() << "onCellClick" << row << column;
    qDebug() << "ATableWidget::onCellDblClick" << tablewidget->currentIndex().row() << tablewidget->currentIndex().column();
    bool enabled = columnProps[column]["enabled"].toBool();
    bool hidden = columnProps[column]["Hidden"].toBool();
    if (!enabled || hidden) {
        nextCell();
    } else {
        QObject *obj = dataobjlist.at(ColumnObjectIndex[column]);
        QString objClass = obj->metaObject()->className();
        if (objClass!="QPushButton" && widget->property("allowEdit").toBool()) {tablewidget->openPersistentEditor(tablewidget->currentItem());}
    }
    //engine->uniClickSlot(tablewidget);
}
void ATableWidget::onCellFocusIn(QTableWidgetItem* current,QTableWidgetItem* prev) {
//    qDebug() << "onCellFocusIn" << current << prev;
//        qDebug() << tablewidget->currentItem() << tablewidget->currentRow() << tablewidget->currentColumn();//->row() << tablewidget->currentItem()->column();
//    if (prev==0)
//        qDebug() << "onCellFocusIn1" << QString::number(current->row())+":"+QString::number(current->column()) << "-:-";
//    else if (current==0)
//        qDebug() << "onCellFocusIn1" << "-:-" << QString::number(prev->row())+":"+QString::number(prev->column());
//    else
//        qDebug() << "onCellFocusIn1" << QString::number(current->row())+":"+QString::number(current->column()) << QString::number(prev->row())+":"+QString::number(prev->column());

//    for (int i=0; i<tablewidget->columnCount(); i++) {
//        qDebug() << i << tablewidget->item(current->row(), i) ;
//    }

//    if (!current || !prev) {return;}

//    int column = current->column();//->column();//tablewidget->currentIndex().column();
//    bool hidden, enabled;
//    //    qDebug() << "ATableWidget::onCellFocusIn"
//    //                << current->row()//tablewidget->currentIndex().row()
//    //                << current->column()//tablewidget->currentIndex().column()
//    //                << AFunc::getModelValue(tablewidget->model(), tablewidget->selectionModel()->selectedIndexes().at(0).row(), tablewidget->selectionModel()->selectedIndexes().at(0).column());
//    enabled = columnProps[column]["enabled"].toBool();
//    hidden = columnProps[column]["Hidden"].toBool();
//    if (enabled && !hidden && !tablewidget->property("selectmode").toBool()) {
//        //qDebug() << "openPersistentEditor" << current->row() << current->column();
//        //1tablewidget->openPersistentEditor(current);
//    }
//    //qDebug() << "onCellFocusIn2" << enabled << hidden;
//    if ((!enabled || hidden) /*&& prev!=0*/) {
//        //        this->disableTableConnects();
//        //            nextCell();
//        //        this->createTableConnects();
//    }
}

void ATableWidget::onCellFocusOut(QTableWidgetItem* current, QTableWidgetItem* prev) {
//    //qDebug() << "onCellFocusOut" << current << prev;
//    //qDebug() << "onCellFocusOut" << current->row()+":"+current->column() << prev->row()+":"+prev->column();;
//    if (!prev) return;
//    int column = prev->column();
//    bool hidden, enabled;
//    //qDebug() << "ATableWidget::onCellFocusOut"
//    //            << prev->row()//tablewidget->currentIndex().row()
//    //            << prev->column();//tablewidget->currentIndex().column()
//    enabled = columnProps[column]["enabled"].toBool();
//    hidden = columnProps[column]["Hidden"].toBool();
//    if (enabled && !hidden) {
//        //qDebug() << "closePersistentEditor" << prev->row() << prev->column();
        tablewidget->closePersistentEditor(prev);
//    }
}
void ATableWidget::onSelChange() {
    //qDebug() << "onSelChange";
    if (tablewidget->selectedItems().count()>0) {
        //qDebug() << tablewidget->selectedItems().count() << tablewidget->selectedItems().at(0)->row()<< tablewidget->selectedItems().at(0)->column();
        // ?? from
        if (!tablewidget->property("selectmode").toBool()) {
            // ?? to
            //1tablewidget->selectionModel()->setCurrentIndex(tablewidget->model()->index(tablewidget->currentItem()->row(), tablewidget->currentItem()->column(), QModelIndex()), QItemSelectionModel::ClearAndSelect);
            //1tablewidget->openPersistentEditor(tablewidget->currentItem());
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Обработка событий
/////////////////////////////////////////////////////////////////////////////////////////////////
bool ATableWidget::eventFilter(QObject *obj, QEvent *event) {
    //if (event->type() == QEvent::KeyPress) {
    //    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    //    qDebug() << keyEvent->key();
    //    if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == 16777217) {
    //        qDebug() << "tab";
    //        this->nextCell();
    //        return true;
    //    }
    //}
        //qDebug() << "event" << obj << event->type();

    //    if (event->type() == QEvent::KeyPress) {
    //        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    //        QString className = obj->metaObject()->className();
    //        qDebug() << "111" << className << keyEvent->key();
    //        if (className == "QWidget" && keyEvent->key() == Qt::Key_Tab) {
    //            qDebug() << "event" << obj << event->type();
    //            widget->focusWidget();

    //        }
    //    }
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        //qDebug() << "key:" << keyEvent->key();
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == 16777220) {
            this->nextCell();
            return true;
        }
        if (keyEvent->key() == Qt::Key_Tab) {
            this->nextCell();
            return true;
        }
        if (keyEvent->key() == Qt::Key_Delete && keyEvent->modifiers()==Qt::ShiftModifier ) {
            if (AGui::dlgAccept("Вы уварены, что хотите удалить выделенные записи?")==true) {
                //this->selectCurrentRows();
                this->deleteSelectedRows();
                return true;
            }
        }

    }
    if (event->type()==QEvent::Resize) {
        this->resizeColumnsToWidth();
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Перемещение по таблице
/////////////////////////////////////////////////////////////////////////////////////////////////
void ATableWidget::nextCell() {
    //qDebug() << "ATableWidget::NEXTCELL()";
    int NRI = getNRI();
    int row = tablewidget->currentIndex().row();
    int column = tablewidget->currentIndex().column();

    // Check if current cell can accept focus
    bool active_current_cell = false;
    bool hidden = columnProps[column]["Hidden"].toBool();
    bool enabled = columnProps[column]["enabled"].toBool();
    if (!hidden && enabled) {active_current_cell=true;}

    tablewidget->setSelectionMode(QAbstractItemView::SingleSelection);

    //qDebug() << "ATableWidget::NEXTCELL()" << row << column;
    bool next_position = false;

    for (int i=column;i<tablewidget->columnCount();i++) {
        //qDebug() << "aa" << row << i;
        bool hidden = columnProps[i]["Hidden"].toBool();
        bool enabled = columnProps[i]["enabled"].toBool();
        //qDebug() << "aaa" << row << i << tablewidget->rowCount() << tablewidget->columnCount() << "nextCell" << hidden << enabled << next_position;
        if (!hidden && enabled && next_position==true) {
            //qDebug() << "bb" << row << i << columnProps[i];
            //qDebug() << "ATableWidget::nextCell()" << tablewidget->currentIndex() << tablewidget->selectionModel()->currentIndex() << tablewidget->model()->index(row, i);

            //tablewidget->selectionModel()->clear();
            //tablewidget->setCurrentCell(row, i);

            //            QItemSelectionModel *selmodel = tablewidget->selectionModel();
            //            QModelIndex index = tablewidget->model()->index(row, i);
            //            selmodel->select(index, QItemSelectionModel::ClearAndSelect);
            //            tablewidget->setSelectionModel(selmodel);
            tablewidget->setCurrentIndex(tablewidget->model()->index(row, i, QModelIndex()));
            //            tablewidget->setCurrentCell(row, i);
            //tablewidget->setCurrentCell(row, i);
            //            tablewidget->selectionModel()->select(tablewidget->model()->index(row, i), QItemSelectionModel::Select);
            //            tablewidget->selectionModel()->setCurrentIndex(tablewidget->model()->index(row, i), QItemSelectionModel::ClearAndSelect);
            return;
        }
        if (i == tablewidget->columnCount()-1 && row < tablewidget->rowCount()-1/*row<NRI*/) {
            row++;
            i=-1;
        }
        if (i == tablewidget->columnCount()-1 && row == tablewidget->rowCount()-1/*row==NRI*/) {
            break;
        }
        next_position=true;
    }

    if (!active_current_cell) {
        for (int i=tablewidget->columnCount()-1;i>=0;i--) {
            qDebug() << "cc" << row << i;
            bool hidden = columnProps[i]["Hidden"].toBool();
            bool enabled = columnProps[i]["enabled"].toBool();
            if (!hidden && enabled) {
                qDebug() << "dd" << row << i;
                //tablewidget->setCurrentIndex(tablewidget->model()->index(row, i, QModelIndex()));
                return;
            }
            if (i == 0 && row>0) {
                row--;
                i=tablewidget->columnCount();
            }
            if (i == 0 && row==0) {
                return;
            }
        }
    }
}
void ATableWidget::prevCell() {
    qDebug() << "ATableWidget::prevCell()";
    int row = tablewidget->currentIndex().row();
    int column = tablewidget->currentIndex().column();
    //qDebug() << "ATableWidget::prevCell()" << NRI << row;
    for (int i=tablewidget->currentColumn();i>=0;i--) {
        if (i == 0 && row>0) {
            row--;
            i=tablewidget->columnCount()-1;
        }
        bool hidden = columnProps[i]["Hidden"].toBool();
        bool enabled = columnProps[i]["enabled"].toBool();
        if (!hidden && enabled) {
            //this->disableTableConnects();
//            tablewidget->selectionModel()->clear();
            //tablewidget->selectionModel()->select(tablewidget->model()->index(row, i-1, QModelIndex()), QItemSelectionModel::Select);
            qDebug() << "ATableWidget::prevCell(+)" << row << i << hidden << enabled;
            //tablewidget->setCurrentIndex(tablewidget->model()->index(row, i, QModelIndex()));
//            tablewidget->selectionModel()->setCurrentIndex(tablewidget->model()->index(row, i-1, QModelIndex()), QItemSelectionModel::NoUpdate);
            //tablewidget->edit(tablewidget->model()->index(row, i-1, QModelIndex()));

            //tablewidget->openPersistentEditor(tablewidget->currentItem());
            //this->createTableConnects();
            return;
        } else {
            qDebug() << "ATableWidget::prevCell(-)" << i << hidden << enabled;
        }
        if (i == 0 && row==0) {nextCell();}
    }
}
int ATableWidget::getNRI() {
    return tablewidget->model()->property("newRowIndex").toInt();
}
int ATableWidget::setNRI(int index) {
    //qDebug() << "ATableWidget::setNRI";
    int result=-1;
    if (flag_allowInsert) {result = index;}
    tablewidget->model()->setProperty("newRowIndex", result);
    return result;
}

void ATableWidget::select(int id) {
    this->selectKey(id);
}
void ATableWidget::selectKey(QVariant data) { // Выделение ключа в виджете
    ADataset::setData(tablewidget, data);
}
void ATableWidget::selectRow(int row){ // Выделение индекса в виджете
    //qDebug() << "ATableWidget::selectRow" << tablewidget << tablewidget->rowCount() << row;
    tablewidget->selectRow(row);
    //    qDebug() << "ATableWidget::selectedRow" ;
}
void ATableWidget::selectCurrentRows() {
    //qDebug() << "ATableWidget::selectCurrentRows";
    QModelIndexList selected = tablewidget->selectionModel()->selectedIndexes();
    for (int i=0;i<selected.count();i++) {
        this->selectRow(selected.at(i).row());//(->deleteRow(first);
    }
}
void ATableWidget::scrollToCurrent(){
    tablewidget->scrollToItem(tablewidget->currentItem(),QAbstractItemView::PositionAtTop);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Работа с ячейками
/////////////////////////////////////////////////////////////////////////////////////////////////
void ATableWidget::setCellData(int row, int column, QVariant data, bool execScript) {
    //qDebug() << "ATableWidget::setCellData";
    QObject *obj = dataobjlist.at(ColumnObjectIndex[column]);
    QTableWidgetItem *item = new QTableWidgetItem(AVariant::convertVariantToString(data.toString(), obj->property("Precision").toString()));
    if (row==1) {
        //qDebug() << "ATableWidget::setCellData" << row << column << data.toString() << item;
    }
    tablewidget->setItem(row, column, item);
    if (created) {
        if (flag_allowActions==true) {engine->uniUpdateSlot(obj);}
    }
}

QVariant ATableWidget::getRecordValue(QString fieldName, int row) { // Поиск по полю data
    //qDebug() << "ATableWidget::getRecordValue" << fieldName << row;
    QVariant result;
    int column = ColumnFieldIndex.keys().at(ColumnFieldIndex.values().indexOf(fieldName));
    if (row==-1) {row = this->getCurrentRow();}
    result = AFunc::getModelValue(tablewidget->model(), row, column);
    return result;
}
void ATableWidget::setRecordValue(QString fieldName, QVariant data, int row) {
   //qDebug() << "ATableWidget::setRecordValue " << fieldName << data << ColumnFieldIndex.values();
   bool result = false;
   QList<QString> values = ColumnFieldIndex.values();
   if (row==-1) {row = this->getCurrentRow();}
   //qDebug() << "ATableWidget::setRecordValue " << row;
   for (int i=0;i<values.count();i++) {
       //qDebug() << values.at(i) << fieldName;
       if (values.at(i)==fieldName) {
            //qDebug() << "xx" << row;
           if (row!=-1) {
               //qDebug() << "ATableWidget::setRecordValue " << ColumnFieldIndex.keys().at(i) << row;
               this->setCellData(row, ColumnFieldIndex.keys().at(i), data);
               result = true;
            }
       }
   }
   if (result==false) {qDebug() << "Warning! ATableWidget::setRecordValue: can't set "+data.toString()+" in "+fieldName+" field.";}
}
QVariant ATableWidget::getColumnValue(QString headerName, int row) { // Поиск по полю header
    //qDebug() << "ATableWidget::getColumnValue";
    //int row = tablewidget->currentRow();
    QVariant result = 0;
    int column = ColumnHeaderIndex.keys().at(ColumnHeaderIndex.values().indexOf(headerName));
    if (row==-1) {row = this->getCurrentRow();}
    if (row!=-1) {
        result = AFunc::getModelValue(tablewidget->model(), row, column);
    }
    return result;
}
void ATableWidget::setColumnValue(QString headerName, QVariant data, int row) {
    //qDebug() << "ATableWidget::setColumnValue";// << fieldName << column << row << data;
    //    int row = tablewidget->currentRow();
    int column = ColumnHeaderIndex.keys().at(ColumnHeaderIndex.values().indexOf(headerName));
    if (row==-1) {row = this->getCurrentRow();}
    if (row!=-1) {
        this->setCellData(row, column, data);
//        tablewidget->setItem(row, column, new QTableWidgetItem(data.toString()));
        this->cellUpdateAction(row, column);
    }
    //    this->cellUpdate(row,column);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// ПОИСК
/////////////////////////////////////////////////////////////////////////////////////////////////
QString ATableWidget::getKeyField() {
    QString dataSource = widget->property("DataSource").toString();
    QString keyField = widget->property("KeyField").toString();
        if (keyField=="") {
            QSqlDatabase db = QSqlDatabase::database();
            keyField = db.primaryIndex(dataSource).name();
            db.~QSqlDatabase();
        }
    //qDebug() << "ATableWidget::getKeyField() " << keyField;
    return keyField;
}
int ATableWidget::getKeyColumn() {
    int result = -1;
    QString keyField =  this->getKeyField();
    if (keyField=="") {return result;}
    //qDebug() << "ATableWidget::getKeyColumn() " << keyField << ColumnFieldIndex;
    result = ColumnFieldIndex.keys().at(ColumnFieldIndex.values().indexOf(keyField));
    return result;
}

QVariant ATableWidget::getSelected() {
    QVariant result;
    QString keyField = this->getKeyField();
    int keyColumn = this->getKeyColumn();
    //qDebug() << "ATableWidget::getSelected" << keyField << keyColumn;
    if (keyField!="" && keyColumn!=-1) {result = this->getRecordValue(keyField);}
    return result;
}
QList<int> ATableWidget::getSelectedIndex(){
    QList<int> result;
    int count = tablewidget->selectionModel()->selectedRows().count();
    //qDebug() << "ATableWidget::getSelectedIndex" << count;
    if (count != 0) {
        for (int i=0;i<count;i++) {
            result.append(tablewidget->selectionModel()->selectedRows().at(i).row());
        }
    }
    return result;
}

int ATableWidget::getSelectedRow() {    // Перенесено в static ADataset::getTableSelectedRow
    return ADataset::getTableSelectedRow(tablewidget);
}


int ATableWidget::findKeyIndex(QVariant data, QString field){ // Поиск индекса в виджете по ключу
    int columnindex=-1;
    int result = -1;
    if (field!="") {columnindex = ColumnFieldIndex.keys().at(ColumnFieldIndex.values().indexOf(field));}
    //qDebug() << "ATableWidget::findKeyIndex" << columnindex;
    result = ADataset::findKeyIndex(tablewidget, data, columnindex);
    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Остальные функции
/////////////////////////////////////////////////////////////////////////////////////////////////
void ATableWidget::procProps() {
    for (int column=0;column<tablewidget->columnCount();column++) {
        QString highlight = dataobjlist.at(ColumnObjectIndex[column])->property("Highlight").toString();
        if (highlight!="") {this->highlightColumn(column, highlight);}
    }
}

void ATableWidget::highlightRow(int row, QString color) {                  // Раскрашивает фон строки TableWidget
    if (tablewidget->rowCount()>row) {
        for (int column=0;column<tablewidget->columnCount();column++) {
//            qDebug() << "ATableWidget::highlightRow" << column;
            QTableWidgetItem *item = tablewidget->item(row, column);
            item->setBackgroundColor(QColor(color));
        }
    }
}
void ATableWidget::highlightRowText(int row, QString color) {              // Раскрашивает текст рядка TableWidget
    //qDebug() << "ATableWidget::highlightRow" << highlightedRows << tablewidget->columnCount();
    if (row < tablewidget->rowCount()) {
        for (int column=0;column<tablewidget->columnCount();column++) {
            //qDebug() << "ATableWidget::highlightRow" << column;
            QTableWidgetItem *item = tablewidget->item(row, column);
            if (item) {
                item->setTextColor(QColor(color));
                highlightedRows.replace(row, color);
            }
            //ADelegate *delegate = static_cast<ADelegate*>(tablewidget->itemDelegate());
        //delegate->setHiglightColor("#aa3148");
        }
    }
}
void ATableWidget::highlightColumn(int column, QColor color) {        // Раскрашивает столбец TableWidget
    if (tablewidget->columnCount()>column) {
        for (int row=0;row<tablewidget->rowCount();row++) {
            QTableWidgetItem *item = tablewidget->item(row, column);
            item->setBackgroundColor(color);
        }
    }
}
void ATableWidget::highlightCell(int row, int column, QColor color) { // Раскрашивает ячейку TableWidget
    if (tablewidget->rowCount()>row && tablewidget->columnCount()>column) {
        QTableWidgetItem *item = tablewidget->item(row, column);
        item->setBackgroundColor(color);
    }
}
void ATableWidget::cellTextColor(int row, int column, QColor color) { // Меняет цвет текста в ячейке TableWidget
    if (tablewidget->rowCount()>row && tablewidget->columnCount()>column) {
        QTableWidgetItem *item = tablewidget->item(row, column);
        item->setTextColor(color);
    }
}

double ATableWidget::sum(QString fieldname) {
    double result = 0;
    QAbstractItemModel *model = 0;
    model = tablewidget->model();
    if (model==0) {return 0;}
    if (fieldname!="" && ColumnFieldIndex.values().contains(fieldname)) {
        int fieldcolumn = ColumnFieldIndex.keys().at(ColumnFieldIndex.values().indexOf(fieldname));
//        qDebug() << "ATableWidget::sum " << fieldname << fieldcolumn << model->rowCount();
        for (int i=0;i<model->rowCount();i++) {
            double value = AFunc::getModelValue(model, i, fieldcolumn).toDouble();
//            qDebug() << "ATableWidget::sum" << value;
            result += value;
        }
    }
    //qDebug() << "ATableWidget::sum" << result;
    return result;//AFunc::round(result,2);
}
void ATableWidget::updateTotals(){
    //qDebug() << "ATableWidget::updateTotals() " << QObject::sender()->objectName();
    for (int i=0;i<dataobjlist.count();i++) {
        QObject *obj = dataobjlist.at(i);
        QString objClassName = obj->metaObject()->className();
        if (objClassName == "QLineEdit") {
            QString totalField = obj->property("Total").toString();
            if (totalField!="" && ColumnFieldIndex.values().contains(totalField)) {
                //qDebug() << "ATableWidget::updateTotals() " << ColumnFieldIndex.values().indexOf(totalField) ;
                //for (int j=0;j<ColumnFieldIndex.values().count();j++) {
                    //qDebug() << ColumnFieldIndex.keys().at(j) << ColumnFieldIndex.values().at(j) << AFunc::getModelValue(tablewidget->model(),0,ColumnFieldIndex.keys().at(j));
                //}
                int keyFieldColumn = ColumnFieldIndex.keys().at(ColumnFieldIndex.values().indexOf(totalField));
                float sum=0;
                for (int j=0;j<tablewidget->model()->rowCount();j++) {
                    //QVariant aaa = AFunc::getModelValue(tablewidget->model(),j,keyFieldColumn);
                    float value = AFunc::getModelValue(tablewidget->model(),j,keyFieldColumn).toFloat();
                    //qDebug() << "value " << value;
                    sum = sum+value;
                }
                ADataset::setData(obj, sum);
                //qDebug() << "setData " << sum << obj;
            }
        }
    }
}

int ATableWidget::insertRow(int row) {
    //qDebug() << "ATableWidget::insertRow" << tablewidget->selectionMode() << tablewidget->selectionBehavior();
    //qDebug() << "ATableWidget::insertRow" << tablewidget << this->objectName() << row;
    this->disableTableConnects();
    int result = -1;
    if (row==-1) {row = tablewidget->rowCount();} // Для вставки в последний ряд
    //qDebug() << "ATableWidget::insertRow rowcount1" << tablewidget->rowCount();
    tablewidget->insertRow(row);
    //qDebug() << "ATableWidget::insertRow rowcount2" << tablewidget->rowCount();
    highlightedRows.insert(row, defaultColor);
    result = row;
    setNRI(row);
    //qDebug() << "ATableWidget::insertRow columnCount" << tablewidget->columnCount();
    for (int i=0;i<tablewidget->columnCount();i++) {
        QString data = dataobjlist.at(ColumnObjectIndex[i])->property("Default").toString();
        this->setCellData(row,   i, data, false);
        //qDebug() << row << i << data;
    }
    this->procProps();
    //AGui::setObjectView(tablewidget); // NEW!
    //qDebug() << "ATableWidget::insertRow" << tablewidget->rowCount() << tablewidget->currentRow() << tablewidget->property("type");
    this->createTableConnects();
    //qDebug() << "//ATableWidget::insertRow";
    return result;
}
void ATableWidget::deleteRow(int row) { // Удаление строки
    //qDebug() << "ATableWidget::deleteRow" << row;
    if (row<0 || row>tablewidget->rowCount()) {return;}
    int keyValue, keyColumn;
    int keyLDValue, keyLDColumn;
    QString keyLDField = widget->property("LKeyField").toString();

    int NRI = getNRI();
    //    qDebug() << "ATableWidget::deleteRow NRI" << NRI;
    if (row!=NRI) {
        keyColumn = this->getKeyColumn();
        keyValue = AFunc::getModelValue(tablewidget->model(), row, keyColumn).toInt();
        if (keyValue!=0) {List_DeletedId.append(keyValue);}
        keyLDColumn = ColumnFieldIndex.keys().at(ColumnFieldIndex.values().indexOf(keyLDField));
        keyLDValue = AFunc::getModelValue(tablewidget->model(), row, keyColumn).toInt();
        if (keyLDValue!=0) {List_DeletedLDId.append(keyLDValue);}
        setNRI(NRI-1);
        //        qDebug() << "ATableWidget::deleteRow remove" << row;
        tablewidget->removeRow(row);
        highlightedRows.removeAt(row);
    }
    engine->uniDeleteSlot(tablewidget);
    //    engine->uniDeleteSlot(widget);
}
void ATableWidget::deleteSelectedRows() { // Удаление выделенных строк в ручном редакторе
    //qDebug() << "ATableWidget::deleteSelectedRows()";
    QMap<int,int> temp;
    QModelIndexList selected = tablewidget->selectionModel()->selectedIndexes();
    for (int i=0;i<selected.count();i++) {
        temp[selected.at(i).row()]=1;
    }
    if (temp.keys().count() > 0) {
        int first = temp.keys().at(0);
        //qDebug() << first;
        for (int i=0;i<temp.keys().count();i++) {
            //            qDebug() << "del:" << first;
            this->deleteRow(first);
        }
    }
}

int ATableWidget::rowCount(){
    return tablewidget->rowCount();
}

int ATableWidget::getCurrentRow() {
    int result=-1;
    result = tablewidget->currentRow();
    return result;
}

QWidget * ATableWidget::getTableWidget() {
    return tablewidget;
}
QAbstractItemModel * ATableWidget::getSqlModel() {
    return tablewidget->model();
}
QAbstractItemModel * ATableWidget::getModel() {
    //qDebug() << "ATableWidget::getModel()";
    return tablewidget->model();
}
void ATableWidget::setEnabled(bool state){
    tablewidget->setEnabled(state);
}

void ATableWidget::deleteKeysId() {
    QAbstractItemModel *model = tablewidget->model();
    int keycolumn = this->getKeyColumn();
    for (int row=0;row<model->rowCount();row++) {
        QVariant result = AFunc::setModelValue(model, row, keycolumn, "");
    }
}

#include "QClipboard"
#include "QApplication"
bool ATableWidget::slot_copyToClipboard(QTableWidget *tablewidget) {
    // Коннектитцо из ADataset::copyToClipboard_slot() а оно из ADataset::Preprocessing
    //qDebug() << "ATableWidget::public_copyToClipboard" << tablewidget;
    QClipboard *clipboard=QApplication::clipboard();
    QTableWidgetItem *tmp_cell;
    QString table_cells = "", tmp_cell_string;
    QTableWidgetSelectionRange range;
    QList <QTableWidgetSelectionRange> ranges = tablewidget->selectedRanges();
    bool b = false;
    if (ranges.isEmpty()) {
        range = QTableWidgetSelectionRange();
    } else {
        range = ranges.first();
        for (int i=0;i<range.columnCount();i++) {
            range.columnCount();
            if (!tablewidget->isColumnHidden(range.leftColumn()+i) && tablewidget->columnWidth(range.leftColumn()+i)!=0) {
                if (b) {table_cells += "\t";}
                table_cells += tablewidget->horizontalHeaderItem(range.leftColumn()+i)->text();
                b=true;
            }
        }
        table_cells += "\n";
    }
    for (int i=0;i<range.rowCount();i++) {
        b=false;
        if (i>0) { table_cells += "\n"; }
        for (int j=0;j<range.columnCount();j++) {
            if (!tablewidget->isColumnHidden(range.leftColumn()+j) && tablewidget->columnWidth(range.leftColumn()+j)!=0) {
                if (b) {table_cells += "\t";}
                tmp_cell = tablewidget->item(range.topRow()+i,range.leftColumn()+j);
                if (tmp_cell) {
                    tmp_cell_string = tmp_cell->text();
                    if ( AVariant::ifNumber(tmp_cell_string) ) { tmp_cell_string.replace(".", ","); }
                    table_cells += tmp_cell_string;
                } else { table_cells += ""; }
                b=true;
            }
        }
    }
    clipboard->setText(table_cells); // заполнение буфера обмена}
    return true;
}


