#include "apos.h"
#include "adatabase.h"
#include "globals.h"
#include <QString>

APos::APos(QObject *parent) : QObject(parent) {
}

QMap<QString, QVariant> APos::getTableInfo(int idTable) {
//    qDebug() << "APos::getTableInfo" << idTable;
    QMap<QString, QVariant> result;
    result["reserved"] = 0;
    int sum = 0;
    if (idTable!=0) {
        QList<QMap<QString, QVariant> > data = ADatabase::selectList("SELECT     TOP (100) PERCENT dbo.regDocs.id, dbo.dictUsers.name AS username, dbo.regDocs.sum FROM dbo.regDocs INNER JOIN dbo.rest_uniDocTable ON dbo.regDocs.id = dbo.rest_uniDocTable.idDoc LEFT OUTER JOIN dbo.dictUsers ON dbo.regDocs.iduser = dbo.dictUsers.id WHERE (dbo.regDocs.iddoctype = 9) AND (dbo.regDocs.ifFinished = 0) AND (dbo.rest_uniDocTable.idTable = "+QString::number(idTable)+") GROUP BY dbo.regDocs.id, dbo.regDocs.sum, dbo.dictUsers.name ORDER BY dbo.regDocs.id");
        for (int i=0;i<data.count();i++) {
            sum = sum + data.at(i)["sum"].toInt();
        }
        result["openOrdersSum"] = sum;
            if (data.count()>0) {result["openedBy"] = data.at(0)["username"];} else {result["openedBy"]="";}
        result["openOrdersCount"] = data.count();
    }
    return result;
}



//void APos::processCompoundDoc(id)
