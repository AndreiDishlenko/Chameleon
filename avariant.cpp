#include "avariant.h"

////////////////////////////////////////////
// Класс для преобразования типов
////////////////////////////////////////////


AVariant::AVariant(QObject *parent) : QObject(parent) {}

bool AVariant::ifNumber(QVariant data) {
    QRegExp reg("[-]*[0-9]+[,\.][0-9]+");
    QString datatype = data.typeName();
//    qDebug() << "ADataset::ifNumber" << datatype << reg.exactMatch(data.toString());
    if (datatype=="double" || datatype=="float" || (datatype=="QString" && reg.exactMatch(data.toString()))) {
        return true;
    } else {return false;}
}

#include <QDebug>
QString AVariant::convertVariantToString(QVariant data, QString precision) {
    QString  temp = data.typeName();
    //qDebug() << "AVariant::convertVariantToString" << data << temp;
    //qDebug() << "ADataset::convertVariantToString" << data << precision << temp;
    if ((temp=="float" || temp=="double" || temp=="uint" || temp =="int") && precision!="") {
        return QString::number(data.toDouble(), 'f', precision.toInt()).replace('.',',');
    } else if (temp=="int" && precision=="") {
        return QString::number(data.toInt(), 'f', 0);
    } else if (temp=="QString") {
        //qDebug() << "ADataset::convertVariantToString";
        QRegExp reg("[-]*[0-9]+[,\.][0-9]*");
        if (reg.exactMatch(data.toString())) {            
            QString temp_str = data.toString();
            temp_str.replace(temp_str.indexOf(","), 1,'.');
            //qDebug() << data << data.toString() << data.type() << data.toDouble() << aaa << aaa.toDouble() << precision.toInt();
            return QString::number(temp_str.toDouble(), 'f', precision.toInt());
        } else {return data.toString();}
    } else if (temp=="QDate") {
        //qDebug() << "ADataset::convertVariantToString DATETIME" << data << temp;
        return data.toDate().toString("dd.MM.yyyy");
    } else if (temp=="QDateTime") {
//        qDebug() << "ADataset::convertVariantToString DATETIME" << data << temp;
        return data.toTime().toString("hh:mm:ss");
    } else {
        return data.toString();
    }
}


bool AVariant::compareTwoVariants(QVariant var1, QVariant var2) {
    if (var1==var2) return true;
    if (AVariant::ifNumber(var1) && AVariant::ifNumber(var2) && var1.toDouble()==var2.toDouble()) return true;
    if (AVariant::convertVariantToString(var1)==AVariant::convertVariantToString(var2)) return true;
    return false;
}

QDate AVariant::convertVariantToDate(QVariant var) {
    QDate date;
    QRegExp regGostDate("^[0-9]{2}[.][0-9]{2}[.][0-9]{4}");
    QRegExp regISODate("^[0-9]{4}[-][0-9]{2}[-][0-9]{2}");
    if (var.typeName()=="QDate" || var.typeName()=="QDateTime") return var.toDate();
    if (regGostDate.exactMatch(var.toString())) {date = QDate::fromString(var.toString(), "dd.MM.yyyy");}
    if (regISODate.exactMatch(var.toString())) {date = QDate::fromString(var.toString(), "yyyy-MM-dd");}
    return date;
}

QString AVariant::convertDateToString(QDate date) {
    return date.toString("dd.MM.yyyy");
}
