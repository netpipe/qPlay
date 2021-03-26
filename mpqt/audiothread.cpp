//#include "audiothread.h"

//bool endOfMusic;

//void syncFunc(HSYNC handle, DWORD channel, DWORD data, void *user)
//{
//    BASS_ChannelRemoveSync(channel, handle);
//    BASS_ChannelStop(channel);
//    qDebug() << "End of playback!";
//    endOfMusic = true;
//}

//AudioThread::AudioThread(QObject *parent) :
//    QThread(parent)
//{
//    if (!BASS_Init(-1, 44100, 0, NULL, NULL))
//        qDebug() << "Cannot initialize device";
//    t = new QTimer(this);
//    connect(t, SIGNAL(timeout()), this, SLOT(signalUpdate()));
//    endOfMusic = true;
//}

//void AudioThread::play(QString filename)
//{
//    BASS_ChannelStop(chan);
//    if (!(chan = BASS_StreamCreateFile(false, (filename.toLocal8Bit()).data(), 0, 0, 0)))
//            qDebug() << "Can't play file:"+filename;
//    else
//    {
//        qDebug() << "Play file:"+filename;
//        endOfMusic = false;
//        BASS_ChannelPlay(chan, true);
//        emit startOfPlayback(BASS_ChannelBytes2Seconds(chan, BASS_ChannelGetLength(chan, BASS_POS_BYTE)));
//        //playing = true;
//        t->start(100);
//        BASS_ChannelSetSync(chan, BASS_SYNC_END, 0, &syncFunc, 0);

//    }
//}

//void AudioThread::pause()
//{
//    BASS_ChannelPause(chan);
//    t->stop();
//    //playing = false;
//    emit pauseOfPlayback();
//}

//void AudioThread::resume()
//{
//    if (!BASS_ChannelPlay(chan, false))
//        qDebug() << "Error resuming";
//    else
//    {
//        t->start(100);
//        emit startOfPlayback(BASS_ChannelBytes2Seconds(chan, BASS_ChannelGetLength(chan, BASS_POS_BYTE)));
//        //playing = true;
//    }
//}

//void AudioThread::playOrPause(QString filename) {
//    if (BASS_ChannelIsActive(chan)==1) {
//        //Playing
//        pause();
//    }
//    else {
//        if (BASS_ChannelIsActive(chan)==3) {//paused
//            resume();
//        }
//        else {
//            play(filename);
//        }
//    }
//}

//void AudioThread::stop()
//{
//    BASS_ChannelStop(chan);
//    t->stop();
//    //playing = false;
//}

//void AudioThread::signalUpdate()
//{
//    if (endOfMusic == false)
//    {
//        //playing = true;
//        emit curPos(BASS_ChannelBytes2Seconds(chan, BASS_ChannelGetPosition(chan, BASS_POS_BYTE)),
//                    BASS_ChannelBytes2Seconds(chan, BASS_ChannelGetLength(chan, BASS_POS_BYTE)));
//    }
//    else
//    {
//        emit endOfPlayback();
//        //playing = false;
//    }
//}

//void AudioThread::changePosition(int position)
//{
//    BASS_ChannelSetPosition(chan, BASS_ChannelSeconds2Bytes(chan, position), BASS_POS_BYTE);
//}

//QString AudioThread::getDuration(QString path) {
//    BASS_StreamFree(chan);
//    chan = BASS_StreamCreateFile(false,path.toLocal8Bit(), 0, 0, 0);
//    return formattedTime(BASS_ChannelBytes2Seconds(chan, BASS_ChannelGetLength(chan, BASS_POS_BYTE)));
//}

//QString AudioThread::formattedTime(double t) {
//    int position = (int) (0.5+t);
//    int min = (int) position / 60 % 60;
//    int sec = (int) position % 60;
//    QString s;
//    s.sprintf("%02d:%02d",min,sec);
//    return s;
//}

//void AudioThread::run()
//{
//    while (1);
//}
