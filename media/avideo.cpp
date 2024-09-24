#include "avideo.h"
#include "afunc.h"
#include <QDebug>
#include <QDir>

AVideo::AVideo(QObject *parent, QString playerPath) : QObject(parent) {
    isPlaying = false;

    renderTarget = new QWidget();
    renderTarget->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    renderTarget->setAttribute(Qt::WA_PaintOnScreen);
    renderTarget->setMinimumSize(176, 144);

    timeLine = new QSlider(Qt::Horizontal);
    timeLine->setStyleSheet("background-color:black;");

    layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(renderTarget);
    layout->addWidget(timeLine);

    mplayerProcess = new QProcess();
    poller = new QTimer();

    connect(mplayerProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(catchOutput()));
    connect(mplayerProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(mplayerEnded(int, QProcess::ExitStatus)));
    connect(poller, SIGNAL(timeout()), this, SLOT(pollCurrentTime()));
    connect(timeLine, SIGNAL(sliderMoved(int)), this, SLOT(timeLineChanged(int)));
}

AVideo::~AVideo() {
    stop();
}

bool AVideo::initWidget(QWidget *wgt){
    if (wgt) {
//        if (parent_wgt) {parent_wgt->setLayout(new QLayout());}
//        qDebug() << "AVideo::initWidget" << wgt;
        parent_wgt = wgt;
        this->setParent(parent_wgt);

        renderTarget->setParent(parent_wgt);
        parent_wgt->setLayout(layout);

        mplayerProcess->setParent(parent_wgt);
        poller->setParent(parent_wgt);
    }
    return true;
}

bool AVideo::setMPlayerPath(QString filename) {
    if (QFile::exists(filename)) {
        mplayerPath = filename;
    } else {
        mplayerPath = "";
    }
return true;
}

bool AVideo::load(QString filename) {
//    qDebug() << "AVideo::load" << filename;
    if (QFile::exists(filename)) {
        movieFile = filename;
    } else {
        movieFile = "";
    }
    return true;
}

bool AVideo::start() {
    if (!parent_wgt) {qDebug() << "Warning (AVideo::start())! Please run setMPlayerPath(filename) first"; return false;}
    if (isPlaying) {qDebug() << "Warning (AVideo::start())! Please run stop() first"; return false;}
    if (mplayerPath=="") {qDebug() << "Warning (AVideo::start())! Please run setMPlayerPath(filename) first"; return false;}
    if (movieFile=="") {qDebug() << "Warning (AVideo::start())! Please run load(filename) first"; return false;}

    QStringList args;

    // On demande а utiliser mplayer comme backend
    args << "-slave";
    // Et on veut ne pas avoir trop de chose а parser :)
    args << "-quiet";
#ifdef Q_WS_WIN
    // reinterpret_cast<qlonglong> obligatoire, winId() ne se laissant pas convertir gentiment ;)
    args << "-wid" << QString::number(reinterpret_cast<qlonglong>(renderTarget->winId()));
    args << "-vo" << "directx:noaccel";
#else
    // Sur linux, aucun driver n'a йtй nйcessaire et pas de manip pour Wid :)
    args << "-wid" << QString::number(renderTarget->winId());

    log.append("Video output driver may not be necessary for your platform. \
                Check: http://www.mplayerhq.hu/DOCS/man/en/mplayer.1.html \
                at the VIDEO OUTPUT DRIVERS section.");
#endif

    args << movieFile;

    // On parse la stdout et stderr au mкme endroit, donc on demande а "fusionnner" les 2 flux
    mplayerProcess->setProcessChannelMode(QProcess::MergedChannels);
    mplayerProcess->start(mplayerPath, args);
    if(!mplayerProcess->waitForStarted(3000))
    {
        qDebug("allez, cherche le bug :o");
        return false;
    }

    // On rйcupиre les infos de base
    mplayerProcess->write("get_video_resolution\n");
    mplayerProcess->write("get_time_length\n");

    poller->start(1000);

    isPlaying = true;

    return true;
}
bool AVideo::stop() {
    qDebug() << "stop";
    if(!isPlaying)
        return true;
    mplayerProcess->write("quit\n");
    if(!mplayerProcess->waitForFinished(3000)) {
        qDebug("Warning! AVideo:stop cant stop process.");
        return false;
    }

    return true;
}

void AVideo::catchOutput() {
    while(mplayerProcess->canReadLine())
    {
        QByteArray buffer(mplayerProcess->readLine());
        log.append(QString(buffer));
        // On vйrifie si on a eu des rйponses
        // rйponse а get_video_resolution : ANS_VIDEO_RESOLUTION='<width> x <height>'
        if(buffer.startsWith("ANS_VIDEO_RESOLUTION"))
        {
            buffer.remove(0, 21); // vire ANS_VIDEO_RESOLUTION=
            buffer.replace(QByteArray("'"), QByteArray(""));
            buffer.replace(QByteArray(" "), QByteArray(""));
            buffer.replace(QByteArray("\n"), QByteArray(""));
            buffer.replace(QByteArray("\r"), QByteArray(""));
            int sepIndex = buffer.indexOf('x');
            int resX = buffer.left(sepIndex).toInt();
            int resY = buffer.mid(sepIndex+1).toInt();
            renderTarget->setMinimumSize(resX, resY);
        }
        // rйponse а get_time_length : ANS_LENGTH=xx.yy
        else if(buffer.startsWith("ANS_LENGTH"))
        {
            buffer.remove(0, 11); // vire ANS_LENGTH=
            buffer.replace(QByteArray("'"), QByteArray(""));
            buffer.replace(QByteArray(" "), QByteArray(""));
            buffer.replace(QByteArray("\n"), QByteArray(""));
            buffer.replace(QByteArray("\r"), QByteArray(""));
            float maxTime = buffer.toFloat();
            timeLine->setMaximum(static_cast<int>(maxTime+1));
        }
        // rйponse а get_time_pos : ANS_TIME_POSITION=xx.y
        else if(buffer.startsWith("ANS_TIME_POSITION"))
        {
            buffer.remove(0, 18); // vire ANS_TIME_POSITION=
            buffer.replace(QByteArray("'"), QByteArray(""));
            buffer.replace(QByteArray(" "), QByteArray(""));
            buffer.replace(QByteArray("\n"), QByteArray(""));
            buffer.replace(QByteArray("\r"), QByteArray(""));
            float currTime = buffer.toFloat();
            timeLine->setValue(static_cast<int>(currTime+1));
        }
    }
}
void AVideo::pollCurrentTime() {
    mplayerProcess->write("get_time_pos\n");
}
void AVideo::timeLineChanged(int pos) {
    mplayerProcess->write(QString("seek " + QString::number(pos) + " 2\n").toUtf8());
}
// Play/stop
void AVideo::switchPlayState() {
    if(!isPlaying)
    {
        if(!start())
            return;
//        log->clear();
        isPlaying = true;
    }
    else
    {
        if(!stop())
            return;
        poller->stop();
//        log->clear();
        isPlaying = false;
    }
}
void AVideo::mplayerEnded(int exitCode, QProcess::ExitStatus exitStatus) {
    isPlaying = false;
    poller->stop();
}
