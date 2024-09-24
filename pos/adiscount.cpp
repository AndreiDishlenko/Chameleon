#include "adiscount.h"
#include "adatabase.h"
#include "adict.h"
#include "afunc.h"
#include "avariant.h"
#include "QDateTime"
#include "QDate"
#include "QTime"
#include "aapplication.h"

ADiscount::ADiscount(QObject *parent) : QObject(parent) {
}

// Проверка формата карточки
bool ADiscount::checkCard(QString cardString) {
    if (cardString.length()!=26) {return false;}
    cardString=cardString.mid(1, cardString.length()-2);
    QStringList cardlist = AFunc::strToList(cardString, "/");
    if (cardlist.count()<4) {
        cardlist = AFunc::strToList(cardString, "=");
    }
    if (cardlist.count()<4) {
        cardlist = AFunc::strToList(cardString, ".");
    }
    if (cardlist.count()<4) {
        return false;
    }
    QString securecode = cardlist.at(0);
    if (securecode!="924") {return false;}
    return true;
}

// Расшифровка карточки
QMap<QString, QString> ADiscount::getCardObjects(QString cardString) {
    QMap<QString, QString> result;
    result["version"]="000";
    result["type"]="000";
    result["cardid"]="000000000000";

    cardString=cardString.mid(1, cardString.length()-2);
    QStringList cardlist = AFunc::strToList(cardString, "/");    
    if (cardlist.count()<4) {
        cardlist = AFunc::strToList(cardString, "=");
    }
    if (cardlist.count()<4) {
        cardlist = AFunc::strToList(cardString, ".");
    }
    if (cardlist.count()<4) {
        return result;
    }
    result["version"] = cardlist.at(1);
    result["type"] = cardlist.at(2);
    result["cardid"] = cardlist.at(3);

    return result;
}
// Регистрация карточки. Если зарегистрирована - возвращает id
int ADiscount::registerCard(QString cardString) {
    int result=0;
    QMap<QString, QString> cardObjects = ADiscount::getCardObjects(cardString);
    ADict *dict = new ADict("dictDCards");
    dict->loadRecords("idCardType='"+cardObjects["type"]+"' AND cardid='"+cardObjects["cardid"]+"'");
    // Register DCard
    if (dict->count()==0) {
        dict->newRecord();
        dict->setCRValue("cardid", cardObjects["cardid"]);
        dict->setCRValue("idCardType", cardObjects["type"]);
        dict->saveChanges();
        dict->~ADict();
        return result;
    }
    result = dict->getCRValue("id").toInt();
    dict->~ADict();
    return result;
}
// Получение накопительный сумм по карте
QMap<QString, float> ADiscount::getCardBonus(QString cardtype, QString cardid) {
    QMap<QString, float> result;
    result["cardsum"] = 0;
    result["cardsumbonus"] = 0;
    ADict *dict = new ADict("dictDCards");
    dict->loadRecords("idCardType='"+cardtype+"' AND cardid='"+cardid+"'");
    // Register DCard
    if (dict->count()>0) {
        result["cardsum"] = dict->getCRValue("sum").toFloat();
        result["cardsumbonus"] = dict->getCRValue("sumbonus").toFloat();
    }
    dict->~ADict();
    return result;
}
// Вычисление скидки
int ADiscount::getDiscount(QString cardString){
//    qDebug() << "ADiscount::cardDiscount" << cardString;
    int discount = 0;
    QMap<QString, QString> cardObjects = ADiscount::getCardObjects(cardString);
    qDebug() << "ADiscount::cardDiscount objects" << cardObjects["version"] << cardObjects["type"] << cardObjects["cardid"];
    QMap<QString, float> cardBonus = ADiscount::getCardBonus(cardObjects["type"], cardObjects["cardid"]);
    qDebug() << "ADiscount::cardDiscount" << cardBonus["cardsum"] << cardBonus["cardsumbonus"];
    int tempdiscount = ADatabase::DMax("discount", "dictDConditions", "condition='cardtype' AND value='"+cardObjects["type"]+"'").toInt();
    if (tempdiscount>discount) {discount = tempdiscount;}
    if (cardObjects["cardid"].length()==12) {
        // Вычисление бонусной скидки
        int tempdiscount = ADatabase::DMax("discount", "dictDConditions", "condition='cardsum' AND value<="+QString::number(cardBonus["cardsum"])).toInt();
        qDebug() << "ADiscount::cardDiscount 1" << tempdiscount;
        if (tempdiscount>discount) {discount=tempdiscount;}
        tempdiscount = ADatabase::DMax("discount", "dictDConditions", "condition='cardbonus' AND value<="+QString::number(cardBonus["cardsumbonus"])).toInt();
        qDebug() << "ADiscount::cardDiscount 2" << tempdiscount;
        if (tempdiscount>discount) {discount=tempdiscount;}
    }
    return discount;
}


