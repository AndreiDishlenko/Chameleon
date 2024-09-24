/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Класс ADatabase реализовывает взаимодействие с базой данных
///
/////////////////////////////////////////////////////////////////////////////////////////////
#include <QMessageBox>
#include <QtSql>
#include "globals.h"
#include "adatabase.h"
#include "agui.h"
#include "afunc.h"
#include <QVariantList>
#include "avariant.h"
#include <QTcpSocket>
#include <QVariantMap>

//////////////////////////////////////////////////
// Конструктор
//////////////////////////////////////////////////
ADatabase::ADatabase(QObject *parent) : QObject(parent) {
    //qDebug() << "ADatabase::ADatabase";
}
ADatabase::~ADatabase() {
    //qDebug() << "~ADatabase";
}

//////////////////////////////////////////////////
// Управление соединениями
//////////////////////////////////////////////////
bool ADatabase::checkConnection(QString conName, QString sender) {
    //qDebug() << "ADatabase::checkConnection" << conName << sender;
    bool result = true;
    if (conName=="") {
        qDebug() << "[Warning] ADatabase::checkConnection any database is not connected" << sender;
        return false;
    }
    if (!QSqlDatabase::connectionNames().contains(conName)) {
        qDebug() << "[Warning] ADatabase::checkConnection. Get unknown connection" << conName << "from" << sender << "Exit.";
        return false;
    }

    //// Check connection with MSSQL
    if (ADatabase::getConnectionDriver(conName) == "QODBC3") {
        if (!ADatabase::pingSQLServer(conName)) {return false;}
        QSqlError error = QSqlDatabase::database(conName).exec("SELECT * FROM reg WHERE id=0").lastError();
        if (error.isValid()) {
            //qDebug() << "ADatabase::checkConnection" << conName << "connection problemm.";
            bool temp=false;
            while (temp==false) {
                if (ADatabase::openConnection(conName)) {temp = true; result=true;}
                    //if (AGui::dlgAccept("Connection lost. Try to re-connect?")) {
                        //qDebug() << "- ADatabase::checkConnection defcon, conlist" << globals["defConnection"] << QSqlDatabase::connectionNames();
                        //qDebug() << "- ADatabase::checkConnection conName database:" << QSqlDatabase::database(conName);
                        //qDebug() << "- ADatabase::checkConnection dbType" << globals["dbType"];
                    //} else {temp = true;}
            }
        }
    }
    return result;
}
bool ADatabase::pingSQLServer(QString conName) {
    //// ??? get dbServer and dbPort to correct ping server
    QString dbServer = globals["dbServer"];
    int dbServerPort = globals["dbServerPort"].toInt();
    QTcpSocket *tcpSocket = new QTcpSocket;
    bool temp=false;
    while (temp==false) {
        tcpSocket->connectToHost(dbServer, dbServerPort);
        if(tcpSocket->waitForConnected(5000)) {
            temp = true;
        } else {
            qDebug() << "[Error] Server "+globals["dbServer"]+" is unavailable.";
            if (!AGui::dlgAccept("[Error] Server "+globals["dbServer"]+":"+globals["dbServerPort"]+" is unavailable. Try to connect again?")) {
                ADatabase::closeConnection(conName);
                //globals["defConnection"]="";
                return false;
            }
        }
    }
    return true;
}
QString ADatabase::createConnection(QString typeofdb, QString name) {
    //qDebug() << "ADatabase::createConnection: "  << typeofdb << name;
    if (name=="") {
        int randomnumber = 0;
        while (randomnumber==0)  {randomnumber = QDateTime::currentDateTime().time().minute()*QDateTime::currentDateTime().time().second() * QDateTime::currentDateTime().time().msec();}        
        name = QString::number(randomnumber);
        if (typeofdb=="QODBC3") {name = "odbc_"+name;}
        if (typeofdb=="QSQLITE") {name = "sqlite_"+name;}
    }
    if (typeofdb=="QODBC3") {
        // Check if SQL server is available by ip and port
        bool temp=false;

        //qDebug() << "ADatabase::createConnection: " << 1;
        QSqlDatabase db = QSqlDatabase::addDatabase("QODBC3", name);

        //qDebug() << "ADatabase::createConnection: "  << "DRIVER={SQL Server};Server="+globals["dbServer"]+"\\"+globals["dbServerAlias"]+","+globals["dbServerPort"]+";Database="+globals["dbName"]+";UID="+globals["dbUser"]+";";
        db.setHostName(globals["dbServer"]);
        db.setPort(globals["dbServerPort"].toInt());
        db.setConnectOptions( "SQL_ATTR_CONNECTION_TIMEOUT=1" );
        db.setConnectOptions( "SQL_ATTR_LOGIN_TIMEOUT=1" );
        db.setUserName(globals["dbUser"]);
        db.setPassword(globals["dbPassword"]);


        db.setDatabaseName("DRIVER={SQL Server};Server="+globals["dbServer"]+"\\"+globals["dbServerAlias"]+","+globals["dbServerPort"]+";Database="+globals["dbName"]+";");
        //qDebug() << "ADatabase::createConnection: " << 2;

        if (!ADatabase::openConnection(name)) {return "";}
        ADatabase::execSql("SET IMPLICIT_TRANSACTIONS OFF", name);
        //ADatabase::execSql("SET LANGUAGE russian", name);
        //ADatabase::execSql("SET DATEFORMAT ymd", name);

    } else if (typeofdb=="QSQLITE") {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", name);
        //qDebug() << "ADatabase::createConnection" << globals["personalFolder"] << globals["dbFile"];
        if (name=="sqconnection") {
            db.setDatabaseName(AFunc::convertToAbsolutePath(globals["personalFolder"]+"localdb.db3"));
        } else {
            db.setDatabaseName(AFunc::convertToAbsolutePath(globals["personalFolder"]+globals["dbFile"]));
        }
        if (!ADatabase::openConnection(name)) {return "";}
    } else {
        return "";
    }
    //qDebug() << "ADatabase::createConnection: "  << QSqlDatabase::connectionNames();
    return name;
}
bool ADatabase::openConnection(QString conName) {
    //qDebug() << "ADatabase::openConnection" << name << globals["dbServer"] << globals["dbServerPort"];//<< QSqlDatabase::database(name);//.driverName() << QSqlDatabase::database(name).databaseName();
    QTime t;
    bool temp=false;
    while (temp==false) {
        t.start();
        if (ADatabase::getConnectionDriver(conName)=="QODBC3") {if (!ADatabase::pingSQLServer(conName)) {return false;}}
        if (!QSqlDatabase::database(conName).open()) {
            qDebug() << "[-] Can't connect database " << QSqlDatabase::database(conName).databaseName() << "at" << QSqlDatabase::database(conName).hostName() << ", user:" << QSqlDatabase::database(conName).userName();
            if (!AGui::dlgAccept(QSqlDatabase::database(conName).lastError().text()+"\n\nDatabase connection error after "+QString::number(t.elapsed()*0.001)+"s. Try again?")) {return false;}
        } else {
            qDebug() << "[+] Database connection" << ADatabase::getConnectionName(conName) << " is opened in " << t.elapsed()*0.001;
            return(true);
        }
    }
}
bool ADatabase::closeConnection(QString name) {
    //qDebug() << "ADatabase::closeConnection" << name;
    //    QSqlDatabase db = QSqlDatabase::database(name);
    //    QString drivername = db.driverName();
    //db.close();
    //    if (db.lastError().isValid()) {qWarning() << "[WARNING] ADatabase::closeConnection: " + db.lastError().text(); return false;}

    QSqlDatabase::removeDatabase(name);
    qDebug() << "[] Database connection" << name << " is closed" << QSqlDatabase::connectionNames();
    return true;
}

