#include "audiothread.h"
#include <QWidget>
#include<QFileInfo>
#include<QFileDialog>
#include<QStandardPaths>
#include<QDateTime>

bool endOfMusic;

void syncFunc(/*HSYNC handle, DWORD channel, DWORD data, void *user*/)
{
   /* BASS_ChannelRemoveSync(channel, handle);
    BASS_ChannelStop(channel);
    qDebug() << "End of playback!";
    endOfMusic = true;*/
}

AudioThread::AudioThread(QObject *parent) :
    QThread(parent)
{
    /*if (!BASS_Init(-1, 44100, 0, NULL, NULL))
        qDebug() << "Cannot initialize device";*/
    m_player = new QMediaPlayer(this);
    //connect(m_player, &QMediaPlayer::durationChanged, this, SLOT(durationChanged));
    m_player->setAudioRole(QAudio::VideoRole);
    qInfo() << "Supported audio roles:";
    for (QAudio::Role role : m_player->supportedAudioRoles())
        qInfo() << "    " << role;
    // owned by PlaylistModel
    m_playlist = new QMediaPlaylist();
    m_player->setPlaylist(m_playlist);

    t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(signalUpdate()));
    connect(m_player, &QMediaPlayer::durationChanged, this, &AudioThread::durationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &AudioThread::positionChanged);
    //connect(m_player, &QMediaPlayer::positionChanged, this, &AudioThread::seek);
    endOfMusic = true;
}
void AudioThread::durationChanged(qint64 duration)
{
    m_duration = duration / 1000;   
}
void AudioThread::positionChanged(qint64 progress)
{
    m_progress = progress/1000;
    updateDurationInfo(progress / 1000);   
}

void AudioThread::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || m_duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
            currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60,
            m_duration % 60, (m_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }

    m_labelmediaduration = tStr;  
    //qDebug() << "Media duration for label:" << m_labelmediaduration;
    emit sliderTime(m_labelmediaduration,m_duration,m_progress);
}
/*void AudioThread::seek(int seconds)
{
    m_player->setPosition(seconds * 1000);
}*/
bool AudioThread::isPlayerAvailable() const
{
    return m_player->isAvailable();
}

void AudioThread::play(QString filename)
{
    /*BASS_ChannelStop(chan);
    if (!(chan = BASS_StreamCreateFile(false, (filename.toLocal8Bit()).data(), 0, 0, 0)))
            qDebug() << "Can't play file:"+filename;
    else
    {
        qDebug() << "Play file:"+filename;
        endOfMusic = false;
        BASS_ChannelPlay(chan, true);
        emit startOfPlayback(BASS_ChannelBytes2Seconds(chan, BASS_ChannelGetLength(chan, BASS_POS_BYTE)));
        //playing = true;
        t->start(100);
        BASS_ChannelSetSync(chan, BASS_SYNC_END, 0, &syncFunc, 0);

    }*/
    //QFileDialog fileDialog;
    //fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    //fileDialog.setWindowTitle(tr("Open Files"));
    QStringList supportedMimeTypes = m_player->supportedMimeTypes();
    if (!supportedMimeTypes.isEmpty()) {
        supportedMimeTypes.append("audio/x-m3u"); // MP3 playlists
        //fileDialog.setMimeTypeFilters(supportedMimeTypes);
    }
   // fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()));
    //if (fileDialog.exec() == QDialog::Accepted)
      //  addToPlaylist(fileDialog.selectedUrls());
    QUrl url(filename);
    QList<QUrl> urls;
    qDebug() << url; 
    //if(urls.size()>0)
       // urls.clear();
    urls.append(url);
    addToPlaylist(urls);
    m_playlist->setCurrentIndex(0);
    m_player->setPosition(0);
    m_player->setMedia(QUrl::fromLocalFile(filename));
   // m_player->setMedia(url);
    qDebug() << "Play file:"+filename;
    endOfMusic = false;
    m_player->play();
    emit startOfPlayback();
    playing = true;
    t->start(100);
  }
