#include "acontent.h"
#include "QAxObject"
#include "qaxwidget.h"
#include "QtGui"

AContent::AContent(QWidget *parent) : QWidget(parent) {
//    qDebug() << "AContent::AContent";
    this->setObjectName("Content");
    widget = parent;
    loaded=false;
    wordwgt = new QAxWidget("Word.Document", widget);
    pdf = new AcroPDFLib::AcroPDF(widget);
}

AContent::~AContent() {
//    qDebug("~AContent");
    wordwgt->~QAxBase();
    pdf->setParent(0);
}

void AContent::init() {
//    qDebug() << "AContent::init()" << widget << widget->objectName();

//    wordwgt->setControl("");
    wordwgt->move(QPoint(0,0));
    wordwgt->resize(widget->size());
    wordwgt->setStyleSheet("border:1px solid black;");

    pdf->setParent(widget);
    pdf->resize(widget->size());
//    QObject::connect(pdf, SIGNAL(clicked()), this, SLOT(test()));

//    window = wordwgt->querySubObject("ActiveWindow()");
//    application = wordwgt->querySubObject("Application()");
//    wordwgt = application->querySubObject("Documents()");


//    excelwgt = new QAxWidget("Excel.Application", widget);
//    excelwgt->move(QPoint(0,0));
//    excelwgt->resize(widget->size());
//    excelwgt->setStyleSheet("border:1px solid black;");
//    excelworkbooks = excelwgt->querySubObject("Workbooks");
//    excelwgt->setProperty("Visible", true);
}

void AContent::test() {
//    qDebug() << "Acontent::test()";
}

void AContent::clear() {
//    qDebug() << "AContent::clear()";
//    wordwgt->clear();
    wordwgt->hide();
//    pdf->clear();
    pdf->hide();
//    excelwgt->clear();
    loaded=false;
}

void AContent::load() {
    QString datatype = widget->property("DataType").toString();
    QString filename = widget->property("FileName").toString();
//    qDebug() << "AContent::load()" << widget << widget->objectName() << datatype << filename;
    this->clear();
    if (datatype=="Word") {
        if (filename=="") {this->clear(); return;}
        QDir dir(filename);
        if (QFile::exists(filename) && !dir.exists()) {
            if (wordwgt->setControl(filename)) {
                window = wordwgt->querySubObject("ActiveWindow()");
                window->querySubObject("View")->querySubObject("Zoom")->setProperty("Percentage", 91);
                wordwgt->show();
                loaded=true;
            } else {qDebug() << "Error AContent::load(). Error in wordwgt->setControl("+filename+")";}
        } else {qDebug() << "Error AContent::load(). "+filename+" not exists.";}
    } else if (datatype=="PDF") {
        QDir dir(filename);
        if (QFile::exists(filename) && !dir.exists()) {
            if (pdf->LoadFile(filename)) {
                pdf->setLayoutMode("OneColumn");
                pdf->setCurrentPage(1);
                posy=0;
                page=0;
                loaded=true;
                pdf->setShowToolbar(false);
                pdf->show();
            } else {qDebug() << "Error AContent::load(). Error in pdf->setControl("+filename+")";}
        } else {qDebug() << "Error AContent::load(). "+filename+" not exists.";}
    } else if (datatype=="Excel") {
//        qDebug() << "Excel";
//        wordwgt->hide();
//        excelwgt->show();
//        if (filename=="") {this->clear(); return;}
//        QDir dir(filename);
//        if (QFile::exists(filename) && !dir.exists()) {
//            if (excelwgt->setControl(filename)) {
//                excelworkbooks->querySubObject("Open(const QString&)", filename);
//                loaded=true;
//            } else {qDebug() << "Error AContent::load(). Error in wordwgt->setControl("+filename+")";  this->clear();}
//        } else {qDebug() << "Error AContent::load(). "+filename+" not exists."; this->clear();}
//        qDebug() << "AContent::load() Excel";
//        excelwgt->setControl(filename);
//        excelwgt->show();;
//        excelwgt->setProperty("Visible", true);
//        QAxObject *workbooks = excelwgt->querySubObject("WorkBooks");
//        workbooks->dynamicCall("Open (const QString&)", filename);
//        QAxObject *workbook = excelwgt->querySubObject("ActiveWorkBook");
//        this->clear();
    } else {
//        wordwgt->setControl("");
        this->clear();
//        excelwgt->hide();
//        wordwgt->hide();
    }
}

void AContent::scrollUp() {
    QString datatype = widget->property("DataType").toString();
    if (loaded) {
        if (datatype=="Word") {
            window->dynamicCall("SmallScroll(0, 10)");
        } else if (datatype=="PDF") {
            if (posy>0) {
                posy-=50;
                pdf->setViewScroll("FitH", posy);
            } else {pdf->gotoPreviousPage();}
        }
    }
}
void AContent::scrollDown() {
    QString datatype = widget->property("DataType").toString();
    if (loaded) {
        if (datatype=="Word") {
            window->dynamicCall("SmallScroll(10)");
        } else if (datatype=="PDF") {
            if (posy==500) {posy=0; pdf->gotoNextPage(); page++;}
            else {
                posy=posy+50;
                pdf->setViewScroll("FitH", posy);
            }
        }
    }
}
void AContent::printOut() {
//    qDebug() << "AContent::printOut()";
    QString datatype = widget->property("DataType").toString();
    if (loaded) {
        if (datatype=="Word") {
            QVariant temp;
            QList<QVariant> tempList;
            tempList << temp << temp << temp << temp << temp << temp << temp << temp << temp << temp << temp << temp << temp << temp << temp << temp << 11520 << 16225;//<< 18720;
            wordwgt->dynamicCall("PrintOut(const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant& )", tempList);
        } else if (datatype=="PDF") {
            pdf->printAll();
        }
    }
}


