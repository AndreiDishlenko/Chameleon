// Видео-плеер

#ifndef AVIDEO_H
#define AVIDEO_H

#include <QWidget>
#include <QApplication>
#include <QProcess>
#include <QVBoxLayout>
#include <QLayoutItem>
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QColor>
#include <QRect>
#include <QLinearGradient>
#include <QSizePolicy>
#include <QPushButton>
#include <QTextEdit>
#include <QSlider>
#include <QCloseEvent>
#include <QTimer>

class AVideo : public QObject {
Q_OBJECT

public:
    AVideo(QObject *parent, QString playerPath="");
    ~AVideo();   


    QString movieFile;
    QString mplayerPath;
    QString log;

protected:
//    bool eventFilter(QObject *obj, QEvent *event);
//    virtual void closeEvent(QCloseEvent *e);

private:

public slots:
    bool setMPlayerPath(QString filename);
    bool initWidget(QWidget *wgt);
    bool load(QString filename);
    bool start();
    bool stop();
    void switchPlayState();

private slots:
    void catchOutput();
    void pollCurrentTime();
    void timeLineChanged(int pos);
    // Play/stop
    void mplayerEnded(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QWidget *renderTarget;
    QProcess *mplayerProcess;
    bool isPlaying;
    QSlider *timeLine;
    QVBoxLayout *layout;
    QWidget *parent_wgt;
    QTimer *poller;

};

#endif // AVIDEO_H
