#include "QApplication"
#include "abuild.h"
#include "adatabase.h"
#include "adict.h"
#include "adoc.h"
#include "afunc.h"
#include "awires.h"
#include "globals.h"
#include "avariant.h"

ABuild::ABuild(QObject *parent) : QObject(parent) {
    this->setObjectName("ABuild");
}

// Определяет учетный период по заданой дате
int ABuild::getPeriodByDate(QDate date) {
//    qDebug() << "ABuild::getPeriodByDate";
    ADict *dictObj = new ADict("dictPeriods");
    dictObj->loadRecords("dateend>='"+AVariant::convertDateToString(date)+"'");
    if (dictObj->count()==0) {
        return 0;
    } else {
        return dictObj->getCRValue("id", 0).toInt();
    }
    dictObj->~ADict();
    return -1;
}

// Определяет дату начала и дату конца указаного периода
QMap<QString, QDate> ABuild::getPeriodDates(int idPeriod) {
//    qDebug() << "ABuild::getPeriodDates";
    QMap<QString, QDate> result;
    ADict *dictObj = new ADict("dictPeriods");
    dictObj->loadRecords();
    if (idPeriod==0) {
        if (dictObj->count()>0) {result["begin"] = dictObj->getCRValue("dateend", dictObj->count()-1).toDate().addDays(1);}
        else {result["begin"] = ADatabase::DMin("date", "regDocs", "").toDate();}
        result["end"] = ADatabase::DMax("date", "regDocs", "").toDate();
    } else {
        if (dictObj->findRecord(idPeriod)) {
            int cr = dictObj->currentRecordI;
            if (cr>0) {
                result["begin"] = dictObj->getCRValue("dateend", cr-1).toDate().addDays(1);
            } else {
                result["begin"] = ADatabase::DMin("date", "regDocs", "").toDate();
            }
            result["end"] = dictObj->getCRValue("dateend").toDate();
        } else {
            return QMap<QString, QDate>();
        }
    }
    dictObj->~ADict();
    return result;
}
QString ABuild::getPeriodMinDate(int idPeriod){
//    qDebug() << "ABuild::getPeriodMinDate";// << ABuild::getPeriodDates(idPeriod)["begin"].toString("dd.MM.yyyy");
    return AVariant::convertDateToString(ABuild::getPeriodDates(idPeriod)["begin"]);
}
QString ABuild::getPeriodMaxDate(int idPeriod){
//    qDebug() << "ABuild::getPeriodMaxDate";// << ABuild::getPeriodDates(idPeriod)["end"].toString("dd.MM.yyyy");
    return AVariant::convertDateToString(ABuild::getPeriodDates(idPeriod)["end"]);
}

