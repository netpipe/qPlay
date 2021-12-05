#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    player = new QMediaPlayer(this);
    videoWidget = new QVideoWidget(this);
    player->setVideoOutput(videoWidget);
    this->setCentralWidget(videoWidget);

    QLabel* volume = new QLabel(this);
    volume->setText("volume");

    slider = new QSlider(this);
    probar = new QProgressBar(this);
    volumeslider = new QSlider(Qt::Horizontal,this);
    volumeslider->setRange(0, 100);
    volumeslider->setFixedWidth(100);
    volumeslider->setValue(100);
    slider->setFixedWidth(700);
    slider->setOrientation(Qt::Horizontal);
    ui->statusBar->addPermanentWidget(slider);
    ui->statusBar->addPermanentWidget(probar);
   ui->statusBar->addPermanentWidget(volume);
   ui->statusBar->addPermanentWidget(volumeslider);

   connect(player,&QMediaPlayer::durationChanged,slider,&QSlider::setMaximum);
   connect(player,&QMediaPlayer::positionChanged,slider,&QSlider::setValue);
   connect(slider,&QSlider::sliderMoved,player,&QMediaPlayer::setPosition);

   connect(player,&QMediaPlayer::durationChanged,probar,&QProgressBar::setMaximum);
   connect(player,&QMediaPlayer::positionChanged,probar,&QProgressBar::setValue);

   connect(volumeslider, SIGNAL(valueChanged(int)), this, SIGNAL(volumeChanged(int)));
   connect(volumeslider, SIGNAL(valueChanged(int)), player, SLOT(setVolume(int)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
   QString filename = QFileDialog::getOpenFileName(this,"open a file","","Audio Files(*.*),Video Files(*.*)");

   on_actionStop_triggered();

   player->setMedia(QUrl::fromLocalFile(filename));

   on_actionPlay_triggered();
}

void MainWindow::on_actionPlay_triggered()
{
    player->play();
    ui->statusBar->showMessage("Playing....");

}

void MainWindow::on_actionPause_triggered()
{
    player->pause();
    ui->statusBar->showMessage("Paused");

}

void MainWindow::on_actionStop_triggered()
{
    player->stop();
    ui->statusBar->showMessage("stopped");

}

void MainWindow::on_actionFullscreen_triggered()
{
this->setWindowState(Qt::WindowFullScreen);
}

void MainWindow::on_actionQuit_triggered()
{
        MainWindow::close();
}

void MainWindow::on_actionExitFullscreen_triggered()
{
this->setWindowState(Qt::WindowNoState);
}
