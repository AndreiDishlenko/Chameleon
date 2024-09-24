#include "adoc.h"
#include "adatabase.h"
#include "adataset.h"
#include "afunc.h"
#include "adict.h"
#include "abuild.h"
#include "globals.h"
#include "aproduction.h"
#include "awires.h"
#include <QMap>

ADoc::ADoc(QObject *parent, QString sender) : QObject(parent) {
    //    AFunc::echoTimer("ADoc::ADoc 0");
    //qDebug() << "ADoc::ADoc("+parent->objectName()+")";
    //QMap<int, QMap<QString, QVariant> > stack;
    lasterror = "";
    usersMap = ADatabase::selectMap("SELECT id FROM dictUsers", "", "ADoc:ADoc("+parent->objectName()+", dictUsers)");
    doctypesMap = ADatabase::selectMap("SELECT id, ifDt, ifKt, ifStore, ifPay, prefix FROM dictDocumentType ", "", "ADoc:ADoc("+parent->objectName()+", dictDocumentType)");
    firmsMap = ADatabase::selectMap("SELECT id, idStore, idAcc, idAgent, idCurrency FROM dictFirms ", "", "ADoc:ADoc("+parent->objectName()+", dictFirms)");
    storesMap = ADatabase::selectMap("SELECT id FROM dictStores ", "", "ADoc:ADoc("+parent->objectName()+", dictStores)");
    accMap = ADatabase::selectMap("SELECT id, ifCashless FROM dictAccounts ", "", "ADoc:ADoc("+parent->objectName()+", dictAccounts)");
    currencyMap = ADatabase::selectMap("SELECT id, name, arate FROM dictCurrency", "", "ADoc:ADoc("+parent->objectName()+", dictCurrency)");
    regDocsDict = new ADict("regDocs", "", this);
    fieldslist  << "id" << "iddoc" << "iddoctype" << "date" << "number" << "idfirm" << "idagent" << "idto_acc"
                << "idfrom_acc" << "idcurrencyMap" << "sum" << "description" << "ifFinished" << "ifDeleted";
//    openedrecord.clear();
//    AFunc::echoTimer("ADoc::ADoc 7");
}
ADoc::~ADoc() {}

bool ADoc::newDoc(int iddoctype, int idfirm, int idagent) {
    //qDebug() << "ADoc::newDoc" << iddoctype << idfirm;
    bool result = true;
    regDocsDict->clearStack();
    regDocsDict->newRecord();
    //    openedrecord.clear();
    record.clear();
    // Set document type
    if (doctypesMap.keys().contains(iddoctype)) {
        record["iddoctype"] = iddoctype;
        ifDt = doctypesMap[iddoctype]["ifDt"].toBool();
        ifKt = doctypesMap[iddoctype]["ifKt"].toBool();
        ifPay = doctypesMap[iddoctype]["ifPay"].toBool();
    }
    else {
        qDebug() << "[ADoc::newDoc] Error! doc of type '"+QString::number(iddoctype)+"'' is not available. Break.";
        record.clear();
        return false;
    }
    ifDt = doctypesMap[iddoctype]["ifDt"].toBool();
    ifKt = doctypesMap[iddoctype]["ifKt"].toBool();
    ifPay = doctypesMap[iddoctype]["ifPay"].toBool();
    this->setUser();
    if (idfirm==0) {
        this->setDefFirm(globals["defFirm"].toInt());
    } else {
        this->setDefFirm(idfirm);
    }
    this->setDefCurrency();
    record["iddoc"] = 0;
    record["date"] = QDateTime::currentDateTime().date();
    record["prefix"] = doctypesMap[record["iddoctype"].toInt()]["prefix"];
    record["number"] = this->nextNumber();
    record["weight"] = 0;
    record["sum"] = float(0);
    record["nds"] = 0;
    record["description"] = "";
    record["ifFinished"] = 0;
    record["ifDeleted"] = 0;
    //regDocsDict->updateRecord(record);

    return result;
}
bool ADoc::openDoc(int id) {
    //    qDebug() << "ADoc::openDoc" << id;
    bool result=true;
    if (id!=0) {
        result = regDocsDict->openRecord(id);
        record = regDocsDict->getCurrentRecord();
        //        openedrecord = record;
        int iddoctype = record["iddoctype"].toInt();
        ifDt = doctypesMap[iddoctype]["ifDt"].toBool();
        ifKt = doctypesMap[iddoctype]["ifKt"].toBool();
        ifPay = doctypesMap[iddoctype]["ifPay"].toBool();
        //        if (!regDocsDict->getCRValue("id").isNull()) {return true;}
        //        QMap<int, QMap<QString, QVariant> > tempMap = ADatabase::selectMap("SELECT id, iddoc, iddoctype, date, number, idfirm, idagent, idto_acc, idfrom_acc, sum, description, ifFinished, ifDeleted FROM regDocsDict WHERE id='"+QString::number(id)+"'");
        //        if (tempMap.count()>0) {
        //            qDebug() << "abc" << tempMap.count() << tempMap[1]["id"].toString();
        //            record = tempMap[id];
        //            result = true;
        //        }
        //        else {lasterror = "Error open pay doc with id='"+QString::number(id)+"'";}
    }
    return result;
}

