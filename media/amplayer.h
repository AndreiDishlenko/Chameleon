// Видео-плеер

#ifndef AMPLAYER_H
#define AMPLAYER_H

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

#ifdef Q_OS_WIN32
const QString mplayerPath("C:/Program Files (x86)/SMPlayer/mplayer/mplayer.exe");
#else
const QString mplayerPath("C:/Program Files (x86)/SMPlayer/mplayer/mplayer.exe");
#endif
const QString movieFile("d:/nightwish.flv");

class AMPlayer : public QWidget {
Q_OBJECT

public:
    AMPlayer(QWidget *parent =0);

protected:
    virtual void closeEvent(QCloseEvent *e);

private:
    bool startMPlayer();
    bool stopMPlayer();

private slots:
    void catchOutput();
    void pollCurrentTime();
    void timeLineChanged(int pos);
    // Play/stop
    void switchPlayState();
    void mplayerEnded(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QPushButton *controller;
    QWidget *renderTarget;
    QProcess *mplayerProcess;
    bool isPlaying;
    QSlider *timeLine;
    QTimer *poller;
    QTextEdit *log;
    
};

#endif // AMPLAYER_H
