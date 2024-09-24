#ifndef APUSH_H
#define APUSH_H

#include <QPushButton>
#include <QLabel>
#include <QVariant>
#include <QDebug>
#include <QStyle>
#include <QGraphicsDropShadowEffect>
#include <QVariant>
#include <QtWebEngineWidgets/QtWebEngineWidgets>

class APush : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QVariant text READ getText WRITE sys_setText)
    Q_PROPERTY(QVariant html READ getHtml WRITE sys_setHtml)
    Q_PROPERTY(QVariant shadowLength READ getShadowLength WRITE setShadowLength)
    Q_PROPERTY(QVariant shadowBlur READ getShadowBlur WRITE setShadowBlur)

public:
    APush(QWidget *parent = 0);

    void sys_setText (QVariant value) {
//        qDebug() << "sys_setText" << value;
        htmlText = "";
        wv->setHtml("");
        textText = value.toString();
        QPushButton::setText(textText);
    }
    QVariant getText() {return textText;}
    void sys_setHtml (QVariant value) {
//        qDebug() << "sys_setHtml" << value;
        htmlText = value.toString();
        QPalette palette;
        palette.setBrush(QPalette::ButtonText, Qt::transparent);
        setPalette(palette);
        wv->setHtml(htmlText);
        //wv->page()->mainFrame()->evaluateJavaScript("document.body.style.margin='0';");
        wv->resize(this->size());
    }
    QVariant getHtml() {return htmlText;}

    QVariant getShadowLength() {return m_shadowLength;}
    void setShadowLength(QVariant value) {
        m_shadowLength = value.toInt();
        applyEffect();
    }

    QVariant getShadowBlur()  {return m_shadowBlur;}
    void setShadowBlur(QVariant value) {
        m_shadowBlur = value.toInt();
        applyEffect();
    }

    QWebEngineView *wv;

private:
    QString textText;
    QString htmlText;

    int m_shadowLength;
    int m_shadowBlur;
    int def_shadowLength;
    int def_shadowBlur;

signals:
    void changeTest(QVariant value);

public slots:
    void setText(QString text);
    void setHtml(QString html);

    void applyEffect() {
        QGraphicsDropShadowEffect *shadow = static_cast<QGraphicsDropShadowEffect *>(this->graphicsEffect());
        shadow->setOffset(m_shadowLength);
        shadow->setBlurRadius(m_shadowBlur);
    }
    void setEffect() {
        this->style()->polish(this);
        m_shadowLength = this->property("shadowLength").toInt();
        m_shadowBlur = this->property("shadowBlur").toInt();
        applyEffect();
    }
    void updateEffect() {
        m_shadowLength = this->property("shadowLength").toInt();
        m_shadowBlur = this->property("shadowBlur").toInt();
        applyEffect();
    }
    void resetEffect() {
        m_shadowLength = def_shadowLength;
        m_shadowBlur = def_shadowBlur;
        applyEffect();
    }

    void refresh();

protected:
    void paintEvent(QPaintEvent *);
    QStyleOptionButton getStyleOption() const;

};

#endif
