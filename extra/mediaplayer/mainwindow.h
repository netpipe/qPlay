#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaContent>
#include <QVideoWidget>
#include <QFileDialog>
#include <QSlider>
#include <QMediaPlaylist>
#include <QAbstractItemView>
#include <QString>
#include <QList>
#include <QStringList>
#include <QDir>
#include <QLabel>
#include <QUrl>
#include <QBoxLayout>
#include <QDebug>
#include <QWidget>
#include <QSplitter>
#include <QListWidget>
#include <QMouseEvent>
#include <QSplitter>
#include <QTime>
#include <QKeyEvent>
#include <QShortcut>
#include <QAction>
#include <QListWidgetItem>
#include <QFormLayout>
#include <QPushButton>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    //Creating Mainwindow object
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

protected:

    //Declaring prtotected functions

    bool eventFilter(QObject *obj, QEvent *event);

private slots:

    //Declaring private slots

    void durationChanged(qint64 duration);

    void positionChanged(qint64 progress);

    void on_actionOpen_triggered();

    void on_actionPlay_triggered();

    void on_actionPause_triggered();

    void on_actionStop_triggered();

    void on_actionMute_triggered();

    void on_actionNext_triggered();

    void on_actionPrevious_triggered();

    void listwidgetdoubleclicked(QListWidgetItem*);

    void displayErrorMessage();

    void seek(int seconds);

    void onCustomContextMenu(const QPoint &);

    void on_actionOpen_2_triggered();

    void on_actionQuit_triggered();

    void on_action10_triggered();

    void on_action20_triggered();

    void on_action30_triggered();

    void on_action40_triggered();

    void on_action50_triggered();

    void on_action60_triggered();

    void on_action70_triggered();

    void on_action80_triggered();

    void on_action90_triggered();

    void on_action100_triggered();

    void on_actionColor_Options_triggered();

    void on_actionPlaylists_triggered();

    void on_actionAbout_triggered();

    void on_actionShortcut_Preview_triggered();

private:

    //Declaring private functions, pointers and variables

    void updateDurationInfo(qint64 currentInfo);
    void openPlaylist();
    void savePlaylist();
    void deletefromPlaylist();

    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QVideoWidget *vw;
    QSlider *slider,*volumeslider;
    QWidget *mw;
    QSplitter *sw;
    QMediaPlaylist *playlist;
    QSplitter *splitter;
    QLabel *durationlabel,*coverLabel,*labelDuration;
    QListWidget *listWidget;
    QShortcut *s1,*s2,*s3,*s4,*s5,*s6,*s7,*s8,*s9,*s10;
    qint64 duration,position,curdur;
    QDialog *colorDialog;
};

#endif // MAINWINDOW_H
