//login: testkey2711213
//password: 123456
//Пін-код касира: 0172348332
 //Ключ ліцензії каси: ff63f939bd364a659d634dd4
//Касса: 4000158035
//Тестовий Касир Касирович

//full_name: "Тестовий Касир Касирович",
//"nin": "34554363",
//"key_id": "f679bcc5ca747b9ff528e35949acd839f23ae3d210a596fb49fb2f502c47157f",

//organization: Тестовий платник 4
//"edrpou": "34554363",
//"tax_number": "123456789020",


#include "aprro.h"
#include "QDebug"
#include "aapplication.h"
#include "globals.h"
#include <QDateTime>

APrro::APrro(QObject *parent) : QObject(parent) {
    //qDebug() << "APrro::APrro";
    ajson = new AJson(this);
    lasterror="";
}

APrro::~APrro() {

}

// Устанавливает соединение с фискальным сервером
// При вызове сбрасывает старое соединение и устанавливает новое с присвоением нового токена
bool APrro::connect_server() {
    //ajson->setServer( AApplication::getLocalSetting("prro_server").toString() );
    QString server = AApplication::getLocalSetting("prro_server").toString();
    QString login = AApplication::getLocalSetting("prro_user").toString();
    QString password = AApplication::getLocalSetting("prro_password").toString();

    //qDebug() << "APrro::connect_server";// << login <<  password;

    if (ajson->connectBearer( server, login, password )) {
        qDebug() << "[+] APrro::connect_server() - connect to fiscal server " << server << " successfully.";
        globals["json_connection_token"] = ajson->access_token;
        lasterror="";
        lasterrorcode=0;
        return true;
    } else {
        qDebug() << "[-] APrro::connect_server() - fiscal server " << server << " is not connected. Error " + QString::number(ajson->lasterrorcode) +". " + ajson->lasterror;
        globals["json_connection_token"] = "";
        lasterror = "Ошибка подключения к фискальному серверу. " + ajson->lasterror + " ";
        lasterrorcode = ajson->lasterrorcode;
        return false;
    }
}

bool APrro::check_connection() {
    if (ajson->access_token=="") {
        qDebug() << "[-] APrro::check_connection() - соединение с ПРРО не установлено. Попытка соединения.";
        if (this->connect_server()) {
            return true;
        } else {
            return false;
        }
    }
    return true;
}


int APrro::check_shift() {

    int result;
    // 1 - opened
    // 2 - not opened
    // 0 - error

    //    QVariantMap result;
    QVariantList opened_shifts = ajson->getShifts("OPENED");

    if (opened_shifts.count()>0) {
       //result = opened_shifts.at(0).toMap();
       lasterror="";
       result = 1;
    }
    if (opened_shifts.count()==0 && ajson->lasterror=="") {
        lasterror="";
        result = 2;
    }
    if (opened_shifts.count()==0 && ajson->lasterror!="") {
        lasterror="Ошибка проверки смены. "+ajson->lasterror;
        result = 0;
    }
    //qDebug() << "APrro::check_shift" << result;
    //    //qDebug() << "AJson::getCurrentShift" << opened_shifts;
    //    return result;
    //    //QVariantMap current_shift = ajson->getCurrentShift();
    //    if (!current_shift.isEmpty()) {
    ////        QDateTime opened_at = current_shift["opened_at"].toDateTime();
    ////        if ( opened_at.secsTo(QDateTime::currentDateTime()) > 86350 ) {
    ////            this->close_shift();
    ////            return this->open_shift();
    ////        }
    //        lasterror="";
    //        return true;
    //    } else {
    //        lasterror=ajson->lasterror;
    //        //return this->open_shift();
    //    }
    return result;
}
bool APrro::open_shift() {
    //qDebug() << "[ ] APrro::open_shift() - открываем смену";
    QString license = AApplication::getLocalSetting("prro_license").toString();

    if ( ajson->createShift(license) ) {
        lasterror = "";
        lasterrorcode = 0;
        qDebug() << "[+] APrro::open_shift() - cмена успешно открыта.";
        return true;
    } else {
        qDebug() << "[-] APrro::open_shift() - не удалось открыть смену." << ajson->lasterror;
        lasterror = "Не удалось открыть смену. " + ajson->lasterror;
        lasterrorcode = ajson->lasterrorcode;
        return false;
    }

}
bool APrro::close_shift() {
    //qDebug() << "[ ] APrro::close_shift() - закрываем смену";
    if ( ajson->closeShift() ) {
        qDebug() << "[+] APrro::close_shift() - cмена успешно закрыта.";
        lasterror = "";
        return true;
    } else {
        qDebug() << "[-] APrro::close_shift() - не удалось закрыть смену." + ajson->lasterror;
        lasterror = "Ошибка закрытия смены. " + ajson->lasterror;
        return false;
    }
}

QString APrro::geterror() {
    return lasterror;
}
int APrro::geterrorcode() {
    return lasterrorcode;
}

#include <docs/adoc.h>
#include <adict.h>
#include "adatabase.h"

