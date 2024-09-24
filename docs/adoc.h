/////////////////////////////////////////////////////////////////////////
///
/// Класс ADoc реализовывает работу с реестром документов (regDocs)
///
/////////////////////////////////////////////////////////////////////////

#ifndef ADOC_H
#define ADOC_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <adict.h>

class ADoc : public QObject
{
    Q_OBJECT
public:
    explicit ADoc(QObject *parent = 0, QString sender="");
    ~ADoc();

    QString lasterror;

    bool ifDt, ifKt, ifPay;

    QMap<QString, QVariant> record;
    QMap<int, QMap<QString, QVariant> > usersMap;
    QMap<int, QMap<QString, QVariant> > doctypesMap;
    QMap<int, QMap<QString, QVariant> > firmsMap;
    QMap<int, QMap<QString, QVariant> > storesMap;
    QMap<int, QMap<QString, QVariant> > accMap;
    QMap<int, QMap<QString, QVariant> > currencyMap;


private:
    QList<QString> fieldslist;
    ADict *regDocsDict;
    QMap<QString, QVariant> openedrecord;

public:
    bool calcWireForCurrentDoc();
    void wireDocInParralel(int iddoc);

signals:

public slots:
    bool newDoc(int iddoctype=0, int idfirm=0, int idagent=0);
    bool openDoc(int id=0);
    bool commit();

    void wireDoc(int iddoc=0);
    void unwireDoc(int iddoc=0);

    bool updateDoc(QMap<QString, QVariant> updateMap);
    void updateParam(QString key, QVariant value);

    QMap<QString, QVariant> getDocData();
    QVariant getParam(QString key);



    bool setUser(int iduser=0);
    bool setDefFirm(int idfirm=0);
    bool setFirm(int idfirm);
    bool setDefStores();
    bool setFromStore(int idstore=0);
    bool setToStore(int idstore=0);
    bool setDefAcc();
    bool setFromAcc(int idaccount=0);
    bool setToAcc(int idaccount=0);
    bool setDefAgent();
    bool setAgent(int idagent=0);
    bool setDefCurrency();
    bool setCurrency(int idcurrency=0);

    int nextNumber();
    void echoRecord();
    QList<int> findSubDocs(int iddoc);
    void deleteSubdocs(int iddoc=0);

    void saveChangesLog(int iddoc, int iduser, QString changes);

};

#endif // ADOC_H
