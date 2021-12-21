#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMediaPlayer>
#include <QVideoWidget>

#include <QDropEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QStringListModel>

#include "mpqt/audiothread.h"
#include "mpqt/playlistManager.h"
#include "mpqt/scanner.h"
#include "mpqt/track.h"
#include "mpqt/trackdelegate.h"
#include "mpqt/trackmodel.h"

#include <downloadmanager.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

      QString fileName;
  QMediaPlayer *player;
  QVideoWidget *vidplayer;
  void playsound(QString test);
  void parseSearch();

  TrackModel *trackModel;
  AudioThread *audio;
  Scanner *scanner;
  PlaylistManager *m_playlistDB = nullptr;

  QList<Track *> tracklist;
  QPoint dragPos;
  bool moving, shuffle;
  short repeatMode;
  long position;
  QAction *clearAction, *scanAction;

protected:
  void addItem(QString s);
  void dropEvent(QDropEvent *e);
  void dragEnterEvent(QDragEnterEvent *e);
  void setRepeatModeIcon();
  void nextTrack(bool next);
  bool hasNextTrack();

public slots:
  // void onStartOfPlayback(double total);
  void onStartOfPlayback();
  void onEndOfPlayback();
  void onPauseOfPlayback();
  void onCurPos(double position, double total);
  void onSlidertime(QString sliderduration, qint64 duration, qint64 progress);
  void onFileAdded(QString file);
  void onClearList();
  void onFindMusic(QString);
private slots:
  void on_closeButton_clicked();
  void on_pushButton_repeat_clicked();
  void on_pushButton_shuffle_clicked();
  void on_pushButton_prev_clicked();
  void on_pushButton_next_clicked();
  void on_listView_clicked(const QModelIndex &index);
  void showContextMenuForWidget(const QPoint &pos);
  void stopScanner();

  void on_SavePlaylist_clicked();

  void on_LoadPlaylist_clicked();

  void on_actionScan_triggered();

  void on_actionClear_triggered();

  void on_Play_clicked();

  void on_horizontalSlider_2_sliderPressed();
  void on_horizontalSlider_2_sliderReleased();
  void on_horizontalSlider_2_sliderMoved(int position);

  void on_horizontalSlider_sliderMoved(int position);

  void on_pushButton_clicked();

  void on_pushButton_play_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_chooseStationbtn_clicked();

  void on_txtIds_currentRowChanged(int currentRow);

  void on_comboBox_currentIndexChanged(const QString &arg1);

  void on_refreshbtn_clicked();

  void on_txtNames_currentRowChanged(int currentRow);

  void on_cmbTheme_currentIndexChanged(const QString &arg1);
void loadStyleSheet(QString sheet_name);

private:
  Ui::MainWindow *ui;

  QString m_trackDuration;
  DownloadManager *dlmanager;
      bool loaded=0;
};
#endif // MAINWINDOW_H
