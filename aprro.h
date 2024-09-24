#ifndef APRRO_H
#define APRRO_H

#include "QObject"
#include "ajson.h"

class APrro: public QObject {
    Q_OBJECT
public:
    APrro(QObject *parent);
    ~APrro();

    QString lasterror;
    int lasterrorcode;

public slots:
    bool connect_server();
    bool check_connection();

    int check_shift();
    bool open_shift();
    bool close_shift();

    QString geterror();
    int geterrorcode();

    bool registerDocument(int iddoc);

    void setAccessToken(QString token);
    void pingserver();

private:
    AJson *ajson;
};

#endif // APRRO_H
