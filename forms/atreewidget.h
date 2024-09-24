#ifndef ATREEWIDGET_H
#define ATREEWIDGET_H

#include <QWidget>
#include <aengine.h>
#include <QTreeWidget>
#include <QLabel>
#include <aengine.h>

class ATreeWidget : public QWidget
{
    Q_OBJECT
public:
    ATreeWidget(QWidget *parent);
    ~ATreeWidget();

    QWidget *widget;                            // Виджет формы
    QTreeWidget *treewidget;                    // Виджет таблицы
    QString connection;                         // Соединение, используемое по умолчанию

    QVariant getRecordValue(QString fieldName, int row);

    void resizeColumnsToWidth();                  // Установить ширину колонок

public slots:
    void update();                              // Обновляет данные в таблице
    QVariant getSelected();


    double sum(int column);
    static bool slot_copyToClipboard(QTreeWidget *treewidget);


private:
    QObjectList objlist;                        // Объекты формы, которые содержат сведения о столбцах с данными
    QString keyField;
    QString groupByField;
    QMap<QString, QMap<QString, QVariant> > fieldslist;

    QList< QMap<QString, QVariant> > columnslist;
    QList< QLabel* > totalslist;

    AEngine *engine;

    bool eventFilter(QObject *obj, QEvent *event);
signals:

};

#endif // ATREEWIDGET_H
