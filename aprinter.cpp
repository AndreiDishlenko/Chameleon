#include "aprinter.h"
#include "afunc.h"
#include "adict.h"
#include "globals.h"
#include <QPrinter>
#include <QString>
#include <QTextStream>
#include <QPrintPreviewDialog>
#include <QPrinterInfo>
#include <QtSql>
#include <QApplication>
#include <QDesktopWidget>

APrinter::APrinter(QObject *parent) : QObject(parent){
    //    AFunc::echoTimer("APrinter::APrinter");
    wv = new QWebEngineView();

    this->setObjectName("APrinter");
    currentPrinter = new QPrinter();
    //currentPrinter->setPageSize(QPageSize(QSize(200,100000)));
    //currentPrinter->setPageSize(QPrinter::A5);
    //currentPrinter->setResolution(300);
    //currentPrinter->setResolution(QPrinter::HighResolution);
    currentPrinter->setCopyCount(1);
    currentPrinter->setDoubleSidedPrinting(false);
    currentPrinter->setDuplex(QPrinter::DuplexNone);
    //currentPrinter->setColorMode(QPrinter::Color);
    //currentPrinter->setPaperSource(QPrinter::Auto);
    currentPrinter->setCreator("ChameleonApp");
    currentPrinter->setDocName("Chameleon Document");
    //currentPrinter->setFullPage(false);
    //currentPrinter->setOrientation(QPrinter::Portrait);
    //currentPrinter->setOutputFormat(QPrinter::NativeFormat);

    setCurrentPrinter(AApplication::getLocalSetting("defCheckPrinter").toString());

    //qDebug() << "APrinter::APrinter" << currentPrinter->printerName();
}
APrinter::~APrinter(){
    //qDebug() << "~APrinter()";
    currentPrinter->~QPrinter();
    wv->~QWebEngineView();
}

/////////////////////////////////////////////////////////////
// Выбор принтера
/////////////////////////////////////////////////////////////
QStringList APrinter::getPrintersList() {
    QStringList result;
    QList<QPrinterInfo> printerslist = QPrinterInfo::availablePrinters();
    //qDebug() << "APrinter::getPrintersList" << printerslist << QPrinterInfo::availablePrinters() << QPrinterInfo::availablePrinterNames() << QPrinterInfo::defaultPrinterName() << defPrn.defaultPrinterName() << defPrn.availablePrinterNames();
    for (int i=0; i < printerslist.count(); i++) {
        result << printerslist.at(i).printerName();
        //qDebug() << "APrinter::getPrintersList()" << i << printerslist.at(i);
    }
    return result;
}

QString APrinter::getDefaultPrinter() {
    QPrinterInfo defPrn;
    return defPrn.defaultPrinter().printerName();
}

void APrinter::setCurrentPrinter(QString prnname){
    //qDebug() << "APrinter::setCurrentPrinter" << prnname;
    QPrinterInfo defPrn;

    if (prnname=="") {
        qWarning() << "[Warning] APrinter::setCurrentPrinter Can't set '"+prnname+"' printer. Set it to default system printer.";
        prnname = defPrn.defaultPrinter().printerName();
    }

    QStringList printersList = this->getPrintersList();
    if (!printersList.contains(prnname)) {
        qWarning() << "[Warning] APrinter::setCurrentPrinter(). '"+prnname+"' is not available. Set it to default system printer.";
        prnname = defPrn.defaultPrinter().printerName();
    }

    //qDebug() << "APrinter::setCurrentPrinter" << prnname;
    //qDebug() << "APrinter::setCurrentPrinter" << QPrinterInfo::availablePrinterNames();
    //qDebug() << currentPrinter->printerName() << QPrinterInfo::defaultPrinterName() << prnname;

    currentPrinter->setPrinterName(prnname);

    int supported_resolution = currentPrinter->supportedResolutions().at(0);
    currentPrinter->setResolution(supported_resolution);

    //qDebug() << "APrinter::setCurrentPrinter" << currentPrinter->supportedResolutions() << currentPrinter->resolution();
    //currentPrinter->setResolution(QPrinter::HighResolution);
    //qDebug() << "done";
}

