#ifndef AIMAGEAREA_H
#define AIMAGEAREA_H

#include <QObject>
#include <QScrollArea>

class AImageArea : public QObject {
    Q_OBJECT
public:
    explicit AImageArea(QObject *parent = 0);
    
signals:
    
public slots:
    bool createImageScroll(QScrollArea *sa, QString imgStorePath, bool adminMode);
    static bool scrollToWV(QScrollArea *sa, QString id);
    static bool scrollFor(QScrollArea *sa, int horizontal, int vertical);
    void preview();
};

#endif // AIMAGEAREA_H
