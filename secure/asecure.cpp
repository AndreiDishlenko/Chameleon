#include "asecure.h"
#include "afunc.h"
#include <QDir>
#include <QFile>
#include <QDebug>

#include "globals.h"
#include <adatabase.h>

ASecure::ASecure(QObject *parent) : QObject(parent) {
    dict = new ADict("dictUsers");
}

ASecure::~ASecure() {
//    qDebug() << "~ASecure()";
    dict->~ADict();
}

QMultiMap<QString , QString> ASecure::find(QString searchDir) {
    QDir dir(searchDir);
    QString filter = "*.log";
    QStringList listFiles = dir.entryList(filter.split(" "), QDir::Files);
    QMultiMap<QString , QString> MMlistFiles;
    for(int i=0;i<listFiles.count();i++) {
        MMlistFiles.insert(searchDir ,listFiles.at(i));
    }
    QStringList listDir = dir.entryList(QDir::Dirs);
    for (int i=0;i<listDir.count();i++) {
        QString subdir = listDir.at(i);
        if (subdir != "." && subdir != "..") {
            MMlistFiles = MMlistFiles + ASecure::find(searchDir+"/"+subdir);
        }
    }
    return MMlistFiles;
}

bool ASecure::encodeDir(QString mode, QString searchDir,QString word){       //word - set key/*
    QMultiMap<QString , QString> dirObjects = ASecure::find(searchDir);
    QMultiMap<QString , QString> :: iterator it = dirObjects.begin();
    for(;it != dirObjects.end(); ++it){
        ASecure::encodeFile(mode, it.key(), it.value(), word);
   }
    return true;
}

bool ASecure::encodeFile(QString mode, QString Path, QString File, QString word){
    bool result = true;
    QString newFile = File;
    if (mode == "encode")
        newFile.replace(QString(".ui"), QString(".ch"));
    else
        newFile.replace(QString(".ch"), QString(".ui"));
    QFile file1(Path+File);
    QFile file2(Path+newFile);
    if(!file1.open(QIODevice::ReadOnly)){
       qDebug() << "Ошибка открытия для чтения файла: " << File;
       result = false;
    }
       if(!file2.open(QIODevice::WriteOnly)){
          qDebug() << "Ошибка открытия для записи файла: " << newFile;
          result = false;
       }
    QByteArray Byarr = file1.readAll();
    if(mode == "encode")
        file2.write(ASecure::encodeStr(Byarr, word));
    else
        file2.write(ASecure::decodeStr(Byarr, word));
    file1.close();
    file1.remove();
    file2.close();
    return result;
}

QByteArray ASecure::encodeStr(const QByteArray& eArr, QString word){
        QByteArray arr(eArr);
        QByteArray wordBA(word.toLatin1());
        int i1=0;
        for(int i=0; i<arr.size(); i++) {
            arr[i] = arr[i] ^ wordBA[i1];
            i1++;
            if (i1==wordBA.count()) {i1=0;}
        }
        return arr.toBase64();
}
QByteArray ASecure::decodeStr(const QByteArray& dArr, QString word){
        QByteArray arr = QByteArray::fromBase64(dArr);

        QByteArray wordBA(word.toLatin1());
        int i1=0;
        for(int i=0; i<arr.size(); i++) {
            arr[i] = arr[i] ^ wordBA[i1];
            i1++;
            if (i1==wordBA.count()) {i1=0;}
        }
        return arr;
}



//QByteArray ASecure::decodeFile( QString Path, QString File, QString word){
//    bool result = true;
//    QDir::setCurrent(Path);
//    QFile file1(File);
//    if(!file1.open(QIODevice::ReadOnly)){
//       qDebug() << "Ошибка открытия для чтения файла: " << File;
//       result = false;
//    }
//    QByteArray Byarr = file1.readAll();
//    QByteArray bary = ASecure::decodeStr(Byarr, word);
//    file1.close();
//    return bary;
//}
//QByteArray bary;
//QBuffer buffer(&bary);
//buffer.open(QIODevice::WriteOnly);                разобрать буфер!!!
//buffer.write(ASecure::decodeStr(Byarr, word));
//    buffer.close();
    //file2.close();            ? по поводу закрытия буфера

////////////////////////////////////////////////////////////////////////////////
// Users Work
////////////////////////////////////////////////////////////////////////////////
int ASecure::createUser(QString login, QString password, QString name, QString card_id) {
    int result = 0;
    QMap<QString, QVariant> updateMap;
    if (login!="" && name!="") {
        dict->loadRecords("login='"+login+"' OR name='"+name+"'");
        if (dict->count()>0) {qDebug() << "ASecure::createUser Can't create user with login="+login+". It exists."; return 0;}
        dict->clearStack();
        dict->newRecord();
        updateMap["login"]=login;
        updateMap["password"]=password;
        updateMap["name"]=name;
        updateMap["card_id"]=card_id;
        dict->updateRecord(updateMap);
        dict->saveChanges();
        result = dict->getCRValue("id").toInt();
    } else {
        qDebug() << "ASecure::createUser. Login or Name is empty. Can't create user.";
    }
    return result;
}

bool ASecure::updateUser(QString login, QString password, QMap<QString, QVariant> updateMap, QString card_id) {
    return true;
}

void ASecure::setCurrentUser(int idUser) {
//    qDebug() << "ASecure::setCurrentUser" << idUser;
    globals["currentUser"] = QString::number(idUser);
}
int ASecure::getCurrentUser() {
    int result = globals["currentUser"].toInt();
    return result;
}
QString ASecure::getUserName() {
//    qDebug() << "ASecure::getUserName" << currentUser;
    return ADatabase::DLookUp("name", "dictUsers", "id='"+globals["currentUser"]+"'", "", "ASecure:getUserName").toString();
}

