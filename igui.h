#ifndef IGUI_H
#define IGUI_H

#include <QObject>
#include <QGridLayout>
#include <QScrollArea>
#include <QMap>
#include "media/apush.h"

class iGui : public QObject
{
    Q_OBJECT
public:
    explicit iGui(QObject *parent = 0);
    ~iGui();

    static bool clearLayout(QGridLayout *layout);
signals:

public slots:
    static QVariantList readGridDataSource(QScrollArea *sa);

    static bool create_iButtonsGrid(QScrollArea *sa);
//    static bool create_iButtonsGrid2(QScrollArea *sa);
    static bool iGuiScrollIcons(QScrollArea *sa, int imgWidth, int imgHeight, QString orientation);


    static QObject * addButton(QScrollArea *sa, QString text, int row, int column, int width, int height, QString btnstyle);
    static void addSpacer(QScrollArea *sa, int row, int column, int width, int height);

    static QObjectList getScrollAPushButtons(QScrollArea *sa);

};

#endif // IGUI_H
