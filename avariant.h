#ifndef AVARIANT_H
#define AVARIANT_H

#include <QObject>

#include "adataset.h"
#include <QVariant>
#include <QRegExp>
#include <QTime>
#include <QDate>

class AVariant : public QObject
{
    Q_OBJECT
public:
    explicit AVariant(QObject *parent = 0);
    
signals:
    
public slots:
    static bool ifNumber(QVariant data);
    static QString convertVariantToString(QVariant data, QString precision="");
    static bool compareTwoVariants(QVariant var1, QVariant var2);

    static QDate convertVariantToDate(QVariant var);
    static QString convertDateToString(QDate date);
//    static bool date2unidate
};

#endif // AVARIANT_H
