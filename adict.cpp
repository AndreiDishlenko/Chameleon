////////////////////////////////////////////////////////////////////////////////////
///
/// Класс для работы с таблицей dictName
///
/// currentTable - таблица в БД
/// currentTableFields - поля из таблиы в БД
///
////////////////////////////////////////////////////////////////////////////////////

#include "adict.h"
#include "afunc.h"
#include "adatabase.h"
#include "globals.h"

ADict::ADict(QString dictName, QString fields, QObject *parent, QString conName) : QObject(parent){
    QObject *obj = 0;
    if (parent!=0) {
        obj = parent->parent();
    }
    //qDebug() << "ADict::ADict" << this << dictName << parent << obj;
    this->setObjectName("ADict");
    if (conName=="") { conName = globals["defConnection"]; }
    setCurrentConnection(conName);
    //    qDebug() << "ADict::ADict" << dictName << conName << parent;
    if (dictName!="") {setCurrentDict(dictName, fields);}
    this->clearStack();
    //qDebug() << "ADict::ADict end";
}
ADict::~ADict() {
    //qDebug() << "~ADict";
}

void ADict::setCurrentConnection(QString conName) {
    //qDebug() << "ADict::setCurrentConnection" << conName;
    connection = conName;
}

bool ADict::setCurrentDict(QString dictName, QString fields) {
    //qWarning() << "ADict::setCurrentDict" << dictName << connection;
    if (!ADatabase::checkTable(dictName, connection)) {
        qWarning() << "[Warning] ADict::setCurrentDict" << dictName << "table is not found." << connection ;
        currentTable = "";
        currentTableFields.clear();;
        this->clearStack();
        return false;
    } else {
        currentTable = dictName;
        currentTableFields = ADatabase::getTableFields(dictName, connection);
        this->setWorkFields(fields);
        this->clearStack();
        return true;
    }
}

void ADict::setWorkFields(QString fields) {
    query_workfields = "";
    if (fields!="") {
        QList<QString> fieldsList = AFunc::strToList(fields);
        for (int i=0;i<fieldsList.count();i++) {
            if (currentTableFields.contains(fieldsList.at(i))) {
                if (query_workfields != "") {query_workfields.append(", ");}
                query_workfields.append(fieldsList.at(i));
            }
        }
    }
    //qDebug() << "ADict::setWorkFields" << query_workfields;
    if (query_workfields=="") {query_workfields="*";}
}

void ADict::clearStack() {
    stack.clear();
    stack_todelete.clear();
    currentRecordI = 0;
    QMap<QString, QVariant> temp;
    stack.append(temp);
}

void ADict::newRecord(/*QMap<QString, QVariant>*/) {
//    qDebug() << "ADict::newRecord" << currentRecordI << stack.count();
    if (stack.count()==1 && stack.at(0).count()==0) {return;}
//    qDebug() << "1";
    if (currentRecordI < stack.count()) {
//        qDebug() << "ADict::newRecord" << currentRecordI;
        currentRecordI = stack.count();
        QMap<QString, QVariant> temp;
        stack.append(temp);
    }
//    qDebug() << "2";
//    this->inStack();
}