#include "avariant.h"
bool ADoc::commit() {
    //qDebug() << "ADoc::commit()" << record["id"];
    if (record["id"].isNull())  {
        int number = ADatabase::DLookUp("number", "regDocs", "idfirm='"+record["idfirm"].toString()+"' AND iddoctype='"+record["iddoctype"].toString()+"' AND number='"+record["number"].toString()+"'").toInt();
        if (number==0) {
            //qDebug() << "ADoc::commit() next number";
            record["number"] = this->nextNumber();
        }
    }
    if (!record["id"].isNull()) {
        QString changes, key;
        for (int i=0;i<record.keys().count();i++) {
            key = record.keys().at(i);
            if (!AVariant::compareTwoVariants(record[key], regDocsDict->getCRValue(key))) {if (changes!="") changes.append(", "); changes.append(key+": "+AVariant::convertVariantToString(regDocsDict->getCRValue(key))+"->"+AVariant::convertVariantToString(record[key]));}
        }
        ADoc::saveChangesLog(record["id"].toInt(), globals["currentUser"].toInt(), changes);
    }
    if (ifPay) {
        int idacc = 0;
        if (ifDt) {idacc = record["idto_acc"].toInt();}
        if (ifKt) {idacc = record["idfrom_acc"].toInt();}
        record["ifCashless"] = accMap[idacc]["ifCashless"];
    }
    regDocsDict->updateRecord(record);
    regDocsDict->saveChanges();
    record["id"] = regDocsDict->getCurrentRecord()["id"];
    //qDebug() << "ADoc::commit() new id:" << record["id"];
    ADatabase::execSql("EXECUTE fillDocsTop "+record["id"].toString());

    return true;
}

#include "adatabase.h"
void ADoc::saveChangesLog(int iddoc, int iduser, QString changes) {
//    ADatabs
    ADatabase::insertParam("regDocsLog", "iddoc, iduser, changes", QString::number(iddoc)+", "+QString::number(iduser)+", '"+changes+"'");
}

/*extern */void ADoc::wireDocInParralel(int iddoc) {
    //    qDebug() << "wireDocInParralel" << iddoc;
    ADoc *doc = new ADoc();
    if (iddoc!=0) {
        if (doc->openDoc(iddoc)) {
            doc->calcWireForCurrentDoc();
        } else {qDebug() << "[ADoc::wireDoc] Warning! Can't open doc with id="+QString::number(iddoc);}
    }
    doc->~ADoc();
    //    qDebug() << "end wireDocInParralel";
}