#include <adatabase.h>
int ASecure::getUser(QString login, QString password, int iduser) {
//    qDebug() << "ASecure::getUser" << login << password << iduser;
    int result = 0;
//    ADict * dict = new ADict("dictUsers");
    QString loginstring;
    if (login=="" && iduser!=0) {
        loginstring = "id='"+QString::number(iduser)+"'";
    } else {
        loginstring = "login='"+login+"'";
    }
    QString where;
    if (password=="") {
        where = loginstring + " AND password IS NULL OR "+loginstring+" AND password=''";
    }
    else {
        where = loginstring + " AND password='"+password+"'";
    }
//    qDebug() << where;
//    dict->loadRecords(where);
//    if (dict->count()>0) {result = dict->getCRValue("id").toInt();}
//    dict->~ADict();
    result = ADatabase::DLookUp("id", "dictUsers", where, "", "Asec:getUserName").toInt();
//    qDebug() << "ASecure::getUser" << where << result;
    return result;
}
int ASecure::getUserCardId(QString cardid) {
    int user=0;
    ADict * dict = new ADict("dictUsers");
    dict->loadRecords("card_id='"+cardid+"'");
    if (dict->count()>0) {
        user = dict->getCRValue("id").toInt();
    }
    dict->~ADict();
    return user;
}

bool ASecure::checkCard(QString cardstring) {
    QRegExp reg(".*924[\/=][0-9]{3}[\/=][0-9]{3}[\/=][0-9]{12}.*");
    if (reg.exactMatch(cardstring)) {return true;}
    else {return false;}
}

QMap<QString, QVariant> ASecure::decodeCard(QString cardstring) {
    QMap<QString, QVariant> result;

    QRegExp reg(".*924[\/=][0-9]{3}[\/=][0-9]{3}[\/=][0-9]{12}.*");
//    int pos = reg.indexIn(cardstring);
//    cardstring = reg.cap(0);
    if (cardstring=="") return result;
    result["version"] = cardstring.section(QRegExp("[\/=]"),1,1);
    result["cardtype"] = cardstring.section(QRegExp("[\/=]"),2,2);
    result["cardid"] = cardstring.section(QRegExp("[\/=]"),3,3).left(12);
//    qDebug() << "ASecure::decodeCard" << result["version"] << result["cardtype"] << result["cardid"];

//    if (cardstring.length()<14) {return result;}
//    cardstring=cardstring.mid(1, cardstring.length()-2);
//    QStringList cardlist = AFunc::strToList(cardstring, "/");
//    qDebug() << "ASecure::decodeCard" << cardstring << cardlist;
//    if (cardlist.count()<4) {return result;}

//    QString securecode = cardlist.at(0);
//    if (securecode!="924") {return result;}
//qDebug() << "ASecure::decodeCard" << 924;
//    result["version"] = cardlist.at(1);
//    result["cardtype"] = cardlist.at(2);
//    result["cardid"] = cardlist.at(3);
//    qDebug() << "ASecure::decodeCard aaa";

    return result;
}

int ASecure::getUserCardString(QString cardstring) {
    int user=0;
    QMap<QString, QVariant> cardMap = ASecure::decodeCard(cardstring);
//    qDebug() << "ASecure::getUserCardString" << cardstring << cardMap;
    if (cardMap.count()==0) {return 0;}
        ADict * dict = new ADict("dictUsers");
        dict->loadRecords("card_id='"+cardMap["cardid"].toString()+"'");
        if (dict->count()>0) {
            user = dict->getCRValue("id").toInt();
        }
        dict->~ADict();
    return user;
}

int ASecure::getUserRole(int idUser) {
    return 0;
}


bool ASecure::checkRights(QString rightsname, int idUser) {
    bool result = false;
    if (idUser==0 || rightsname=="") {return false;}

    ADict * dict = new ADict("dictUsers");
    dict->openRecord(idUser);
    if (dict->count()==0) {dict->~ADict(); return false;}
    int idrole = dict->getCRValue("idRole").toInt();
//    qDebug() << "ASecure::checkRights " << idUser << idrole;

    dict->setCurrentDict("dictRights");
    dict->loadRecords("name='"+rightsname+"'");
    if (dict->count()==0) {dict->~ADict(); return false;}
    int idrights = dict->getCRValue("id").toInt();
//    qDebug() << "ASecure::checkRights idrights" << idrights;

    dict->setCurrentDict("uniRoleRights");
    dict->loadRecords("idRole='"+QString::number(idrole)+"' AND idRights='"+QString::number(idrights)+"'");
    if (dict->count()>0) {result = true;}
//    qDebug() << "ASecure::checkRights result" << result;


    dict->~ADict();
    return result;
}
#include "adatabase.h"
bool ASecure::addUser(QString name, QString password, int role, QString card_id) {
    return ADatabase::insertParam("dictUsers", "name, password, card_id, idRole", "'"+name+"', '"+password+"', '"+card_id+"', "+QString::number(role));
}


//bool ASecure::setUserLogin(QString login, QString password) {
//    if (login!="") {
//        int user = ASecure::checkUser(login, password);
//        if (user!=0) {
//            currentUser = user;
//            return true;
//        }
//    }
//    return false;
//}
//bool ASecure::setUserId(int iduser, QString password) {
////    qDebug() << "ASecure::setUserId1" << iduser;
//    if (iduser!=0) {
//        int user=ASecure::checkUser("", password, iduser);
////        qDebug() << "ASecure::setUserId2" << user;
//        if (user!=0) {
//            currentUser = user;
//            return true;
//        }
//    }
//    return false;
//}