bool ADict::openRecord(int id) {
//    qDebug() << "ADict::openRecord" << id;
    this->clearStack();
    this->deleteCurrent();
    if (!this->addRecord(id)) { this->newRecord(); return false;}
    return true;
}
bool ADict::addRecord(int id){
    //qDebug() << "ADict::addRecord" << currentTable << id;
    if (currentTable=="") {return false;}
    if (id!=0) {
        //qDebug() << "ADict::addRecord 1";
        QMap<int, QMap<QString, QVariant> > tempMap = ADatabase::selectMap("SELECT "+query_workfields+" FROM " + currentTable + " WHERE id='"+QString::number(id)+"'", connection, "ADict:addrecord");
        //qDebug() << "ADict::addRecord" << tempMap << "SELECT "+query_workfields+" FROM " + currentTable + " WHERE id='"+QString::number(id)+"'";
        if (tempMap[id].isEmpty()) {lasterror = "Error open pay doc with id='"+QString::number(id)+"'"; return false;}
//        this->inStack();
        this->newRecord();
//        qDebug() << "addRecord" << tempMap[id];
        stack.replace(currentRecordI, tempMap[id]);
    }
    return true;
}
bool ADict::loadRecords(QString whereString, QMap<QString, QVariant> whereMap, QString orderby) {
    //qDebug() << "ADict::loadRecords" << currentRecordI << stack.count();
    if (currentTable=="") {return false;}
    this->clearStack();
    stack.clear();
    QString where;
    if (whereString=="") {
        //qDebug() << "ADict::loadRecords wherestring==" << currentTableFields << whereMap;
        for (int i=0;i<whereMap.count();i++) {
            if (currentTableFields.contains(whereMap.keys().at(i))) {
                if(where!="") {where.append(" AND ");}
                where.append(" "+whereMap.keys().at(i)+"='"+whereMap.values().at(i).toString()+"'");
            }
        }
    }
    else {
        where = whereString;
    }
    if (where!="") {where.prepend(" WHERE ");}
    if (orderby!="") {orderby.prepend(" ORDER BY ");}
    else {orderby=" ORDER BY id";}
    QString querystring = "SELECT "+query_workfields+" FROM " + currentTable + where + orderby;

    stack = ADatabase::selectList(querystring, connection);
    //    this->setCurrentRecord(0);
    //    if (stack.count()==0) {currentRecordI = -1;}
    //    else { currentRecordI = 0; }
    //    ADatabase::selectList(querystring);
    //    currentRecordI = stack.count()-1;

    //    qDebug() << "ADict::loadRecords2 " << currentRecordI << stack.count();
    //    qDebug() << "ADict::loadRecords" << stack.count() << querystring;
    this->setCurrentRecord(0);
//    currentRecordI = stack.count()-1;

//    qDebug() << "ADict::loadRecords2 " << currentRecordI << stack.count();
//    qDebug() << "ADict::loadRecords" << stack.count() << querystring;
//    if (!stack.isEmpty()) {this->setCurrentRecord(0);}

    return true;
}

bool ADict::setCurrentRecord(int i) {
    if (i>=0 && i<stack.count()) {
        currentRecordI = i;
    } else {
        currentRecordI = -1;
        return false;
    }
    return true;
}
bool ADict::findRecord(int id) {
    for (int i=0;i<stack.count();i++) {
        if (id == getCRValue("id", i).toInt()) {
            this->setCurrentRecord(i);
            return true;
        }
    }
    return false;
}
QMap<QString, QVariant> ADict::getCurrentRecord(int i) {
//    qDebug() << "ADict::getCurrentRecord" << stack.count();
    if (i==-1 && currentRecordI!=-1) {return stack.at(currentRecordI);}
    if (i>=0 && i<stack.count()) {return stack.at(i);}
    QMap<QString, QVariant> result;
    return result;
}

QVariant ADict::getCRValue(QString key, int record_i) {
    QVariant result;
    if (record_i>=0 && record_i<stack.count()) {return stack.at(record_i)[key];}
    if (currentRecordI!=-1  && stack.count()!=0) {return stack.at(currentRecordI)[key];}
    return result;
}
void ADict::setCRValue(QString key, QVariant value) {
//    qDebug() << "ADict::setCRValue";
//    qDebug() << "ADict::setCRValue" << stack.count();
    QMap<QString, QVariant> tempMap;
    if (currentRecordI!=-1) {
        tempMap = stack.at(currentRecordI);
        tempMap[key] = value;
        stack.replace(currentRecordI, tempMap);
    }
}

bool ADict::updateRecord(QMap<QString, QVariant> updateMap) {
    //    qDebug() << "ADict::updateRecord" << updateMap;
    //    qDebug() << "ADict::updateRecord 2" << currentTableFields;
    QVariant data;
    //    qDebug() << "ADict::updateRecord" << currentTable;
    if (currentTable=="") {return false;}
    //    qDebug() << "ADict::updateRecord" << currentTableFields;
    for (int i=0;i<updateMap.count();i++) {
        if (currentTableFields.contains(updateMap.keys().at(i))) {
            QString typeName =updateMap.values().at(i).typeName();
            if (typeName=="QDate")
                {data = updateMap.values().at(i).toDate().toString(Qt::SystemLocaleDate);}
            else
                {data = updateMap.values().at(i);}
            this->setCRValue(updateMap.keys().at(i), data);
        }
        else {
            qDebug() << "[WARNING] ADict::updateRecord. Table '"+currentTable+"' doesn't contains field '"+updateMap.keys().at(i)+"'";
        }
    }
    //    qDebug() << "ADict::updateRecord " << this->getCRValue("iddoctype");
    return true;
}