void ADoc::wireDoc(int iddoc) {
    //    qDebug() << "ADoc::wireDoc";
    if (iddoc==0 && (record["id"]=="" || record["id"].isNull())) {return;}
    if (iddoc==0 && (record["id"]!="" && !record["id"].isNull())) {iddoc = record["id"].toInt();}
    if (iddoc!=0) {
        if (!this->openDoc(iddoc)) {qDebug() << "[ADoc::wireDoc] Warning! Can't open doc with id="+QString::number(iddoc); return;}
    }
    //    qDebug() << "ADoc::wireDoc" << iddoc;
    this->updateParam("ifFinished", true);
    this->commit();

    QString iddoctype = this->getParam("iddoctype").toString();
    bool ifProduction = ADatabase::DLookUp("ifProduction", "dictDocumentType", "id='"+iddoctype+"'", "", "ADoc:wireDoc").toBool();
    if (ifProduction) {
        AWires::calcWireForProduction(iddoc);
    }
    if (iddoctype=="32") { AWires::calcWireForInventory(iddoc); }


//    QString querystring = "EXEC dbo.createDocWire "+QString::number(iddoc); //
//    ADatabase::execSql(querystring);

//    ADict *regDocsCont = new ADict("regDocsCont");
//    regDocsCont->loadRecords("iddoc='"+QString::number()+"'")
//    QFuture<void> future = QtConcurrent::run(wireDocInParralel, iddoc);
//    wireDocInParralel(iddoc);
}
void ADoc::unwireDoc(int iddoc) {
//    qDebug() << "ADoc::unwireDoc";
    if (iddoc==0 && (record["id"]=="" || record["id"].isNull())) {return;}
    if (iddoc==0 && (record["id"]!="" && !record["id"].isNull())) {iddoc = record["id"].toInt();}
    if (iddoc!=0) {
        if (!this->openDoc(iddoc)) {qDebug() << "[ADoc::unwireDoc] Warning! Can't open doc with id="+QString::number(iddoc); return;}
    }
    QString iddoctype = this->getParam("iddoctype").toString();
    bool ifProduction = ADatabase::DLookUp("ifProduction", "dictDocumentType", "id='"+iddoctype+"'", "", "ADoc:unwiredoc").toBool();
    if (ifProduction) {this->deleteSubdocs();}
    if (iddoctype=="32") { this->deleteSubdocs(); }
    this->updateParam("ifFinished", false);
    this->commit();
//    QString querystring = "EXEC dbo.createDocWire "+QString::number(iddoc); //
//    ADatabase::execSql(querystring);
//    QFuture<void> future = QtConcurrent::run(wireDocInParralel, iddoc);
//    wireDocInParralel(iddoc);

}

// Проводка текущего НЕ ПЛАТЕЖНОГО документа
bool ADoc::calcWireForCurrentDoc() {
//    qDebug() << "ADoc::calcWireForCurrentDoc()";
    bool result=true;
    QDate datedoc = QDate::fromString(record["date"].toString(),"dd.MM.yyyy");
    int idfirm=record["idfirm"].toInt();
    int iddoctype = record["iddoctype"].toInt();
    bool ifpay = doctypesMap[iddoctype]["ifpay"].toBool();
    if (ifpay) {
        qDebug() << "[ADoc::wireDoc] Warning! Can't wire pay doc with id="+record["id"].toString();
        return false;
    }
//    QList<QMap<QString, QVariant> > plulist = ADatabase::selectList("SELECT idPlu, idUnits FROM dbo.regDocsCont WHERE (idDoc = '"+record["id"].toString()+"') GROUP BY idPlu, idUnits");
//    QString conName = ADatabase::createConnection(glob_vars["dbType"]);
//    ADatabase::beginTransaction(glob_vars["reserveConnection"]);
//    qDebug() << "ADoc::calcWireForCurrentDoc plu count " << plulist.count() << AVariant::convertDateToString(datedoc);
//    if (ifDt) {
//        int idstore=record["idto_store"].toInt();
//        for (int i=0;i<plulist.count();i++) {if(!AWires::createPluWire(idfirm, idstore, plulist.at(i)["idPlu"].toInt(), plulist.at(i)["idUnits"].toInt(), datedoc, QDate(), glob_vars["reserveConnection"])) {result=false;}}
//    }
//    if (ifKt) {
//        int idstore=record["idfrom_store"].toInt();
//        for (int i=0;i<plulist.count();i++) {if (!AWires::createPluWire(idfirm, idstore, plulist.at(i)["idPlu"].toInt(), plulist.at(i)["idUnits"].toInt(), datedoc, QDate(), glob_vars["reserveConnection"])) {result=false;}}
//    }
//    if (result) {ADatabase::commitTransaction(glob_vars["reserveConnection"]);}
//    else {ADatabase::rollbackTransaction(glob_vars["reserveConnection"]); qDebug() << "Warning! ADoc::calcWireForCurrentDoc: errors in creating plu wire's. RollBack!";}
//    qDebug() << record["id"].toString();
    QString querystring = "EXEC dbo.createPluWire "+record["id"].toString(); //
    if (!ADatabase::execSql(querystring)) {result = false;}

    //    ADatabase::closeConnection(glob_vars["reserveConnection"]);
    return result;
}

