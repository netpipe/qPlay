#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QFileDialog>
#include <QVideoWidget>
#include <QProgressBar>
#include <QSlider>
#include <QLabel>
#include <QtWidgets>
#include <QFormLayout>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPushButton>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionPlay_triggered();

    void on_actionPause_triggered();

    void on_actionStop_triggered();

    void on_actionFullscreen_triggered();

    void on_actionQuit_triggered();

    void on_actionExitFullscreen_triggered();

private:
    Ui::MainWindow *ui;
    QMediaPlayer* player;
    QVideoWidget* videoWidget;
    QSlider* slider;
    QSlider* volumeslider;
    QProgressBar* probar;
   QLabel* volume;

};

#endif // MAINWINDOW_H
