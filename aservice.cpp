#include "aservice.h"
#include "afunc.h"
#include "aapplication.h"
#include <QString>


AService::AService(QObject *parent) : QObject(parent) {
}
AService::~AService() {
//    qDebug() << "~AService";
}

bool AService::syncDir(QString SDir_Server, QString SDir_Host, QProgressBar *progressbar) {
    qDebug() << "AService::syncDir" << SDir_Server << SDir_Host;
    QDir DirHst;
    if (!DirHst.exists(SDir_Server)) {qDebug() << "Error ABuild::syncDir. Directory '"+SDir_Server+"' is not available."; return false;}
    if (!DirHst.exists(SDir_Host)) {qDebug() << "Error ABuild::syncDir. Directory '"+SDir_Host+"' is not available."; return false;}
    QMultiMap<QString , QString> MMDir_Server = AFunc::getDirTree(SDir_Server, SDir_Server);
    QMultiMap<QString , QString> MMDir_Host = AFunc::getDirTree(SDir_Host, SDir_Host);
    QFile tempFile;
    int count, counter = 0;

    QMultiMap<QString , QString> :: iterator it;

    it = MMDir_Server.begin();
    for(;it != MMDir_Server.end(); ++it) {counter++;}

    if (counter==0) {count=1;} else {count = counter;}
    counter=0;

    it = MMDir_Server.begin();
    for(;it != MMDir_Server.end(); ++it){
        counter++;
        progressbar->setValue(20+counter*30/count);

        QString it_servDir = it.key(), it_servFile = it.value();
        QString it_hostDir = it_servDir;
        if(!MMDir_Host.contains(it_hostDir, it_servFile)){
            DirHst.mkdir(SDir_Host+it_hostDir);
        }
        QString servFilePath = SDir_Server+it_servDir+"/" +it_servFile;
        QString hostFilePath = SDir_Host + it_hostDir + "/" + it_servFile;

        if (MMDir_Host.contains(it_hostDir, it_servFile)){
            QFile copyFile(servFilePath);
            QFile newFile(hostFilePath);
            QFileInfo servF_inf(copyFile);
            QFileInfo hostF_inf(newFile);
            QDateTime last_ModDateServ = servF_inf.lastModified();
            QDateTime last_ModDateHost = hostF_inf.lastModified();
            int size_s = copyFile.size();
            int size_h = newFile.size();
            if(size_s != size_h || last_ModDateHost != last_ModDateServ ){
                newFile.remove(hostFilePath);
                tempFile.copy(servFilePath, hostFilePath);
            }
        } else {
            tempFile.copy(servFilePath, hostFilePath);
        }
    }

    it = MMDir_Host.begin();
    for(;it != MMDir_Host.end(); ++it){
        QString temp_HostDir = it.key(), temp_HostFile = it.value();
        if(!MMDir_Server.contains(temp_HostDir, temp_HostFile)){
            tempFile.remove(SDir_Host +temp_HostDir + "/" +temp_HostFile);
        }
    }
    return true;
}


