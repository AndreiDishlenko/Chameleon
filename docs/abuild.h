    #ifndef ABUILD_H
#define ABUILD_H

#include <QObject>
#include <QDate>
#include <QMap>
#include "adict.h"
#include "adatabase.h"
#include <QProgressBar>
#include <QTreeView>

class ABuild : public QObject {
    Q_OBJECT
public:
    explicit ABuild(QObject *parent = 0);
    QProgressBar *progressbar;

private:
signals:

public slots:
    int getPeriodByDate(QDate date);
    static QMap<QString, QDate> getPeriodDates(int idPeriod=0);
    QString getPeriodMinDate(int idPeriod=0);
    QString getPeriodMaxDate(int idPeriod=0);

    bool calculateResults(int idFirm, int idStore, QString calcdate = "");


    bool closeCurrentPeriod(int idFirm, int idStore, QString name, QDate dateend);
    bool openLastPeriod(int idFirm, int idStore);

    bool fillBase();
    bool fillBase2();

    bool destroyDBDocs();

    bool admin_clearRegDocs();
    //    static bool createPluWiring(int idFirm, int idStore, int idPlu, int idUnit, QDate dateend = QDate());
//    bool createPluSalesCost(int idFirm, int idStore, int idPeriod, int idUnit);
//    bool createCurrentPluPartyStore(int idFirm, int idStore, int idPlu, QDate dateend = QDate());
//    bool createCurrentPartyStore(int idFirm, int idStore, QDate dateend = QDate());
};

#endif // ABUILD_H
