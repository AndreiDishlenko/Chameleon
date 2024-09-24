#include "apush.h"

#include <QPainter>
#include <QPixmap>
#include <QTextDocument>
#include <QIcon>
#include <QSize>
#include <QMenu>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QDebug>
#include <QGraphicsDropShadowEffect>


APush::APush(QWidget *parent) :  QPushButton(parent) {
    htmlText = "HTML button";
    wv = new QWebEngineView();
    QPalette palette = wv->palette();
    palette.setBrush(QPalette::Base, Qt::transparent);
    //55wv->page()->setPalette(palette);
    wv->setAttribute(Qt::WA_OpaquePaintEvent, false);
    //55wv->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    //55wv->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
//    this->setStyleSheet("border:1px solid #333333; border-radius:4px; font:normal 11px Tahoma; background-color:#eeeeee;");
    QObject::connect(wv, SIGNAL(loadFinished(bool)), this, SLOT(refresh()));

    def_shadowLength = 0;
    def_shadowBlur = 0;

////        def_shadowLength = this->property("shadowLength").toInt();
////        if (def_shadowLength>15) {def_shadowLength=0;}
////        def_shadowBlur = this->property("shadowBlur").toInt();
////        if (def_shadowBlur>15) {def_shadowBlur=0;}

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(parent);
    this->setGraphicsEffect(shadow);
    shadow->setEnabled(true);
    resetEffect();

    setProperty("shadowLength", def_shadowLength);
    setProperty("shadowBlur", def_shadowBlur);

    QObject::connect(this, SIGNAL(pressed()), this, SLOT(resetEffect()));
    QObject::connect(this, SIGNAL(released()),this, SLOT(setEffect()));

    QPushButton::setText(htmlText);
}
void APush::setText(QString text){
    this->setProperty("text", text);
}

void APush::setHtml(QString html){
    this->setProperty("html", html);
}

#include <QRect>
void APush::paintEvent(QPaintEvent *event) {
//    qDebug() << "APush::paintEvent" << htmlText;
    if (htmlText!="") {
//        wv->setStyleSheet(this->styleSheet());
        QPoint point;
        QRect buttonRect = rect();
        wv->resize(buttonRect.width()-2, buttonRect.height()-2);
//        if (!icon().isNull()) {
//            point = QPoint(buttonRect.x() + buttonRect.width() / 2 + iconSize().width() / 2 + 2, buttonRect.y() + buttonRect.height() / 2);
//        } else {
//            point = QPoint(buttonRect.x() + buttonRect.width() / 2, buttonRect.y() + buttonRect.height() / 2);
//        }


//        buttonRect.translate(point.x() - wvPixmap.width() / 2, point.y() - wvPixmap.height() / 2);
        QPixmap wvPixmap(buttonRect.width()-2, buttonRect.height()-2);
        wvPixmap.fill(Qt::transparent);
//        wvPixmap = QPixmap::grabWidget(wv, buttonRect);
        wv->render(&wvPixmap, QPoint(), QRegion(), IgnoreMask);

        QStylePainter p(this);
        QStyleOptionButton opt = this->getStyleOption();
        p.drawControl(QStyle::CE_PushButton, opt);
//        p.drawPixmap(buttonRect.left()+1, buttonRect.top()+1, wvPixmap.width()-2, wvPixmap.height()-2, wvPixmap);
        p.drawPixmap(buttonRect.left()+1, buttonRect.top()+1, buttonRect.width()-2, buttonRect.height()-2, wvPixmap);
    } else {
        QPushButton::paintEvent(event);
    }
}

QStyleOptionButton APush::getStyleOption() const {
    QStyleOptionButton opt;
    opt.initFrom(this);

    opt.features = QStyleOptionButton::None;
    if (isFlat())
        opt.features |= QStyleOptionButton::Flat;
    if (menu())
        opt.features |= QStyleOptionButton::HasMenu;
    if (autoDefault() || isDefault())
        opt.features |= QStyleOptionButton::AutoDefaultButton;
    if (isDefault())
        opt.features |= QStyleOptionButton::DefaultButton;
    if (isDown() || (menu() && menu()->isVisible()))
        opt.state |= QStyle::State_Sunken;
    if (isChecked())
        opt.state |= QStyle::State_On;
    if (!isFlat() && !isDown())
        opt.state |= QStyle::State_Raised;

    opt.text = text();
    opt.icon = icon();
    opt.iconSize = iconSize();
    return opt;
}

void APush::refresh() {
    this->repaint();

}
