#include "afunc.h"
#include "adataset.h"
#include "adatabase.h"
#include "qstringlist.h"
#include "globals.h"
#include "qdebug.h"
#include <QSqlRecord>
#include <QDateTime>

#include "adict.h"
#include "avariant.h"

#include <QSqlTableModel>

AFunc::AFunc(QObject *parent) : QObject(parent) {
}
AFunc::~AFunc() {
//    qDebug() << "~AFunc()";
}

bool AFunc::checkFolder(QString foldername) {
    if (!AFunc::dirExists( foldername )) {qWarning() << "Configuration folder '" + foldername + "' is not found."; return false;}
    return true;
}

QStringList AFunc::strToList(QString str, QString div, bool full) {
    //qDebug() << "ATableWidgetStatic::resizeColumns" << str;
    QStringList result;
    int prev = 0;
    int delcount = str.count(div);
    for (int i=0;i<=delcount;i++) {
            if (full) {
                result << str.mid(prev,str.indexOf(div, prev)-prev);
            } else {
                result << str.mid(prev,str.indexOf(div, prev)-prev).trimmed();
            }
            prev=str.indexOf(div, prev)+1;
        }
    return result;
}
QString AFunc::ListToStr(QStringList str_list, QString div) {
    QString result;
    for (int i=0;i<str_list.count();i++) {
            result.append(str_list.at(i));
            if (i!=str_list.count()-1) {
                result.append(div+" ");
            }
        }
    return result;
}

// Обрабатівает строку типа idCont=%id% либо idCont='aaa'
QMap<QString, QString> AFunc::getStrParam(QString str){
    QMap<QString, QString> result;
    QString name, value, flag, sflag;
    flag="name";
    for (int i=0;i<str.size();i++) {
        QString chr = QString(str.at(i));

        if (flag=="name" && chr=="=") {flag=""; chr="";}
        if (flag=="" && chr==",") {flag="name"; chr="";}
        if (flag=="name" && chr!="") {name.append(str.at(i));}

        if (chr=="%" && sflag=="") {flag="value"; sflag="begin"; chr="";}
        if (chr!="" && chr!="%" && flag=="value") {value.append(str.at(i));}
        if (chr=="%" && sflag=="begin") {flag=""; sflag=""; name.replace(" ",""); /*value.replace(" ",""); */result[name]=value; value=""; name="";}
//    qDebug() << chr << flag << sflag << value;
    }
    return result;
}

// Работа с моделями
QVariant AFunc::getQueryModelValue(QSqlQueryModel *model, int row, QString columnName){
    QVariant result = model->record(row).value(model->record(row).indexOf(columnName));
//        if (result.type()==QVariant::DateTime) {result = AVariant::convertDateToString(result.toDate());}
    return result;
}
bool AFunc::setQueryModelValue(QSqlQueryModel *model, int row, QString fieldname, QVariant data) {
        if (model==0 || model->rowCount()==0) {return false;}
    int column = model->record(0).indexOf(fieldname);
        if (column==-1) {return false;}
    QModelIndex index = model->index(row, column, QModelIndex());
//    qDebug() << "AFunc::setQueryModelValue " << model << model->headerData(column,Qt::Horizontal,Qt::DisplayRole) << index << data;
    return model->setData(index,data,Qt::EditRole);
    }
QVariant AFunc::getModelValue(QAbstractItemModel *model, int row, int column){
    QModelIndex index = model->index(row,column,QModelIndex());
    QVariant result = model->data(index,Qt::DisplayRole);
    //qDebug() << "AFunc::getModelValue" << result;
    // if (result.type()==QVariant::DateTime) {result = AVariant::convertDateToString(result.toDate());}
    return result;
}
bool AFunc::setModelValue(QAbstractItemModel *model, int row, int column, QVariant data) {
    if (model==0 || model->rowCount()==0) {return false;}
    QModelIndex index = model->index(row, column, QModelIndex());
    //    qDebug() << "AFunc::setQueryModelValue " << model << model->headerData(column,Qt::Horizontal,Qt::DisplayRole) << index << data;
    return model->setData(index,data,Qt::EditRole);
}

