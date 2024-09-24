#ifndef AWEBWIDGET_H
#define AWEBWIDGET_H

#include <QObject>
#include <QWebEngineView>

class AWebWidget : public QObject
{
public:
    AWebWidget(QWidget *parent);
    ~AWebWidget();

    QWidget *container;             // QWebEngineView parent Widget
    QWebEngineView *webwidget;

    void update();
protected:
    bool eventFilter(QObject *obj, QEvent *ev);
};

#endif // AWEBWIDGET_H
