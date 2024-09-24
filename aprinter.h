#ifndef APRINTER_H
#define APRINTER_H

#include <QObject>
#include <QString>
#include <QPrinter>
#include <QtSql>
#include <QWebEngineView>

class APrinter : public QObject {
    Q_OBJECT
public:
   APrinter(QObject *parent = 0);
   ~APrinter();

   QPrinter *currentPrinter;

   QString HtmlSource;
   QWebEngineView *wv;

//   QList<QString> getPrintersList();
//   QString getDefaultPrinterName();

   static bool printCheck(QString Check);
//   static QString parseHtmlFile(QString htmlcheck, QAbstractItemModel *model, QMap <QString, QString> mapname);

   QString ListHtm;
private:

   bool printSqlQueryModel(QString fileName, QMap<QString, QVariant> headerMap, QSqlQueryModel *sqlmodel, QString prName="");
   static QString parseHtmlFile(QString filename, QMap<QString, QVariant> mapname, QAbstractItemModel * model);
   QPrinter * getPrinterByName(QString printerName="");

private slots:
   void preview();
   void print();

public slots:
   QStringList getPrintersList();

   QString getDefaultPrinter();

   void setCurrentPrinter(QString prnname);

   bool previewHtml(QString html);
   bool printHtml(QString html, QString printername="");

//   bool previewSql(QString fileName, QMap<QString, QVariant> headerMap, QString querystring);
//   bool printSql(QString fileName, QMap<QString, QVariant> headerMap, QString querystring);
//   bool printSqlToHtml(QString fileName, QMap<QString, QVariant> headerMap, QString querystring, QString htmlfilename);

   void previewSlot(QPrinter *prn);

   //   bool printDict(QString fileName, QMap<QString, QVariant> headerMap, ADict *dict);
   //   void printWidget(QString fileName, QMap<QString, QVariant> headerMap, QWidget *tableformwidget);
   //   bool printPrepare(QString fileName, QMap<QString, QVariant> headerMap, QString querystring);
   //   bool printPreview(QString fileName, QMap<QString, QVariant> headerMap, QSqlQueryModel *model);
//   void printReport(QString fileName, QMap<QString, QVariant> headerMap, ATableWidget *tableform);
};

#endif // APRINTER_H