// Функции преобразования
QVariantMap AFunc::getQueryModelRow(QSqlQueryModel *model, int row) {
    int columnCount = model->columnCount();
    QVariantMap res;
    for (int i=0;i<columnCount;i++) {
        QString key = model->headerData(i,Qt::Horizontal).toString();
        QVariant value = model->record(row).value(i);
        res[key] = value;
    }
    return res;
}
QVariantMap AFunc::getModelRow(QAbstractItemModel *model, int row) {
    int columnCount = model->columnCount();
    QVariantMap res;
    for (int i=0;i<columnCount;i++) {
        QModelIndex index = model->index(row, i, QModelIndex());
        QString key = model->headerData(i,Qt::Horizontal).toString();
        QVariant value = model->data(index,Qt::DisplayRole).toString();
        res[key] = value;
    }
    return res;
}

void AFunc::echoTimer(QString msg) {
    int sec = QDateTime::currentDateTime().time().second();
    int msec = QDateTime::currentDateTime().time().msec();
    int pause = sec*1000+msec-timer;
    qDebug() << msg << "Timer: " << sec << "sec." << msec << "msec. Pause:" << pause;;
    timer = sec*1000+msec;
}
QString AFunc::getCurrentDate() {
      return AVariant::convertDateToString(QDate::currentDate());
}
QString AFunc::getCurrentYear() {
      return QString::number(QDate::currentDate().year());
}
QString AFunc::getCurrentMonth() {
      return QString::number(QDate::currentDate().month());
}
QString AFunc::getCurrentDay() {
      return QString::number(QDate::currentDate().day());
}
QString AFunc::getCurrentTime() {
      return QTime::currentTime().toString("hh:mm:ss");
}
QString AFunc::dateAddDay(QVariant var, int i) {

    QDate dat = AVariant::convertVariantToDate(var);
//    qDebug() << "AFunc::dateAddDay" << var << dat;
    return AVariant::convertDateToString(dat.addDays(i));
}
//QString AFunc::dateConvertFromIso(QVariant var) {
//    QDate dat = var.toDate();
//    return AVariant::convertDateToString(dat);
//}



QVariant AFunc::getGlobVar(QString key) {
    return globals[key];
}

QVariant AFunc::getDefCurrency() {
     QString defCurrency = AApplication::getRegValue("defCurrency").toString();
     QVariant result = ADatabase::DLookUp("name", "dictCurrency", "id="+defCurrency, "", "AFunc:getDefCurrency");
     return result;
}

void AFunc::echoModel(QAbstractItemModel *model) {
    qDebug() << "--- AFunc::echoModel : " << model;
    QStringList temp;
    temp.clear();
    for (int i1=0;i1<model->columnCount();i1++) {
        temp << model->headerData(i1,Qt::Horizontal,Qt::EditRole).toString();
    }
    qDebug() << temp;
    for (int i=0;i<model->rowCount();i++) {
        temp.clear();
        for (int i1=0;i1<model->columnCount();i1++) {
            temp << AFunc::getModelValue(model,i,i1).toString();
        }
        qDebug() << temp;
    }
    qDebug() << "--------------";
}

QObject * AFunc::createAPush(QWidget *parent) {
    APush *newButton = new APush(parent);
    return newButton;
}

double AFunc::round(double num, int precision) {
    int div=1;
    for (int i=0;i<precision;i++) {div=div*10;}
    double a = qRound(num*div);
    double result = a / div;
//    qDebug() << "AFunc::round" << num << div << a << result;
    return result;
}

#include <QApplication>
QString AFunc::appPath() {
    return QApplication::applicationDirPath();
}

////////////////////////////////////////////////////
// Функции для работа с файлами и папками
////////////////////////////////////////////////////
bool AFunc::dirExists(QString dirname) {
    //    qDebug() << "AFunc::dirExists" << QDir::currentPath()+gvUiPath+filename << result;
    QDir dir(dirname);
    bool result = dir.exists(dirname);
    return result;
}
bool AFunc::dirCreate(QString dirname) {
    QDir dir;
    return dir.mkpath(dirname);
}