//---------------------------------------------------------
// Работа с данными
//---------------------------------------------------------
bool ADoc::updateDoc(QMap<QString, QVariant> updateMap) {
    //qDebug() << "ADoc::updateDoc" << updateMap;
    QString key;
    QVariant value;
    int firmparam=-1;
    for (int i=0;i<updateMap.count();i++) {
        key = updateMap.keys().at(i);
        value = updateMap.values().at(i);
        if (key.toLower()=="idfirm") {firmparam=i; updateParam(key, value);}
    }
    for (int i=0;i<updateMap.count();i++) {
        key = updateMap.keys().at(i);
        value = updateMap.values().at(i);
        if (firmparam!=i) {updateParam(key, value);}
    }
    return true;
}
void ADoc::updateParam(QString key, QVariant value) {
    //qDebug() << "ADoc::updateParam" << key << value;
    if (key.toLower() == "idfirm") {this->setFirm(value.toInt());}
    else if (key.toLower() == "idagent") {this->setAgent(value.toInt());}
    else if (key.toLower() == "idfrom_store") {this->setFromStore(value.toInt());}
    else if (key.toLower() == "idto_store") {this->setToStore(value.toInt());}
    else if (key.toLower() == "idfrom_acc") {this->setFromAcc(value.toInt());}
    else if (key.toLower() == "idto_acc") {this->setToAcc(value.toInt());}
    else if (key.toLower() == "idcurrency") {this->setCurrency(value.toInt());}
    else if (key.toLower() != "id" && key.toLower() != "iddoctype") {record[key] = value;}
    else if (key.toLower() == "iddoc") {this->setCurrency(value.toInt());}
    else { record[key]=value; }
    //qDebug() << "ADoc::updateParam" << key << record[key];
}

QMap<QString, QVariant> ADoc::getDocData() {
    QMap<QString, QVariant> result = record;
    //qDebug() << "ADoc::getDocData" << record;
    //qDebug() << "";
    result["ifDt"] = ifDt;
    result["ifKt"] = ifKt;
    result["ifPay"] = ifPay;
    return result;
}
QVariant ADoc::getParam(QString key) {

    return record[key];
}

//---------------------------------------------------------
// Обновление полей документа
//---------------------------------------------------------
bool ADoc::setUser(int iduser) {
//    qDebug() << "setUser" << iduser << currentUser;
//    record["iduser"] =  ADatabase::DLookUp("id", "dictUsers", "id='"+QString::number(iduser)+"'").toInt();
    if (iduser!=0 && usersMap.keys().contains(iduser))
        {record["iduser"] = iduser;}
    else
        {record["iduser"] = globals["currentUser"].toInt();}
    return true;
}

