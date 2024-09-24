#ifndef AEXPORTXML_H
#define AEXPORTXML_H

#include <QObject>
#include <QVariantList>

class AExportXml : public QObject
{
    Q_OBJECT
public:
    explicit AExportXml(QObject *parent = 0);

signals:
    
public slots:
    QString exportDict(QString filename, QVariantList vlist);
};

#endif // AEXPORTXML_H