QString AFunc::readFile(QString filename) {
    //qDebug() << "AFunc::readFile" << globals["configurationPath"]+filename;
    if (filename.contains("$")) {
        filename.remove("$");
        filename = AFunc::unionFolderNames(globals["uiPath"], filename);
    } else {
         filename = AFunc::unionFolderNames(globals["configurationPath"], filename);
    }

    //filename = AFunc::unionFolderNames(globals["configurationPath"], filename);

    QFile file( filename );
    if (!file.open(QFile::ReadOnly)) {
        qDebug() << "[Warning] AFunc::readFile. Can't open file: "+filename+". "+file.errorString();
    }
    //QString result = QLatin1String(file.readAll());
    QString result = file.readAll();
    file.close();


//    QFile file(filename);
//    if (!file.open(QFile::ReadOnly)) {
//        qDebug() << "Warning (ADataset::parseHtmlFile): Can't open "+filename+". "+file.errorString();
//        return "";
//        }
//    QTextStream stream(&file);
//    QString htmlcheck = stream.readAll();
//    file.close();



    return result;
}
bool AFunc::fileExists(QString filename) {
    QFile file(filename);
    bool result = file.exists();
//    qDebug() << "AFunc::fileExists" << filename << result;
    return result;
}
bool AFunc::uiFileExists(QString filename) {
    QString file = AFunc::unionFolderNames(globals["configurationPath"],filename);
    //qDebug() << "AFunc::uifileExists" << file;
    bool result = AFunc::fileExists(file);
    return result;
}
bool AFunc::fileCopy(QString source, QString destination) {
    return QFile::copy(source, destination);
}
bool AFunc::fileDelete(QString filename) {
    return QFile::remove(filename);
}

bool AFunc::dirDelete(QString dirname) {
    QDir *dir = new QDir();
    return dir->rmdir(dirname);
}

#include "QFileDialog"
QString AFunc::fileDialog(QString filter, QString currentPath) {
    QString result;
    QFileDialog *dialog = new QFileDialog(0, "Выберите файл");
    if (glob_fileDialog_dir.dirName()!="") {dialog->setDirectory(glob_fileDialog_dir);}
    if (filter!="") {
        dialog->setNameFilter(filter);
    }
    if (currentPath!="" && AFunc::fileExists(currentPath)) {
        dialog->setDirectory(QDir(currentPath));
    } else {
        dialog->setDirectory(QDir::currentPath());
    }
    QStringList fileNames;
    if (dialog->exec())
        fileNames = dialog->selectedFiles();
    glob_fileDialog_dir = dialog->directory();
    dialog->~QFileDialog();
    if (fileNames.count()>0) {result = fileNames.at(0);}
    return result;
}

QString AFunc::convertToAbsolutePath(QString path) {
    if (path=="") {return path;}

//    qDebug() << QDir::currentPath();
    QRegExp reg("(^[a-zA-Z]{1}[:].+)");
    QRegExp reg1("(^([\\\\]|[/]{2}).+)");
    if (!reg.exactMatch(path) && !reg1.exactMatch(path)) {
        if (path.at(0)!='/') {path = "/"+path;}
        path = AFunc::unionFolderNames(QDir::currentPath(), path);
    } else if (!reg.exactMatch(path) && reg1.exactMatch(path)) {
        return path;
    }
    path.replace("\\\\", "\\");
    path.replace("//", "/");
//    if (path.at(path.length()-1)!='/') {path = path+'/';}

    return path;
}

