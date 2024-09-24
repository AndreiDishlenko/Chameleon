#include "ajson.h"
#include "aapplication.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonArray>

#include <QTimer>
#include <QEventLoop>
#include <QTextEdit>
#include <QUuid>



AJson::AJson(QObject *parent) {
    //qDebug() << "AJson::AJson" << parent;
    my_parent = parent;
}
AJson::~AJson() {}


QString AJson::errorParser(QJsonObject obj) {
    QString result="";
    int errorcode = obj["ErrorCode"].toInt();
    if (errorcode==3) {
        result = "Сервер недоступен.";
    } else if  (errorcode==99) {
        result = "Отсутствует сетевое подключение.";
    } else if  (errorcode==204) {
        result = "Сервер ПРРО требует авторизацию. " + obj["message"].toString() + ". ";
    } else {
        result = obj["message"].toString()+" ";
    }
    return result;
}

// Connect to RestAPI server, return token
bool AJson::connectBearer(QString server, QString login, QString password) {
    //qDebug() << "AJson::connectBearer";
    //if (!this->pingServer(rest_server, "AJson::connectBearer")) {return "";}
    QString query = "/api/v1/cashier/signin";

    QJsonObject obj;
        obj["login"] = login;
        obj["password"] = password;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    obj = this->post(server+query, data);
    if (obj["StatusCode"].toInt()==200) {
        rest_server = server;
        access_token = obj["access_token"].toString();
        lasterror="";
    } else {
        //qDebug() << "[ERROR " + QString::number(obj["ErrorCode"].toInt()) + "] AJson::connectBearer. "+obj["Error"].toString()+". "+obj["message"].toString();
        access_token="";
        lasterrorcode = obj["ErrorCode"].toInt();
        lasterror = this->errorParser(obj);
        return false;
    }
    return true;
}

QList<QVariant> AJson::getShifts(QString status) {
    //qDebug() << "AJson::getShifts";
    QList<QVariant> result;
    //if (!this->pingServer(rest_server, "AJson::getShifts")) {return result;}
    QString query = "/api/v1/shifts";

    QMap<QString, QString> query_params;
        query_params["statuses"] = status;

    QJsonObject obj = this->get(rest_server+query, query_params);
    if (obj["StatusCode"].toInt()==200) {
        QVariantMap response = obj.toVariantMap();
        result = response["results"].toList();
        lasterror="";
    } else {
        //qDebug() << "[WARNING] AJson::getShifts. "+obj["Error"].toString()+". "+obj["message"].toString();
        lasterrorcode = obj["ErrorCode"].toInt();
        lasterror = this->errorParser(obj);
    }

    return result;
}

bool AJson::createShift(QString license) {
    //qDebug() << "AJson::createShift";
    //if (!this->pingServer(rest_server, "AJson::createShift")) {return false;}
    QString query = "/api/v1/shifts";

    QJsonObject obj;
        obj["id"] = QUuid::createUuid().toString();
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    QMap<QString, QString> header_map;
        header_map["X-License-Key"] = license;

    obj = this->post(rest_server+query, data, header_map);
    //  qDebug() << "AJson::post_createShift" << obj["StatusCode"] << obj["ErrorCode"];
    if (obj["StatusCode"].toInt()==202) {
        lasterror="";
        lasterrorcode = 0;

        QVariantList responce;
        for (int i=0;i<100;i++) {
            responce = this->getShifts("OPENED");
            if (responce.length()>0) {
                return true;
            }
            qDebug() << "Повторная проверка открытой смены.";
        }
        lasterror = "Не дождался открытия смены";
        lasterrorcode = 302;
        return false;
    } else {
        //qDebug() << "[WARNING] AJson::createShift. "+obj["Error"].toString()+". "+obj["message"].toString();
        lasterrorcode = obj["ErrorCode"].toInt();
        lasterror = this->errorParser(obj);
        return false;
    }
}