/////////////////////////////////////////////////////////////
// ПЕЧАТАЮЩИЕ СЛОТЫ
/////////////////////////////////////////////////////////////
bool APrinter::previewHtml(QString html) {
    QObject::connect(wv, SIGNAL(loadFinished(bool)), this, SLOT(preview()));
    wv->page()->setHtml(html);
    return true;
}
bool APrinter::printHtml(QString html, QString printername) {
    //qDebug() << "APrinter::printHtml" << printername;
    if (printername!="") {
        this->setCurrentPrinter(printername);
    }
    QObject::connect(wv, SIGNAL(loadFinished(bool)), this, SLOT(print()));
    wv->page()->setHtml(html, QUrl("file://") );

    return true;
}

/////////////////////////////////////////////////////////////
// ПЕЧАТАЮЩИЕ ФУНКЦИИ
/////////////////////////////////////////////////////////////
void APrinter::print() { // Печать HTML
    //qDebug() << currentPrinter->printerName();
    //QPrinter *prn = currentPrinter;
    QObject::disconnect(wv, SIGNAL(loadFinished(bool)), this, SLOT(print()));
    if (currentPrinter) {
        qDebug() << "APrinter::print() print to:" << currentPrinter->printerName();
        //currentPrinter->setResolution(QPrinter::HighResolution);
        wv->page()->print(currentPrinter, [=](bool){});
        //wv->show();
    }
}

void APrinter::preview(){ // Печать HTML с предпросмотром
    QObject::disconnect(wv, SIGNAL(loadFinished(bool)), this, SLOT(preview()));
    //    qDebug() << "APrinter::preview";
    //    HtmlSource = HtmlString;
    //    if (prName=="") {prName = this->getDefaultPrinterName();}
    //    if (prName=="") {qDebug() << "Warning! APrinter::printPreview There are no default printers. Print cancelled."; return false;}
    QPrintPreviewDialog preview(currentPrinter);
    int width=QApplication::desktop()->screenGeometry().width()-16;
    int height=QApplication::desktop()->screenGeometry().height()-80;
    preview.setGeometry(QRect(8,30, width, height));

    QObject::connect(&preview, SIGNAL(paintRequested(QPrinter*)), wv, SLOT(print(QPrinter*)));
    preview.exec();
    QObject::disconnect(&preview, SIGNAL(paintRequested(QPrinter*)), wv, SLOT(print(QPrinter*)));
    //    QObject::disconnect(&preview, SIGNAL(paintRequested(QPrinter*)),this, SLOT(previewSlot(QPrinter*)));
}

#include <QTextDocument>
#include "aapplication.h"
void APrinter::previewSlot(QPrinter *prn){ // Системный SLOT для отображения preview
    //    QString strHtmlImgTag( "<img width=\"100\" height=\"100\" src=\"data:image/png;base64,%1\"/>" );
    //    QBuffer imageBuffer;
    //    imageBuffer.open(QBuffer::ReadWrite);
    //    QImage img( "D:/salateira.png" );
    //    if( img.save( &imageBuffer, "PNG" ) )
    //    {
    //        HtmlSource.append( strHtmlImgTag
    //        .arg( QLatin1String(imageBuffer.buffer().toBase64()) ) );
    //    }

    //    QWebEngineView *wv1 = new QWebEngineView();
    //    wv1->setHtml(HtmlSource);
    //    wv1->print(prn);

    QWebEngineView *wv = new QWebEngineView();
    //    QDir dir;
    //    QUrl url(QUrl::fromLocalFile(dir.absoluteFilePath("D:/Chameleon/salateira.png")));
    //    QString path = QDir::currentPath();
    //55wv->setHtml(HtmlSource);
    //55wv->print(prn);

    //    wv->setRenderHint(QPainter::SmoothPixmapTransform, true);
    //    wv->page()->settings()->setAttribute(QWebSettings::AutoLoadImages, true);
    //    wv->page()->settings()->setAttribute(QWebSettings::PrintElementBackgrounds, false);


    //    wv->page()->settings()->setAttribute(QWebSettings::AutoLoadImages, true);
    //    wv->page()->settings()->setAttribute(QWebSettings::PrintElementBackgrounds, false);
    ////        wv->load(QUrl("www.elotouch.com.ua"));
    //    AApplication::pause(1000);

    //    QImage *img = new QImage("D:/salateira.png");
    //        QTextDocument *rt = new QTextDocument();
    //        qDebug() << wv->size().width() << ", " << wv->size().height();
    //        rt->setPageSize(wv->size());
    //        rt->setHtml(HtmlSource);
    //        rt->print(prn);
    //        rt->~QObject();

    //        wv->~QObject();
}

