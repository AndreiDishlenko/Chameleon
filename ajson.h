#ifndef AJSON_H
#define AJSON_H


#include <QObject>
#include <QTextEdit>
#include <QJsonObject>


class AJson
{
public:
    explicit AJson(QObject *parent = 0);
    ~AJson();

    QString errorParser(QJsonObject obj);

    QString access_token;

    //bool setServer(QString server);
    //bool pingServer(QString server, QString sender="");

    bool connectBearer(QString server, QString login, QString password);

    QList<QVariant> getShifts(QString status);
    //QVariantMap AJson::getCurrentShift();
    bool createShift(QString license);
    bool closeShift();




    QJsonObject get(QString query, QMap<QString, QString> map);
    QJsonObject post(QString query, QByteArray request_body, QMap<QString, QString> header_map = QMap<QString, QString>());

    QVariantMap createReciept(QVariantMap request_map);


    void echoJson(QJsonDocument doc);
    void outputQJsonObject(QJsonObject obj);

    QString rest_server;
    QString lasterror;
    int lasterrorcode;

private:
    QObject *my_parent;



};

#endif // AJSON_H
