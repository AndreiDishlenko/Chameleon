#include "alicense.h"
#include "grdapi.h"
#include "QDebug"
#include <QByteArray>
#include "asecure.h"
#include "afunc.h"
#include "globals.h"
#include <QDateTime>
#include <adatabase.h>

ALicense::ALicense(QObject *parent) : QObject(parent) {
    encodeKey = "6784973861";
    //    if (!this->initapi()) {
//        qDebug() << "Ошибка инициализации ключа";
//    }
}
ALicense::~ALicense() {
    qDebug() << "~ALicense()";
    this->closeapi();
}

bool ALicense::initapi() {
//    qDebug() << "ALicense::initapi";
    int result;
    // Startup
    GrdStartup(GrdFMR_Local);
    HANDLE hGrd_temp;
    hGrd_temp = GrdCreateHandle(hGrd_temp, GrdCHM_SingleThread, NULL);
    hGrd = hGrd_temp;

    // GrdSetAccessCodes
    QByteArray publicKeyArr = "BQYMBA4PBg4GBA==";
    QByteArray privateReadKeyArr = "BQEMAgEPCgEGAg==";
    GrdSetAccessCodes(hGrd, ASecure::decodeStr(publicKeyArr, encodeKey).toLongLong(0,10), ASecure::decodeStr(privateReadKeyArr, encodeKey).toLongLong(0,10), 0 ,0);
    // GrdSetFindMode
    DWORD dwFlags = GrdFM_NProg;// + GrdFM_SN + GrdFM_Ver + GrdFM_Mask;
    GrdSetFindMode(
            hGrd,
            GrdFMR_Local,                   //dwRemoteMode
            dwFlags,                        //dwFlags
            globals["progNo"].toInt(),    //dwProg
            0,                              //dwID
            1,                              //dwSN
            1,                              //dwVer
            1,                              //dwMask
            0,                              //dwType
            GrdFMM_ALL,                     //dwModels
            GrdFMI_USB);                    //dwInterfaces

    // GrdFind
    DWORD *pdwID;
    TGrdFindInfo *pFindInfo;
    result = GrdFind(hGrd, GrdF_First, pdwID, pFindInfo);
//    qDebug() << "GrdFind" << result;
    if (result!=0) {
        GrdSetFindMode(hGrd, GrdFMR_Local, dwFlags, 0, 0, 1, 1, 1, 0, GrdFMM_ALL, GrdFMI_USB);
        GrdFind(hGrd, GrdF_First, pdwID, pFindInfo);
    }
    GrdLogin(hGrd, 0, 0);
//    qDebug() << "ALicense::initapi end";
    globals["tempkey"] = QVariant::fromValue(this->readPI(3, 32)).toString();
    return true;
}
bool ALicense::closeapi() {
    GrdLogout(hGrd, 0);
    GrdCloseHandle(hGrd);
    GrdCleanup();
    return true;
}

QByteArray ALicense::readPI(int num, const int size) {
//    qDebug() << "ALicense::readPI";
    //55BYTE pData[size];
//    BYTE *pData = new BYTE[size];
//    BYTE pData[size];
    //55GrdPI_Read(hGrd, num , 0, size, &pData, NULL, NULL);
    QByteArray ba;
    //55for (int i=0;i<sizeof(pData);i++) {
        //55ba[i] = pData[i];
    //55}
//    qDebug() << "ALicense::readPI end" << ba;
    return ba;
}


bool ALicense::checklicense() {
    bool result = false;
//    qDebug() << "ALicense::checklicense()" << ASecure::encodeStr(this->readPI(1, 16), glob_vars["tempkey"]);
//    QByteArray license = "DkRWUQ4EQ1wPWVsDTVBfVQ==";
//    QByteArray etalonLicense = ASecure::decodeStr(license, encodeKey);

//    qDebug() << "ALicense::checklicense() uikey" << this->readPI(2, 16);
//    qDebug() << "checklicense" << this->readPI(1, 16);


//    qDebug() << "License::checklicense()" << ASecure::encodeStr(this->readPI(1, 16), glob_vars["tempkey"]);
    if (ASecure::encodeStr(this->readPI(1, 16), globals["tempkey"]) != "WwosPWZaNhxsBxVHPAYTVA==") {qFatal("");}
//    qDebug() << "1";
//    if (AFunc::getRegValue("approved").toString()!="924") {qFatal("Secure key not found!");}
//    QDate currentDate = QDateTime::currentDateTime().date();
//    QDate criticalDate;
//    criticalDate.setDate(2012, 02, 15);
//    if (currentDate>=criticalDate){ADatabase::updateParam("reg", "value='26'", "K='approved'");}

    return result;
}

bool ALicense::readkey() {
    QByteArray keyLicense = this->readPI(2, 32);
    globals["uikey"].clear();
    for (int i=0;i<keyLicense.length();i++) {
        globals["uikey"].append(keyLicense.at(i));
    }
    if (keyLicense.length()>0) {return true;}
//    qDebug() << "uikey" << glob_vars["uikey"].length();
    return true;
}
