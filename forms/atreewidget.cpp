///  ///////////////////////////////////////////////////////////////////////////
///
/// Реализует работу таблицы со сворачиваемыми элементами
/// Не предназначена для редактирования
/// Используется только для представления отчетов
///
/// ////////////////////////////////////////////////////////////////////////////

#include "atreewidget.h"
#include "aform.h"
#include "afunc.h"
#include "globals.h"
#include <forms/atreewidget.h>
#include <adataset.h>
#include <QHeaderView>
#include <QScrollBar>
#include <avariant.h>

ATreeWidget::ATreeWidget(QWidget *parent) : QWidget(parent) {
    //qDebug() << "constructor ATreeWidget, parent:" << parent->objectName();
    this->setObjectName("Table");
    this->setParent(parent);
    //created=false;
    widget = parent;
    engine = parent->findChild<AEngine*>("qs");
    //flag_allowInsert = widget->property("allowInsert").toBool();

    /// Формирование всех обьектов, содержащих сведения о данных представления
    objlist = AForm::getFormDataObjects(widget);

    /// Расстановка полей по порядку
    for (int i=0;i<objlist.count()-1;i++){
        for (int j=0;j<objlist.count()-1;j++){
            int to1=objlist.at(j)->property("TableOrder").toInt();
            int to2=objlist.at(j+1)->property("TableOrder").toInt();
            if (to1>to2) {objlist.swap(j,j+1);}
        }
    }

    /// Создание списка полей формы и их свойств
    QString objclassname;
    QWidget objtemp;
    int column=0;
    treewidget = 0;

    /// Ключевое поле
    keyField = widget->property("KeyField").toString();
//    groupByField = widget->property("groupBy").toString();
//    if (keyField!="") {
//        QMap<QString, QVariant> temp;
//        temp["Column"]=0;
//        temp["Data"]=keyField;
//        temp["Header"]=widget->property("Header");
//        temp["Hidden"]=false;
//        temp["TableOrder"]=0;
//        temp["ColumnWidth"]=0;
//        fieldslist[keyField] = temp;
//        columnslist << temp;
//        column++;
//    } else {
//        qDebug() << "[WARNING] KeyField in " << widget->objectName() << "table is not defined.";
//    }

    // -- Составление списка полей со свойствами
    for (int i=0;i<objlist.count();i++) {
        objclassname = objlist.at(i)->metaObject()->className();
        /// Find treewidget
        if (objclassname=="QTreeWidget") {
            treewidget = static_cast<QTreeWidget*>(objlist.at(i));
        }
        if (objlist.at(i)->property("Data").toString()!=""  &&
                (objclassname=="QLineEdit" || objclassname=="QComboBox")) {
            QWidget * objtemp = static_cast<QWidget *>(objlist.at(i));
            QMap<QString, QVariant> temp;
            temp["Column"]=column;
            temp["Data"]=objlist.at(i)->property("Data");
            temp["Header"]=objlist.at(i)->property("Header");
            if (temp["Data"].toString() != groupByField) {
                temp["Hidden"] = objlist.at(i)->property("Hidden");
            } else {
                temp["Hidden"] = true;
            }
            temp["TableOrder"]=objlist.at(i)->property("TableOrder");
            temp["ColumnWidth"]=objlist.at(i)->property("ColumnWidth");
            temp["Precision"]=objlist.at(i)->property("Precision");
            temp["Alignment"] = objlist.at(i)->property("alignment");
            temp["ifTotals"] = objlist.at(i)->property("ifTotals");
            fieldslist[objlist.at(i)->property("Data").toString()] = temp;
            columnslist << temp;
            column++;
        }
    }

    /// SET default db connection of TreeWidget
    if (widget->property("Connection").toString()!="") {
        connection = widget->property("Connection").toString();
    } else {
        connection = globals["defConnection"];
    }

    /// Create treewidget if it's not founded, set it parameters
    if (treewidget==0) {
        treewidget = new QTreeWidget(widget);
        //ADataset::prepareModelObject(treewidget);
        treewidget->show();
    }

    widget->installEventFilter(this);
    treewidget->installEventFilter(this);
    //treewidget->setObjectName("treeWidget");
    treewidget->move(QPoint(0,0));
    treewidget->setVisible(true);
    treewidget->setWordWrap(true);
    treewidget->verticalScrollBar()->setVisible(true);
    treewidget->raise();
    treewidget->setSortingEnabled(true);
    //treewidget->setSelectionMode(QAbstractItemView::MultiSelection);
    //treewidget->setEditTriggers(QAbstractItemView::AnyKeyPressed | QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked); // Мгновенное редактирование
    //treewidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    //treewidget->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);


    //-- Добавление столбцов в TreeWidget
    //treewidget->setColumnCount(columnslist.count()+1);
    QTreeWidgetItem * header = treewidget->headerItem();
    int firstvisiblecolumn=-1;
    for (int i=0;i<columnslist.count();i++) {
        //qDebug() << i << columnslist.at(i)["Column"] << columnslist.at(i)["Header"] << columnslist.at(i)["Data"] << columnslist.at(i)["ColumnWidth"];
        // Создание заголовков
        header->setText(i, columnslist.at(i)["Header"].toString().replace("\\n","\n"));
        header->setTextAlignment(i, Qt::AlignCenter);
        // Создание ячеей итогов в подвале
        if (widget->property("ifTotals").toBool()) {
            QLabel *label = new QLabel(widget);
            label->setProperty("type", "tw_totals_label");
            label->setAlignment(Qt::AlignCenter);
            // Надпись Итоги в итогах
            if (!columnslist.at(i)["Hidden"].toBool() &&
                    columnslist.at(i)["ColumnWidth"].toString().remove("%").toInt()>0 &&
                    firstvisiblecolumn==-1) {
                firstvisiblecolumn = i;
                label->setProperty("ifFirst", true);
                label->setText("Итоги:");
            }
            totalslist << label;
            if (columnslist.at(i)["Hidden"].toBool()) { label->hide(); qDebug() << "hide";}
        }
        if (columnslist.at(i)["Hidden"].toBool()) { treewidget->header()->hideSection(i); }
    }

    QHeaderView * header1 = treewidget->header();
    //header1->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header1->setMinimumSectionSize(0);
    header1->resizeSection(0,0);
    this->resizeColumnsToWidth();
}

