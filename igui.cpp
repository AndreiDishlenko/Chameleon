#include "igui.h"
#include <QDebug>
///////////////////////////////////////////////////////////////////////
// CLASS iGui provide set of functions for advanced work with GUI forms
///////////////////////////////////////////////////////////////////////

iGui::iGui(QObject *parent) : QObject(parent) {}
iGui::~iGui() {
//    qDebug() << "~iGui";
}

// PRIVATE functions
bool iGui::clearLayout(QGridLayout *layout) {
    QLayoutItem *item;
    while( (item = layout->itemAt(0)) ) {
//        qDebug() << item;
        layout->removeItem( item );
        if (item->widget()) {
            item->widget()->hide();
            layout->removeWidget(item->widget());
            delete item->widget();
        }
        delete item;
        layout->update();
    }
    return true;
}

#include "adataset.h"
#include "adatabase.h"
QVariantList iGui::readGridDataSource(QScrollArea *sa) {
//    QList< QMap<QString, QVariant> > result;
    QVariantList result;
    QString query = ADataset::readDataSource(sa);
    QList<QMap<QString, QVariant> > tempList = ADatabase::selectList(query);
    QMap<QString, QVariant> temp;
    for (int i=0;i<tempList.count();i++) {
        temp["id"] = tempList.at(i).value("id").toString();
        temp["name"] = tempList.at(i).value("name").toString();
        temp["idGroup"] = tempList.at(i).value("idGroup").toString();
        result.append(temp);
    }
    return result;
}

#include <agui.h>
bool iGui::create_iButtonsGrid(QScrollArea *sa) {
    int columns = sa->property("Columns").toInt();
    int buttonsWidth = sa->property("ButtonsWidth").toInt();
//    int rows = sa->property("Rows").toInt();
//    int buttonsHeight = sa->property("ButtonsHeight").toInt();
    if (columns==0) {
        columns = int(sa->width()/buttonsWidth - 0.5);
    }
    sa->setProperty("columns", columns);

    QGridLayout *layout = static_cast<QGridLayout*>(sa->widget()->layout());
    iGui::clearLayout(layout);
    layout->setHorizontalSpacing(sa->property("Spacings").toInt());
    layout->setVerticalSpacing(sa->property("Spacings").toInt());
    layout->setMargin(sa->property("Margins").toInt());

    AGui::setObjectView(sa);
    return true;
}

QObject * iGui::addButton(QScrollArea *sa, QString text, int row, int column, int width, int height, QString btnstyle) {
    APush *btn = new APush();
    btn->setStyleSheet(btnstyle);
    btn->setMinimumSize(width, height);
    btn->setMaximumSize(width, height);
    btn->setHtml(text);
//    btn->setHtml("<p style='"+textstyle+"'>"+text+"</p>");
//    btn->setText("<center><p style='"+sa->property("textStyle").toString()+"'>"+text+"</p></center>");
//    btn->setProperty("text", "<center><p style='"+sa->property("textStyle").toString()+"'>"+text+"</p></center>");
    QGridLayout *layout = static_cast<QGridLayout*>(sa->widget()->layout());
    layout->addWidget(btn, row, column);
    return btn;
}

void iGui::addSpacer(QScrollArea *sa, int row, int column, int width, int height) {
    QSpacerItem * vSpacer = new QSpacerItem(width, height, QSizePolicy::Expanding, QSizePolicy::Expanding);
    QGridLayout *layout = static_cast<QGridLayout*>(sa->widget()->layout());
    layout->addItem(vSpacer, row, column);
}

// Create grid of iPhone style buttons
#include <QVariant>
#include "afunc.h"

//bool iGui::create_iButtonsGrid(QScrollArea *sa) {
//    QList< QMap<QString, QVariant> > btnMap = iGui::readGridDataSource(sa);
//    int columns = sa->property("Columns").toInt();
//    int rows = sa->property("Rows").toInt();
//    int buttonsWidth = sa->property("ButtonsWidth").toInt();
//    int buttonsHeight = sa->property("ButtonsHeight").toInt();
//    if (columns==0) {
//        if (rows!=0) {columns = btnMap.count()/rows;}
//        else {columns = int(sa->width()/buttonsWidth - 0.5);}
//    }
//    QGridLayout *layout = static_cast<QGridLayout*>(sa->widget()->layout());
//    iGui::clearLayout(layout);
//    layout->setHorizontalSpacing(sa->property("Spacings").toInt());
//    layout->setVerticalSpacing(sa->property("Spacings").toInt());
//    layout->setMargin(sa->property("Margins").toInt());
//    int id;
//    QString btntext;
//    int row = 0, column = -1;
////    QMap<QString, QVariant> record;
//    for (int i=0;i<btnMap.count();i++) {
//        id = btnMap.at(i)["id"].toInt();
//        btntext = btnMap.at(i)["name"].toString();
//        group = btnMap.at(i)["idGroup"].toString();
//        APush *btn = new APush();
//        btn->setStyleSheet(sa->property("btnStyle").toString());
//        btn->setMinimumSize(buttonsWidth,buttonsHeight);
//        btn->setMaximumSize(buttonsWidth, buttonsHeight);
//        btn->setObjectName("obj"+id);
//        btn->setProperty("text", "<center><p style='"+sa->property("textStyle").toString()+"'>"+btntext+"</p></center>");
//        btn->setProperty("id", id);
////            btn->setHtml("<CENTER>"+btntext+"</CENTER>");