bool AJson::closeShift() {
    //qDebug() << "AJson::closeShift";
    //if (!this->pingServer(rest_server, "AJson::closeShift")) {return false;}
    QString query = "/api/v1/shifts/close";

    QJsonObject obj;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    obj = this->post(rest_server+query, data);
    if (obj["StatusCode"].toInt()==202) {
        lasterror="";
        return true;
    } else {
        qDebug() << "[WARNING] AJson::closeShift. "+obj["Error"].toString()+". "+obj["message"].toString();
        lasterror=obj["message"].toString();
        return false;
    }

}


//// Check if opened shift is available. In this case return 1
// If shift is created, but still not opened return 2
// If it doesn't have opened and created shifts - return 0
#include "afunc.h";
void AJson::echoJson(QJsonDocument doc) {
    QByteArray data = doc.toJson();
    QStringList sl = AFunc::strToList(data, "\n", true);
    //QString s;
    for (int i=0;i<sl.count();i++) {
        //s = sl.at(i)
        qDebug() << sl.at(i);
    }
}

//// Create document
QVariantMap AJson::createReciept(QVariantMap request_map) {
    //qDebug() << "AJson::createReciept";
    QVariantMap zero_result;

    //if (!this->pingServer("AJson::createReciept")) {return zero_result;}
    QString query = "/api/v1/receipts/sell";

    QJsonObject obj = QJsonObject::fromVariantMap(request_map);
        obj["id"] = QUuid::createUuid().toString();
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    //this->echoJson(doc);

    // Creating
    obj = this->post(rest_server+query, data);
    //qDebug() << "AJson::createReciept" << obj["StatusCode"] << obj["ErrorCode"];
    if (obj["StatusCode"].toInt()!=201) {
        //qDebug() << "[WARNING] AJson::createReciept. "+obj["Error"].toString()+". "+obj["message"].toString();
        lasterrorcode = obj["ErrorCode"].toInt();
        lasterror = this->errorParser(obj);
        return zero_result;
    }
      
    // Check
    query = "/api/v1/receipts/"+obj["id"].toString();
    QMap<QString, QString> params_map;

    QVariantMap responce;
    for (int i=0;i<20;i++) {
        responce = this->get(rest_server+query, params_map).toVariantMap();
        if (responce["status"].toString()=="DONE") {
            lasterror = "";
            return responce;
        }
        AApplication::pause(1000);
    }

    qDebug() << "[WARNING] AJson::createReciept could not wait for the registration of the '"+obj["id"].toString()+"' document.";
    lasterror = "Нет ответа от фискального сервера.";
    return zero_result;
}


// ------ GET and POST queries -------------
// Получает Query Parameters QMap<QString, QString> params_map
// Использует access_token и строку запроса
// Возвращает результаты запроса а формате QJsonObject