QString ADatabase::getConnectionName(QString conName) {
//    qDebug() << "ADatabase::getConnectionName" << conName << QSqlDatabase::connectionNames();
    if (conName=="") {conName=globals["defConnection"];}
//    if (ADatabase::getConnectionDriver(conName)=="QSQLITE") {conName = globals["defConnection"];}
    return conName;
}
QString ADatabase::getConnectionDriver(QString conName) {
    //qDebug() << "ADatabase::getConnectionDriver";
    QString prefix = conName.left(conName.indexOf("_"));
    if (prefix=="odbc") {return "QODBC3";}
    if (prefix=="sqlite" || conName=="sqconnection") {return "QSQLITE";}
    return "";
}

//////////////////////////////////////////////////
// Управление транзакциями
//////////////////////////////////////////////////
void ADatabase::beginTransaction(QString conName) {
    conName = ADatabase::getConnectionName(conName);
//    qDebug() << "ADatabase::beginTransaction" << conName;
    if (ADatabase::checkConnection(conName)) QSqlDatabase::database(conName).transaction ();
//    ADatabase::execSql("BEGIN TRANSACTION", conName);
}
void ADatabase::commitTransaction(QString conName) {
    conName = ADatabase::getConnectionName(conName);
//    qDebug() << "ADatabase::commitTransaction" << conName;
    if (ADatabase::checkConnection(conName)) QSqlDatabase::database(conName).commit();
}
void ADatabase::rollbackTransaction(QString conName) {
    conName = ADatabase::getConnectionName(conName);
//    qDebug() << "ADatabase::rollbackTransaction" << conName;
    if (ADatabase::checkConnection(conName)) QSqlDatabase::database(conName).rollback();
}

