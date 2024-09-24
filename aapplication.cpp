#include "stylehelper.h"

#include "aform.h"

#include "aapplication.h"
#include <QUiLoader>
#include "QMdiArea"
#include <QMdiSubWindow>
// CLASSES declaration
#include "globals.h"
#include "agui.h"
#include "adatabase.h"
#include "adataset.h"
//#include "aform.h"
#include "aengine.h"
#include "ui_uionewidget.h"
#include "ui_uimainwindow.h"
#include "ui_uimainwindow_posadmin.h"
#include "ui_uimainwindow_info.h"
#include "ui_uistartscreen.h"
//#include "ui_uilogin.h"
#include "docs/abuild.h"
#include "secure/asecure.h"
// SELF declaration
#include "QDesktopWidget"

//#include "adialog.h"
#include "QDialog"
#include "QPainter"
#include <afunc.h>
#include <QAbstractItemModel>
#include <QKeyEvent>
#include <QStackedWidget>
#include <QStringList>
#include <QWidget>
#include <QProgressBar>

#include "aservice.h"

AApplication::AApplication() {
    //qDebug() << "AApplication::AApplication()";
    this->setObjectName("AApp");

    currentAppWidget = new QWidget();

    this->setAutoFillBackground(true);
    this->setAttribute(Qt::WA_DeleteOnClose);
    //this->setWindowFlag(Qt::NoDropShadowWindowHint);

    engine = new AEngine(currentAppWidget);
    engine->addEngineBaseObject(this);

    globals["showShadows"]=AApplication::getLocalSetting("showShadows").toString();

    progressbar = new QProgressBar();
    defPrinter = new APrinter(this);

    //currentAppWidget->setWindowIcon(QIcon(":/icons/icons/OneDrive.ico"));
}
AApplication::~AApplication() {
    //qDebug() << "~AApplication()" << globals["defConnection"] << globals["reserveConnection"];
    ADatabase::closeConnection("sqconnection");
    ADatabase::closeConnection(globals["defConnection"]);
    //ADatabase::closeConnection(globals["reserveConnection"]);
    qDebug() << "[] Exit application..";
}

//#include "aform.h"


