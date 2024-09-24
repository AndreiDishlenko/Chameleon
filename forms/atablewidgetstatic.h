#ifndef ATABLEWIDGETSTATIC_H
#define ATABLEWIDGETSTATIC_H

#include <QObject>
#include <QTableWidget>

class ATableWidgetStatic : public QObject {
    Q_OBJECT
public:
    ATableWidgetStatic();

    static void static_createTableWidget(QTableWidget * tablewidget);
    static void static_clear(QTableWidget * tablewidget);
    static void static_update(QWidget *widget, QTableWidget * tblWidget, QMap<int, QMap<QString, QVariant> > columnProps = QMap<int, QMap<QString, QVariant> >());
    //static void static_updateDelegate(QTableWidget *tw, QString fieldName="", int row=-1);
    static void static_updateRecord(QWidget *widget, QTableWidget *tablewidget, int id, QMap<int, QMap<QString, QVariant> > columnProps = QMap<int, QMap<QString, QVariant> >());

    static void resizeColumns(QTableWidget * tablewidget);

    static void static_select(QTableWidget *tablewidget, int id);

    static QModelIndex findIndex(QTableWidget *tablewidget, int id);
    static QMap<int, QMap<QString, QVariant> > getColumnProps(QTableWidget *tablewidget);

};

#endif // ATABLEWIDGETSTATIC_H
