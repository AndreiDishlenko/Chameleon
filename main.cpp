// QT declaration
#include <QApplication>
#include <QDebug>
#include <QtSql>
#include <QScriptEngine>
#include <QFontDatabase>
#include <QSslSocket>

// CLASSES declaration
#include "aapplication.h"

// SELF declaration
#include <QMessageBox>
#include "afunc.h"


#include "QGroupBox"
#include "secure/alicense.h"
#include "adict.h"
#include "docs/adoc.h"
#include "pos/adiscount.h"

//#include "QTextBrowser"
#include <QTranslator>
//#include <QImageReader>
#include <QTimer>
#include <adatabase.h>
#include "docs/abuild.h"
#include "agui.h"
#include <QImageReader>
#include "globals.h"
#include <iostream>

//void myMessageOutput(QtMsgType type, const char *msg) {
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {

    QString txt;
    switch (type) {
    case QtDebugMsg:
        std::cout << msg.toStdString() << std::endl;
        txt = QString("["+QTime::currentTime().toString("hh:mm:ss")+"] %1").arg(msg);
        break;
    case QtWarningMsg:
        std::cout << msg.toStdString() << std::endl;
        txt = QString("Warning: %1").arg(msg);
    break;
    case QtCriticalMsg:
        std::cout << msg.toStdString() << std::endl;
        txt = QString("Critical: %1").arg(msg);
    break;
    case QtFatalMsg:
        std::cout << msg.toStdString() << std::endl;
        txt = QString("Fatal: %1").arg(msg);
    break;
    }
    QFile outFile(QDir::currentPath()+"/logs/"+QDate::currentDate().toString("yyMMdd")+"-"+QTime::currentTime().toString("hh")+"h.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;

}

void logFilesProcess() {
    QDir logDir(globals["logPath"]);
    QStringList filters;
    filters << "*.log";
    QStringList fileslist = logDir.entryList(filters, QDir::Files, QDir::Time);
    int logStorePeriod = globals["LogStorePeriod"].toInt();
    if (logStorePeriod==0) {logStorePeriod=5;}
    QDateTime logFirstDate = QDateTime::currentDateTime().addDays(-logStorePeriod);
    for (int i=0;i<fileslist.count();i++) {
        QFileInfo fi(globals["logPath"]+fileslist.at(i));
        if (fi.created()<logFirstDate) {QFile::remove(globals["logPath"]+fileslist.at(i));}
    }
}

// Чтение .ini файла
QMultiMap<QString, QString>  readIniFile(QString filename) {
    QMultiMap<QString, QString>  result;
    QFile file(filename);
    if (!file.open(QFile::ReadOnly)) {
        qDebug() << "Warning (readIniFile): Can't open "+filename+". "+file.errorString();
        return result;
    }
    QString line, K, T;
    QTextStream accIn(&file), lineIn(&line);
    while (!accIn.atEnd()) {
        line = accIn.readLine();
        lineIn.flush();
                int pos = line.indexOf("=",0);
                if (pos==0) continue;
                K = line.mid(0, pos).trimmed();
                T = line.mid(pos+1, line.length()-pos).trimmed();
                if (K.left(2)!="//" && K!="") {
                    result.insert(K, T);
                }
    }
    file.close();
//    QString inicontent = QLatin1String(file.readAll());
//    file.close();
//    int x = 0;
//    int y = 0;
//    for (int i=0;i<inicontent.count("\n");i++) {
//        y = inicontent.indexOf("\n", x);
//        QString mid = inicontent.mid(x,y-x);
//        int pos = mid.indexOf("=",0);
//        QString K = mid.mid(0, pos).remove("\r").remove("\n").trimmed();
//        if (K.left(2)!="//" && K!="") {
//            result.insert(K, mid.mid(pos+1,mid.length()-pos).remove("\r").remove("\n"));
//        }
//        x=y+1;
//    }
    return result;
}

// Формиование основных настроек приложения, обьединение их с /personal/config.ini
/* Настройки могут быть нескольких типов:
 * 1. Заданные вручную
 * 2. Из файла config.ini
 * 3. Из локальной базы данных localdb.db3  - AApplication::getLocalSetting
 * 4. Из БД MSSQL (таблица reg)             - AFunc::getRegValue/setRegValue
 */
void setGlobalVariables(QString appname="") {
    //qDebug() << "main:setGlobalVariables";
    /*
    dbType                  - тип базы данных основного хранилища (QODBC, по умолчанию)
    dbFile                  - файл БД, в случае использовании файловой БД
    showShadows
    */
    globals["coreVersion"]  =       "0.6.2";    // Версия ядра
    globals["globRelease"]  =       "";         // Наименование конфигурации
    globals["currentUser"] =        QString::number(0);


    globals["logPath"] = 5;                     // Период хранения log файлов

    globals["applicationPath"] =    QDir::currentPath()+"/";
    globals["uiPath"] =             AFunc::unionFolderNames(QDir::currentPath(), "/data/ui/");
    globals["logPath"] =            QDir::currentPath()+"/logs/";
    globals["personalFolder"] =     QDir::currentPath() + "/personal/";
    globals["library_path"] =       QDir::currentPath() + "/system/libs/";



    //globals["configurationFolder"] = "";
    //globals["configurationPath"] = "";
    globals["ServerFileStore"] = "";                    // Непонятная переменная, используется только в в классе AImageArea

    if (!AFunc::checkFolder(globals["applicationPath"]) || !AFunc::checkFolder(globals["uiPath"]) || !AFunc::checkFolder(globals["logPath"]) || !AFunc::checkFolder(globals["personalFolder"]) || !AFunc::checkFolder(globals["library_path"]) ) {
        return;
    }

    //globals.unite(readIniFile(globals["personalFolder"]+"config.ini"));           // Добавление конфигурационного файла к списку глобальных переменных

    globals["configurationFolder"] = appname;
    globals["configurationPath"] = AFunc::unionFolderNames( globals["uiPath"], globals["configurationFolder"] );
    //qDebug() << globals["configurationPath"];
    if (!AFunc::checkFolder( globals["configurationPath"])) {
        qDebug() << "[Error] Не найдена папка конфигурации " << globals["configurationFolder"];
        exit(0);
    }

    globals["progNo"]="";               // Номер конфигурации - используется при кодировании
    if (appname=="info") { globals["uikey"] = "MoQwrGQLd24JWEujzFZcaPQmVWZGoIp4"; }
    globals["tempkey"]="";


    globals["defLocation"] = "";        // БД, используемая по-умолчанию на старте загрузки
    globals["activeLocation"] = "";     // Текущая информационная база ( номер из локальной БД dictLocations )
    globals["activeLocationName"] = ""; // Имя текущей информационной базы

    globals["defConnection"]="";        // имя текущего соединения с основной БД
    globals["reserveConnection"]="";    // имя резервного соединение с БД

    globals["dbServer"] = "";
    globals["dbType"] = "";
    globals["dbServerAlias"] = "";
    globals["dbServerPort"] = "";
    globals["dbName"] = "";
    globals["dbUser"] = "";
    globals["dbPassword"] = "";

    globals["defFirm"]="";              // подразделение, к которому пренадлежит торговая точка (используется в front-office)
    globals["defStore"]="";             // склад по-умолчанию с которого происходят отгрузки в (используется в front-office)
    globals["defAccount"]="";             // склад по-умолчанию с которого происходят отгрузки в (используется в front-office)

    globals["showShadows"];


}

int main(int argc, char *argv[]) {
    #ifdef QT_NO_DEBUG
        qInstallMessageHandler(myMessageOutput);
    #endif

    QApplication a(argc, argv);
//
//    QThread::currentThread()->setPriority(QThread::LowPriority);

    //qDebug() << QDir::currentPath()+"/logs/"+QDate::currentDate().toString("yyMMdd")+"-"+QTime::currentTime().toString("hh")+"h.log";

    AFunc::echoTimer("** Begin open timer 0.");

    qDebug() << "";
    qDebug() << "Launching the Chameleon application ver." << qPrintable(globals["app_version"]);
    qDebug() << "--------------------------------------------------";
    qDebug() << "[] Programm path: " << a.applicationDirPath();

    //////////////////////////////////////////////////
    // ___ 1. Установка кодировок
    //////////////////////////////////////////////////
    //55QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Windows-1251"));
    //55QTextCodec::setCodecForTr(QTextCodec::codecForName("Windows-1251"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("IBM 866"));
    //55QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    //55QWebSettings::globalSettings()->setAttribute(QWebSettings::AutoLoadImages, true);
    //////////////////////////////////////////////////
    // Подключение шрифтов
    //////////////////////////////////////////////////
    int id = QFontDatabase::addApplicationFont( AFunc::unionFolderNames(a.applicationDirPath(), "/system/fonts/Roboto-Black.ttf") ); //путь к шрифту
    QString family = QFontDatabase::applicationFontFamilies(id).at(0); //имя шрифта
    QFont f(family);
    //////////////////////////////////////////////////
    // ___ 2. Русификация кнопок графического интерфейса
    //////////////////////////////////////////////////
    QTranslator * qt_translator = new QTranslator();
    if (!qt_translator->load( QDir::currentPath()+"/system/qtbase_ru.qm" ) ) {
      delete qt_translator;
      return false;
    }
    a.installTranslator( qt_translator );
    //////////////////////////////////////////////////
    // ___ 3. Инициализация генератора рандомных чисел
    //////////////////////////////////////////////////
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    //////////////////////////////////////////////////
    // ___ 4. Подключение папок
    //////////////////////////////////////////////////
    QString dataPath = "/system";
    QDir dataDir = QDir::currentPath() + dataPath;

    if (!dataDir.exists()) {qCritical() << "Can't find folder " + dataDir.path(); return false;}
    QStringList paths = a.libraryPaths();
    paths <<  dataDir.path();
    paths <<  dataDir.path() + "/libs";
    paths <<  dataDir.path() + "/script";
    paths <<  dataDir.path() + "/plugins";
    paths <<  dataDir.path() + "/plugins/sqldrivers";
    //paths <<  "./plugins";
    paths <<  QDir::currentPath() + "/data/css";
    QCoreApplication::setLibraryPaths(paths);
    qDebug() << "[] Connected folders:";
    QStringList temp = a.libraryPaths();
    foreach (QString inp, temp) {qDebug() << inp;}

    //////////////////////////////////////////////////
    // ___ 6. Консольная информация
    //////////////////////////////////////////////////

    qDebug() << "[] Libraries location: ";
    qDebug() << QLibraryInfo::location(QLibraryInfo::LibraryExecutablesPath);
    //print(Value1, Value2)
    qDebug() << "[] Available extensions: ";
    QScriptEngine engine;
    foreach (QString temp, engine.availableExtensions()) {qDebug() << temp;}
    qDebug() << "[] Available DB drivers: ";
    foreach (QString temp, QSqlDatabase::drivers()) {qDebug() << temp;}
    qDebug() << "[] Supported image formats: ";
    qDebug() << QImageReader::supportedImageFormats();
    qDebug() << QStyleFactory::keys();
    qDebug() << "[] SSL support: " << QSslSocket::supportsSsl();

    //QLibrary library("data/script/qtscript_core.dll");
    //library.load();
    //qDebug() << "Load library: " << library.errorString();

    //////////////////////////////////////////////////
    // ___ 7. Разбор коммандной строки
    //////////////////////////////////////////////////
    AApplication *app = new AApplication();

    QString appname = "fastfood";
    QString runmode = "";
    for (int i=1;i<=argc;i++) {
        QString arg = argv[i];
        if (i < argc) {
            QString nextarg = argv[i+1];
            if (arg == "-console") {QTextCodec::setCodecForLocale(QTextCodec::codecForName("IBM 866"));}
            if (arg == "-s") {runmode="settings";}
            if (arg == "-w") {app->flag_fullscreen=0;}
        }
    }
    //runmode="settings";

    //////////////////////////////////////////////////
    // ___ 5. Создание глобальных переменных
    //////////////////////////////////////////////////

    setGlobalVariables(appname);
    ADatabase::createConnection("QSQLITE", "sqconnection");
    globals["Lang"] = AApplication::getLocalSetting("Lang").toString();
    // ?????????????????? Написать процедуру чтения globs из sqconnection

    qDebug() << "[] Initialization langugage: " << globals["Lang"];
    //////////////////////////////////////////////////
    // ___ 8. Старт приложения
    //////////////////////////////////////////////////
    logFilesProcess();

    //    // File store synchronization
    //    bool serverSynchronization = AApplication::getLocalSetting("serverSynchronization").toBool();
    //    if (serverSynchronization) {
    //        QString SDir_Server = AFunc::convertToAbsolutePath(AApplication::getLocalSetting("serverStorePath").toString());
    //        QString SDir_Host = AFunc::convertToAbsolutePath(AApplication::getLocalSetting("localStorePath").toString());
    //        AService::syncDir(SDir_Server, SDir_Host, progressbar);
    //    }


    QObject::connect(&a, SIGNAL(focusChanged(QWidget*,QWidget*)), app, SLOT(changeFocus(QWidget*,QWidget*)));

    //2
    //2    ALicense *al = new ALicense();
    //2    al->initapi();
    //2    al->readkey();
    //2    al->checklicense();
    //2    QTimer * timer1 = new QTimer(app);
    //2    QObject::connect(timer1, SIGNAL(timeout()), al, SLOT(checklicense()));
    //2    timer1->start(500000);
    //2    QTimer * timer2 = new QTimer(app);
    //2    QObject::connect(timer2, SIGNAL(timeout()), al, SLOT(readkey()));
    //2    timer2->start(300000);


    qDebug() << "[] Поехали Here we go...";

    /// Инициализация конфигурации, ее GUI
    app->startApp(appname, runmode);



    return a.exec();
}
