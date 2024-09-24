#include "ATableWidgetStatic.h"

#include <QStandardItem>
#include "adataset.h"
#include "adatabase.h"
#include "afunc.h"
#include <QStringList>
#include <QDebug>
#include "agui.h"
#include "avariant.h"
#include "adelegate.h"
#include "forms/atablewidget.h"

ATableWidgetStatic::ATableWidgetStatic() {}

void ATableWidgetStatic::static_createTableWidget(QTableWidget * tablewidget) {
    //qDebug() << "ATableWidget::static_createTableWidget" << tablewidget->objectName();
    QStandardItemModel *model = static_cast<QStandardItemModel*>(tablewidget->model());
    model->removeRows(0, model->rowCount());
    model->removeColumns(0, model->columnCount());
    // Чтение источника данных
    QString sqlString = ADataset::readDataSource(tablewidget);
    if (tablewidget->property("ifDebug").toBool()) {qDebug() << "[Debug SQL : ATableWidget::static_createTableWidget] " << sqlString;}
    QString connection = tablewidget->property("Connection").toString();
    if (sqlString=="") {return;}

    // Insert columns to table, update headers
    QList<QString> fieldsList = ADatabase::selectFieldsList(sqlString, connection);
    if (fieldsList.count()==0) {return;}
    tablewidget->setProperty("Fields", AFunc::ListToStr(fieldsList));

    QString header;
    for (int i=0;i<fieldsList.count();i++) {
        model->insertColumn(i);
        //qDebug() << "ATableWidgetStatic::static_createTableWidget" << i << tablewidget->itemAt(i);
        header = fieldsList.at(i);
        header.replace("\\n","\n");
        tablewidget->setHorizontalHeaderItem(i, new QTableWidgetItem(header));
        tablewidget->setWordWrap(true);
    }


//    QTableWidget tablewidget->item(0,0)
//    // Set columns alignment. Read it from ColumnsAlign widget property
//    QStringList alignFieldsList = AFunc::strToList(tablewidget->property("ColumnsAlign").toString());
//    for (int i=0;i<tablewidget->columnCount();i++) {
//        if (alignFieldsList[i]=="center") {
//            //tablewidget->itemAt(i,1)->setTextAlignment(Qt::AlignHCenter);

//        }
//    }


    ATableWidgetStatic::resizeColumns(tablewidget);
    AGui::setObjectView(tablewidget);
}

// Read ColumnWidth property (0,100.. or 0,100%), resize columns
void ATableWidgetStatic::resizeColumns(QTableWidget * tablewidget) {
    QStandardItemModel *model = static_cast<QStandardItemModel*>(tablewidget->model());
    QStringList columnsWidthList = AFunc::strToList(tablewidget->property("ColumnsWidth").toString());
    QStringList fieldsList = AFunc::strToList(tablewidget->property("Fields").toString());
    if (fieldsList.count()==0) {return;}

    int vscroll_width=0;
    if (tablewidget->verticalScrollBar()->isVisible()) {vscroll_width=tablewidget->verticalScrollBar()->width();}
    vscroll_width = 0;
    //qDebug() << tablewidget << tablewidget->verticalScrollBar()->visibleRegion().isEmpty() << tablewidget->verticalScrollBar()->sizeHint().width() << tablewidget->verticalScrollBar()->maximum();
    //qDebug() << tablewidget->verticalScrollBar()->isEnabled() << tablewidget->verticalScrollBar()->isVisible() << tablewidget->verticalScrollBar()->isHidden();
    //qDebug() << "ATableWidgetStatic::resizeColumns" << tablewidget << tablewidget->verticalScrollBar()->width() << tablewidget->verticalScrollBar()->height() << tablewidget->verticalScrollBar()->isVisible()  << vscroll_width;
    //qDebug() << "ATableWidgetStatic::resizeColumns" << tablewidget << tablewidget->horizontalScrollBar()->width() << tablewidget->horizontalScrollBar()->height() << tablewidget->horizontalScrollBar()->isVisible();
    int tablewidth = tablewidget->width() - vscroll_width;
    int columnwidth;

    // Search last visible column
    int lastcolumn=0;
    for (int i=0;i<fieldsList.count();i++) {
        if (i<columnsWidthList.count()) {
            if (columnsWidthList[i].contains('%')) {
                columnwidth = columnsWidthList[i].remove("%").toInt()*tablewidth/100;
            } else {
                columnwidth = columnsWidthList[i].toInt();
            }
            if (columnwidth>0) lastcolumn=i;
        }
    }
    columnsWidthList = AFunc::strToList(tablewidget->property("ColumnsWidth").toString());
    int sumwidth = 0;
    for (int i=0;i<fieldsList.count();i++) {
        if (i<columnsWidthList.count()) {
            if (columnsWidthList[i].contains('%')) {
                columnwidth = columnsWidthList[i].remove("%").toInt()*tablewidth/100;
            } else {
                columnwidth = columnsWidthList[i].toInt();                
            }
            // Stretch last visible column to full table width
            if (i==lastcolumn) {
                if (sumwidth<tablewidth) {
                    columnwidth = tablewidth-sumwidth-2;
                }
            }
            tablewidget->setColumnWidth(i, columnwidth);
            sumwidth = sumwidth + columnwidth;
        }
    }
}
void ATableWidgetStatic::static_clear(QTableWidget * tablewidget) {
    //qDebug() << "ATableWidgetStatic::static_clear";
    int rowcount = tablewidget->rowCount();
    for (int i=0;i<rowcount;i++) {tablewidget->removeRow(0);}
}
void ATableWidgetStatic::static_update(QWidget *widget, QTableWidget * tablewidget, QMap<int, QMap<QString, QVariant> > columnProps) {
    //qDebug() << "ATableWidget::static_update" << tablewidget << widget->property("ifDebug").toBool() << tablewidget->columnCount();
    if (tablewidget->columnCount()==0) {ATableWidgetStatic::static_createTableWidget(tablewidget);}
    if (tablewidget->columnCount()==0) {return;}

    QStandardItemModel *model = static_cast<QStandardItemModel*>(tablewidget->model());
    ATableWidget:static_clear(tablewidget);

    if (columnProps.isEmpty()) {columnProps = ATableWidgetStatic::getColumnProps(tablewidget);}
    QString sqlString = ADataset::readDataSource(widget);

    if (widget->property("ifDebug").toBool()) {qDebug() << "[Debug SQL : ATableWidget::static_update] " << sqlString;}

    QString connection = widget->property("Connection").toString();

    QList< QMap<QString, QVariant> > records = ADatabase::selectList(sqlString, connection);
    //qDebug() << "ATableWidget::static_update" << sqlString << records.count();
    QString data, precision;
    int rowHeight = tablewidget->property("rowHeight").toInt();

    //qDebug() << "ATableWidget::static_update records" << records.count();
    for (int i=0;i<records.count();i++) {
        model->insertRows(i, 1);
        //qDebug() << i << columnProps.at(0)["Data"].toString() << records.at(i)[columnProps.at(0)["Data"].toString()].toString();
        //qDebug() << "ATableWidget::static_update" << columnProps.count();
        //qDebug() << "ATableWidget::static_update columns" << columnProps.count();
        for (int i1=0;i1<columnProps.count();i1++) {
            data = AVariant::convertVariantToString(records.at(i)[columnProps[i1]["Data"].toString()], columnProps[i1]["Precision"].toString());
            model->setData(model->index(i, i1), data);
        }
        if (rowHeight!=0) {tablewidget->setRowHeight(i, rowHeight);}
    }
    //qDebug() << "ATableWidget::static_update END";
}

