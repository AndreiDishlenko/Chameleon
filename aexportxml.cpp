#include "aexportxml.h"
#include <QtXml/QDomDocument>
#include <QFile>
#include <QDebug>

AExportXml::AExportXml(QObject *parent) : QObject(parent) {
}

QString AExportXml::exportDict(QString filename, QVariantList vlist) {
    QString result;

    QDomDocument *doc = new QDomDocument();
    QDomNode xmlNode = doc->createProcessingInstruction("xml", "version=\"1.0\" encoding=\"windows-1251\" standalone=\"no\"");
    doc->insertBefore(xmlNode, doc->firstChild());

    QDomElement root = doc->createElement("DOCUMENT");
    doc->appendChild(root);

    QMap<QString, QVariant> rowmap;
    for (int i=0;i<vlist.count();i++){
        QDomElement row = doc->createElement("ROW");
        root.appendChild(row);

        rowmap = vlist.at(i).toMap();
        for (int i1=0;i1<rowmap.keys().count();i1++) {
            QDomElement attr = doc->createElement(rowmap.keys().at(i1));
            row.appendChild(attr);

            QDomText value = doc->createTextNode(rowmap.values().at(i1).toString());
            attr.appendChild(value);
        }
    }

//    doc->save();


    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)){
       return  "Ошибка открытия файла \""+filename+"\" для записи.";
    }
    QTextStream stream(&file);
    stream << doc->toString();
//    file.write(doc->toByteArray());
    file.close();

    result="Файл \""+filename+"\" успешно создан.";
    return result;
}
