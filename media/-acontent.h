#ifndef -ACONTENT_H
#define -ACONTENT_H

#include <QObject>
#include <QWidget>
#include "qaxwidget.h"
#include "-acropdflib.h"

class AContent : public QWidget {
    Q_OBJECT

public:
    explicit AContent(QWidget *parent = 0);
    ~AContent();

    bool loaded;

    int page;
    int posy;

    QWidget *widget;

    QAxWidget *wordwgt;
    QAxWidget *excelwgt;
    AcroPDFLib::AcroPDF *pdf;

    QAxObject *excelworkbooks;


    QAxWidget *document;
    QAxObject *application;
    QAxObject *documents;
    QAxObject *window;

    QAxObject *wordwindow;

    void load();

signals:

public slots:
    void init();
    void clear();
    void scrollUp();
    void scrollDown();
    void printOut();
    void test();
};

#endif // -ACONTENT_H