QJsonObject AJson::get(QString query, QMap<QString, QString> params_map) {
    QJsonObject result;

    QString params_string="";
    for (int i=0;i<params_map.size();i++) {
        params_string.append(params_map.keys().at(i)+"="+params_map.value(params_map.keys().at(i)));
        if (i<params_map.size()-1) {params_string.append("&");}
    }

    if (params_string!="") {query.append("?"+params_string);}

    //qDebug() << "AJson::get query:" << query;

    QNetworkAccessManager *mgr = new QNetworkAccessManager();
    const QUrl url(query);
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer "+access_token.toUtf8());

    QNetworkReply *reply = mgr->get(request);

    QTimer timer;
    timer.setSingleShot(true);

    QEventLoop loop;
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    timer.start(10000);   // 10 secs. timeout
    loop.exec();

    if(timer.isActive()) {
        timer.stop();
        if(reply->error() == QNetworkReply::NoError) {
            result = QJsonDocument::fromJson(reply->readAll()).object();
        }
        else {
            result = QJsonDocument::fromJson(reply->readAll()).object();
            result["Error"] = reply->errorString();
            //qDebug() << "[ERROR] AJson::get" << reply->errorString();
            //qDebug() << "[ERROR] AJson::get" << result["message"].toString();
        }
        result["StatusCode"] = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    }
    else {
        QObject::disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        result["message"] = "Time is out";
        reply->abort();
    }
    reply->deleteLater();
    return result;
}
QJsonObject AJson::post(QString query, QByteArray request_body, QMap<QString, QString> header_map) {
    //qDebug() << "AJson::post" << query;
    //qDebug() << "AJson::post" << request_body;
    //qDebug() << "AJson::post" << header_map;
    QJsonObject result;

    QNetworkAccessManager *mgr = new QNetworkAccessManager();
    const QUrl url(query);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer "+access_token.toUtf8());
    for (int i=0;i<header_map.keys().count();i++) {
        request.setRawHeader(header_map.keys().at(i).toUtf8(), header_map[header_map.keys().at(i)].toUtf8());
    }

    QNetworkReply *reply = mgr->post(request, request_body);
    //qDebug() << reply->error();
    QTimer timer;
    timer.setSingleShot(true);

    QEventLoop loop;
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    timer.start(3000);   // 10 secs. timeout
    loop.exec();

    if (timer.isActive()) {
        timer.stop();

        if (reply->error() == QNetworkReply::NoError) {
            result = QJsonDocument::fromJson(reply->readAll()).object();
        } else {
            result = QJsonDocument::fromJson(reply->readAll()).object();
            //qDebug() << "[Error] AJson::post" << url;
            //qDebug() << "[Error] AJson::post" << request_body;
            //qDebug() << "[Error] AJson::post" << reply->error();
            //qDebug() << "[Error] AJson::post" << reply->errorString();
            //if (reply->error()==3) {qDebug() << "aaa";}
            result["ErrorCode"] = reply->error();
            result["Error"] = reply->errorString();
            //qDebug() << "[ERROR] AJson::post" << reply->readAll().toStdString();
            //qDebug() << "[ERROR] AJson::post" << result["message"].toString();
        } 
        result["StatusCode"] = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    } else {
        QObject::disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        result["ErrorCode"] = 99;
        result["Error"] = "Сервер ПРРО не доступен.";
        reply->abort();
    }
    reply->deleteLater();

    return result;
}


void AJson::outputQJsonObject(QJsonObject obj) {
    //qDebug() << "AJson::outputQJsonObject";

    //QString result = QJsonDocument(obj).toJson(QJsonDocument::Indented);

    //MainWindow *mw = static_cast<MainWindow*>(my_parent);
    //mw->outputLog(result);

    //        QVariantMap map = obj.toVariantMap();
    //        QVariantList list = map["detail"].toList();;
    //        for (int i=0;i<list.size();i++) {

    //            qDebug() << list.at(i);
    //            //qDebug() << map.
    //        }

    //qDebug() << "outputQJsonObject" << obj.length() << obj.size();
    //qDebug() << obj.keys().at(0) << obj.value(obj.keys().at(0)).toString();

    //QVariantMap map = obj.toVariantMap();
    //for (int i=0;i<obj.size();i++) {
    //qDebug() << obj.
    //}
}


//bool AJson::setServer(QString server){
//    if (!pingServer(server, "AJson::setServer")) {
//        qDebug() << "[-] Fiscal server " << rest_server << " is not connected.";
//        return false;
//    } else {
//        rest_server = server;
//        qDebug() << "[+] Connect to fiscal server " << rest_server << " successfully.";
//    }
//    return true;
//}
//bool AJson::pingServer(QString server, QString sender){
//    if (server=="") {
//        //qDebug() << "[WARNING] "+sender+"->AJson::pingServer. ;
//        lasterror="Соединение с фискальным сервером не установлено.";
//        return false;
//    } else {
//        lasterror="";
//        return true;
//    }
//}


//QVariantMap AJson::getCurrentShift() {
//    QVariantMap result;
//    QVariantList opened_shifts = this->getShifts("OPENED");

//    //qDebug() << "AJson::getCurrentShift" << opened_shifts;

//    if (opened_shifts.count()>0) {
//        result = opened_shifts.at(0).toMap();
//    }

//    return result;
//}