// Устанавливается фирма/фирма по умолчанию
bool ADoc::setDefFirm(int idfirm) {
    if (idfirm==0) {
        idfirm = AApplication::getRegValue("defFirm").toInt();
    }
//    qDebug() << "ADoc::setDefFirm" << idfirm;
    if (!firmsMap.keys().contains(idfirm)) {
        this->setFirm(0);
        qDebug() << "[ADoc::setDefFirm] Warning! Can't find default firm. idfirm is setted to 0.";
        return false;
    }
    this->setFirm(idfirm);
    return true;
}
bool ADoc::setFirm(int idfirm) {
    if (!firmsMap.keys().contains(idfirm)) {
        qDebug() << "[ADoc::setFirm] Warning! Can't find idfirm="+QString::number(idfirm)+". idfirm doesn't changed.";
        return false;
    }
    record["idfirm"] = idfirm;
    this->setDefStores();
    this->setDefAcc();
    this->setDefAgent();
    return true;
}
// Устанавливает склады по умолчанию из справочника фирм (dictFirms), если определена фирма
// Устанавливает склады в соответствии и флагами ifDt, ifKt
bool ADoc::setDefStores() {
    record["idfrom_store"] = 0;
    record["idto_store"] = 0;
    int idfirm = record["idfirm"].toInt();
    if (idfirm==0) {
        qDebug() << "[Warning] ADoc::setDefStores idfirm=0. Default store is set to 0!";
        return false;
    }
    //int defstore = firmsMap[idfirm]["idStore"].toInt();
    int defstore = AApplication::getLocalSetting("defStore").toInt();
    bool ifstore = doctypesMap[record["iddoctype"].toInt()]["ifStore"].toBool();
    if (ifstore && ifKt) {this->setFromStore(defstore);}
    if (ifstore && ifDt) {this->setToStore(defstore);}
//    qDebug() << "ADoc::setDefStores" << record["idto_store"] << record["idfrom_store"];
    return true;
}
bool ADoc::setFromStore(int idstore) {
    if (record["iddoctype"].toInt()==0) {
        qDebug() << "[ADoc::setFromStore] Warning! Document type is 0, so can't set idfrom_store.";
        return false;
    }
    if (!storesMap.keys().contains( idstore )) {
        qDebug() << "[ADoc::setFromStore] Warning! Can't find " + QString::number(idstore) + " account, so can't set idfrom_store.";
        return false;
    }
    record["idfrom_store"] = idstore;
    //qDebug() << "ADoc::setFromStore" << idstore;
    return true;
}
bool ADoc::setToStore(int idstore) {
    //qDebug() << "ADoc::setToStore" << idstore;
    if (record["iddoctype"].toInt()==0) {
        qDebug() << "[ADoc::setToStore] Warning! Document type is 0, so can't set idfrom_store.";
        return false;
    }
    if (!storesMap.keys().contains( idstore )) {
        qDebug() << "[ADoc::setToStore] Warning! Can't find " + QString::number(idstore) + " account, so can't set idto_store.";
        return false;
    }
    record["idto_store"] = idstore;

    return true;
}

// Устанавливает счета по умолчанию из справочника фирм (dictFirms), если определена фирма
// Устанавливает счета в соответствии и флагами ifDt, ifKt
bool ADoc::setDefAcc() {
    record["idfrom_acc"]=0;
    record["idto_acc"]=0;
    int idfirm = record["idfirm"].toInt();
    if (idfirm==0) {
        qDebug() << "[ADoc::setDefAcc] Warning! idfirm=0. acc's is setted 0!";
        return false;
    }
    //int defaccount = firmsMap[idfirm]["idAcc"].toInt();
    int defaccount = AApplication::getLocalSetting("defAccount").toInt();
    this->setFromAcc(defaccount);
    this->setToAcc(defaccount);

    return true;
}
bool ADoc::setFromAcc(int idaccount) {
    if (record["iddoctype"].toInt()==0) {
        qDebug() << "[ADoc::setFromAcc] Warning! Document type is 0, so can't set idfrom_acc.";
        return false;
    }
    if (!accMap.keys().contains( idaccount )) {
        qDebug() << "[ADoc::setFromAcc] Warning! Can't find " + QString::number(idaccount) + " account. idfrom_acc not changed.";
        return false;
    }
    if (ifPay && ifKt) {record["idfrom_acc"] = idaccount;}
    //qDebug() << "ADoc::setFromAcc" << idaccount;
    return true;
}
bool ADoc::setToAcc(int idaccount) {
    if (record["iddoctype"].toInt()==0) {
        qDebug() << "[ADoc::setToAcc] Warning! Document type is 0, so can't set idto_acc.";
        return false;
    }
    if (!accMap.keys().contains( idaccount )) {
        qDebug() << "[ADoc::setToAcc] Warning! Can't find " + QString::number(idaccount) + " account. idto_acc not changed.";
        return false;
    }
    if (ifPay && ifDt) {record["idto_acc"] = idaccount;}
    //qDebug() << "ADoc::setToAcc" << idaccount;
    return true;
}