bool ABuild::fillBase() {
//    qDebug() << "ABuild::fillBase";
    QSqlDatabase::database().close();
    QSqlDatabase::database().open();
//    int idFirm=1;
//    int idStore=1;
    ADict *dictPlu = new ADict("dictPlu");
    dictPlu->loadRecords("ifDish=0 AND ifDeleted=0");
    QMap<int, int> pluSale, pluIncome;
    for (int i=0;i<dictPlu->count();i++) {
        pluSale[dictPlu->getCRValue("id", i).toInt()] = 11000;
        pluIncome[dictPlu->getCRValue("id", i).toInt()] = 11594;
    }
    ADict *dictDocsCont = new ADict("regDocsCont");
    ADoc *doc = new ADoc(this, "ABuild:fillBase("+this->parent()->objectName()+")");
    int plupos=0;
    int plucount;
//    int daycount=1;
    QMap<QString, QVariant> updateMap;
//    for (int daycount=1;daycount<=30;daycount++) {
//    qDebug() << "day" << daycount;

//    qDebug() << pluIncome;
    for (int i=1;i<=31;i++) {
        AFunc::echoTimer("bill № "+QString::number(i));
        doc->newDoc(1, globals["defFirm"].toInt());                                          // PAUSE 40-50
        updateMap.clear();
        updateMap["date"] = QString::number(i)+".06.2011";
        updateMap["ifFinished"] = "1";
        doc->updateDoc(updateMap);
        doc->commit();                                          // PAUSE 20
        updateMap.clear();
        updateMap["idDoc"] = doc->getDocData()["id"];
        plucount=0;
        dictDocsCont->clearStack();
        for (int plupos=0;plupos<pluIncome.count();plupos++) {
            dictDocsCont->newRecord();
            updateMap["idPlu"] = pluIncome.keys().at(plupos);
            updateMap["count"] = "374";
            updateMap["idUnits"] = "3";
            updateMap["price"] = dictPlu->getCRValue("id", plupos).toInt()+1.8;
            updateMap["sum"] = updateMap["count"].toDouble()*updateMap["price"].toDouble();
            dictDocsCont->updateRecord(updateMap);
         }
        dictDocsCont->saveChanges();                                // PAUSE 110-120
    }
    int billcount = 1;
    for (int i1=1;i1<271;i1++) {
        QSqlDatabase::database().close();
        QSqlDatabase::database().open();
        for (int i=1;i<=101;i++) {
            AFunc::echoTimer("bill № "+QString::number(billcount));
            billcount++;
            doc->newDoc(5, globals["defFirm"].toInt());                                          // PAUSE 40-50
            updateMap.clear();
            updateMap["date"] = QString::number(qrand()%31)+".06.2011";
            updateMap["ifFinished"] = "1";
            doc->updateDoc(updateMap);
            doc->commit();                                          // PAUSE 20
            updateMap.clear();
            updateMap["idDoc"] = doc->getDocData()["id"];
            plucount=0;
            dictDocsCont->clearStack();
            while (plucount<5) {
                dictDocsCont->newRecord();
                updateMap["idPlu"] = pluSale.keys().at(plupos);
                updateMap["count"] = "2.2";
                updateMap["idUnits"] = "3";
                updateMap["price"] = updateMap["idPlu"].toInt()+2.34;
                updateMap["sum"] = updateMap["count"].toDouble()*updateMap["price"].toDouble();
                dictDocsCont->updateRecord(updateMap);
                if (plupos<pluSale.count()-1) {plupos++;}
                else {plupos=0;}
                plucount++;
            }
            dictDocsCont->saveChanges();                                // PAUSE 110-120
        }
    }
    dictDocsCont->~ADict();
    doc->~ADoc();
    return true;
}
bool ABuild::fillBase2() {
//    qDebug() << "ABuild::fillBase2";
    QString query1 = "INSERT INTO regDocs (date, description, idagent, idcurrency, iddoc, iddoctype, idfirm, idfrom_acc, idfrom_store, idto_acc, idto_store, iduser, ifDeleted, ifFinished, nds, number, prefix, sum, weight) VALUES ('1.07.2011', '', '4', '0', '0', '5', '1', '0', '1', '0', '0', '0', '0', '1', '0', '1366', 'РН', '0', '0')";
    QString query2 = "INSERT INTO regDocsCont (count, idDoc, idPlu, idUnits, price, sum) VALUES ('2.3', '2495', '18', '3', '18.34', '0')";
    QSqlQuery query;
    for (int i=0;i<100;i++) {
        AFunc::echoTimer("bill "+QString::number(i));
        QSqlDatabase::database ().transaction ();
        query.exec(query1);
        query.exec(query2);
        query.exec(query2);
        query.exec(query2);
        query.exec(query2);
        query.exec(query2);
        QSqlDatabase::database ().commit();
    }
    return true;
}