ATreeWidget::~ATreeWidget() {
    QObject::disconnect(treewidget, 0, 0 , 0);
}

void ATreeWidget::update() {
    //qDebug() << "ATreeWidget::update";
    /// Read Datasource
    QString sqlString = ADataset::readDataSource(widget).replace("\n", " ");
    if (widget->property("ifDebug").toBool()) {qDebug() << "[Debug SQL : ATreeWidget::update] " << sqlString;}
    QList< QMap<QString, QVariant> > records = ADatabase::selectList(sqlString, connection);
    QString data, precision;

    treewidget->clear();

    qDebug() << "";
    qDebug() << "ATreeWidget::update" << records.count() << columnslist.count() << treewidget << sqlString.count("\n");

    /// Insert Data in TreeWidget
    for (int i=0;i<records.count();i++) {
        QTreeWidgetItem *twitem = new QTreeWidgetItem(treewidget);
        for (int i1=0;i1<columnslist.count();i1++) {
              twitem->setData(i1, Qt::DisplayRole, records.at(i)[columnslist.at(i1)["Data"].toString()]);
              //qDebug() << columnslist.at(i1)["Data"].toString() << records.at(i)[columnslist.at(i1)["Data"].toString()];
        }
        for (int i1=0;i1<columnslist.count();i1++) {
            twitem->setTextAlignment(i1, columnslist.at(i1)["Alignment"].toInt());
        }
        treewidget->insertTopLevelItem(i, twitem);
    }

    /// IF GroupBy field is not empty, then process Table
    QString groupby = widget->property("groupBy").toString();
    if (groupby!="" && fieldslist[groupby]["Data"]!="") {
        int groupbycolumn = fieldslist[groupby]["Column"].toInt();

        /// Sort
        treewidget->sortByColumn(groupbycolumn, Qt::AscendingOrder);

        /// Take array of groupby values
        QStringList groupby_list;
        QString temp="";
        for (int i=0;i<treewidget->topLevelItemCount();i++) {
            QTreeWidgetItem * twi = treewidget->topLevelItem(i);
            temp = twi->data(groupbycolumn, Qt::DisplayRole).toString();
            //qDebug() << "temp" << temp;
            if (!groupby_list.contains(temp) && temp!="") groupby_list<<temp;
        }
        //qDebug() << treewidget->topLevelItemCount() << groupby << groupbycolumn;
        //AFunc::echoStringList(groupby_list);


        /// Insert new toplevel items
        int i2=0;
        int index = treewidget->topLevelItemCount();
        for (int i=0;i<groupby_list.count();i++) {
            QTreeWidgetItem *twitem = new QTreeWidgetItem(treewidget);
            twitem->setData(0, Qt::DisplayRole, groupby_list.at(i));
            //twitem->setDisabled(true);
            twitem->setFirstColumnSpanned(true);
            twitem->setExpanded(true);
            treewidget->insertTopLevelItem(index, twitem);
            //qDebug() << "insertTopLevelItem" << twitem->data(0, Qt::DisplayRole).toString();
            index++;
            i2 = treewidget->topLevelItemCount();
            //qDebug() << "--" << groupby_list.at(i);
            /// transfer old toplevel item to slave
            for (int i1=0;i1<i2;i1++) {
                QTreeWidgetItem * twi = treewidget->topLevelItem(i1);
                temp = twi->data(groupbycolumn, Qt::DisplayRole).toString();
                //qDebug() << temp;
                if (temp == groupby_list.at(i)) {
                    twi = treewidget->takeTopLevelItem(i1);
                    //qDebug() << "--insertTopLevelItem" << twi->data(2, Qt::DisplayRole).toString();
                    //qDebug() << twi->data(1, Qt::DisplayRole).toString();
                    twitem->addChild(twi);
                    i1--;
                    i2--;
                }
            }
        }
    }

    for (int i=0;i<columnslist.count();i++) {
        //qDebug() << columnslist.at(i)["Data"];
        if (columnslist.at(i)["ifTotals"].toBool()) {
            totalslist.at(i)->setText(QString::number(ATreeWidget::sum(i), 'f', 2));
        }
    }

}