int ADiscount::calcDocDiscount(int idDoc) {
    ADict *dict = new ADict("dictDConditions");
    int discount=0;

    QDate date;// = new QDate();

    dict->loadRecords("condition='dayweek' AND value='"+QString::number(date.currentDate().dayOfWeek())+"'");
    if (dict->count()>0 && dict->getCRValue("discount").toInt()>discount) {discount=dict->getCRValue("discount").toInt();}

    dict->loadRecords("condition='daymonth' AND value='"+QString::number(date.currentDate().day())+"'");
    if (dict->count()>0 && dict->getCRValue("discount").toInt()>discount) {discount=dict->getCRValue("discount").toInt();}

    QTime time = QDateTime::currentDateTime().time();
    QTime timefrom, timeto;
    dict->loadRecords("condition='bytime'");
    for (int i=0;i<dict->count();i++) {
        timefrom = QTime::fromString(dict->getCRValue("ffrom", i).toString(), "hh:mm");
        timeto = QTime::fromString(dict->getCRValue("fto", i).toString(), "hh:mm");
        if (timefrom<=time && time<=timeto && dict->getCRValue("discount", i).toInt()>discount) { discount=dict->getCRValue("discount", i).toInt(); }
    }

    date = QDateTime::currentDateTime().date();
    QDate datefrom, dateto;
    dict->loadRecords("condition='bydate'");
    for (int i=0;i<dict->count();i++) {
        datefrom = QDate::fromString(dict->getCRValue("ffrom", i).toString(), "dd.MM.yyyy");
        dateto = QDate::fromString(dict->getCRValue("fto", i).toString(), "dd.MM.yyyy");
        if (datefrom<=date && date<=dateto && dict->getCRValue("discount", i).toInt()>discount) { discount=dict->getCRValue("discount", i).toInt(); }
    }

    dict->loadRecords("condition='fixed'");
    if (dict->getCRValue("discount").toInt()>discount) {discount=dict->getCRValue("discount").toInt();}

    dict->loadRecords("condition='max'");
    int max = dict->getCRValue("discount").toInt();

    dict->loadRecords("condition='orderposcount'");
    int orderposcount = dict->getCRValue("value").toInt();
    int orderposdiscount = dict->getCRValue("discount").toInt();

    dict->loadRecords("condition='ordersum'");
    int ordersum = dict->getCRValue("value").toInt();
    int ordersumdiscount = dict->getCRValue("discount").toInt();

    // ---
    dict->setCurrentDict("regDocsCont");
    dict->loadRecords("idDoc='"+QString::number(idDoc)+"'");
    if (dict->count()>=orderposcount && orderposdiscount>discount) {discount = orderposdiscount;}

    // ---
    dict->setCurrentDict("regDocs");
    dict->loadRecords("id='"+QString::number(idDoc)+"'");
    if (dict->getCRValue("sum").toFloat()>ordersum && ordersumdiscount>discount) {discount = ordersumdiscount;}

    if (discount>max && max!=0) { discount=max; }
    dict->~ADict();
    return discount;
//    qDebug() << "Discount= '"+QString::number(discount)+"'";
}
void ADiscount::setDocDiscount(int idDoc, int idCard, float discount){
    ADict *dict = new ADict("regDocs");
    float k = 1-discount/100;
    if (dict->openRecord(idDoc)) {
//        dict->setCRValue("discount", discount);
        float sum = dict->getCRValue("sum").toFloat();
        float sum_discount = AFunc::round(sum * k, 2);
        dict->setCRValue("idCard", idCard);
        dict->setCRValue("discount", QString::number(discount));
        dict->setCRValue("full_sum", QString::number(sum));
        dict->setCRValue("sum", QString::number(sum_discount));
        dict->saveChanges();
    }

    dict->setCurrentDict("regDocsCont");
    dict->loadRecords("iddoc='"+QString::number(idDoc)+"'");
    float price = 0;
    float newprice = 0;
    float newsum = 0;
    for (int i=0;i<dict->count();i++) {
        dict->setCurrentRecord(i);
        price = dict->getCRValue("price", i).toFloat();
        newprice = AFunc::round(price*k,2);
        newsum = AFunc::round(dict->getCRValue("sum", i).toFloat()*k,2);        
        dict->setCRValue("full_price", QString::number(price));
        dict->setCRValue("price", QString::number(newprice));
        dict->setCRValue("sum", QString::number(newsum));
    }
    dict->saveChanges();
    dict->~ADict();
}
void ADiscount::calcResults() {
    ADict *dict = new ADict("dictDConditions");
    dict->loadRecords("condition='cardperiod'");
    int cardperiod = dict->getCRValue("value").toInt();

    QDate date;
    QString startdate = AVariant::convertDateToString(date.currentDate().addDays(-cardperiod));
    QString today = AVariant::convertDateToString(date.currentDate());

//    qDebug() << "ADiscount::calcResults" << "EXEC dbo.bonus_update '"+startdate+"', '"+today+"'";

    ADatabase::execSql("EXEC dbo.bonus_update '"+startdate+"', '"+today+"'");
}
int ADiscount::getCurrentBonus(int idcard) {
    QDate date = AApplication::getRegValue("lastbonuscalcdate").toDate();
    QString lastupdate = AVariant::convertDateToString(date);
    QString today = AVariant::convertDateToString(date.currentDate());
    int result = ADatabase::DLookUp("sumbonus", "bonus_currentsum('"+lastupdate+"', '"+today+"')", "id='"+QString::number(idcard)+"'", "", "ADisc:getCurrentBonus").toInt();
//    qDebug() << "ADiscount::getCurrentBonus" << lastupdate << result;
    return result;
}
