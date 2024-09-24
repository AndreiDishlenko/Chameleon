#ifndef ASERVICE_H
#define ASERVICE_H

#include <QObject>
#include <QProgressBar>

class AService : public QObject
{
    Q_OBJECT
public:
    explicit AService(QObject *parent = 0);
    ~AService();
    static bool syncDir(QString SDir_Server, QString SDir_Host, QProgressBar *progressbar);

signals:
    
public slots:
    
};

#endif // ASERVICE_H