// ПАРСЕР



//bool APrinter::printDict(QString fileName, QMap<QString, QVariant> headerMap, ADict *dict) {
    //void APrinter::printWidget(QString fileName, QMap<QString, QVariant> headerMap, QWidget *tableformwidget) {
    //    qDebug() << "APrinter::printWidget";
    //    ATableWidget *tableform = tableformwidget->findChild<ATableWidget*>("Table");
    //    QString readyHtml = APrinter::parseHtmlFile(fileName, headerMap, tableform->getSqlModel());
    //    APrinter::printPreview(readyHtml);
//}


// Открывает файл, парсит html, отправляет на печать
bool APrinter::printSqlQueryModel(QString fileName, QMap<QString, QVariant> headerMap, QSqlQueryModel *sqlmodel, QString prName) {
    bool result = true;
//    QString forPrint = APrinter::parseHtmlFile(fileName, headerMap, sqlmodel);
//    APrinter::print(forPrint/*, prName*/);
//    qDebug() << forPrint;
//  APrinter *PPrewv = new APrinter();
//  PPrewv->printPreview(forPrint);

    return result;
}
//bool APrinter::previewDict(QString fileName, QMap<QString, QVariant> headerMap, ADict *dict) {
//bool APrinter::previewWidget(QString fileName, QMap<QString, QVariant> headerMap, QObject *obj) {
//bool APrinter::previewSql(QString fileName, QMap<QString, QVariant> headerMap, QString *querystring) {

//void APrinter::printReport(QString fileName, QWidget *formwidget, QWidget *tableformwidget) {
//    ATableWidget *tableform = formwidget->findChild<ATableWidget*>("Form");
////    qDebug() << form << tableform;
//    QMap <QString, QString> mapname;
//    QAbstractItemModel *model1 = tableform->tableview->model();
//    QString report = APrinter::parseHtmlFile(fileName, model1, mapname);
//    APrinter::printPreview(report);
//}




//bool APrinter::previewSql(QString fileName, QMap<QString, QVariant> headerMap, QString querystring) {
//    QSqlQueryModel *qModel = new QSqlQueryModel();
//    ADatabase::selectSql(querystring, "", qModel);
//    if (qModel->lastError().isValid()) {
//        qDebug() << qModel->lastError();
//        }
//    QString readyHtml = APrinter::parseHtmlFile(fileName, headerMap, qModel);
////    qDebug() << "APrinter::previewSql" << QDir::currentPath();
//    qModel->~QSqlQueryModel();
//    QObject::connect(wv, SIGNAL(loadFinished(bool)), this, SLOT(preview()));
//    wv->setHtml(readyHtml);
//    return true;
//}

//bool APrinter::printSql(QString fileName, QMap<QString, QVariant> headerMap, QString querystring) {

//    QObject::connect(wv, SIGNAL(loadFinished(bool)), this, SLOT(print()));
//    wv->setHtml(readyHtml);
//    return true;
//}

//bool APrinter::printSqlToHtml(QString fileName, QMap<QString, QVariant> headerMap, QString querystring, QString htmlfilename) {
//    QSqlQueryModel *qModel = new QSqlQueryModel();
//    ADatabase::selectSql(querystring, "", qModel);
////    qModel->setQuery(querystring);
//    if (qModel->lastError().isValid()) {
//        qDebug() << qModel->lastError();
//        }
//    QString readyHtml = APrinter::parseHtmlFile(fileName, headerMap, qModel);
//    qModel->~QSqlQueryModel();



//    return true;
//}

//QString APrinter::getDefaultPrinterName() {
//}
//QPrinter * APrinter::getPrinterByName(QString printerName) {
//    QPrinter *result = new QPrinter();
//    if (printerName=="") {
//        result = currentPrinter;
//    } else {
//        result->setPrinterName(printerName);
//    }
//    return result;
//}