//////////////////////////////////////////////////
// Первичные Запросы к БД
//////////////////////////////////////////////////
QSqlQuery * ADatabase::newSelectSql(QString querystring, QString conName, QString sender) {
    //AFunc::echoTimer("ADatabase::newSelectSql");
    if (conName=="sqconnection") {
        //qDebug() << "ADatabase::newSelectSql" << querystring << conName ;
    }

    conName = ADatabase::getConnectionName(conName);
    if (!ADatabase::checkConnection(conName, sender+"->ADb:newSelectSql()") || querystring=="") {return 0;}
    if (ADatabase::getConnectionDriver(conName)=="QSQLITE") {querystring.replace("dbo.", "").replace("'","\"");}

    QSqlQuery *query = new QSqlQuery(QSqlDatabase::database(conName));
    query->setForwardOnly(true);
    query->exec(querystring);
    //qDebug() << "ADatabase::newSelectSql" << querystring;
    if (query->lastError().isValid()) {
        qDebug() << "[Error] ADatabase::newSelectSql" << query->lastError() << querystring << conName << sender;
    }
    //AFunc::echoTimer("ADatabase::newSelectSql 4");
//    qDebug() << "ADatabase::newSelectSql" << sender;
    return query;
}
QSqlQueryModel * ADatabase::selectSql(QString querystring, QString conName, QSqlQueryModel *qModel) {
    //qDebug() << "ADatabase::selectSql" << querystring << conName;
    conName = ADatabase::getConnectionName(conName);
    if (!ADatabase::checkConnection(conName, "ADatabase::selectSql") || querystring=="") {return 0;}

    //    QSqlQueryModel *qModel=new QSqlQueryModel();
    qModel->clear();
    qModel->setQuery(querystring, QSqlDatabase::database(conName));
    if (qModel->lastError().isValid()) {
        qDebug() << "ADatabase::selectSql:" << qModel->lastError() << querystring;
        if (!ADatabase::checkConnection(conName)) qModel = ADatabase::selectSql(querystring, conName, qModel);
    }
    while (qModel->canFetchMore()) {
        qModel->fetchMore();
    }
    return qModel;
}
bool ADatabase::execSql(QString querystring, QString conName) {
    //qDebug() << "ADatabase::execSql " << querystring << conName;
    conName = ADatabase::getConnectionName(conName);
    if (!ADatabase::checkConnection(conName, "ADatabase::execSql") || querystring=="") {return 0;}
    if (ADatabase::getConnectionDriver(conName)=="QSQLITE") {querystring.replace("dbo.", "");}
    QSqlQuery * query = new QSqlQuery(QSqlDatabase::database(conName));
    bool result  = query->exec(querystring);
    //    QSqlQuery query = QSqlDatabase::database(conName).exec(querystring);
    if (query->lastError().isValid()) {
        qDebug() << "ADatabase::execSql error: " << query->lastError() << querystring;
        if ( !ADatabase::checkConnection(conName) ) result = ADatabase::execSql(querystring, conName);
    }
    query->~QSqlQuery();
    return result;
}
int ADatabase::insertSql(QString querystring, QString conName) {
    int result = 0;
    conName = ADatabase::getConnectionName(conName);
    if (!ADatabase::checkConnection(conName, "ADatabase::insertSql") || querystring=="") {return 0;}
    //qDebug() << "ADatabase::insertSql " << querystring << conName;
    if (ADatabase::getConnectionDriver(conName)=="QSQLITE") {querystring.replace("dbo.", "");}
    QSqlQuery * query = new QSqlQuery(QSqlDatabase::database(conName));
    query->exec(querystring);
    //QSqlQuery query = QSqlDatabase::database(conName).exec(querystring);
    if (query->lastError().isValid()) {
        qDebug() << "[ERROR DB] ADatabase::insertSql error: " << query->lastError() << querystring;
        if ( !ADatabase::checkConnection(conName) ) result = ADatabase::insertSql(querystring, conName);
    }
    if (ADatabase::getConnectionDriver(conName)=="QSQLITE") { query->exec("SELECT last_insert_rowid() AS id"); }
    if (query->first()) {
        result = query->value(0).toInt();
    }
    query->~QSqlQuery();
    return result;
}

