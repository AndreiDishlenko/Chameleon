#ifndef ADATASET_H
#define ADATASET_H

#include <QObject>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QStandardItemModel>
#include <QTreeView>
#include <QTableWidget>
#include <QComboBox>
#include <QDate>

class ADataset : public QObject
{
    Q_OBJECT
private:

public:
    explicit ADataset(QObject *parent = 0);
    ~ADataset();
    static QString readDataSource(QObject *object);    // Чтение параметров, получение модели данных
    //static QSqlQueryModel * readDataSource1(QObject *object, QAbstractItemModel *model);    // Чтение параметров, получение модели данных
    static QSqlQueryModel * readSourceModel(QObject *object, QSqlQueryModel *qModel = new QSqlQueryModel());    // Чтение параметров, получение модели данных
//  static QSqlQuery * readSourceQuery(QObject *object);

signals:
public slots:
    static void prepareModelObject(QObject *object);
    static void updateObject(QObject *object, int id=0);
    static QVariant getData(QObject *object, int column=-1);
    static bool setData(QObject *object, QVariant data);
    static void setDefault(QObject *object);



    void fillComboBox(QObject *obj, QStringList stringList);
    void setComboBoxData(QObject *obj, QString text);


//    static QVariant detectNumber(QVariant data, int precision);



//    void slot_copyToClipboard();

    // Simple TableWidget works functions
    static QVariant getTableCellData(QTableWidget *tablewidget, int row, int column);                   // Получает значение ячейки простого QTableWidget
    static void setTableCellData(QTableWidget *tablewidget, int row, int column, QVariant data);        // Устанавливает значение ячейки простого QTableWidget
    static int findTableCellData(QTableWidget *tablewidget, int row, QVariant data);
    static int getTableSelectedRow(QTableWidget *tablewidget);

    static int findModelIndex(QAbstractItemModel *model, int column, QVariant data);
    static int findKeyIndex(QObject *object, QVariant data, int column=-1);
    QList<int> findTreeTableSubRecords(QString dictName, int id);

    // Work with different data
    static QString convertToScriptDate(QString srcdate);
    static QDate convertToDate(QString srcdate);


    static QString updateLocalStringVariables(QString var, QObject *object);                   // Преобразовывает строковую переменную в значение (используется в sqlSource и defValue)
    static QString updateStringVariablesFromWidget(QString str, QObject *object);
    static QString parseHtml(QString html, QMap <QString, QVariant> mapname, QString querystring);
};

#endif // ADATASET_H
