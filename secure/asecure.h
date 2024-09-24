#ifndef ASECURE_H
#define ASECURE_H

#include <QObject>
#include <QMultiMap>
#include "adict.h"
#include "qmap.h"

class ASecure : public QObject
{
    Q_OBJECT
private:
    static QMultiMap<QString , QString> find(QString searchDir);

    ADict *dict;

public:
    ASecure(QObject *parent);
    ~ASecure();

    static bool encodeDir(QString mode, QString searchDir, QString word);
    static bool encodeFile(QString mode, QString Path, QString File, QString word);

    static QByteArray encodeStr(const QByteArray& eArr, QString word);
    static QByteArray decodeStr(const QByteArray& dArr, QString word);

public slots:


    int createUser(QString login, QString password, QString name, QString card_id="");
    bool updateUser(QString login, QString password, QMap<QString, QVariant> updateMap, QString card_id="");
//    bool setUserPassword(QString login, QString oldpassword, QString newpassword, QString card_id=0);
//    bool changeUserName(QString login, QString oldpassword, QString newpassword, QString card_id=0);

    static void setCurrentUser(int idUser);
    static int getCurrentUser();
    static QString getUserName();

    bool checkCard(QString cardstring);
    bool addUser(QString name, QString password, int role, QString cardstring="");

//    bool setUserLogin(QString login, QString password);
//    bool setUserId(int iduser, QString password);
//    bool setUserCardId(QString card_id);

    QMap<QString, QVariant> decodeCard(QString cardstring);

    int getUser(QString login, QString password, int iduser=0);
    int getUserCardId(QString cardid);
    int getUserCardString(QString cardstring);
    int getUserRole(int idUser);



    bool checkRights(QString rightsname, int idUser);
};

#endif // ASECURE_H