//////////////////////////////////////////////////////////////////////
// Параметрированые запросы к БД
//////////////////////////////////////////////////////////////////////
QSqlQueryModel * ADatabase::selectParams(QString dataSource, QString fields, QString filter) {
    QSqlQueryModel *qModel;
    QString querystring;
    querystring.append("SELECT "+fields+" FROM "+dataSource);
    if (!filter.isEmpty()) {querystring.append(" WHERE "+filter);}
    qModel = selectSql(querystring);
    return qModel;
}
int ADatabase::insertParam(QString dataSource, QString fields, QString values, QString conName){
    conName = ADatabase::getConnectionName(conName);
    if (conName=="") {
        qDebug() << "[Warning] ADatabase::updateParam() database is not connected";
        return false;
    }

    QString querystring;
    if (ADatabase::getConnectionDriver(conName)=="QODBC3") {
        querystring = "INSERT INTO "+dataSource+" ("+fields+") OUTPUT INSERTED.id AS id VALUES ("+values+")";
        //qDebug() << "ADatabase::insertParam" << conName << querystring;
        return ADatabase::insertSql(querystring, conName);
    } else {
        querystring = "INSERT INTO "+dataSource+" ("+fields+") VALUES ("+values+")";
        //qDebug() << "ADatabase::insertParam" << conName << querystring;
        return ADatabase::insertSql(querystring, conName);
    }
    return 0;
}
bool ADatabase::updateParam(QString dataSource, QString updatestring, QString conditions, QString conName){
    conName = ADatabase::getConnectionName(conName);
    if (conName=="") {
        qDebug() << "[Warning] ADatabase::updateParam() database is not connected";
        return false;
    }

    QString querystring = "UPDATE "+dataSource+" SET "+updatestring+" WHERE ";
    //qDebug() << "ADatabase::updateParam" << querystring << conName;
    if (conditions != "") {querystring.append(conditions);}
    else {querystring.append("id=''");}
    return execSql(querystring, conName);
}
bool ADatabase::deleteParam(QString dataSource, QString conditions, QString conName) {
    conName = ADatabase::getConnectionName(conName);
    if (conName=="") {
        qDebug() << "[Warning] ADatabase::deleteParam() database is not connected";
        return false;
    }

    QString querystring = "DELETE FROM "+dataSource+" WHERE ";
    //qDebug() << "ADatabase::deleteParam" << querystring;
    conName = ADatabase::getConnectionName(conName);
    if (conditions != "") {querystring.append(conditions);}
        else {querystring.append("id=''");}
    return execSql(querystring, conName);
}

