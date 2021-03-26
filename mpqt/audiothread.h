//#ifndef AUDIOTHREAD
//#define AUDIOTHREAD
//#include <QThread>
//#include <QDebug>
//#include <QTimer>

//void syncFunc(HSYNC handle, DWORD channel, DWORD data, void *user);

//class AudioThread : public QThread
//{
//    Q_OBJECT
//public:
//    explicit AudioThread(QObject *parent = 0);
//    //bool playing;
//    void run();
//private:
//    unsigned long chan;
//    QTimer *t;
//signals:
//    void startOfPlayback(double total);
//    void endOfPlayback();
//    void pauseOfPlayback();
//    void curPos(double position, double total);
//public slots:
//    void play(QString filepath);
//    void playOrPause(QString filepath);
//    void pause();
//    void resume();
//    void stop();
//    void signalUpdate();
//    void changePosition(int position);
//    QString getDuration(QString path);
//    QString formattedTime(double t);
//};
//#endif // AUDIOTHREAD