QString AFunc::directoryDialog(QString startDir) {
    QString result;
    QFileDialog *dialog = new QFileDialog(0, "Выберите директорию");
    if (glob_fileDialog_dir.dirName()!="") {dialog->setDirectory(glob_fileDialog_dir);}
    dialog->setFileMode(QFileDialog::Directory);
    startDir = AFunc::convertToAbsolutePath(startDir);
//    qDebug() << "AFunc::directoryDialog" << startDir;
    if (startDir!="" && AFunc::dirExists(startDir)) {
//        qDebug() << "AFunc::directoryDialog 1" << AFunc::convertToAbsolutePath(startDir);
        dialog->setDirectory(QDir(startDir));
    } else {
//        qDebug() << "AFunc::directoryDialog 2" << QDir::currentPath();
        dialog->setDirectory(QDir::currentPath());
    }
    QStringList fileNames;
    if (dialog->exec())
        fileNames = dialog->selectedFiles();
    glob_fileDialog_dir = dialog->directory();
    dialog->~QFileDialog();
    if (fileNames.count()>0) {result = fileNames.at(0);}
    return result;
}

bool AFunc::saveToFile(QString filename, QString data){
    qDebug() << "AFunc::saveToFile" << filename;
    bool result = true;
    if (filename=="") {return false;}
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)){
       QTextStream stream(&file);
       stream << data;
       file.close();
       if(stream.status()!= QTextStream::Ok) {qDebug() << "AFunc::saveToFile file "+filename+" save Error"; result=false;}
    } else {
        qDebug() << "AFunc::saveToFile can't open "+filename+"."; result = false;
    }
    return result;
}



QString AFunc::parseHtmlFile(QString filename, QMap <QString, QVariant> mapname, QString querystring) {
    //    qDebug() << "AFunc::parseHtmlFile" << mapname;
    QString html = AFunc::readFile(filename);
    QString result = ADataset::parseHtml(html, mapname, querystring);
    return result;
}

QMultiMap<QString , QString> AFunc::getDirTree(QString PathDir, QString startDir) {
    QDir dir(PathDir);
    QString filter = "*";
    QString tempString;
    QStringList listFiles = dir.entryList(filter.split(" "), QDir::Files);
    QMultiMap<QString , QString> MMlistFiles;
    for(int i=0;i<listFiles.count();i++) {
        tempString = PathDir;
        tempString.replace(startDir, "");
        MMlistFiles.insert(tempString, listFiles.at(i));
    }
    QStringList listDir = dir.entryList(QDir::AllDirs);
    for (int i=0;i<listDir.count();i++) {
        QString subdir = listDir.at(i);
        if (subdir!="." && subdir!="..") {
            tempString = PathDir;
            tempString.replace(startDir, "");
            MMlistFiles.insert(tempString+"/"+subdir, "");
            MMlistFiles = MMlistFiles + AFunc::getDirTree(PathDir+"/"+subdir, startDir);
        }
    }
    return MMlistFiles;
}

//2 #include <QAxObject>
//2void AFunc::openApplication(QString application, QString fileName) {
//    qDebug() << "AFunc::openApplication" << application << fileName;
//2    if (application=="Word") {
//2        QAxObject *application = new QAxObject("Word.Application");
//2        application->setProperty("Visible", true);
//2        QList<QVariant> params;
//2        params << QVariant(fileName);// << QVariant(false) << QVariant(false) << QVariant(false) << QVariant("") << QVariant("") << QVariant(false);
//2        // << "" << "" << "wdOpenFormatAuto" << "" << true;
//2//        application->querySubObject("Documents()")->querySubObject("Open(const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&)", params);
//2        application->querySubObject("Documents()")->querySubObject("Open(const QVariant&)", params);
//2    }
//2}

QString AFunc::unionFolderNames(QString firstname, QString secondname) {    
    QString result;
    if (firstname.right(1)!="/") {firstname.append("/");}
    if (secondname.left(1)=="/") {secondname.remove(0,1);}
    result = firstname + secondname;
    return result;
}

void AFunc::echoStringList(QList<QString> list, QString sender) {
    for (int i=0;i<list.count();i++)
        qDebug() << sender << i << list.at(i);
}

void AFunc::echoObjectList(QObjectList list) {
    for (int i=0;i<list.count();i++)
        qDebug() << i << list.at(i);
}