void ATreeWidget::resizeColumnsToWidth() {
    /// Динамическая подгонка таблицы под виджет
    int tableheight = ((widget->size().height()-40)/25 )*25+40; //55qRound( (widget->size().height()-40)/25 )*25+40;
    if (!widget->property("ifTotals").toBool()) {
        treewidget->resize(QSize(widget->size().width(), widget->size().height()));
        //widget->resize(QSize(widget->size().width(), tableheight+25));
    } else {
        treewidget->resize(QSize(widget->size().width(), widget->size().height()-25));
        //widget->resize(QSize(widget->size().width(), tableheight+25));
    }

    /// Изменение ширины столбцов
    int columnwidth, temp=1;
    for (int i=0;i<columnslist.count();i++) {
        if (columnslist.at(i)["ColumnWidth"].toString().contains("%")) {
            columnwidth = columnslist.at(i)["ColumnWidth"].toString().remove("%").toInt()*(treewidget->width()-treewidget->verticalScrollBar()->geometry().width())/100;
        } else {
            columnwidth = columnslist.at(i)["ColumnWidth"].toInt();
        }
        //qDebug() << "resize" << i << columnwidth << columnslist.at(i)["Header"] << columnslist.at(i)["data"] << columnslist.at(i)["Hidden"];
        treewidget->header()->resizeSection(i, columnwidth);
        if (i==columnslist.count()-1) {
            columnwidth = treewidget->width()-temp;
        }
        if (widget->property("ifTotals").toBool()) {
            if (!columnslist.at(i)["Hidden"].toBool() && !totalslist.at(i)->property("isFirst").toBool()) {
                totalslist.at(i)->resize(QSize(columnwidth, 25));
                totalslist.at(i)->move(QPoint(temp, treewidget->height()));
                temp=temp+columnwidth;
            } else {
                totalslist.at(i)->hide();
            }
        }
    }
}

QVariant ATreeWidget::getSelected() {
    //qDebug() << "ATreeWidget::getSelected()";
    QVariant result = -1;
    //qDebug() << "ATreeWidget::getSelected()" << keyField;
    if (keyField!="") {
        //qDebug() << "ATreeWidget::getSelected()1" << keyField << fieldslist[keyField]["Column"];
        //66 result = treewidget->selectedItems().at(0)->data(fieldslist[keyField]["Column"].toInt(), Qt::DisplayRole).toInt();
        result = treewidget->selectedItems().at(0)->data(fieldslist[keyField]["Column"].toInt(), Qt::DisplayRole);
        //qDebug() << "ATreeWidget::getSelected()2" << keyField << fieldslist[keyField]["Column"] << result;
        //        result = this->getRecordValue(keyField, 2);
    } else {
        qDebug() << "[WARNING] TreeWidget " << treewidget->objectName() << "keyField is not defined";
    }
    return result;
}

//bool ATreeWidget::select(QVarinat data) {
//    qDebug() << "ATreeWidget::select";
//}

QVariant ATreeWidget::getRecordValue(QString fieldName, int row) { // Поиск по полю data
//    qDebug() << "ATreeWidget::getRecordValue" << fieldName << row;
//    QVariant result;
//    int column = ColumnFieldIndex.keys().at(ColumnFieldIndex.values().indexOf(fieldName));
//    if (row==-1) {row = this->getCurrentRow();}
//    result = AFunc::getModelValue(treewidget->model(), row, column);
//    return result;
    return 0;
}

double ATreeWidget::sum(int column) {
    double result = 0;
    for (int i=0;i<treewidget->topLevelItemCount();i++) {
        QTreeWidgetItem *twitem = treewidget->topLevelItem(i);
        result = result+twitem->data(column, Qt::DisplayRole).toDouble();
        for (int i1=0;i1<twitem->childCount();i1++) {
            result = result+twitem->child(i1)->data(column, Qt::DisplayRole).toDouble();
        }
    }
    //qDebug() << "ATreeView::sum" << column << result;
    return result;
}

bool ATreeWidget::eventFilter(QObject *obj, QEvent *event) {
    //qDebug() << "ATreeWidget::eventFilter" << obj << event->type();
    if (event->type()==QEvent::Resize) {
        this->resizeColumnsToWidth();
    }
    if (event->type()==QEvent::MouseButtonDblClick) {
        //qDebug() << "ATreeWidget::eventFilter" << obj;
    }
    return false;
}

bool ATreeWidget::slot_copyToClipboard(QTreeWidget *treewidget) {
    //qDebug() << "ATreeWidget::public_copyToClipboard" << treewidget;
    //QClipboard *clipboard=QApplication::clipboard();
    return true;
}
