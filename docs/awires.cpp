#include "awires.h"
#include "qdebug.h"
#include "adoc.h"
#include "adict.h"
#include "adatabase.h"
#include "afunc.h"
#include "abuild.h"
#include "avariant.h"

AWires::AWires(QObject *parent) : QObject(parent) {
}

// Создание проводки по товару в текущем периоде
bool AWires::createPluWire(int idFirm, int idStore, int idPlu, int idUnit, QDate calcdate, QDate dateend, QString conName) {
//    qDebug() << "ABuild::createPluWire" << idFirm << idStore << idPlu << idUnit << calcdate << dateend << conName;
    bool result = true;
//    AFunc::echoTimer("ABuild::createPluWire for idPlu="+QString::number(idPlu)+", idUnits="+QString::number(idUnit));
    QMap<QString, QDate> dates = ABuild::getPeriodDates();
    if (dateend!=QDate()) {dates["end"] = dateend;}
    if (calcdate==QDate()) {calcdate = dates["begin"];}
//     << AVariant::convertDateToString(dates["begin"]) << AVariant::convertDateToString(dates["end"]) << AVariant::convertDateToString(calcdate);
    if (dates["begin"].isNull() || dates["end"].isNull() || dates["begin"]>dates["end"]) {qDebug() << "Error"; return false;}
    QString querystring = "EXEC dbo.createPluWire "+QString::number(idFirm)+", "+QString::number(idStore)+", "+QString::number(idPlu)+", "+QString::number(idUnit)+", '"+AVariant::convertDateToString(dates["begin"])+"'"+", '"+AVariant::convertDateToString(dates["end"])+"', '"+AVariant::convertDateToString(calcdate)+"'"; //
    if (!ADatabase::execSql(querystring, conName)) {result = false;}
//    qDebug() << "--- end of ABuild::createPluWire";
    return result;
}

bool AWires::calcWireForProduction(int iddoc) {
//    qDebug() << "AProduction::calcWireForDoc";
    bool result=false;

        ADoc *doc_writeoff = new ADoc(0, "AWires:calcWireForProduction");
        doc_writeoff->newDoc(14);
        doc_writeoff->commit();

        result = ADatabase::execSql("EXECUTE createProduction "+QString::number(iddoc)+", "+doc_writeoff->getParam("id").toString());
    //    ADatabase::execSql("EXECUTE createProduction "+QString::number(iddoc)+", "+doc_writeoff->getParam("id").toString());

        doc_writeoff->wireDoc();
        doc_writeoff->~ADoc();

//        AGui::msgBox("Производство было выполненно успешно. В случае образования \"минусовых\" остатков, откорректируйте склад и заново проведите производство.");

    return result;
}