//////////////////////////////////////////////////////////////////////
// Преобразования query
//////////////////////////////////////////////////////////////////////
QList<QMap<QString, QVariant> > ADatabase::selectList(QString querystring, QString conName) {
    //qDebug() << "ADatabase::selectList" << querystring << conName;
    QList<QMap<QString, QVariant> > result;

    QSqlQuery * query = ADatabase::newSelectSql(querystring, conName, "ADb:selectList");
    if (query==0) {return result;}

    QSqlRecord record;
    QMap<QString, QVariant> temprecord;
    bool a = query->next();
    while (a) {
        record = query->record();
        for (int i=0;i<record.count();i++) {temprecord[record.fieldName(i)] = record.value(i);}
        result.append(temprecord);
        temprecord.clear();
        a = query->next();
    }
    //qDebug() << "ADatabase::selectList" << querystring << conName;
    query->~QSqlQuery();
    //if (result.count()==0) {temprecord["0"]="0"; result.append(temprecord);}
    return result;
}
QMap<int, QMap<QString, QVariant> > ADatabase::selectMap(QString querystring, QString conName, QString sender) {
    //qDebug() << "ADatabase::selectMap" << querystring;
    QMap<int, QMap<QString, QVariant> > result;

    QSqlQuery * query = ADatabase::newSelectSql(querystring, conName, sender+"->ADb:selectMap");
    if (query==0) {return result;}

    bool a = query->next();
    if (!a) return result;
    int keycolumn = query->record().indexOf("id");
    if (keycolumn==-1) {qDebug() << "Warning! ADatabase::queryToMap - can't find keycolumn "+keycolumn; return result;}
    QMap<QString, QVariant> temprecord;
    QSqlRecord record;
    while (a) {
        record = query->record();
        for (int i=0;i<record.count();i++) { temprecord[record.fieldName(i)] = record.value(i); }
        result[record.value(keycolumn).toInt()] = temprecord;
        temprecord.clear();
        a = query->next();
    }
    query->~QSqlQuery();
    return result;
}
QVariantList ADatabase::selectVList(QString querystring)   {
    QVariantList result;

    QSqlQuery * query = ADatabase::newSelectSql(querystring);
    if (query==0) {return result;}

    QSqlRecord record;
    QMap<QString, QVariant> temprecord;
    bool a = query->next();
    while (a) {
        record = query->record();
        for (int i=0;i<record.count();i++) {temprecord[record.fieldName(i)] = record.value(i);}
        result.append(temprecord);
        temprecord.clear();
        a = query->next();
    }
    query->~QSqlQuery();
    return result;
}
QVariantMap ADatabase::selectVMap(QString querystring, QString keyfield)   {
    QVariantMap result;

    QSqlQuery * query = ADatabase::newSelectSql(querystring);
    if (query==0) {return result;}

    bool a = query->next();
    if (!a) return result;
    if (keyfield=="") {keyfield="id";}
    int keycolumn=query->record().indexOf(keyfield);
    if (keycolumn==-1) {qDebug() << "Warning! ADatabase::queryToVariantMap - can't find keyfield "+keyfield; return result;}
    QMap<QString, QVariant> temprecord;
    QSqlRecord record;
    while (a) {
        record = query->record();
        for (int i=0;i<record.count();i++) { temprecord[record.fieldName(i)] = record.value(i); }
        result[record.value(keycolumn).toString()] = temprecord;
        temprecord.clear();
        a = query->next();
    }
    //qDebug() << "ADatabase::selectVMap" << result;
    query->~QSqlQuery();
    return result;
}
QList<QString> ADatabase::selectFieldsList(QString querystring, QString conName) {
    QList<QString> result;
    conName = ADatabase::getConnectionName(conName);

    QSqlQuery * query = ADatabase::newSelectSql(querystring, conName, "ADb:selectFieldsList");
    if (query==0) {return result;}

    query->setForwardOnly(true);
    bool isRecord = query->next();
    if (isRecord) {
        QSqlRecord record = query->record();
        for (int i=0;i<record.count();i++) {result.append(record.fieldName(i));}
    }
    query->~QSqlQuery();
    //qDebug() << "ADatabase::getFieldsList" << result;
    return result;
}