// Устанавливается контрагент/контрагент по умолчанию из справочника фирм (dictFirms)/из конфигурации
bool ADoc::setDefAgent() {
    record["idagent"] = 0;
    int idfirm = record["idfirm"].toInt();
    if (idfirm==0) {
        qDebug() << "[ADoc::setDefAgent] Warning! idfirm=0. idagent is setted 0!";
        return false;
    }
    int defagent = firmsMap[idfirm]["idAgent"].toInt();//ADatabase::DLookUp("id", "dictAccounts", "id='"+firmsMap[idfirm]["idAcc"].toString()+"'").toInt();
    if (!this->setAgent(defagent)) {
        qDebug() << "[ADoc::setDefAgent] Warning!  Can't find agent. idagent is setted 0!";
        return false;
    }
    return true;
}
bool ADoc::setAgent(int idagent) {
//    qDebug() << "SetAgent " << idagent;
    idagent = ADatabase::DLookUp("id", "dictAgents", "id='"+QString::number(idagent)+"'", "", "ADoc:setAgent").toInt();
    if (idagent==0) {
        qDebug() << "[ADoc::setAgent] Warning! Can't find " + QString::number(idagent) + " agent. idagent not changed.";
        return false;
    }
    record["idagent"] = idagent;

    return true;
}
bool ADoc::setDefCurrency() {
    record["idcurrency"] = 0;
    int idfirm = record["idfirm"].toInt();
    if (idfirm==0) {
        qDebug() << "[ADoc::setDefCurrency] Warning! idfirm=0. idcurrency is setted 0!";
        return false;
    }
    int defcurrency = firmsMap[idfirm]["idCurrency"].toInt();
//        qDebug() << "ADoc::setDefCurrency" << idfirm << firmsMap << defcurrency;
    if (!this->setCurrency(defcurrency)) {
        qDebug() << "[ADoc::setDefCurrency] Warning! Can't find currency. idcurrency is setted 0!";
        return false;
    }
    if (ifKt || ifDt) { record["arate"] = currencyMap[defcurrency]["arate"]; }
    return true;
}
bool ADoc::setCurrency(int idcurrency) {
//    qDebug() << "setCurrency" << idcurrency;
    if (currencyMap.keys().contains(idcurrency)) {
        record["idcurrency"] = idcurrency;
    }
    else {
        record["idcurrency"]=0;
    }
    return true;
}
//---------------------------------------------------------
// Public Slots functions: для работы с открытым документом
//---------------------------------------------------------
int ADoc::nextNumber() {
//    qDebug() << "ADoc::nextNumber()";
    int result = 0;
    if (record["idfirm"].toInt()!=0 && record["iddoctype"].toInt()!=0) {
        result = ADatabase::DMax("number", "regDocs", "idfirm='"+record["idfirm"].toString()+"' AND iddoctype='"+record["iddoctype"].toString()+"'").toInt()+1;
//        qDebug() << "nextNumber" << result;
    }
    return result;
}
void ADoc::echoRecord() {
    qDebug() << "ADoc::echoRecord()";
    for (int i=0;i<record.count();i++) {
        qDebug() << record.keys().at(i) << "=" << record.values().at(i);
    }
}

QList<int> ADoc::findSubDocs(int iddoc) {
    QList<int> result;
    ADict *dict = new ADict("regDocs");
    int id;
    if (iddoc==0) {iddoc = record["id"].toInt();}
    dict->loadRecords("iddoc='"+QString::number(iddoc)+"'");
    if (dict->count()>0) {
        for (int i=dict->count()-1;i>=0;i--) {
            id = dict->getCRValue("id", i).toInt();
            result.append(id);
            result.append(findSubDocs(id));
        }
    }
    dict->~ADict();
    return result;
}

void ADoc::deleteSubdocs(int iddoc) {
//    qDebug() << "ADoc::deleteSubdocs" << iddoc;
//    ADoc *doc_temp = new ADoc();
    int id;
    ADict *dict = new ADict("regDocs");
    if (iddoc==0) {iddoc = record["id"].toInt();}
    dict->loadRecords("iddoc='"+QString::number(iddoc)+"'");
    if (dict->count()>0) {
        for (int i=dict->count()-1;i>=0;i--) {
            id = dict->getCRValue("id", i).toInt();
    //        qDebug() << iddoc << dict->count() << i;
            deleteSubdocs(id);
//            doc_temp->openDoc(id);
//            doc_temp->de
//            bool ifFinished = doc_temp->getDocData()["ifFinished"].toBool();
//            if (ifFinished) {
//                doc_temp->updateParam("ifFinished", "0");
//                doc_temp->commit();
//                wireDocInParralel(id);
//            }
            dict->deleteCurrent(i);

        }
        dict->saveChanges();
    }
    dict->~ADict();
//    doc_temp->~ADoc();
}