void ATableWidgetStatic::static_updateRecord(QWidget *widget, QTableWidget *tablewidget, int id, QMap<int, QMap<QString, QVariant> > columnProps) {
    //qDebug() << "ATableWidget::static_updateRecord BEGIN" << id;
    if (tablewidget->columnCount()==0) {ATableWidgetStatic::static_createTableWidget(tablewidget);}
    if (tablewidget->columnCount()==0) {return;}
    QString sqlString = ADataset::readDataSource(widget);
    QString connection = widget->property("Connection").toString();
    QList< QMap<QString, QVariant> > records = ADatabase::selectList(sqlString, connection);
    //qDebug() << "ATableWidget::static_updateRecord select" << records;
    QMap<QString, QVariant> record; int position;
    for (int i=0;i<records.count();i++) { if (records.at(i)["id"]==id) { record = records.at(i); position = i; } }
    QAbstractItemModel *model = tablewidget->model();
    QModelIndex index = ATableWidgetStatic::findIndex(tablewidget, id);
    if (columnProps.isEmpty()) {columnProps = ATableWidgetStatic::getColumnProps(tablewidget);}
    if (!index.isValid() && !record.isEmpty()) {
            tablewidget->insertRow(position);
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

void ATableWidgetStatic::static_select(QTableWidget *tablewidget, int id) {
    //qDebug() << "ATableWidget::static_select";
    QModelIndex index = ATableWidgetStatic::findIndex(tablewidget, id);
    //qDebug() << "ATableWidget::static_select" << index << index.row();
    tablewidget->selectRow(index.row());
    tablewidget->scrollTo(index);
}

QModelIndex ATableWidgetStatic::findIndex(QTableWidget *tablewidget, int id) {
    //qDebug() << "ATableWidgetStatic::findIndex";
    QModelIndex index;
    QAbstractItemModel *model = tablewidget->model();
    int keyColumn = tablewidget->property("keyColumn").toInt();
    //qDebug() << "ATreeView::findIndex" << model << id << model->rowCount(parentIndex);
    for (int i=0;i<model->rowCount();i++) {
        if (model->data(model->index(i, keyColumn)).toInt()==id && !index.isValid()) {index = model->index(i, keyColumn);}
        if (index.isValid()) {return index;}
    }
    return index;
}


QMap<int, QMap<QString, QVariant> > ATableWidgetStatic::getColumnProps(QTableWidget *tablewidget) {
    //qDebug() << "ATableWidgetStatic::getColumnProps";

    QMap<int, QMap<QString, QVariant> > result;
    QAbstractItemModel *model = tablewidget->model();
    QMap<QString, QVariant> temp;
    for (int i=0;i<model->columnCount();i++) {
        result[i]["Data"] = tablewidget->horizontalHeaderItem(i)->data(Qt::DisplayRole).toString();
    }
    return result;
}