//////////////////////////////////////////////////////////////////////
// Универсальные запросы второго уровня
//////////////////////////////////////////////////////////////////////
QVariant ADatabase::DLookUpQuery(QString field, QString querystring, QString conName, QString sender){
    QVariant result="";
    //qDebug() << "ADatabase::DLookUp" << querystring << conName;

    QSqlQuery * query = ADatabase::newSelectSql(querystring, conName, sender+"->ADb:dlookup");
    if (query==0) {return result;}

    bool isRecord = query->first();
    query->setForwardOnly(true);
    if (isRecord) {
        result = query->value(query->record().indexOf(field));
        if (result.type()==QVariant::DateTime) {result = AVariant::convertDateToString(result.toDate());}
    }
    query->~QSqlQuery();

    return result;
}

QVariant ADatabase::DLookUp(QString field, QString dict, QString criteria, QString conName, QString sender){
    QVariant result="";
    QString querystring = "SELECT "+field+" AS "+field+" FROM "+dict;
    if (!criteria.isEmpty()) {querystring.append(" WHERE " + criteria);}
    //qDebug() << "ADatabase::DLookUp" << querystring << conName;

    QSqlQuery * query = ADatabase::newSelectSql(querystring, conName, sender+"->ADb:dlookup");
    if (query==0) {return result;}

    bool isRecord = query->first();
    query->setForwardOnly(true);
    if (isRecord) {
        result = query->value(query->record().indexOf(field));
        if (result.type()==QVariant::DateTime) {result = AVariant::convertDateToString(result.toDate());}
    }
    query->~QSqlQuery();

    return result;
}
QVariant ADatabase::DMin(QString field, QString dict, QString criteria, QString conName){
    QVariant result;  

    QString querystring = "SELECT MIN("+field+") AS "+field+" FROM "+dict;
    if (!criteria.isEmpty()) {querystring.append(" WHERE " + criteria);}
    //qDebug() << "ADatabase::DMin" << querystring;

    QSqlQuery * query = ADatabase::newSelectSql(querystring, conName, "ADb:DMin");
    if (query==0) {return result;}

    bool isRecord = query->first();
    query->setForwardOnly(true);
    if (isRecord) {
        result = query->value(query->record().indexOf(field));
        if (result.type()==QVariant::DateTime) {result = AVariant::convertDateToString(result.toDate());}
    }
    query->~QSqlQuery();

    return result;
}
QVariant ADatabase::DMax(QString field, QString dict, QString criteria, QString conName){
    QVariant result;
    QString querystring = "SELECT MAX("+field+") AS "+field+" FROM "+dict;
    if (ADatabase::getConnectionDriver(conName)=="QSQLITE") {querystring = "SELECT MAX(ifnull("+field+",0)) AS "+field+" FROM "+dict;}
    if (!criteria.isEmpty()) {querystring.append(" WHERE " + criteria);}
    //qDebug() << "ADatabase::DMax" << querystring;

    QSqlQuery * query = ADatabase::newSelectSql(querystring, conName, "ADb:DMax");
    if (query==0) {return result;}

    bool isRecord = query->first();
    query->setForwardOnly(true);
    if (isRecord) {
        result = query->value(query->record().indexOf(field));
        if (result.type()==QVariant::DateTime) {result = AVariant::convertDateToString(result.toDate());}
    }
    query->~QSqlQuery();
    return result;
}
QVariant ADatabase::DSum(QString field, QString dict, QString criteria, QString conName){
    QVariant result;
    QString querystring = "SELECT SUM("+field+") AS "+field+" FROM "+dict;
    if (!criteria.isEmpty()) {querystring.append(" WHERE " + criteria);}

    QSqlQuery * query = ADatabase::newSelectSql(querystring, conName, "ADb:DSum");
    if (query==0) {return result;}

    bool isRecord = query->first();
    query->setForwardOnly(true);
    if (isRecord) {
        result = query->value(query->record().indexOf(field));
        if (result.type()==QVariant::DateTime) {result = AVariant::convertDateToString(result.toDate());}
    }
    query->~QSqlQuery();
    return result;
}
bool ADatabase::checkTable(QString tableName, QString conName) {
    //qDebug() << "ADatabase::checkTable" << tableName;
    QString querystring;
    if (tableName=="") {return false;}
    if (ADatabase::getConnectionDriver(conName)=="QODBC3")
        {querystring = "SELECT * FROM dbo.sysobjects WHERE xtype = 'U' AND name='" + tableName + "'";}
    if (ADatabase::getConnectionDriver(conName)=="QSQLITE")
        {querystring = "SELECT * FROM sqlite_master where name='"+tableName+"'";}

    QSqlQuery *query = ADatabase::newSelectSql(querystring, conName, "ADb:checktable");
    if (query==0) {return false;}

    bool isRecord = query->first();
    query->setForwardOnly(true);
    query->~QSqlQuery();
    if (!isRecord) {return false;}
    return true;
}
//////////////////////////////////////////////////////////////////////
// Service proc's
//////////////////////////////////////////////////////////////////////

