#ifndef ATREEVIEW_H
#define ATREEVIEW_H

#include <QObject>
#include <QTreeView>
#include <QSqlQueryModel>
#include <QStandardItem>
#include "aengine.h"
#include "adelegate.h"

class ATreeView : public QObject
{
    Q_OBJECT
public:
    ATreeView(QWidget *parent = 0);
    ~ATreeView();

    QWidget *widget;
    QTreeView *treeview;
    AEngine *engine;
    QString formtype;
    QMenu *menu;

    //    bool isFlat;
    int startpos;
    int keyColumn;

    QList<int> objectsList;      // Columns objects (Position in dataobjlist)
    //    QList<QString> headersList;  // Columns headers
    //    QList<QString> fieldsList;  // Columns fieldd
    QList< QMap<QString, QVariant> > columnProps;
    QMap<int, QMap <QString, QVariant> > objProperties;

    void createTreeView();    
    void createTableConnects();
    ADelegate * createDelegate(QObject *obj);

    static void static_createTreeView(QTreeView *tv);

    ////////////////////////////////////////////
    // Обновление и выделение модели
    ////////////////////////////////////////////

    static void static_update(QWidget *widget, QTreeView *treeview, int startId=0, QList< QMap<QString, QVariant> > columnProps = QList< QMap<QString, QVariant> >());
    static void static_updateRecord(QWidget *widget, QTreeView *treeview, int id, QList< QMap<QString, QVariant> > columnProps = QList< QMap<QString, QVariant> >());
    static void static_select(QTreeView *treeview, int id, QList< QMap<QString, QVariant> > columnProps = QList< QMap<QString, QVariant> >());

    static void addTVNode(QStandardItem *parentNode, int node, QList< QMap<QString, QVariant> > tree, QList< QMap<QString, QVariant> > columnProps, bool isFlat);
    static QList<int> findSubNodes(QTreeView *treeview, QModelIndex parentIndex);
    static QList<int> findSubRecords(QMap<int, QMap<QString, QVariant> > array, int id);
    static QModelIndex findIndex(QTreeView *treeview, int id, QModelIndex parentIndex=QModelIndex());

    static QList< QMap<QString, QVariant> > getColumnProps(QTreeView *treeview);
    static int getColumnCount(QTreeView *treeview);
    static QStringList getColumnsList(QTreeView *treeview);
    ////////////////////////////////////////////
    // РАЗНОЕ
    ////////////////////////////////////////////
    int getColumnPosition(QString columnName);
    static QVariant getTVData(QTreeView *tv, int column);

    static int getHeaderPosition(QTreeView *tv, QString headerName);

private:
    QObjectList dataobjlist;

public slots:
    void update(int startId=0);
    void updateRecord(int id);
    void select(int id);
    int getSelected();
    double sum(QString fieldname);
    static QMultiMap<int, bool> getTvState(QTreeView *tv, QStandardItem *parItem);
    static void setTvState(QTreeView *tv, QStandardItem *parItem, QMultiMap<int, bool> stateMap);
    static void expandTVNode(QTreeView *tv, QModelIndex index);
    static bool slot_copyToClipboard(QTreeView *treeview);

    static QVariant getTVSelParent(QTreeView *tv);
};

#endif // ATREEVIEW_H
