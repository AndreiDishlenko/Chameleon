#ifndef AAPPLICATION_H
#define AAPPLICATION_H

#include <QObject>
#include <QMainWindow>
#include <QMap>
#include <QMdiArea>
#include <QSignalMapper>
#include <QSqlDatabase>
#include <QProgressBar>
#include <QScrollArea>

#include "aprinter.h"
#include "adatabase.h"
#include "aengine.h"





class AApplication : public QMainWindow {
    Q_OBJECT
//    Q_PROPERTY(int alpha READ alpha WRITE setAlpha)
private:
    QString defApplication;
    QString currentApplication;

    bool readConfFile();

    int rights;

    int gridx;
    int gridy;
    int gridstep;

    QSignalMapper *windowMapper;
    QAction *actCloseWindow;
    QAction *actCloseAllWindow;
    QAction *actNextWindow;
    QAction *actPrevWindow;

    QList<QDialog*> dialog_objects;

    bool eventFilter(QObject *obj, QEvent *event);

    QWidget *startscreen;

public:
    AApplication();
    ~AApplication();

    /// --- MDI mode ---
    void initMdiMode();
    void showMdiLoginScreen();
    void initOneWidgetMode();

    /// --- Fastfood mode mode ---
    QWidget* prepareStartScreen(QString type);
    //    void showOneWidgetStartScreen();
//    QProgressBar *progressbar;
    void startWidget(QString name);

    void setWidget(QWidget *widget);
    void includeWidget(QWidget *widget);
    void echoWidgetStruct(QObject *widget, int level);
    void echoChld(QWidget *wgt);

    void loadFormData(QWidget *widget);
    QObjectList getSubForms(QObject *obj);    // Получить все вложены формы

    void processMdiMenu(QObject *widget);

    QWidget *currentAppWidget;                // Главный виджет формы
    bool flag_fullscreen=1;

    QMdiArea *mdiArea;
    QProgressBar * progressbar;

    QString css;

    APrinter *defPrinter;
    AEngine *engine;

    QMap<QObject*, QString> openedWidgets;
    QMap<QMdiSubWindow*, QMdiSubWindow*> mdiParents;

    QTimer *timer;


public slots:
    void startApp(QString appname="", QString runmode="");


    static bool createMainDBConnections(QString location="");
    static void closeAllMainDBConnections();


    void showPOSLogin(QString message="");

    // Menu Bar
    void createActions();
    void updateMenus();
    bool execCurrentFormScript(QString script);
    QMap<QString,QAction*> getMenuActions(QMenu *menu);
    void enableMenuActions(QMdiSubWindow *mdiwindow=0);

    // Функции MDI
    QWidget* newMdiWindow(QString uiFileName, QMap<QString, QVariant> paramMap = QMap<QString, QVariant>());
    void setActiveSubWindow(QWidget *widget);
    QWidget* getParentWidget();
    void closeActiveWindow();

    void pauseApp(bool state=true);
    void blockApp();
    void exitApp();
    QVariant dialog(QString uiFileName, QMap<QString, QVariant> paramMap = QMap<QString, QVariant>());

    QString getRelease();
    QString getVersion();

    void shutDown();
    void destroyObject(QObject *obj);
    static void pause(uint msec);

    static QString getGlobalVar(QString name);
    static void echoGlobVars();

    static QVariant getLocalSetting(QString key);           // Получить локальную переменную ( БД SQLITE )
    bool setLocalSetting(QString key, QVariant value);      // Установить локальную переменную ( БД SQLITE )
    static QVariant getRegValue(QString key);               // Получить переменную из реестра БД MS SQL
    void setRegValue(QString key, QVariant data);           // Установить переменную в реестра БД MS SQL


    void changeFocus(QWidget *oldwgt, QWidget *newwgt);
    static void updateWindowTitle(QWidget *wgt);
    void updateMainWindowTitle();
    void mdiModeLogin();

private slots:
    void menuAction();
    void destroyMdiWindow_slot(QMdiSubWindow *current = 0);
    void activateMdiWindow_slot();

signals:
    void focusChanged(QWidget *oldwgt, QWidget *newwgt);

};

#endif // AAPPLICATION_H