//        if (column==columns-1)) {
//            QSpacerItem * vSpacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding);
//            layout->addItem(vSpacer, row, columns);
//            column=0; row++;
//        } else {column++;}
//        layout->addWidget(btn, row, column);
//        sa->setProperty("id", id);
//        QString action = ADataset::updateStringVariablesFromWidget(sa->property("onClick").toString(), sa);
//        btn->setProperty("onClick", action);
//        btn->show();
//    }
//    QSpacerItem * vSpacer = new QSpacerItem(1000, 1000, QSizePolicy::Expanding, QSizePolicy::Expanding);
//    layout->addItem(vSpacer, row+1, columns);
//    AGui::setObjectView(sa);
//    return true;
//}


// Create iPhone style icons
bool iGui::iGuiScrollIcons(QScrollArea *sa, int imgWidth, int imgHeight, QString orientation) {
    QGridLayout *layout = static_cast<QGridLayout*>(sa->widget()->layout());
    iGui::clearLayout(layout);

    layout->setHorizontalSpacing(3);
    layout->setVerticalSpacing(3);
    layout->setMargin(3);
    layout->setSizeConstraint(QLayout::SetMinimumSize);
    //layout->setAlignment(Qt::AlignRight);

    sa->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    if (imgWidth==0 && imgHeight==0) {return false;}

    QString sqlString = ADataset::readDataSource(sa);
    if (sqlString=="") {return false;}

    QList< QMap<QString, QVariant> > records = ADatabase::selectList(sqlString);
    if (records.count()==0) {
        sa->hide();
        return false;
    }

    int width=0;
    int height=0;
    QSize size;
    QString id, name;

    if (records.count()==0) {return false;}

    int colcount=6;
    int rowcount = qCeil( qreal(records.count()) / qreal(colcount) );

    sa->setMinimumHeight((rowcount)*(imgHeight)+12);
    sa->setMaximumHeight((rowcount)*(imgHeight)+12);

    int row=0;
    int column = 0;

    //qDebug() << "iGui::iGuiScrollIcons" << sa->width() << records.count() << colcount << rowcount;

    for (int i=0;i<records.count();i++) {
        if (column == colcount-1) {row++; column=0;}
        id = records.at(i).value("id").toString();

        QPushButton *btn = new QPushButton();

        QLabel *label = new QLabel(records.at(i).value("name").toString(), btn);
        QSizePolicy sp = label->sizePolicy();
        sp.setHorizontalStretch(5);
        label->setSizePolicy(sp);
        label->setMaximumWidth(1000);
        label->setWordWrap(true);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("margin:10px;");

        QLabel *label_price = new QLabel(records.at(i).value("price").toString()+"\nгрн.", btn);
        label_price->setAlignment(Qt::AlignCenter);
        label_price->setMinimumWidth(35);
        label_price->setMinimumHeight(imgHeight-6);
        label_price->setStyleSheet("font-weight:bold; color:white; border:0px; background-color:#999999;");

        auto btn_layout = new QHBoxLayout(btn);
        btn_layout->setMargin(3);
        btn_layout->setSpacing(0);

        btn_layout->addWidget(label, 0, Qt::AlignCenter);
        btn_layout->addWidget(label_price, 0, Qt::AlignCenter);
        btn_layout->itemAt(0)->widget()->setStyleSheet("");

        btn->setStyleSheet("QWidget {background-color:white; } QPushButton {border:1px solid #999999;  }");

        btn->setMinimumSize(imgWidth,imgHeight);
        //btn->setMaximumSize(imgWidth,imgHeight);
        btn->setObjectName("obj"+id);

        if (orientation=="horizontal") {
            layout->addWidget(btn, row, column);
            //qDebug() << i << row << column;
        } else if (orientation=="vertical") {
            layout->addWidget(btn, i*2, 0);
        }
        sa->setProperty("id", id);
        QString action = ADataset::updateStringVariablesFromWidget(sa->property("onClick").toString(), sa);
        btn->setProperty("onClick", action);
        btn->show();
        column++;
    }
    AGui::setObjectView(sa);

    return true;
}



QObjectList iGui::getScrollAPushButtons(QScrollArea *sa) {
    QObjectList objlist;
    QGridLayout *layout = static_cast<QGridLayout*>(sa->widget()->layout());
    QLayoutItem *item;
    QString classname;
    for (int i=0;i<layout->count();i++) {
        item = layout->itemAt(i);
//        QSpacerItem * vSpacer = static_cast<QSpacerItem *>(item);
//        qDebug() << vSpacer;
        if (item->widget()) {
            classname = item->widget()->metaObject()->className();
            if (classname=="QPushButton"){
                objlist.append(item->widget());
            }
        }
    }
    return objlist;
}

