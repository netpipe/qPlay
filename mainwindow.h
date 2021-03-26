#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMediaPlayer>

#include <QMouseEvent>
#include <QStringListModel>
#include <QDropEvent>
#include <QMimeData>

//#include "audiothread.h"
#include "mpqt/scanner.h"
#include "mpqt/track.h"
#include "mpqt/trackmodel.h"
#include "mpqt/trackdelegate.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QMediaPlayer*player;

    TrackModel *trackModel;
  //  AudioThread *audio;
    Scanner *scanner;

    QList<Track *> tracklist;
    QPoint dragPos;
    bool moving,shuffle;
    short repeatMode;
    long position;
    QAction *clearAction,*scanAction;

protected:
    void addItem(QString s);
    void dropEvent(QDropEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void setRepeatModeIcon();
    void nextTrack(bool next);
    bool hasNextTrack();

public slots:
    void onStartOfPlayback(double total);
    void onEndOfPlayback();
    void onPauseOfPlayback();
    void onCurPos(double position, double total);
    void onFileAdded(QString file);
    void onClearList();
    void onFindMusic();
private slots:
    void on_closeButton_clicked();
    void on_horizontalSlider_sliderPressed();
    void on_horizontalSlider_sliderReleased();
    void on_pushButton_play_clicked();
    void on_pushButton_repeat_clicked();
    void on_pushButton_shuffle_clicked();
    void on_pushButton_prev_clicked();
    void on_pushButton_next_clicked();

    void on_listView_clicked(const QModelIndex &index);
    void showContextMenuForWidget(const QPoint &pos);
    void stopScanner();


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