bool ADict::deleteCurrent(int i) {
//    qDebug() << "ADict::deleteCurrent" << stack.count() << i;
    if (currentTable=="" || stack.count()==0) {return false;}
    int deleterow = currentRecordI;
    if (i!=-1) {deleterow = i;}
    if (deleterow<0 || deleterow>=stack.count()) {return false;}
    if (stack.at(deleterow)["id"].toString()!="") {
        stack_todelete.append(stack.at(deleterow)["id"].toInt());
    }
    stack.removeAt(deleterow);
    if (currentRecordI>=stack.count()) {currentRecordI = -1;}
    return true;
}

#include "adataset.h"
bool ADict::saveChanges() {
    //qDebug() << "ADict::saveChanges()" << currentTable << this << stack;
    if ((stack.count()==0 || (stack.count()==1 && stack.at(0).isEmpty())) && stack_todelete.count()==0) {return false;}
    if (currentTable=="") {return false;}
    bool result = true;

    QMap<QString, QVariant> tempRecord;
    //    this->inStack();

    QString fields, values, updatestring, value, valuetype;
    QString currentKey;
    QVariant currentValue;

    //    QString conName = ADatabase::createConnection();
    if (stack.count()==0 && stack_todelete.count()==0) {return false;}

    bool transaction=false;
    if (stack.count()>1) {
        transaction=true;
        ADatabase::beginTransaction();
    }

    //qDebug() << "ADict::saveChanges() stack" << stack.count();

    int id=0;
    for (int i=0;i<stack.count();i++) {
        tempRecord = stack.at(i);
        id = tempRecord["id"].toInt();

        tempRecord.remove("id");
        for (int i1=0;i1<tempRecord.count();i1++) {
            currentKey = tempRecord.keys().at(i1);
            currentValue = tempRecord.values().at(i1);

            fields.append(currentKey);
            valuetype = currentValue.typeName();

            // !!! ПРИВЕДЕНИЕ ТИПОВ ДЛЯ ЗАПИСИ В БД
            if (currentValue.isNull()) {value="null";}
            else if (valuetype=="QDate") {value = "'" + currentValue.toString() + "'";}
            else {value = "'" + currentValue.toString().replace("'", "''") + "'";}

            values.append(value);
            updatestring.append(currentKey+"="+value);
            if(i1<tempRecord.count()-1 && tempRecord.keys().at(tempRecord.count()-1)!="id") {
                fields.append(", ");
                values.append(", ");
                updatestring.append(", ");
            }
            //            qDebug() << "ADict::saveChanges saveChanges" << currentKey << currentValue << value;
        }
        tempRecord["id"] = id;
        id=0;
        //        qDebug() << "ADict::saveRecord() ID: " << tempRecord["id"];
        //qDebug() << "ADict::saveChanges()" << tempRecord["id"];
        if (tempRecord["id"].isNull() || tempRecord["id"]=="" || tempRecord["id"]==0) {

            int id = ADatabase::insertParam(currentTable, fields, values, connection);
            //qDebug() << "ADict::saveChanges()" << currentTable << id << stack.at(i);
            if (id==0) {result = false;}
            else {
                tempRecord["id"] = id;               
                stack.replace(i, tempRecord);
            }
        } else {
            //            qDebug() << "ADict::saveChanges stack" << stack;
            //            qDebug() << "ADict::saveChanges updatestring" << updatestring;
            if (!ADatabase::updateParam(currentTable, updatestring, "id='"+tempRecord["id"].toString()+"'", connection)) {result=false;}
        }
        fields=""; values=""; updatestring="";
    }
    for (int i=0;i<stack_todelete.count();i++) {
        //        qDebug() << "ADict::saveChanges stack_todelete " << stack_todelete.count() << stack_todelete.at(i);
        if (!ADatabase::deleteParam(currentTable, "id='"+QString::number(stack_todelete.at(i))+"'", connection)) {result=false;}
    }

    if (transaction) {ADatabase::commitTransaction();}
    //    ADatabase::closeConnection(conName);

    stack_todelete.clear();

    return result;
}

int ADict::count() {
    return stack.count();
}
void ADict::echoCR(int i) {
    if (i==-1) { i = currentRecordI; }
    //qDebug() << stack.at(i);
}
void ADict::echoDict() {
    for (int i=0;i<stack.count();i++) {
        this->echoCR(i);
    }
}

//void ADict::inStack() {
//    if (!currentRecord.isEmpty()) {
//        stack.append(currentRecord);
//        currentRecord.clear();
//    }
//}