QList<QVariant> ADatabase::getTableFields(QString tableName, QString conName) {
    QList<QVariant> result;
    QString query;
    //if (!ADatabase::checkTable(tableName, conName)) {return result;}
    if (ADatabase::getConnectionDriver(conName)=="QODBC3") {
        query = "SELECT name FROM syscolumns WHERE id = (SELECT id FROM sysobjects WHERE xtype='U' AND name='" + tableName + "')";
        QList<QMap<QString, QVariant> > tempList = ADatabase::selectList(query);
        for (int i=0;i<tempList.count();i++) {
            result.append(tempList.at(i)["name"]);
        }
    }
    if (ADatabase::getConnectionDriver(conName)=="QSQLITE") {
        query = "PRAGMA table_info(" + tableName + ");";
        QList<QMap<QString, QVariant> > tempList = ADatabase::selectList(query, conName);
        for (int i=0;i<tempList.count();i++) {
            result.append(tempList.at(i)["name"]);
        }
    }
    return result;
}

// !!! РЕАЛИЗОВАТЬ Сервис БД
bool ADatabase::reindexTables() {
    return ADatabase::execSql("EXEC dbo.reindex_db");
//    ADatabase::execSql("DBCC DBREINDEX('regDocs') WITH NO_INFOMSGS");
//    ADatabase::execSql("DBCC DBREINDEX('regDocsCont') WITH NO_INFOMSGS");
//    ADatabase::execSql("DBCC DBREINDEX('uniPluPartyStore_current') WITH NO_INFOMSGS");
//    ADatabase::execSql("DBCC DBREINDEX('uniPluSalesCost') WITH NO_INFOMSGS");
}
void ADatabase::shrinkDb() {
    ADatabase::execSql("DBCC ShrinkDatabase(Sales)");
}
QString ADatabase::masterServerReplication(QString tablename) {
    QString result="";
    int id = ADatabase::DLookUp("id", "[46.219.14.2].chameleon.dbo.sysobjects", "xtype = 'U' AND name='" + tablename + "'","", "ADatabase::masterServerReplication").toInt();
    if (id>0) {
//        ADatabase::execSql("EXEC sp_configure 'show advanced options', 1 "
//                           "reconfigure with override"
//                           "EXEC sp_configure 'Ad Hoc Distributed Queries', 1"
//                           "reconfigure with override"
//                           "EXEC sp_addlinkedserver '46.219.14.2', 'SQL Server'"
//                           "EXEC sp_addlinkedsrvlogin '46.219.14.2', 'true', NULL, 'sa', 'root'");
        QList<QMap<QString, QVariant> > flist1_temp = ADatabase::selectList("SELECT name FROM [46.219.14.2].chameleon.dbo.syscolumns WHERE id = (SELECT id FROM [46.219.14.2].chameleon.dbo.sysobjects WHERE xtype='U' AND name='" + tablename + "')");
        QList<QMap<QString, QVariant> > flist2_temp = ADatabase::selectList("SELECT name FROM syscolumns WHERE id = (SELECT id FROM sysobjects WHERE xtype='U' AND name='" + tablename + "')");

        QList<QString> fieldslist, flist1, flist2;
        for (int i=0;i<flist1_temp.count();i++) flist1 << flist1_temp.at(i)["name"].toString();
        for (int i=0;i<flist2_temp.count();i++) flist2 << flist2_temp.at(i)["name"].toString();
        for (int i=0;i<flist1.count();i++) {
            if (flist2.contains(flist1.at(i)) && flist1.at(i)!="id") fieldslist << flist1.at(i);
        }
        QString query = "MERGE "+tablename+" AS target "
                        "USING [46.219.14.2].chameleon.dbo."+tablename+" AS source "
                        "ON (target.id = source.id) "
                        "WHEN MATCHED THEN "
                        "UPDATE SET "+fieldslist.at(0)+"=source."+fieldslist.at(0)+" ";
        for (int i=1;i<fieldslist.count();i++) { query += ", "+fieldslist.at(i) + "=source."+fieldslist.at(i)+" ";}
        query += "WHEN NOT MATCHED THEN "
                 "INSERT (id ";
        for (int i=0;i<fieldslist.count();i++) { query += ", "+fieldslist.at(i)+" "; }
        query += ") "
                "VALUES (source.id";
        for (int i=0;i<fieldslist.count();i++) { query += ", source."+fieldslist.at(i)+" "; }
        query += "); ";
        //qDebug() << query;
        ADatabase::execSql("SET IDENTITY_INSERT "+tablename+" ON");
        ADatabase::execSql(query);
        ADatabase::execSql("SET IDENTITY_INSERT "+tablename+" OFF");
    } else {
        result = "Таблица "+tablename+" не найдена на сервере публикаций";
    }
    return result;
}

//////////////////////////////////////////////////////////////////////
// Transaction proc's
//////////////////////////////////////////////////////////////////////
void ADatabase::DebugCurrentStateOfNetworkConnections() {
    qDebug() << "";
    qDebug() << "--- Current Database State -----";

    qDebug() << "Active connections:" << QSqlDatabase::connectionNames();

    qDebug() << "defConnection:" << globals["defConnection"];
    qDebug() << "reserveConnection:" << globals["reserveConnection"];
    qDebug() << "deflocation, activelocation:" << globals["defLocation"] << globals["active Location"];

    qDebug() << "--------------------------------";
    qDebug() << "";
}