bool AWires::calcWireForProduction_old(int iddoc) {
    qDebug() << "AProduction::calcWireForDoc";
    bool result=true;
    ADoc *doc_production = new ADoc();
    doc_production->openDoc(iddoc);

    ADoc *doc_writeoff = new ADoc();
    doc_writeoff->newDoc(14);
    doc_writeoff->setFirm(doc_production->getDocData()["idfirm"].toInt());
    doc_writeoff->updateParam("idfrom_store", doc_production->getDocData()["idfrom_store"]);
    doc_writeoff->updateParam("date", doc_production->getDocData()["date"]);
    doc_writeoff->updateParam("iddoc", doc_production->getParam("id"));
    doc_writeoff->updateParam("ifFinished", "1");
    doc_writeoff->commit();

    ADict *dict_production = new ADict("regDocsCont");
    ADict *dict_writeoff = new ADict("regDocsCont");
    ADict *dict_plucard = new ADict("uniPluCard");
    ADict *dict_plu = new ADict("dictPlu");

    QString idFirm = doc_production->getParam("idfirm").toString();
    QString idStore = doc_production->getParam("idfrom_store").toString();

    int idproduct, idUnits, idplu, i2, portionscount;
    float calcsum, sum, cardcount, count, newcount, outlet, partycount, partycost, writeoff_cnt;
    bool ifWeight;
    QString partyid;

    dict_production->loadRecords("idDoc='"+doc_production->getParam("id").toString()+"'");
    for (int i=0;i<dict_production->count();i++) {
        dict_production->setCurrentRecord(i);
        idproduct = dict_production->getCRValue("idPlu", i).toInt();
        qDebug() << "product:" << idproduct;
        dict_plu->openRecord(idproduct);
        dict_plucard->loadRecords("idProduct='"+QString::number(idproduct)+"'");
        calcsum = 0;
        for (int i1=0;i1<dict_plucard->count();i1++) {
            idplu = dict_plucard->getCRValue("idPlu", i1).toInt();
//            ifWeight = dict_plu->getCRValue("ifWeight").toBool();
            portionscount = dict_plu->getCRValue("portionscount").toInt();
            if (portionscount==0) { portionscount=1; }
            cardcount = dict_plucard->getCRValue("cardcount", i1).toFloat();
//            qDebug() << "component:" << idplu << cardcount;
            idUnits = dict_plucard->getCRValue("idUnits", i).toInt();
            count = dict_production->getCRValue("count", i).toFloat();
//            qDebug() << "AProduction::calcWireForDoc idplu from plucard " << idplu << count;
            newcount = 0;
//            qDebug() << "AProduction::calcWireForDoc ifWeight " << ifWeight << "brutto" << brutto << "portion" << portion << "count" << count  ;
            if (ifWeight) {
////                portion = dict_plu->getCRValue("portion").toFloat();
//                brutto = dict_plucard->getCRValue("brutto", i1).toFloat();
//                sumbrutto = dict_plu->getCRValue("brutto").toFloat();
//                if (sumbrutto!=0) {newcount = AFunc::round(brutto*count/sumbrutto, 2);} else {brutto=0;}
                newcount = AFunc::round(cardcount * count / outlet, 2);
            } else {
                newcount = AFunc::round(cardcount * count / portionscount , 2);
            }
            ADict *dict_store = new ADict("uniPluPartyStore_current");
            dict_store->loadRecords("idfirm='"+idFirm+"' AND idstore='"+idStore+"' AND idplu='"+QString::number(idplu)+"' AND idUnits='"+QString::number(idUnits)+"'");
//            qDebug() << "idfirm='"+idFirm+"' AND idstore='"+idStore+"' AND idplu='"+QString::number(idplu)+"' AND idUnits='"+idUnits+"'";
            i2=0;
            dict_store->setCurrentRecord(i2);
            partyid = dict_store->getCRValue("id").toString();
            partycount = dict_store->getCRValue("dtcount").toFloat();
            partycost = dict_store->getCRValue("price").toFloat();
//            qDebug() << "AProduction::readParty " << QString::number(idproduct) << QString::number(newcount) << QString::number(partycount) << QString::number(partycost);
            writeoff_cnt=0;
            while (newcount>0 && dict_store->count()>i2) {
                if (partycount>0) {
                    if (newcount>=partycount) {
                        writeoff_cnt = partycount;
                        newcount = newcount - partycount;
                        partycount = 0;
                    } else {
                        writeoff_cnt = newcount;
                        partycount = partycount-newcount;
                        newcount = 0;
                    }

                    sum = AFunc::round(writeoff_cnt*partycost, 2);
                    calcsum = calcsum + sum;
                    dict_writeoff->newRecord();
                    dict_writeoff->setCRValue("iddoc", doc_writeoff->getParam("id"));
                    dict_writeoff->setCRValue("idPlu", idplu);
                    dict_writeoff->setCRValue("count", writeoff_cnt);
                    dict_writeoff->setCRValue("idUnits", idUnits);
                    dict_writeoff->setCRValue("portion", 1);
                    dict_writeoff->setCRValue("price", partycost);
                    dict_writeoff->setCRValue("nds", 0);
                    dict_writeoff->setCRValue("sum", sum);
//                    qDebug() << "ADatabase::createPlu1 " << QString::number(writeoff_cnt) << QString::number(partycost);
                    dict_store->setCRValue("dtcount", partycount);

//                    ADatabase::updateParam("uniPluPartyStore_current", "dtcount='0'", "id='"+partyid+"'");
//                    qDebug() << "ADatabase::updateParam1 " << 0;
                }

                i2++;
                dict_store->setCurrentRecord(i2);
                partyid = dict_store->getCRValue("id").toString();
                partycount = dict_store->getCRValue("dtcount").toFloat();
                partycost = dict_store->getCRValue("price").toFloat();
//                qDebug() << "AProduction::readParty " << i2 << QString::number(partycount) << QString::number(partycost);
            }

            if (newcount>0) {
                dict_writeoff->newRecord();
                dict_writeoff->setCRValue("iddoc", doc_writeoff->getParam("id"));
                dict_writeoff->setCRValue("idPlu", idplu);
                dict_writeoff->setCRValue("count", newcount);
                dict_writeoff->setCRValue("idUnits", idUnits);
                dict_writeoff->setCRValue("portion", 1);
                dict_writeoff->setCRValue("price", 0);
                dict_writeoff->setCRValue("nds", 0);
                dict_writeoff->setCRValue("sum", 0);
//                qDebug() << "ADatabase::createPlu2 " << QString::number(newcount) << QString::number(0);
                partycount = partycount - newcount;
                if (partycount<0) {partycount = 0;}
                dict_store->setCRValue("dtcount", QString::number(partycount));
//                ADatabase::updateParam("uniPluPartyStore_current", "dtcount='"+QString::number(partycount)+"'", "id='"+partyid+"'");

            }
            dict_store->saveChanges();
            dict_store->~ADict();
        }

        dict_production->setCRValue("sum", calcsum);
        float calcprice = 0;
        float productcount = dict_production->getCRValue("count", i).toFloat();
        if (productcount!=0) { calcprice = calcsum / productcount; }
        dict_production->setCRValue("price", calcprice);
    }

    sum=0;
    dict_production->saveChanges();
    for (int i=0;i<dict_production->count();i++) {sum = sum + dict_production->getCRValue("sum", i).toFloat();}
    dict_production->~ADict();
//    qDebug() << "sum1: " << QString::number(sum);
    doc_production->updateParam("sum", sum);
    doc_production->commit();
    doc_production->~ADoc();

    sum=0;
    dict_writeoff->saveChanges();
    for (int i=0;i<dict_writeoff->count();i++) {sum = sum + dict_writeoff->getCRValue("sum", i).toFloat();}
    dict_writeoff->~ADict();
//    qDebug() << "sum2: " << QString::number(sum);
    doc_writeoff->updateParam("sum", sum);
    doc_writeoff->commit();
    doc_writeoff->~ADoc();

    dict_plucard->~ADict();
    dict_plu->~ADict();

    return result;
}

