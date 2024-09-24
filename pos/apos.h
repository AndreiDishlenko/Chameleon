#ifndef APOS_H
#define APOS_H

#include <QObject>
#include <QMap>
#include <QVariant>

class APos : public QObject
{
    Q_OBJECT
public:
    explicit APos(QObject *parent = 0);

signals:

public slots:
    QMap<QString, QVariant> getTableInfo(int idTable=0);
};

#endif // APOS_H
