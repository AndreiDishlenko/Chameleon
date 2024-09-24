#ifndef ADISCOUNT_H
#define ADISCOUNT_H

#include <QObject>
#include <QMap>

class ADiscount : public QObject {
Q_OBJECT
public:
    explicit ADiscount(QObject *parent = 0);

public slots:
    static bool checkCard(QString cardString);
    static QMap<QString, QString> getCardObjects(QString cardString);
    static int registerCard(QString cardString);
//    static QString getCardId(QString cardString);
    static QMap<QString, float> getCardBonus(QString cardtype, QString cardid);
    static int getDiscount(QString cardString);

//    // Получение первоначальной скидки по новому документу
    static int calcDocDiscount(int idDoc);
    void setDocDiscount(int idDoc, int idCard, float discount);
    static void calcResults();
    int getCurrentBonus(int idcard=0);

};

#endif // ADISCOUNT_H