bool AWires::calcWireForInventory(int iddoc) {
//    qDebug() << "AWires::calcWireForInventory" << iddoc;
    bool result=true;

    ADatabase::deleteParam("regDocs", "iddoc='"+QString::number(iddoc)+"'");

    ADoc *doc_inventory = new ADoc();
    doc_inventory->openDoc(iddoc);

    QList< QMap<QString, QVariant> > writeoff;
    QList< QMap<QString, QVariant> > comeon;

    QMap<QString, QVariant> temp;
    QList< QMap<QString, QVariant> > list_writeoff;
    QList< QMap<QString, QVariant> > list_comeon;
    float price, remain, present;

    ADict *dict_inventory = new ADict("regDocsInventory");
    dict_inventory->loadRecords("idDoc='"+QString::number(iddoc)+"'");
    for (int i=0;i<dict_inventory->count();i++) {
        if (dict_inventory->getCRValue("present", i).isNull()) continue;
        price = dict_inventory->getCRValue("price", i).toFloat();
        remain = dict_inventory->getCRValue("remain", i).toFloat();
        present = dict_inventory->getCRValue("present", i).toFloat();
        if (remain>present) {
            temp["idPlu"] = dict_inventory->getCRValue("idPlu", i).toInt();;
            temp["count"] = remain-present;
            temp["idUnits"] = dict_inventory->getCRValue("idUnits", i).toFloat();
            temp["portion"] = 1;
            temp["price"] = price;
            temp["sum"] = (remain-present)*price;
            list_writeoff.append(temp);
//
        }
        if (remain<present) {

            temp["idPlu"] = dict_inventory->getCRValue("idPlu", i).toInt();;
            temp["count"] = present-remain;
            temp["idUnits"] = dict_inventory->getCRValue("idUnits", i).toFloat();
            temp["portion"] = 1;
            temp["price"] = price;
            temp["sum"] = (present-remain)*price;
            list_comeon.append(temp);
//            dict_comeon->newRecord();
//            dict_comeon->updateRecord(temp);
        }
    }
    dict_inventory->~ADict();

    float sum=0;
//    qDebug() << "AWires::calcWireForInventory" << list_writeoff.count();
    if (list_writeoff.count()>0) {
        ADoc *doc_writeoff = new ADoc();
        doc_writeoff->newDoc(18);
        doc_writeoff->updateParam("idfirm", doc_inventory->getDocData()["idfirm"]);
        doc_writeoff->updateParam("date", doc_inventory->getDocData()["date"]);
        doc_writeoff->updateParam("idfrom_store", doc_inventory->getDocData()["idfrom_store"]);
        doc_writeoff->updateParam("from_person", doc_inventory->getDocData()["from_person"]);
        doc_writeoff->updateParam("iddoc", doc_inventory->getParam("id"));
//        qDebug() << "AWires::calcWireForInventory 1" << doc_inventory->getDocData()["date"];
        doc_writeoff->updateParam("ifFinished", "1");
        doc_writeoff->commit();
        ADict *dict_writeoff = new ADict("regDocsCont");
        for (int i=0;i<list_writeoff.count();i++) {
            dict_writeoff->newRecord();
//            list_writeoff.at(i)["idDoc"] = doc_writeoff->getParam("id");
            dict_writeoff->setCRValue("idDoc", doc_writeoff->getParam("id"));
            dict_writeoff->updateRecord(list_writeoff.at(i));
        }
        dict_writeoff->saveChanges();
        sum=0;
        for (int i=0;i<dict_writeoff->count();i++) {sum = sum + dict_writeoff->getCRValue("sum", i).toFloat();}
        doc_writeoff->updateParam("sum", sum);
        doc_writeoff->commit();
        dict_writeoff->~ADict();
        doc_writeoff->~ADoc();
    }

    if (list_comeon.count()>0) {
        ADoc *doc_comeon = new ADoc();
        doc_comeon->newDoc(19);
        doc_comeon->updateParam("idfirm", doc_inventory->getDocData()["idfirm"]);
        doc_comeon->updateParam("date", doc_inventory->getDocData()["date"]);
        doc_comeon->updateParam("idto_store", doc_inventory->getDocData()["idfrom_store"]);
        doc_comeon->updateParam("to_person", doc_inventory->getDocData()["from_person"]);
        doc_comeon->updateParam("iddoc", doc_inventory->getParam("id"));
        doc_comeon->updateParam("ifFinished", "1");
//        qDebug() << "AWires::calcWireForInventory 2" << doc_inventory->getDocData()["date"];
        doc_comeon->commit();
        ADict *dict_comeon = new ADict("regDocsCont");
        for (int i=0;i<list_comeon.count();i++) {
            dict_comeon->newRecord();
            dict_comeon->setCRValue("idDoc", doc_comeon->getParam("id"));
//            dict_comeon.at(i)["idDoc"] = doc_comeon->getParam("id");
            dict_comeon->updateRecord(list_comeon.at(i));
        }
        dict_comeon->saveChanges();
        sum=0;
        for (int i=0;i<dict_comeon->count();i++) {sum = sum + dict_comeon->getCRValue("sum", i).toFloat();}
        doc_comeon->updateParam("sum", sum);
        doc_comeon->commit();
        dict_comeon->~ADict();
        doc_comeon->~ADoc();
    }


    return result;

}
