#ifndef AGUI_H
#define AGUI_H

#include <QTableView>
#include <QAbstractItemModel>
#include <QTreeView>
#include <QStandardItem>
#include <QTableWidget>
#include <QPushButton>
#include <QScrollArea>
//#include <QWebView>

class AGui: public QObject {
Q_OBJECT
public:
    explicit AGui(QObject *parent = 0);
    ~AGui();

    static QWidget* openUi(QString uiName, QWidget * parent = 0);
//    static void Preprocessing(QObject *object);

    static QObjectList getSubWidgets(QObject *obj);
    static QObjectList getSubForms(QObject *obj);
//    static QObjectList getWidgetObjs(QObject *obj);
    static QObjectList getFormObjs(QObject *obj);
    static QObjectList getFormAllObjs(QObject *obj);

    static void dialog();
//    static void setTvRowHeight(QTableView *tv, int height);

public slots:
    static void setObjectView(QObject *object);
    static void startWidgetLocalization(QWidget *wgt);
    static void setObjectLocalization(QObject *object);
    static void msgBox(QString text);
    static bool dlgAccept(QString label);
    static int dlgAcceptWide(QString label);
    static QString dlgInput(QString title, QString label, QString text = "");
    static QString dlgInputPassword(QString title, QString label, QString text = "");


    static void moveWidget(QWidget *widget, int x, int y);
    static void smoothMoveWidget(QWidget *widget, int x_offset, int y_offset);
    static void resize(QWidget *widget, int width=-1, int height=-1);
//    QVariantMap getSelected(QObject *obj);

    static void selectAllText(QObject *obj);
    static void openComboBox(QObject *obj);

    static void setFocus(QWidget *wgt);
    static void hideWidget(QWidget *wgt);
    static void showWidget(QWidget *wgt);
    QWidget * getParentWidget(QWidget * widget);


    static bool setProperty(QWidget *wgt, QString name, QVariant value);

    QObject * createWidget(QWidget *parent, QString type, int x=0, int y=0, int width=100, int height=100, QString name="");
    void destroyWidget(QWidget *widget);
    QObjectList getChildren(QWidget *wgt, QString datatype="");

    QObjectList getAllChildrens(QObject *obj);
    bool checkObjectContains(QObject *wgt, QObject *obj);

    void scrollToCurrent(QTableWidget *tw);

    void setTabEnabled(QTabWidget *tab, int index, bool state);

    void setPBText(QPushButton *pb, QString text);
    void setLeAlign(QLineEdit *le, QString align);
    void insertLeText(QObject *obj, QString text);
    void LeBackspace(QObject *obj);

    void setHtml(QObject * obj, QString html);
    static void setTWHeader(QTableWidget *table, QString orientation, bool state);
    void setLeCursorPosition(QLineEdit *le, int pos);

//    static void table_highlightRow(QTableWidget *tablewidget, int row, QString color);
    static void table_highlightRowText(QTableWidget *tablewidget, int row, QString color="", bool ifBold=false);
//    static void table_highlightColumn(QTableWidget *tablewidget, int column, QColor color);
//    static void table_highlightCell(QTableWidget *tablewidget, int row, int column, QColor color);
//    static void table_cellTextColor(QTableWidget *tablewidget, int row, int column, QColor color);

    //    void setNoSelection(QObject *obj);
};

#endif // AGUI_H
