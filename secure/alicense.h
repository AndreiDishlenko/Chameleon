#ifndef ALICENSE_H
#define ALICENSE_H

#include <QObject>
#include "grdapi.h"

class ALicense : public QObject
{
    Q_OBJECT
public:
    explicit ALicense(QObject *parent = 0);
    ~ALicense();
    bool initapi();
    bool closeapi();

    QByteArray readPI(int num, const int size);

private:
    HANDLE hGrd;
    QString encodeKey;

signals:

public slots:
    bool checklicense();
    bool readkey();

};

#endif // ALICENSE_H
