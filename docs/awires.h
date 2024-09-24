#ifndef AWIRES_H
#define AWIRES_H

#include <QObject>
#include <adict.h>
#include <QDate>

class AWires : public QObject {
    Q_OBJECT
public:
    explicit AWires(QObject *parent = 0);

    static bool createPluWire(int idFirm, int idStore, int idPlu, int idUnit, QDate calcdate = QDate(), QDate dateend = QDate(), QString conName="");
    static bool calcWireForProduction(int iddoc);
    static bool calcWireForProduction_old(int iddoc);
    static bool calcWireForInventory(int iddoc);
    
signals:
    
public slots:

};

#endif // AWIRES_H