// ПЕРИОДЫ
bool ABuild::closeCurrentPeriod(int idFirm, int idStore, QString name, QDate dateend) {
//    ABuild::createCurrentPartyStore(idFirm, idStore, dateend);
    bool result=true;
//!!    ABuild::calculateResults(idFirm, idStore, AVariant::dateend);
    ADict *dictPeriods = new ADict("dictPeriods");
        dictPeriods->newRecord();
        QMap<QString, QVariant> updateMap;
        updateMap["idFirm"] = idFirm;
        updateMap["idStore"] = idStore;
        updateMap["name"] = name;
        updateMap["dateend"] = dateend;
        dictPeriods->updateRecord(updateMap);
        if (!dictPeriods->saveChanges()) {result=false;}
        if (dictPeriods->getCRValue("id").toInt()==0) {qDebug() << "ABuild::closeCurrentPeriod. Error add record in dictPeriods"; return false;}
        int idPeriod = dictPeriods->getCRValue("id").toInt();
    dictPeriods->~ADict();
    if (!ADatabase::execSql("INSERT INTO [dbo].[uniPluPartyStore_arch](idperiod, idfirm, idstore, iddoc, date, idplu, dtcount, idUnits, price) SELECT "+QString::number(idPeriod)+" AS idperiod, idfirm, idstore, iddoc, date, idplu, dtcount, idUnits, price FROM uniPluPartyStore_current")) {
        qDebug() << "ABuild::closeCurrentPeriod. Error in transfer data from uniPluPartyStore_current to uniPluPartyStore_arch.";
        result=false;
    }
    return result;
}
bool ABuild::openLastPeriod(int idFirm, int idStore) {
    bool result = true;
    int idMaxPeriod = ADatabase::DMax("id", "dictPeriods", "idFirm='"+QString::number(idFirm)+"' AND idStore='"+QString::number(idStore)+"'").toInt();

    ADatabase::beginTransaction();

    if (!ADatabase::deleteParam("dictPeriods", "id='"+QString::number(idMaxPeriod)+"'")) {result=false;}
    if (!ADatabase::deleteParam("uniPluPartyStore_arch", "idPeriod='"+QString::number(idMaxPeriod)+"' AND idFirm='"+QString::number(idFirm)+"' AND idStore='"+QString::number(idStore)+"'")) {result=false;}

    if (result) {
        ADatabase::commitTransaction();
        ABuild::calculateResults(idFirm, idStore);
    }
    else {ADatabase::rollbackTransaction(); qDebug() << "Warning! ABuild::openLastPeriod: errors in open period. RollBack!";}

    return result;
}
// Перерасчет партий и себистоимостей для ВСЕХ товаров в данном периоде по конкретную дату
bool ABuild::calculateResults(int idFirm, int idStore, QString calcdate) {
//    qDebug() << "ABuild::calculateResults";
//    bool result = true;
//    QMap<QString, QDate> dates = ABuild::getPeriodDates();
    if (calcdate=="") {calcdate = ABuild::getPeriodMinDate();}
//    QList<QMap<QString, QVariant> > pluList = ADatabase::selectList("SELECT idPlu, idUnits FROM dbo.unionPluMove WHERE (idfirm = "+QString::number(idFirm)+") AND (idstore = "+QString::number(idStore)+") AND (date>='"+dates["begin"].toString("dd.MM.yyyy")+"') AND (date<='"+dates["end"].toString("dd.MM.yyyy")+"') GROUP BY idPlu, idUnits");
//    QString conName = ADatabase::createConnection();
//    ADatabase::beginTransaction(conName);
    progressbar->setVisible(true);
    progressbar->setValue(10);
//    for (int i=0;i<pluList.count();i++) {
//        progressbar->setValue(i*100/pluList.count());
//        QApplication::processEvents();
//        if (!AWires::createPluWire(idFirm, idStore, pluList.at(i)["idPlu"].toInt(), pluList.at(i)["idUnits"].toInt(), QDate(), dateend, conName))
//            {result = false;}
//    }
    bool result = ADatabase::execSql("EXECUTE calculatePluSalesCost "+QString::number(idFirm)+", "+QString::number(idStore)+", '"+calcdate+"'");
//    qDebug() << "EXECUTE calculatePluSalesCost "+QString::number(idFirm)+", "+QString::number(idStore)+", '"+calcdate+"'";
//    progressbar->setValue(100);
//    if (result) {ADatabase::commitTransaction(conName);}
//    else {ADatabase::rollbackTransaction(conName); qDebug() << "Warning! ABuild::calculateResults: errors in creating plu wire's. RollBack!";}
//    ADatabase::closeConnection(conName);

    progressbar->setVisible(false);
//    progressbar->setValue(0);
//    QApplication::processEvents();
    return result;
}


bool ABuild::destroyDBDocs() {
    ADatabase::execSql("DELETE FROM regDocsCont");
    ADatabase::execSql("DELETE FROM regDocs");
    ADatabase::execSql("DELETE FROM uniPluPartyStore_current");
    ADatabase::execSql("DELETE FROM uniPluPartyStore_arch");
    ADatabase::execSql("DELETE FROM uniPluSalesCost");
    ADatabase::execSql("DELETE FROM dictPeriods");
    return true;
}

bool ABuild::admin_clearRegDocs() {
    return ADatabase::execSql("DELETE FROM regDocs WHERE ifDeleted='true' AND ifFinished='false'");
}