static bool isPlaylist(const QUrl &url) // Check for ".m3u" playlists.
{
    if (!url.isLocalFile())
        return false;
    const QFileInfo fileInfo(url.toLocalFile());
    return fileInfo.exists() && !fileInfo.suffix().compare(QLatin1String("m3u"), Qt::CaseInsensitive);
}

void AudioThread::addToPlaylist(const QList<QUrl> &urls)
{
    for (auto &url: urls) {
        if (isPlaylist(url))
            m_playlist->load(url);
        else
            m_playlist->addMedia(url);
    }
}

void AudioThread::pause()
{
   /* BASS_ChannelPause(chan);*/
      m_player->pause();
    t->stop();
    playing = false;  
    emit pauseOfPlayback();
}

void AudioThread::resume()
{
  /*  if (!BASS_ChannelPlay(chan, false))
        qDebug() << "Error resuming";
    else
    {
        t->start(100);
        emit startOfPlayback(BASS_ChannelBytes2Seconds(chan, BASS_ChannelGetLength(chan, BASS_POS_BYTE)));
        //playing = true;
    }*/
     t->start(100);
     m_player->play();
     emit startOfPlayback();
     playing=true;
}

void AudioThread::playOrPause(QString filename) {
 /*if (BASS_ChannelIsActive(chan)==1) {
     pause();
  }
    else {
            //Playing
    if (BASS_ChannelIsActive(chan)==3) {//paused
        resume();
        }
        else {
          play(filename);
     }
  }*/ //Old code using Bass Library

    /* // Code added to maintain the state of media
    switch (m_playerState) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
         play(filename);
        break;
    case QMediaPlayer::PlayingState:
        emit pause();
        break;
    }*/
    if(QMediaPlayer::PlayingState && playing)
    {
        pause();
    }
    else if(QMediaPlayer::PausedState && !playing)
    {
        resume();
        endOfMusic = false;
        play(filename);
    }
}

void AudioThread::stop()
{
   // BASS_ChannelStop(chan);
    m_player->pause();
    t->stop();    
    playing = false;
}

void AudioThread::signalUpdate()
{
    if (endOfMusic == false)
    {
        playing = true;
        //emit curPos(BASS_ChannelBytes2Seconds(chan, BASS_ChannelGetPosition(chan, BASS_POS_BYTE)),
                 //   BASS_ChannelBytes2Seconds(chan, BASS_ChannelGetLength(chan, BASS_POS_BYTE)));
        emit curPos(m_player->position(), m_player->duration());      
    }
    else
    {
        emit endOfPlayback();
        playing = false;
    }
}

//void AudioThread::changePosition(int position)
void AudioThread::changePosition(qint64 position)
{
   // BASS_ChannelSetPosition(chan, BASS_ChannelSeconds2Bytes(chan, position), BASS_POS_BYTE);
}
QString AudioThread::getDuration(QString path) {
    //BASS_StreamFree(chan);
    //chan = BASS_StreamCreateFile(false,path.toLocal8Bit(), 0, 0, 0);
    //return formattedTime(BASS_ChannelBytes2Seconds(chan, BASS_ChannelGetLength(chan, BASS_POS_BYTE)));
   if (path.isNull()) { return ""; }
   QUrl url(path);
   QList<QUrl> urls;
   qDebug() << url;  
   QDateTime lm = QFileInfo(QFile(path)).lastModified();
   qint64 epoch = lm.toMSecsSinceEpoch();
   QString str = QString::number(epoch);
  // qDebug() <<"The duration of file" << str;
   return formattedTime(str);
   // return formattedTime(m_labelmediaduration);     

}
//QString AudioThread::formattedTime(double t1)
QString AudioThread::formattedTime(QString t1)
{
    //int position = (int) (0.5+t1);
    //int min = (int) position / 60 % 60;
    //int sec = (int) position % 60;
   // QString s=t1;
    //s.sprintf("%02d:%02d",min,sec);
   // qDebug()<<"Song duration formatted: "<<s;
    return NULL;
}

void AudioThread::run()
{
    while (1);
}
