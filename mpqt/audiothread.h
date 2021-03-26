#ifndef AUDIOTHREAD
#define AUDIOTHREAD
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QAudio>

QT_BEGIN_NAMESPACE
class QMediaPlayer;
class QAudioProbe;
QT_END_NAMESPACE

//#include <src/mpqt/bass.h>


//void syncFunc(HSYNC handle, DWORD channel, DWORD data, void *user);
//void __stdcall syncFunc(HSYNC handle, DWORD channel, DWORD data, void *user);
class AudioThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioThread(QObject *parent = 0);
    bool playing;
    void run();
    bool isPlayerAvailable() const;
    void setCustomAudioRole(const QString &role);
    void addToPlaylist(const QList<QUrl> &urls);


private:
    unsigned long chan;
    QTimer *t;
    QMediaPlayer::State m_playerState = QMediaPlayer::StoppedState;
    QMediaPlayer *m_player = nullptr;
    QMediaPlaylist *m_playlist = nullptr;
    qint64 m_duration;
    qint64 m_progress;
    QString m_labelmediaduration;
    void updateDurationInfo(qint64 currentInfo);
private slots:
        //void durationChanged(qint64 duration);    
signals:
   // void startOfPlayback(double total);
    void startOfPlayback();
    void endOfPlayback();
    void pauseOfPlayback();
    void curPos(double position, double total);   
    void sliderTime(QString sliderduration, qint64 duration,qint64 progress);
public slots:
    void play(QString filepath);
    void playOrPause(QString filepath);
    void pause();
    void resume();
    void stop();
    void signalUpdate();
    //void changePosition(int position);
    void changePosition(qint64 position);
    QString getDuration(QString path);
   // QString formattedTime(double t);
    QString formattedTime(QString t);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
   // void seek(int seconds);
 };
#endif // AUDIOTHREAD

