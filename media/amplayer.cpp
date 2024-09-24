#include "amplayer.h"

AMPlayer::AMPlayer(QWidget *parent) : QWidget(parent), isPlaying(false) {
    controller = new QPushButton("Play");

    renderTarget = new QWidget(this);
    renderTarget->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    renderTarget->setAttribute(Qt::WA_PaintOnScreen);
    renderTarget->setMinimumSize(176, 144);

    timeLine = new QSlider(Qt::Horizontal);

    log = new QTextEdit;
    log->setReadOnly(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(controller);
    layout->addWidget(renderTarget);
    layout->addWidget(timeLine);
    layout->addWidget(log);
    setLayout(layout);

    mplayerProcess = new QProcess(this);

    poller = new QTimer(this);

    connect(controller, SIGNAL(clicked()), this, SLOT(switchPlayState()));
    connect(mplayerProcess, SIGNAL(readyReadStandardOutput()),
        this, SLOT(catchOutput()));
    connect(mplayerProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
        this, SLOT(mplayerEnded(int, QProcess::ExitStatus)));
    connect(poller, SIGNAL(timeout()), this, SLOT(pollCurrentTime()));
    connect(timeLine, SIGNAL(sliderMoved(int)), this, SLOT(timeLineChanged(int)));
}

void AMPlayer::closeEvent(QCloseEvent *e) {
    stopMPlayer();
    e->accept();
}

bool AMPlayer::startMPlayer() {
    if(isPlaying)
        return true;

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

    log->append("Video output driver may not be necessary for your platform. \
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
bool AMPlayer::stopMPlayer() {
    if(!isPlaying)
        return true;
    mplayerProcess->write("quit\n");
    if(!mplayerProcess->waitForFinished(3000))
    {
        qDebug("ZOMG, зa plante :(");
        return false;
    }

    return true;
}

void AMPlayer::catchOutput() {
    while(mplayerProcess->canReadLine())
    {
        QByteArray buffer(mplayerProcess->readLine());
        log->append(QString(buffer));
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
void AMPlayer::pollCurrentTime() {
    mplayerProcess->write("get_time_pos\n");
}
void AMPlayer::timeLineChanged(int pos) {
    mplayerProcess->write(QString("seek " + QString::number(pos) + " 2\n").toUtf8());
}
// Play/stop
void AMPlayer::switchPlayState() {
    if(!isPlaying)
    {
        if(!startMPlayer())
            return;

        log->clear();
        controller->setText("Stop");
        isPlaying = true;
    }
    else
    {
        if(!stopMPlayer())
            return;

        poller->stop();
        log->clear();
        controller->setText("Play");
        isPlaying = false;
    }
}
void AMPlayer::mplayerEnded(int exitCode, QProcess::ExitStatus exitStatus) {
    isPlaying = false;
    controller->setText("Play");
    poller->stop();
}