/////////////////////////////////////////////////////////////////////////////////////////////////
// Инициализация
/////////////////////////////////////////////////////////////////////////////////////////////////
void AApplication::startApp(QString appname, QString runmode) {
    //qDebug() << "[] Initialization GUI " << appname << "  in " << runmode << " mode.";

    this->currentApplication = appname;

    if (appname!="fastfood" && appname!="restaurant" && appname!="posadmin" && appname!="test") {
        qDebug() << "[ERROR] Unknown configuration '"+appname+"'. Exit.";
        this->exitApp();
        return;
    }

    if (runmode=="settings") {  /// Start 'settings' configuration
        if (globals["Lang"]=="RU") {
            globals["globRelease"] = "Настройки - Chameleon`";
        } else if (globals["Lang"]=="EN") {
            globals["globRelease"] = "Settings - Chameleon";
        }
        this->initOneWidgetMode();
        this->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
        this->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
        //this->setWindowFlag(Qt::NoDropShadowWindowHint);
        //this->setWindowFlags(Qt::FramelessWindowHint);

        globals["defLocation"] = this->getLocalSetting("defLocation").toString();
        this->createMainDBConnections();
        this->startWidget("$/general/settings.ui");
        if (appname=="posadmin") {
            QTabWidget *tw = currentAppWidget->findChild<QTabWidget *>("tabWidget");
            QWidget *w = currentAppWidget->findChild<QWidget *>("wFrontoffice");
            if (w && tw) {tw->removeTab(tw->indexOf(w));}
            w = currentAppWidget->findChild<QWidget *>("wHardware");
            if (w && tw) {tw->removeTab(tw->indexOf(w));}
        }
        this->resize(this->centralWidget()->size());
        this->css = AFunc::readFile("$/general/default.css");
        this->setStyleSheet(this->css);
        AGui::startWidgetLocalization(this);
        this->updateMainWindowTitle();
        this->setWindowIcon(QIcon(":/icons/icons/icon_settings.ico"));
        this->show();
        this->installEventFilter(this);
        return;
    }
    if (appname=="posadmin") {  /// Start 'posamin' configuration
        if (globals["Lang"]=="RU") {
            globals["globRelease"] = "Chameleon: Управление розничной торговлей";
        } else if (globals["Lang"]=="EN") {
            globals["globRelease"] = "Chameleon: Retail management";
        }
        globals["progNo"] = "1";
        Ui::uiMainWindow_posadmin ui;
        ui.setupUi(this);
        this->setWindowIcon(QIcon(":/icons/icons/icon.ico"));
        this->resize(QSize(1200,700));
        //this->setWindowState(Qt::WindowMaximized);
        this->initMdiMode();
        this->css = AFunc::readFile("/css/default.css");
        this->setStyleSheet(this->css);
        this->processMdiMenu(this);
        AGui::startWidgetLocalization(this);
        this->updateMainWindowTitle();
        this->show();
        this->mdiModeLogin();
        this->installEventFilter(this);
        return;
    }
    if (appname=="test") {  /// Start 'posamin' configuration
        Ui::uiOneWidget ui;
        ui.setupUi(this);
        this->menuBar()->hide();
        this->statusBar()->hide();
        this->resize(QSize(1024,768));

        this->createMainDBConnections(this->getLocalSetting("defLocation").toString());
        this->css = AFunc::readFile("/css/default.css");
        this->startWidget("/dictPrice.ui");

        AEngine *engine = currentAppWidget->findChild<AEngine*>("qs");
        this->setStyleSheet(this->css);

        this->show();
    }
    if (appname=="fastfood") {  /// Start 'fastfood' configuration
        if (globals["Lang"]=="RU") {
            globals["globRelease"] = "Chameleon: фастфуд касса";
        } else if (globals["Lang"]=="EN") {
            globals["globRelease"] = "Chameleon: fastfood checkbox";
        }
        globals["progNo"] = "2";
        Ui::uiOneWidget ui;
        ui.setupUi(this);
        this->setWindowIcon(QIcon(":/icons/icons/icon_fastfood.ico"));
        this->menuBar()->hide();
        this->statusBar()->hide();
        this->resize(QSize(1024,768));

        startscreen = this->prepareStartScreen("automation");            /// Show start screen bootloader..
        //startscreen->setWindowFlag( Qt::WindowStaysOnTopHint );
        if (this->flag_fullscreen)
            startscreen->showFullScreen();
        else
            startscreen->show();

        QCoreApplication::processEvents();

        progressbar->setValue(5);
        QCoreApplication::processEvents();
        this->createMainDBConnections(this->getLocalSetting("defLocation").toString());
        progressbar->setValue(30);
        QCoreApplication::processEvents();
        this->css = AFunc::readFile("/css/pos.css");
        this->startWidget("/fastfood.ui");

        AEngine *engine = currentAppWidget->findChild<AEngine*>("qs");
        this->setStyleSheet(this->css);
        progressbar->setValue(90);
        QCoreApplication::processEvents();

        this->setWindowFlag(Qt::WindowStaysOnTopHint, false);
        //this->setWindowState(Qt::WindowMaximized);

        if (this->flag_fullscreen)
            this->showFullScreen();
        else
            this->show();

        QCoreApplication::processEvents();

        // Finally resizing scalable elements (works only after ->show()
        float scale = static_cast<float>(currentAppWidget->width()) / static_cast<float>(1920);
        AForm *form = currentAppWidget->findChild<AForm *>("Form");
        form->scaleSubObjects(scale);

        startscreen->close();
        QCoreApplication::processEvents();
        engine->exec("startApplication();");

        this->installEventFilter(this);
        return;
    }
    //if (appname=="restaurant") {globals["progNo"] = "3";}
    //if (appname=="infobox") {globals["progNo"] = "5";}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// БД
/////////////////////////////////////////////////////////////////////////////////////////////////
bool AApplication::createMainDBConnections(QString location) {
    //qDebug() << "AApplication::createMainDBConnections()" << globals["defConnection"] << globals["reserveConnection"] << globals["defLocation"];

    if (location=="") {location = globals["defLocation"];}
    if (location=="" || location=="0") {
        qDebug() << "[Error] AApplication::createMainDBConnections" << "Active connection is not defined";
        return false;
    }
    //qDebug() << "AApplication::createMainDBConnections()" << location;

    QList<QMap<QString, QVariant> > resultMap = ADatabase::selectList("SELECT * FROM dictLocations WHERE id='"+location+"'", "sqconnection");
    if (resultMap.count()==0) {
        qDebug() << "[Error] AApplication::createMainDBConnections" << "Active connection is not found";
        return false;
    }

    QMap<QString, QVariant> result = resultMap.at(0);
    globals["dbServer"] = result["dbServer"].toString();
    globals["dbType"] = result["dbType"].toString();
    globals["dbServerAlias"] = result["dbServerAlias"].toString();
    globals["dbServerPort"] = result["dbServerPort"].toString();
    globals["dbName"] = result["dbName"].toString();
    globals["dbUser"] = result["dbUser"].toString();
    globals["dbPassword"] = result["dbPassword"].toString();

    if (globals["defConnection"]!="") {ADatabase::closeConnection(globals["defConnection"]);}
    if (globals["reserveConnection"]!="") {ADatabase::closeConnection(globals["defConnection"]);}

    globals["defConnection"] = ADatabase::createConnection(globals["dbType"]);
    //globals["reserveConnection"] = ADatabase::createConnection(globals["dbType"]);
    //qDebug() << "AApplication::createMainDBConnections()" << globals["defConnection"];

    //    qDebug() << "AApplication::createMainDBConnections() 2";
    if (globals["defConnection"]=="") {
        qDebug() << "[Error] AApplication::createMainDBConnections can't create default connection";
        return false;
    }
    globals["activeLocation"] = location;
    return true;
}
void AApplication::closeAllMainDBConnections() {
    //qDebug() << "AApplication::closeMainDBConnections()" << globals["defConnection"] << globals["reserveConnection"];

    //ADatabase::closeConnection(globals["reserveConnection"]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MDI Mode initialization functions
/////////////////////////////////////////////////////////////////////////////////////////////////
void AApplication::initMdiMode() {
    this->centralWidget()->setObjectName("central widget");

    mdiArea = new QMdiArea(this);
    mdiArea->setObjectName("mdiArea");
    mdiArea->setActivationOrder(QMdiArea::StackingOrder);
    mdiArea->setViewMode(QMdiArea::SubWindowView);
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->verticalScrollBar()->setStyle(QStyleFactory::create("Fusion"));
    mdiArea->horizontalScrollBar()->setStyle(QStyleFactory::create("Fusion"));
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
    //mdiArea->setObjectName("mdi area");

    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));

    this->createActions();

    gridx=0;
    gridy=0;
    gridstep = 30;

    this->menuBar()->show();
    this->statusBar()->setObjectName("statusbar");
    this->statusBar()->show();
    this->setCentralWidget(mdiArea);
    this->updateMenus();

    progressbar->~QProgressBar();
    progressbar = new QProgressBar(this);
    progressbar->setRange(0,100);
    progressbar->setObjectName("progressbar");
    progressbar->setStyleSheet("QProgressBar {max-width:100px;}");
    progressbar->setVisible(false);

    this->statusBar()->addWidget(progressbar);
}
void AApplication::initOneWidgetMode() {
    //    QMenuBar *menuBar = this->findChild<QMenuBar*>("menubar");
    //    QStatusBar *statusBar = this->findChild<QStatusBar*>("statusbar");
}
void AApplication::showMdiLoginScreen() {
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// One Widget Mode initialization functions
/////////////////////////////////////////////////////////////////////////////////////////////////
QWidget* AApplication::prepareStartScreen(QString type) {
    //    qDebug() << "AApplication::showStartScreen" << type;
    QString name = "$/general/startscreen.ui";
    QWidget *widget = AGui::openUi(name);

    if (!widget) {
        qDebug() << "[ERROR] Can't open start widget '"+name+"'.";
        this->exitApp();
    }
    this->setWidget(widget);
    QPushButton *pbt = widget->findChild<QPushButton *>("pButton");
    if (type=="automation") {
        pbt->setIcon(QIcon(":/images/images/logo_automation.png"));
    } else if (type=="infobox") {
        pbt->setIcon(QIcon(":/images/images/logo_infobox.png"));
    } else {
        pbt->setIcon(QIcon(":/images/images/logo.png"));
    }

    //QDialog *dlg = static_cast<QDialog*>(widget);

    widget->setAttribute(Qt::WA_DeleteOnClose);
    widget->setWindowFlag(Qt::FramelessWindowHint);
    widget->setWindowFlag(Qt::BypassWindowManagerHint);
    //widget->setWindowFlag( Qt::X11BypassWindowManagerHint );
    widget->setStyle(QStyleFactory::create("Fusion"));

    progressbar->~QProgressBar();
    progressbar = widget->findChild<QProgressBar*>("progressBar");
    QPalette p = progressbar->palette();
    p.setColor(QPalette::Highlight, QColor(33,66,99));
    progressbar->setPalette(p);

    return widget;
}

void AApplication::startWidget(QString name) {
    //qDebug() << "AApplication::startWidget" << name;
    currentAppWidget = AGui::openUi(name);
    if (!currentAppWidget) {
        qDebug() << "[ERROR] Can't open start widget '"+name+"'.";
        this->exitApp();
    }
    currentAppWidget->setWindowFlag(Qt::FramelessWindowHint);
    currentAppWidget->setStyle(QStyleFactory::create("Fusion"));
    progressbar->setValue(50);
    QCoreApplication::processEvents();
    this->setWidget(currentAppWidget);
    progressbar->setValue(80);
    QCoreApplication::processEvents();
    this->setCentralWidget(currentAppWidget);
    return;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// POS configuration functions
/////////////////////////////////////////////////////////////////////////////////////////////////
void AApplication::showPOSLogin(QString message) { //return idUser
    //qDebug() << "AApplication::showPOSLogin" << message;
    QDialog *dlg = static_cast<QDialog*>(AGui::openUi("/pos/poslogin.ui"));
    dlg->setWindowFlag( Qt::WindowStaysOnTopHint);
    dlg->setAttribute(Qt::WA_DeleteOnClose);;
    dlg->setWindowFlags(Qt::FramelessWindowHint);
    dlg->resize(this->size());
    dlg->move(0, 0);
    this->setWidget(dlg);
    QLineEdit *le = dlg->findChild<QLineEdit*>("lineEdit");
    le->setFocus(Qt::TabFocusReason);
    dlg->exec();

    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

}

void AApplication::processMdiMenu(QObject *widget) {
    QMenuBar *menuBar = widget->findChild<QMenuBar*>("menubar");
    menuBar->setFixedHeight(24);
    if (menuBar==0) {return;}
    for (int i=0;i<menuBar->children().count();i++){
        QObject *object = menuBar->children().at(i);
        QString objectName = object->objectName();
        QString objectClassName = object->metaObject()->className();
        if (objectClassName=="QMenu" && objectName!="") {
            QMenu *menu = menuBar->findChild<QMenu*>(menuBar->children().at(i)->objectName());
            menu->setWindowFlag(Qt::NoDropShadowWindowHint);
//            menu->setSizePolicy();
            //AGui::setObjectLocalization(menu);
            for (int i1=0;i1<menu->actions().count();i1++) {
                QAction *act = menu->actions().at(i1);

                AGui::setObjectLocalization(act);
                if (!act->property("System").toBool() && act->objectName()!="") {
                    QString ui_file = act->property("ui_file").toString();
                    //qDebug() << "AApplication::processMdiMenu" << act->property("System").toBool() << act->objectName() << AFunc::fileExists(AFunc::unionFolderNames(globals["configurationPath"], ui_file));
                    if (!AFunc::fileExists(AFunc::unionFolderNames(globals["configurationPath"], ui_file)) || ui_file=="" ) {
                        act->setVisible(false);
                    }
                }
                if (act->property("Hidden").toBool()) {act->setVisible(false);}
                QObject::connect(act,SIGNAL(triggered()),this,SLOT(menuAction()));
            }
        }
    }
}

void AApplication::pauseApp(bool state) {
    if (state) {this->setEnabled(false);}
    else {this->setEnabled(true);}
}
void AApplication::blockApp() {
    return this->showPOSLogin();
}
void AApplication::exitApp() {
    this->~AApplication();
    exit(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Menu Bar
/////////////////////////////////////////////////////////////////////////////////////////////////
void AApplication::createActions() {
    actCloseWindow = new QAction(QIcon(":/images/new.png"), tr("&Закрыть окно"), this);
//    actCloseWindow->setShortcuts(QKeySequence::Close);
    actCloseWindow->setStatusTip(tr("Закрыть текущее окно"));
    connect(actCloseWindow, SIGNAL(triggered()), mdiArea, SLOT(closeActiveSubWindow()));

    actCloseAllWindow = new QAction(QIcon(":/images/new.png"), tr("&Закрыть все окна"), this);
//    actCloseAllWindow->setShortcuts(QKeySequence::Close);
    actCloseAllWindow->setStatusTip(tr("Закрыть все окна"));
    connect(actCloseAllWindow, SIGNAL(triggered()), mdiArea, SLOT(closeAllSubWindows()));

    actNextWindow = new QAction(QIcon(":/images/new.png"), tr("&Следующее окно"), this);
    actNextWindow->setShortcuts(QKeySequence::NextChild);
    actNextWindow->setStatusTip(tr("Переключиться на следующее окно"));
    connect(actNextWindow, SIGNAL(triggered()), mdiArea, SLOT(activateNextSubWindow()));

    actPrevWindow = new QAction(QIcon(":/images/new.png"), tr("&Предыдущее окно"), this);
    actPrevWindow->setShortcuts(QKeySequence::PreviousChild);
    actPrevWindow->setStatusTip(tr("Переключиться на предыдущее окно"));
    connect(actPrevWindow, SIGNAL(triggered()), mdiArea, SLOT(activatePreviousSubWindow()));

}
void AApplication::updateMenus() {
    QMenu *menuView = this->menuBar()->findChild<QMenu *>("menuWindow");
    menuView->clear();
    menuView->addAction(actCloseWindow);
    menuView->addAction(actCloseAllWindow);
    menuView->addSeparator();
    menuView->addAction(actNextWindow);
    menuView->addAction(actPrevWindow);
    bool hasMdiChild = (mdiArea->subWindowList().size() != 0);
    if (hasMdiChild) {
        menuView->addSeparator();
        QMdiSubWindow *currentWindow = mdiArea->currentSubWindow();
        QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
        //qDebug() << "sub windows: " << windows;
        for (int i = 0; i < windows.size(); ++i) {
            QString text = windows.at(i)->windowTitle();
            QAction *action  = menuView->addAction(text);
            action->setCheckable(true);
            action->setChecked(currentWindow->windowTitle() == text);
            connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
            windowMapper->setMapping(action, windows.at(i));
        }
    }
}
void AApplication::menuAction() {
    QString uiFileName = QObject::sender()->property("ui_file").toString();
    //qDebug() << "AApplication::menuAction" << uiFileName;
    if (uiFileName!="") {
        // READ MENU PARAMS
        QStringList paramList;
        paramList << "docType"
            << "dict"
            << "mdiMenuName"
            << "report_file"
            << "filter";
        QMap<QString, QVariant> paramMap;
        for (int i=0; i<paramList.count();i++) {
            //qDebug() << "AApplication::menuAction" << i << QObject::sender()->property(paramList.at(i).toLocal8Bit().data()).toString();
            if (paramList.at(i)=="mdiMenuName") {
                paramMap[paramList.at(i)] = QObject::sender()->property("text").toString();
            } else {
                paramMap[paramList.at(i)] = QObject::sender()->property(paramList.at(i).toLocal8Bit().data()).toString();
            }
        }
        this->newMdiWindow(uiFileName, paramMap);
        //qDebug() << "AApplication::menuAction" << paramMap;
    }
    QString execFormScript = QObject::sender()->property("execFormScript").toString();
    QString onClick = QObject::sender()->property("onClick").toString();

    if (execFormScript!="") {
        //engine->exec(execFormScript, "AApplication::menuAction");
        AForm *form = mdiArea->activeSubWindow()->widget()->findChild<AForm*>("Form");
        form->script(execFormScript);
        //qDebug() <<  << mdiArea->activeSubWindow()->widget()->objectName() << engine << engine->parent()->objectName() << QObject::sender() << QObject::sender()->objectName();
    }
    if (onClick!="") engine->exec(onClick, "AApplication::menuAction");

    //QScriptValue result = engine->evaluate(onClick);
    //if (result.isError()) qDebug() << "Script error:" << result.toString();
    //if (execFormScript!="") {
    //        if (!this->execCurrentFormScript(execFormScript)) {AGui::msgBox("AApplication::menuAction() ошибка в выполнении скрипта формы.");}
    //    }
}

bool AApplication::execCurrentFormScript(QString script) {
    bool result = false;
    QMdiSubWindow *current = mdiArea->currentSubWindow();
    if (current) {
        AEngine *engine = mdiArea->currentSubWindow()->widget()->findChild<AEngine*>("qs");
        result = engine->exec(script);
    }
    return result;
}
QMap<QString,QAction*> AApplication::getMenuActions(QMenu *menu) {
    //qDebug() << "AApplication::getMenuActions" << menu->actions().count();
    QMap<QString,QAction*> result;
    for (int i=0;i<menu->actions().count();i++) {
        result[menu->actions().at(i)->objectName()] = menu->actions().at(i);
    }
    return result;
}
void AApplication::enableMenuActions(QMdiSubWindow *mdiwindow) {
    //qDebug() << "AApplication::enableMenuActions" << mdiwindow->objectName() << mdiwindow->metaObject()->className();
    QMenu *menuActions = this->menuBar()->findChild<QMenu *>("menuActions");
    if (menuActions) {
        QMap<QString,QAction*> menuMap = this->getMenuActions(menuActions);
        QAction * print = menuMap["actionPrint"];
        QAction * printPreview = menuMap["actionPrintPreview"];
        QAction * exportExcel = menuMap["actionExportExcel"];
        QAction * exportHtml = menuMap["actionExportHTML"];

        QString externalHtmlForm;
        if (mdiwindow!=0) {externalHtmlForm = mdiwindow->widget()->property("externalHtmlForm").toString();}
        //qDebug() << "AApplication::enableMenuActions" << mdiwindow->widget()->objectName() << externalHtmlForm << QDir::currentPath()+gvUiPath+externalHtmlForm << AFunc::fileExists(QDir::currentPath()+gvUiPath+externalHtmlForm);
        if (externalHtmlForm!="") {
            print->setEnabled(true);
            printPreview->setEnabled(true);
            exportExcel->setEnabled(true);
            exportHtml->setEnabled(true);
        } else {
            print->setEnabled(false);
            printPreview->setEnabled(false);
            exportExcel->setEnabled(false);
            exportHtml->setEnabled(false);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Функции MDI
/////////////////////////////////////////////////////////////////////////////////////////////////
QWidget* AApplication::newMdiWindow(QString uiFileName, QMap<QString, QVariant> paramMap) {
    //qDebug() << "AApplication::newMdiWindow begin" << uiFileName << paramMap;
    //AFunc::echoTimer("** Begin open timer ("+uiFileName+").");
    QWidget *widget = AGui::openUi(uiFileName); // normal timer = 70
    if (widget==0) {return 0;}

    widget->setAttribute(Qt::WA_DeleteOnClose);
    widget->setProperty("IconFile", "/icons/mdiwindow.ico");

    /// Перенос передаваемых параметров главного меню на Widget
    for (int i=0;i<paramMap.count();i++) {
        AGui::setProperty(widget, paramMap.keys().at(i), paramMap.values().at(i));
    }
    /// Множественное открытие одного окна
    bool multipleOpen = widget->property("multipleOpen").toBool();
    bool notopen = false;
    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    for (int i=0;i<windows.size();i++) {
        if (windows.at(i)->widget()->objectName() == widget->objectName() && multipleOpen == false) {notopen=true; }
    }
    if (!notopen) {
        QMdiSubWindow *current = mdiArea->currentSubWindow();        
        /// Флаги окна
        Qt::WindowFlags flags;
        flags = Qt::WindowCloseButtonHint|Qt::WindowMinimizeButtonHint|Qt::WindowSystemMenuHint;
        if (widget->property("ifCanMaximize").toBool()) {
            flags |= Qt::WindowMaximizeButtonHint;
        }
        /// Открываем окно MDI
        QMdiSubWindow *mdiwindow = mdiArea->addSubWindow(widget, flags);       
        mdiwindow->setAttribute(Qt::WA_DeleteOnClose);
        mdiwindow->setStyle(QStyleFactory::create("Fusion"));
        QPalette p = qApp->palette();
        p.setColor(QPalette::Highlight, QColor(51,102,153));
        mdiwindow->setPalette(p);
        //mdiwindow->setAttribute(Qt::WA_TranslucentBackground);            // Делаем фон главного виджета прозрачным
        //mdiwindow->setOption(QMdiSubWindow::RubberBandResize);
        /// Дополнение дерева наследования окон в экземпляре класса, для того, чтобы была возможность определить родителя окна
        if (current) {
            mdiParents[mdiwindow]=current;
        } else {
            mdiParents[mdiwindow]=0;
        }
        /// Системное имя окна
        int randomnumber = QDateTime::currentDateTime().time().minute()*QDateTime::currentDateTime().time().second() * QDateTime::currentDateTime().time().msec();
        mdiwindow->setObjectName(QString::number(randomnumber));
        /// Размер окна
        mdiwindow->resize(widget->width()+14,widget->height()+37);
        //mdiwindow->setMinimumSize(widget->width()+14,widget->height()+37);
        //mdiwindow->setMaximumSize(widget->width()+14,widget->height()+37);
        /// Положение окна
        if (gridx*gridstep+widget->width()>this->width()) {gridx=0;}
        if (gridy*gridstep+widget->width()>this->height()-40) {gridy=0;}
        mdiwindow->move(QPoint(gridx*gridstep, gridy*gridstep));
        gridx++;
        gridy++;
        /// Заголовок окна
        AApplication::updateWindowTitle(mdiwindow);
        /// Иконка окна, параметр IconFile виджета
        if (widget->property("IconFile").toString()!="") {
            mdiwindow->setWindowIcon(QIcon(AFunc::unionFolderNames(globals["configurationPath"], widget->property("IconFile").toString())));
        } else {
            mdiwindow->setWindowIcon(QIcon(":/icons/icons/icon_empty.ico"));
        }
        /// Обработка TitleBar - НЕ РЕАЛИЗОВАНА
        //QString style = "QMdiSubWindow { border:1px solid red; border-radius:0px; }";
        //QStyleOptionTitleBar qs = mdiwindow
        //mdiwindow->setWindowFlags(Qt::FramelessWindowHint);           // Отключаем оформление окна
        //mdiwindow->setStyleSheet(style);                              // Устанавливаем стиль виджета
        //mdiwindow->show

        /// СОЗДАНИЕ ФОРМЫ, коннекты
        this->setWidget(widget);
        QObject::connect(mdiwindow,SIGNAL(destroyed()),this,SLOT(destroyMdiWindow_slot()));
        QObject::connect(mdiwindow,SIGNAL(aboutToActivate()),this,SLOT(activateMdiWindow_slot()));
        //this->enableMenuActions(mdiwindow);
        mdiwindow->show();                                              // normal timer = 40
        return widget;
    }
    return 0;
}
QWidget* AApplication::getParentWidget() {
//    qDebug() << "AApplication::getParentWidget";
    QMdiSubWindow *current = mdiArea->currentSubWindow();
    if (!current) {return 0;}
    QMdiSubWindow *parent = mdiParents[current];
//    qDebug() << parent << parent->objectName();
    if (parent==0) {return 0;}
    if (!mdiArea->subWindowList().contains(parent)) {return 0;}
    return parent->widget();
}

void     AApplication::activateMdiWindow_slot() {
    QMdiSubWindow *current = static_cast<QMdiSubWindow*>(QObject::sender());
    this->enableMenuActions(current);   // Menu actions activation depends of mdi window action settings presets (enable/disable)
}
void     AApplication::destroyMdiWindow_slot(QMdiSubWindow *mdiWindow) {
    //qDebug() << "AApplication::destroyMdiWindow_slot";
    if (mdiWindow==0) {
        mdiWindow = static_cast<QMdiSubWindow*>(QObject::sender());
    }
    mdiParents.remove(mdiWindow);
    if (mdiArea->currentSubWindow()!=0)
        this->enableMenuActions(mdiArea->currentSubWindow());
    QObject::disconnect(mdiWindow, 0, 0, 0);
}
void     AApplication::closeActiveWindow() {
    this->destroyMdiWindow_slot(mdiArea->currentSubWindow());
    mdiArea->closeActiveSubWindow();
}
void     AApplication::setActiveSubWindow(QWidget *window) {
    if (!window) return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Функции формы
/////////////////////////////////////////////////////////////////////////////////////////////////
void AApplication::setWidget(QWidget *widget) {
    // AFunc::echoTimer("--- AApp:setWidget(begin) " + widget->objectName());
    //qDebug() << "AApplication::setWidget" << widget;

    /// Загрузка из файлов вложенных виджетов указанных через директиву [includeWidget]
    this->includeWidget(widget);
    QStatusBar *statusbar = this->statusBar();
    QProgressBar *progressbar = this->statusBar()->findChild<QProgressBar*>("progressbar");

    /// Создание списка вложенных виджетов, которые являются формами данных. Основная форма включется в первую очередь.
    QObjectList objlist_forms = AForm::getSubForms(widget);
    objlist_forms.prepend(widget);

    for (int i=0;i<objlist_forms.count();i++) {
        //qDebug() << "AApplication::set sub_Widget" << objlist_forms.at(i);
        QObject *objtemp = objlist_forms.at(i);
        QWidget *wgt = static_cast<QWidget *>(objtemp);
        /// создание экземпляра формы для каждой формы данных
        AForm *form = new AForm(wgt);
        /// ФУнкционал связанный с основной темой (документооборот, системные функции)
        ABuild *abuild = new ABuild(form);
        abuild->progressbar = progressbar;
        /// Создание скриптового движка
        AEngine *engine = form->engine;
        engine->addEngineBaseObject(this);          // [AApp]
        engine->addEngineBaseObject(widget);        //
        engine->addEngineBaseObject(form);          // [Form]
        engine->addEngineBaseObject(defPrinter);    // APrinter
        engine->addEngineBaseObject(abuild);
        // defPrinter->setDefaultPrinter();
        if (statusbar) {engine->addEngineBaseObject(statusbar);}
        if (progressbar) {engine->addEngineBaseObject(progressbar);}
    }

    // AFunc::echoTimer("AApp:setWidget(start execution)");

    /// Выполнение скриптов каждой формы в отдельности, создание коннектов
    for (int i=objlist_forms.count()-1;i>=0;i--) {
        QObject *objtemp = objlist_forms.at(i);
        AForm *form = objtemp->findChild<AForm *>("Form");
        float scale = static_cast<float>(currentAppWidget->width()) / static_cast<float>(1920);
        form->scaleSubObjects(scale);
        AEngine *engine = form->engine;
        QString ptescript = form->getPteScripts();
        engine->exec(ptescript);
        form->createObjectsConnects();


    }


    //AGui::setObjectLocalization(widget);
    //qDebug() << "AApp:setWidget(end)";
}


/// Подключение вложенных форм виджета, указанных в параметре [includeWidget], рекрсивным методом
void AApplication::includeWidget(QWidget *widget) {
    //qDebug() << "AApplication::includeWidget" << widget << widget->objectName();
    QGridLayout *layout=0;

    QObjectList objlist_widgets = AForm::getSubWidgets(widget);
    for (int i=0;i<objlist_widgets.count();i++) {

        QObject *objtemp = objlist_widgets.at(i);
        QString include = ADataset::updateStringVariablesFromWidget(objtemp->property("includeWidget").toString(), objtemp);

        if (include!="") {
            // Opening include widget
            QWidget *incwgt = AGui::openUi(include);
            if (incwgt!=0) {
                QWidget *source_wgt = static_cast<QWidget *>(objtemp);
//                incwgt->setStyleSheet(source_wgt->styleSheet());
                QString name = source_wgt->objectName();
                QSize size = source_wgt->size();
                QPoint pos = source_wgt->pos();
                QString parentClassName = source_wgt->parentWidget()->metaObject()->className();

                QList<QGridLayout *> layout_list = source_wgt->parentWidget()->findChildren<QGridLayout *>();
                // Find layout of current Widget
                for (int i=0; i<layout_list.count(); i++) {
                    if (layout_list.at(i)->indexOf(source_wgt)!=-1) {
                        layout = layout_list.at(i);
                    }
                }

                if (parentClassName=="QStackedWidget") {
                    //qDebug() << "AApplication::includeWidget QStackedWidget" << source_wgt << include;
                    QStackedWidget *sw = static_cast<QStackedWidget*>(source_wgt->parentWidget());
                    int index = sw->indexOf(source_wgt);
                    sw->removeWidget(source_wgt);
                    sw->insertWidget(index, incwgt);
                    sw->setCurrentWidget(incwgt);
                    incwgt->move(QPoint(pos.x()+1, pos.y()+1));
                    incwgt->resize(QSize(size.width()-1, size.height()-1));
                } else if (layout!=0) {
                    //qDebug() << "AApplication::includeWidget QLayout" << source_wgt << layout << include;
                    QWidget *parent = static_cast<QWidget*>(source_wgt->parentWidget());
                    incwgt->setParent(parent);
                    incwgt->setObjectName(name);
                    int index = layout->indexOf(source_wgt);
                    if (index!=-1) {
                        int row, col, rowspan, colspan;
                        layout->getItemPosition(index, &row, &col, &rowspan, &colspan);
                        //qDebug() << row << col << rowspan << colspan << size << pos;
                        layout->removeWidget(source_wgt);
                        layout->addWidget(incwgt, row, col, rowspan, colspan);
                        layout->update();
                    }
                    layout=0;
                    source_wgt->~QWidget();
                } else {
                    //qDebug() << "AApplication::includeWidget QWidget";
                    QWidget *parent = static_cast<QWidget*>(source_wgt->parentWidget());
                    incwgt->setParent(parent);
                    incwgt->setObjectName(name);
                    incwgt->update();
                    incwgt->resize(size);
                    incwgt->move(pos);
                    source_wgt->~QWidget();
                }
                incwgt->show();
                objlist_widgets.replace(i, incwgt);
                this->includeWidget(incwgt);
            }
        }
    }
}
void AApplication::echoWidgetStruct(QObject *obj, int level) {
    QList<QString> formtypeslist; formtypeslist << "dataform" << "tablewidget" << "treeview" << "map";
    QString tab="", ifform="";
    for (int i=0;i<level;i++) {tab.append("  ");}
    QObjectList objlist;
    QObjectList objlisttemp = obj->children();
    QString objClassName = obj->metaObject()->className();
    // Добавление к списку
    for (int i=0;i<objlisttemp.count();i++) {
        QObject *objtemp = objlisttemp.at(i);
        objClassName = objtemp->metaObject()->className();
        if (objClassName=="QWidget") {
            if (formtypeslist.contains(objtemp->property("FormType").toString())) {ifform="*";}
            qDebug() << tab+ifform+objtemp->objectName();
            AApplication::echoWidgetStruct(objtemp, level+1);
        }
        if (objClassName == "QTabWidget") {
            QTabWidget *qtab = static_cast<QTabWidget*>(objtemp);
            for (int i=0;i<qtab->count();i++) {
                QWidget *wgt = qtab->widget(i);
                if (formtypeslist.contains(wgt->property("FormType").toString())) {ifform="*";}
                qDebug() << tab+ifform+wgt->objectName();
                AApplication::echoWidgetStruct(wgt, level+1);
            }
        }
        if (objClassName == "QStackedWidget") {
            QStackedWidget *sw = static_cast<QStackedWidget*>(objtemp);
            for (int i=0;i<sw->count();i++) {
                QWidget *wgt = sw->widget(i);
                if (formtypeslist.contains(wgt->property("FormType").toString())) {ifform="*";}
                qDebug() << tab+ifform+wgt->objectName();
                AApplication::echoWidgetStruct(wgt, level+1);
            }
        }
        ifform="";
    }

}
void AApplication::echoChld(QWidget *wgt){
    qDebug() << wgt->children();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// -- Остальное
/////////////////////////////////////////////////////////////////////////////////////////////////
QVariant AApplication::dialog(QString uiFileName, QMap<QString, QVariant> paramMap) {
    //qDebug() << "AApplication::dialog" << uiFileName;
    QVariant result;
    QWidget *wgt = AGui::openUi(uiFileName);  
    wgt->setParent(this);

    if (wgt!=0) {
        QDialog *dlg = static_cast<QDialog*>(wgt);
        bool WA_DeleteOnClose = wgt->property("WA_DeleteOnClose").toBool();
        if (WA_DeleteOnClose) {wgt->setAttribute(Qt::WA_DeleteOnClose);}

        /// Transparent background
        bool transparentBackground = wgt->property("transparentBackground").toBool();
        if (transparentBackground) {
            dlg->setAttribute(Qt::WA_TranslucentBackground,true);
        } else {
            dlg->setAttribute(Qt::WA_TranslucentBackground,false);
        }
        /// Borderless dialog
        bool borderless = wgt->property("borderless").toBool();
        if (borderless) {
            dlg->setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint);
        } else {
            dlg->setWindowFlags( Qt::Dialog );
        }

        /// Fullscreen dialog
        bool fullscreen = wgt->property("fullscreen").toBool();

        if (fullscreen) {
            //qDebug() << "AApp:dialog size" << this->size();
            dlg->resize(this->size());
            //dlg->move(this->pos());
            dlg->move(0,0);
        } else {
            dlg->move(this->pos().x() + (this->width() - dlg->width()) / 2, this->pos().y() + (this->height() - dlg->height()) / 2);
        }
        //qDebug() << "AApplication::dialog :" << currentAppWidget->pos().x() << this->width() << dlg->width();


        for (int i=0;i<paramMap.count();i++) {
            AGui::setProperty(dlg, paramMap.keys().at(i), paramMap.values().at(i));
        }
        dlg->setProperty("mainwidget", true);
        this->setWidget(dlg);
        //dlg->setStyleSheet(css);

        int temp = dlg->exec();
        if (temp!=0 && temp!=-1 && WA_DeleteOnClose==false) {
            QString doneText = dlg->property("doneText").toString();
            //qDebug() << "AApplication::dialog" << doneText;
            if (doneText!="") {
                result = doneText;
            } else {
                result = temp;
            }
            //qDebug() << "AApplication::dialog" << result;
            wgt->~QWidget();
        } else {
            result=temp;
        }

    } else {
        AGui::msgBox("[Error] AApplication::dialog не найден форма "+uiFileName);
        result=-1;
    }

    return result;
}

void    AApplication::shutDown() {
    QProcess::startDetached("shutdown -s -f -t 00");
}
void AApplication::destroyObject(QObject *obj) {
    obj->~QObject();
}

QString AApplication::getRelease () {
    return globals["globRelease"];
}
QString AApplication::getVersion () {
    return globals["coreVersion"];
}

void AApplication::pause(uint msec) {
    QTime time;
    time.start();
    for(int i=0;time.elapsed() < msec;){}
}

QString AApplication::getGlobalVar(QString name) {
    return globals[name];
}
void AApplication::echoGlobVars() {
    qDebug() << "AApplication::echoGlobVars()" << globals;
}

///////////////////////////////////////////////
//// ЛОКАЛЬНЫЕ ПЕРЕМЕННЫЕ И ЗНАЧЕНИЯ РЕЕСТРА
///////////////////////////////////////////////
QVariant AApplication::getLocalSetting(QString key) {
    //qDebug() << "getLocalSetting" << key;
    QVariant var="";
    var = ADatabase::DLookUp("value", "local_settings", "key='"+key+"'", "sqconnection", "AApp:getLocalSetting("+key+")");
    //qDebug() << "AApplication::getLocalSetting1" << var;
    if (var.toString()=="") {
        //qDebug() << "AApplication::getLocalSetting1" << var << key << globals[key];
        //qDebug() << "getLocalSetting 1" << globals[key];
        return globals[key];
    }
    else {
        //qDebug() << "AApplication::getLocalSetting2" << var << key << var;
        //qDebug() << "getLocalSetting 2" << var;
        return var;
    }
}
bool AApplication::setLocalSetting(QString key, QVariant value) {
    bool result;
    //qDebug() << "AApplication::setLocalSetting" << key << value;
    QVariant id = ADatabase::DLookUp("id", "local_settings", "key='"+key+"'", "sqconnection", "AApp:setLocalSetting");
    if (id.toString()!="") {
        result = ADatabase::updateParam("local_settings", "value='"+value.toString()+"'", "key='"+key+"'", "sqconnection");
    } else {
        result = ADatabase::insertParam("local_settings", "key, value", "'"+key+"', '"+value.toString()+"'", "sqconnection");
    }
    //QVariant id = sqlite_db->DLookUp("id", "local_settings", "key='"+key+"'", "sqconnection");
    //if (id.toString()!="") {
        //result = sqlite_db->updateParam("local_settings", "value='"+value.toString()+"'", "key='"+key+"'", "sqconnection");
    //} else {
        //result = sqlite_db->insertParam("local_settings", "key, value", "'"+key+"', '"+value.toString()+"'", "sqconnection");
    //}
    globals[key] = value.toString();
    return result;
}

QVariant AApplication::getRegValue(QString key) {
    //qDebug() << "AApplication::getRegValue" << key;
    QVariant result="";
    QList<QMap<QString, QVariant> > resultList = ADatabase::selectList("SELECT value FROM reg WHERE K='"+key+"'");
    if (resultList.count()>0) {result = resultList.at(0)["value"];}
    //qDebug() << result;
    return result;
}
void AApplication::setRegValue(QString key, QVariant data) {
    //    qDebug() << "AApplication::setRegValue" << key << data;
    QList<QMap<QString, QVariant> > resultList = ADatabase::selectList("SELECT value FROM reg WHERE K='"+key+"'");
    if (resultList.count()>0)
        ADatabase::updateParam("reg", "value='"+data.toString().replace("'","''")+"'", "K='"+key+"'");
    else
        ADatabase::insertParam("reg", "K, value", "'"+key+"', '"+data.toString().replace("'","''")+"'");
}

////////////////////////////////////////////////////////////////

void AApplication::changeFocus(QWidget *oldwgt, QWidget *newwgt) {
    emit focusChanged(oldwgt, newwgt);
}
void AApplication::updateMainWindowTitle() {
    //qDebug() << "AApplication::updateMainWindowTitle()" << globals["activeLocationName"] << globals["globRelease"] << globals["coreVersion"] << globals["activeLocationName"];
    QString temp = globals["activeLocationName"];
    if (temp!="") {temp = temp + " - ";}
    this->setWindowTitle(temp + globals["globRelease"] + " v."+globals["coreVersion"]);
}
void AApplication::updateWindowTitle(QWidget *wgt) {
    //qDebug() << "AApplication::updateWindowTitle()" << globals["activeLocationName"] << globals["globRelease"] << globals["coreVersion"] << globals["activeLocationName"];
    if (globals["Lang"]=="RU" && wgt->property("Ru").toString()!="") {
        wgt->setWindowTitle(wgt->property("Ru").toString());
    } else if (globals["Lang"]=="EN" && wgt->property("En").toString()!="") {
        wgt->setWindowTitle(wgt->property("En").toString());
    }
}

// LOGIN into POSADMIN configuration
void AApplication::mdiModeLogin() {
    //qDebug() << "AApp:mdiModeLogin" << globals["configurationFolder"]+"/login.ui";
    int result = this->dialog("login.ui").toInt();
    if (result == -1) { AApplication::exitApp(); }
}


bool AApplication::eventFilter(QObject *obj, QEvent *event) {
//    if (event->type() == QEvent::KeyPress || event->type() == QEvent::MouseButtonDblClick || event->type() == QEvent::MouseButtonPress || event->type()== QEvent::MouseMove) {
//        timer->stop();
//        timer->start(240000);
//    }
    if (event->type()==QEvent::Resize) {
        //qDebug() << this->children();
        float scale = static_cast<float>(currentAppWidget->width()) / static_cast<float>(1920);
        if (currentAppWidget!=0) {
            for (int i=0;i<currentAppWidget->children().count();i++) {
                QString objclassname = currentAppWidget->children().at(i)->metaObject()->className();
                if (objclassname=="AForm") {
                    AForm *aform = static_cast<AForm*>(currentAppWidget->children().at(i));
                    aform->scaleSubObjects(scale);
                }
            }
        }
        for (int i=0;i<this->children().count();i++) {
            QString objclassname = this->children().at(i)->metaObject()->className();
            if (objclassname=="QDialog") {
                QWidget *wgt = static_cast<QWidget*>(this->children().at(i));
                wgt->resize(currentAppWidget->size());
                for (int i=0;i<wgt->children().count();i++) {
                    QString objclassname = wgt->children().at(i)->metaObject()->className();
                    if (objclassname=="AForm") {
                        AForm *aform = static_cast<AForm*>(wgt->children().at(i));
                        aform->scaleSubObjects(scale);
                    }
                }

            }
        }
    }
    return false;
}
