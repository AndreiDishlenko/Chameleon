/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Класс ATableWidget
/// - реализовывает табличную форму с данными
/// - Обеспечивает взаимосвязь с базой данных (загрузка, сохранение)
///
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ATABLEWIDGET_H
#define ATABLEWIDGET_H

#include <QTableWidget>

#include "aapplication.h"
#include "aengine.h"
#include "forms/adelegate.h"

class ATableWidget : public QObject
{
    Q_OBJECT
public:
    ATableWidget(QWidget *parent);
    ~ATableWidget();

    //void setCurrentConnection(QString conName="");

    QWidget *widget;                            // Виджет формы
    QTableWidget *tablewidget;
    AEngine *engine;                            // Скриптовый движок формы
    QString connection;

    QList<QString> highlightedRows;
    QString defaultColor;

    void createTableWidget();
    void resizeColumnsToWidth();

    void createTableConnects();
    void disableTableConnects();


private:
    bool created;


    QMap<int, QMap<QString, QVariant> > columnProps;

    QMap<int, QString> ColumnHeaderIndex;       // Индекс <Столбец, header таблицы>
    QMap<int, QString> ColumnFieldIndex;        // Индекс <Столбец, поле БД>
    QMap<int, QString> ColumnLDFieldIndex;      // Индекс <Столбец, поле БД>
    QMap<int, QString> ObjectsFieldsIndex;      // Индекс <Позиция objlist, поле Data элемента>
    QObjectList dataobjlist;
    QMap<int, int> ColumnObjectIndex;           // Индекс <Столбец, позиция QObjectList>
    QMap<int, int> ColumnOrder;                 // Индекс <Cтолбец, TableOrder>
    QList<int> NotNullColumns;

    QList<int> List_DeletedId;                  // Удаленные Id
    QList<int> List_DeletedLDId;                // Удаленные Id для режима LD

    bool get_New_FlagValue(int row);
    void set_New_FlagValue(int row, QString value);
    bool flag_allowInsert;                      // Флаг разрешающий добавлять записи
    bool flag_allowActions;

    // Надо реализовать
    QString flag_loaded_color;
    bool can_change_loaded;
    QMap<int, bool> LoadedStateIndex;           // Индекс <id записи, состояние(изменен/нет)>

    int getNRI();
    int setNRI(int index=-1);

    void setCellData(int row, int column, QVariant data, bool execScript=true);



protected:
    bool eventFilter(QObject *obj, QEvent *ev);
    bool loadTableWidget();
    void procProps();

public slots:

    void enableWidget();
    void disableWidget();
    void execFunction(QString script);
    void createTableDelegats();

    void update();

    void updateDelegats();
    void updateFieldDelegats(QString fieldName); //aaa


    void updateRecord(int id);
    void clear();

    double sum(QString fieldname);

    void updateTotals();

    void cellChangeAction(int row, int column);
    void cellUpdateAction(int row, int column);
    void onCellClick(int row, int column);
    void onCellDblClick(int row, int column);
    void onCellFocusIn(QTableWidgetItem* current, QTableWidgetItem* prev);
    void onCellFocusOut(QTableWidgetItem* current, QTableWidgetItem* prev);
    void onSelChange();

    int insertRow(int row=-1);
    void deleteRow(int row=-1);
    void deleteSelectedRows();

    QVariant getRecordValue(QString fieldName, int row=-1);
    void setRecordValue(QString fieldName, QVariant data, int row=-1);
    QVariant getColumnValue(QString headerName, int row=-1);
    void setColumnValue(QString headerName, QVariant data, int row=-1);

    bool saveTableWidget(QString param="");
    //    bool saveTableWidgetLD(QString param="");
    //    void saveTableWidgetLD_old();

    int rowCount();
    QAbstractItemModel * getSqlModel();
    QAbstractItemModel * getModel();
    QWidget * getTableWidget();
    int getCurrentRow();

    // Перемещение по таблице
    void nextCell();
    void prevCell();

    void select(int id);
    void selectKey(QVariant data);

    void selectRow(int row);
    void selectCurrentRows();
    void scrollToCurrent();
    // Поиск
    QVariant getSelected();
    QList<int> getSelectedIndex();
    int getSelectedRow();
    int findKeyIndex(QVariant data, QString field="");

    void setEnabled(bool state=true);
    //    void cellChange(int row, int column);

    void highlightRow(int row, QString color);
    void highlightRowText(int row, QString color);
    void highlightColumn(int column, QColor color);
    void highlightCell(int row, int column, QColor color);
    void cellTextColor(int row, int column, QColor color);

    QString getKeyField();              // Получить имя ключевого столбца
    int getKeyColumn();              // Получить номер ключевого столбца

    void deleteKeysId();                // Удалить значения ключевых столбцов - для создания подчиненных документов

    static bool slot_copyToClipboard(QTableWidget *tablewidget);
};

#endif // ATABLEWIDGET_H