bool APrro::registerDocument(int iddoc) {
    qDebug() << "";
    qDebug() << "[ ] APrro::registerDocument() - регистрируем чек " << QString::number(iddoc);
    lasterror = "";
    lasterrorcode = 0;

    if (!this->check_connection()) {
        qDebug() << "[-] APrro::registerDocument() - reciept " << QString::number(iddoc) << " is not registered.";
        lasterror = lasterror + "Чек не зарегистрирован.";
        return false;
    }
//    if (!this->check_shift()) {
//        qDebug() << "[WARNING] APrro::registerDocument. Shift is not opened.";
//        lasterror = "Ошибка фискализации. Смена не открыта.";
//        errorcode = 2;
//        return false;
//    }

    QList<QMap<QString, QVariant>> records;

    records = ADatabase::selectList("SELECT * FROM get_fiscal_content("+QString::number(iddoc)+")");
    //qDebug() << "SELECT * FROM get_fiscal_content("+QString::number(iddoc)+")";

    QVariantMap query;
    QVariantList goods;
    QVariantMap plu, plu_div;
    float sum_float;
    for (int i=0;i<records.count();i++) {
        plu["code"]=records.at(i)["code"];
        plu["name"]=records.at(i)["name"];
        plu["price"]=records.at(i)["price"].toFloat()*100;
        plu_div["good"] = plu;
        plu_div["quantity"] = records.at(i)["count"].toFloat()*1000;
        //sum_float = sum_float + (records.at(i)["price"].toFloat() * records.at(i)["count"].toFloat());
        goods << plu_div;
        //qDebug() << "APrro::registerDocument" << plu["code"] << plu["name"] << plu_div["quantity"] << plu["price"] ;
    }
    //qDebug() << "APrro::registerDocument" << sum;
    query["goods"]=goods;

    QVariantMap email;
    QVariantList email_sub;
        email["email"]="andrei@solut.com.ua";
        email_sub << "andrei@solut.com.ua";
        email["emails"] = email_sub;
    query["delivery"] = email;


    ADoc *regDocs = new ADoc(this, "APrro::registerDocument");
    regDocs->openDoc(iddoc);

    QMap<QString, QVariant> document = regDocs->getDocData();
    //document = ADatabase::selectVMap(
                //"SELECT * FROM regDocs WHERE id="+QString::number(iddoc));
    if (document["iddoctype"]!=9) {
        qDebug() << "[WARNING!] APrro::registerDocument document "+document["number"].toString()+" is not retail sale.";
        lasterror = "Фискализировать возможно только розничную продажу.";
        return false;
    }
    if (document["fiscal_code"]!="") {
        qDebug() << "[WARNING!] APrro::registerDocument document "+document["number"].toString()+" is already registered";
        lasterror = "Продажа уже фискализирована.";
        return false;
    }

    QVariantList payments;
    QVariantMap pay;
    if (document["ifCashless"].toBool()) {
        pay["type"]="CASHLESS";
    } else {
        pay["type"]="CASH";
    }
    //int sum_int = sum_float*100;
    pay["value"]=document["sum"].toFloat()*100;
    payments << pay;
    query["payments"] = payments;

//    QVariantMap discount;
//    if (document["discount"].toInt()>0) {
//        discount["type"]="DISCOUNT";
//        discount["mode"]="PERCENT";
//        discount["value"]=document["discount"].toInt();
//        query["discounts"] = payments;
//    }


    QVariantMap result;
    for (int i=1;i<2;i++) {
        result = ajson->createReciept(query);
        if (result.isEmpty()) {
            lasterrorcode = ajson->lasterrorcode;
            lasterror = ajson->lasterror + "Чек не зарегистрирован.";
            qDebug() << "[-] APrro::registerDocument() - не удачная попытка. Error" << lasterrorcode << lasterror;
            // Просроченный токен JWT
            if (lasterrorcode==204) {
                if (!this->connect_server()) {
                    qDebug() << "[-] APrro::registerDocument() - чек" << document["number"].toString() << "не зарегистрирован.";
                    qDebug() << "";
                    lasterror = lasterror + "Чек не зарегистрирован.";
                    return false;
                }
                i--;
            }
            // Не открыта смена
            if (lasterrorcode==302) {
                if (!this->open_shift()) {
                    qDebug() << "[-] APrro::registerDocument() - чек" << document["number"].toString() << "не зарегистрирован.";
                    qDebug() << "";
                    lasterror = lasterror + "Чек не зарегистрирован.";
                    return false;
                }
                i--;
            }
        } else {
            lasterror="";
            lasterrorcode = 0;
            break;
        }
        if (i==1) {return false;}
    }

    regDocs->updateParam("fiscal_code", result["fiscal_code"]);
    regDocs->commit();

    qDebug() << "[+] APrro::registerDocument() - reciept " << document["number"].toString() << " registered succesfully.";
    qDebug() << "";

    //    qDebug() << "APrro::registerDocument()" << result["id"].toString();
    //    qDebug() << result["status"].toString();
    //    qDebug() << .toString();
    //    qDebug() << result["barcode"].toString();
    //    qDebug() << result["footer"].toString();
    //    qDebug() << result["is_sent_dps"].toString();

    return true;
}

#include "adatabase.h"
void APrro::pingserver() {
    QTcpSocket *tcpSocket = new QTcpSocket;
    qDebug() << "APrro::pingserver" << ajson->rest_server;
    tcpSocket->connectToHost("dev-api.checkbox.in.ua", 80);
    if (tcpSocket->waitForConnected(3000)) {
        qDebug() << "available";
    } else {
        qDebug() << "not available";
    }
}

void APrro::setAccessToken(QString token) {
    ajson->access_token = token;
}


