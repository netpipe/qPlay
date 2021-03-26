#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include <player.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton_play->setIcon(QIcon("./Resource/img/btn_play.png"));
 //   ui->pushButton_play->setIcon(QIcon(":/Resource/img/btn_play.png"));
 //   ui->label_3->setPixmap(QPixmap(":/Resource/img/freemp.png"));
 //   ui->pushButton_shuffle->setIcon(QIcon(":/Resource/img/btn_shuffle_off.png"));
  //  ui->pushButton_prev->setIcon(QIcon(":/Resource/img/btn_previous.png"));
 //   ui->pushButton_next->setIcon(QIcon(":/Resource/img/btn_next.png"));
 //   ui->pushButton_repeat->setIcon(QIcon(":/Resource/img/btn_repeat.png"));
    //repeatMode = 0;
    repeatMode = 0;
    moving = false;
    setRepeatModeIcon();
    shuffle = false;
    position = 0;
    ui->listView->setEditTriggers(0);
    ui->listView->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listView,SIGNAL(customContextMenuRequested(const QPoint &)),
            SLOT(showContextMenuForWidget(const QPoint &)));

    //ui->horizontalSlider_2->setMaximum(0);
    //ui->horizontalSlider_2->setSliderDown(true);
    trackModel = new TrackModel;
    trackModel->list = tracklist;

    ui->listView->setModel(trackModel);
    ui->listView->setItemDelegate(new TrackDelegate);
    setAcceptDrops(true);
    scanner = NULL;

    //audiothread
    //Added By Rajiv
    audio = new AudioThread(this);
    //connect(audio, SIGNAL(startOfPlayback(double )), this, SLOT(onStartOfPlayback(double)));
    connect(audio, SIGNAL(startOfPlayback()), this, SLOT(onStartOfPlayback()));
    connect(audio, SIGNAL(endOfPlayback()), this, SLOT(onEndOfPlayback()));
    connect(audio, SIGNAL(pauseOfPlayback()), this, SLOT(onPauseOfPlayback()));
    connect(audio, SIGNAL(curPos(double, double)), this, SLOT(onCurPos(double, double)));
    connect(audio, SIGNAL(sliderTime(QString,qint64,qint64)), this, SLOT(onSlidertime(QString,qint64,qint64)));
    //connect(ui->horizontalSlider_2, &QSlider::sliderMoved, this,SLOT(seek(int)));
    connect(ui->horizontalSlider_2, &QSlider::sliderMoved, this,&MainWindow::seek);
    //connect(ui->horizontalSlider_2, SIGNAL(sliderPressed), this, SLOT(on_horizontalSlider_sliderPressed()));

    clearAction = new QAction(tr("Clear"), this);
    connect(clearAction, SIGNAL(triggered()), this, SLOT(onClearList()));
    scanAction = new QAction(tr("Scan home dir: "+(QDir::home()).absolutePath().toUtf8()), this);
    connect(scanAction, SIGNAL(triggered()), this, SLOT(onFindMusic()));

    QMenuBar *m_menuBar = new QMenuBar(this);
    QMenu *playlistMenu = new QMenu(QString::fromUtf8("Playlist"));
    playlistMenu->addAction(clearAction);
    playlistMenu->addAction(scanAction);
    m_menuBar->addAction(playlistMenu->menuAction());

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showContextMenuForWidget(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);
    contextMenu.addAction(clearAction);
    contextMenu.addAction(scanAction);
    contextMenu.exec(ui->listView->mapToGlobal(pos));
}

void MainWindow::dropEvent(QDropEvent *ev)
{
   QList<QUrl> urls = ev->mimeData()->urls();
   stopScanner();

   foreach(QUrl url, urls)
   {
       qDebug()<<"path:"<< url.path();
       QFileInfo fi(url.path());
       if (fi.isDir()) {
           scanner = new Scanner(fi.dir(),this);
           connect(scanner,SIGNAL(fileAdded(QString)),this,SLOT(onFileAdded(QString)));
           scanner->start();
       }
       if (fi.isFile()) {
           addItem(url.path());
       }
   }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *ev)
{
    ev->accept();
}

void MainWindow::onClearList() {
    stopScanner();
    tracklist.clear();
    trackModel->deleteAllTracks();
    audio->stop();
    ui->horizontalSlider_2->setMaximum(0);
}

void MainWindow::stopScanner() {
    if (scanner!=NULL && scanner->isRunning()) {
        scanner->stopped = true;
        if(!scanner->wait(2000)) {
            scanner->terminate();
            scanner->wait(); //Note: We have to wait again here!
        }
        scanner->deleteLater();
        scanner = NULL;
    }
}

void MainWindow::onFindMusic() {
    stopScanner();
    QDir dir =QDir::home();
    scanner = new Scanner(dir,this);
    connect(scanner,SIGNAL(fileAdded(QString)),this,SLOT(onFileAdded(QString)));
    scanner->start();
}

void MainWindow::addItem(QString s) {

    QFileInfo fi(s);
    Track *track = new Track;
    track->path = s;
    QString dirPath = fi.absoluteDir().absolutePath();
    track->artist = dirPath.split("/").last();
    track->title = fi.completeBaseName();
    track->durationStr = audio->getDuration(s);
    tracklist.append(track);
    trackModel->appendTrack(track);
}

void MainWindow::on_closeButton_clicked()
{
    MainWindow::close();
}

/*void MainWindow::onStartOfPlayback(double total)*/
void MainWindow::onStartOfPlayback()
{
   // ui->horizontalSlider_2->setMaximum(total);
    ui->pushButton_play->setIcon(QIcon(":/Resource/img/btn_pause.png"));
    QModelIndex index = trackModel->index(position, 0);

    if ( index.isValid() ) {
        ui->listView->selectionModel()->clear();
        ui->listView->selectionModel()->select( index, QItemSelectionModel::Select );
        ui->listView->scrollTo(index);
        ui->label_artist->setText(tracklist.at(position)->artist);
        ui->label_title->setText(tracklist.at(position)->title);
    }
}

void MainWindow::onEndOfPlayback() {
    ui->pushButton_play->setIcon(QIcon(":/Resource/img/btn_play.png"));
    nextTrack(true);
}
void MainWindow::seek(int seconds)
{
     //m_player->setPosition(seconds * 1000);
    // qDebug() << "Position" << seconds;
}

void MainWindow::nextTrack(bool next) {
    short add = 1;
    if (!next) add = -1;
    switch (repeatMode) {
    case 1:
        //looped track
        break;
    case 2:
        //looped playlist
        position=position+add;
        if (position>=tracklist.size()) {
            position = 0;
        }
        if (position<0) {
            position = tracklist.size()-1;
        }
        break;
    default:
        position=position+add;
        break;
    }
    if (hasNextTrack()) {
        audio->play(tracklist.at(position)->path);
    }
    else {
        audio->stop();
    }
}

void MainWindow::onPauseOfPlayback() {
    ui->pushButton_play->setIcon(QIcon(":/Resource/img/btn_play.png"));
}

void MainWindow::onCurPos(double position, double total) {
   //qDebug() << "Position" << position << "Total" << total;
  if (!moving) {
        //ui->horizontalSlider_2->setValue(position*1000);
       // ui->horizontalSlider_2->setRange(0, position / 1000);
        // ui->label_duration_2->setText(audio->formattedTime(position));
    }
}
void MainWindow::onSlidertime(QString sliderduration, qint64 duration, qint64 progress)
{
  if (!moving)
  {
     //qDebug() << "Slider position" << sliderduration;
     ui->horizontalSlider_2->setMaximum(duration);
     ui->horizontalSlider_2->setValue(progress);
     ui->label_bgr->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
     ui->horizontalLayout_4->setContentsMargins(0,0,0,0);
     ui->label_duration_2->setAlignment(Qt::AlignTop | Qt::AlignLeft);
     ui->label_duration_2->setContentsMargins(0,0,0,0);
     ui->label_duration_2->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
     ui->label_duration_2->setWordWrap(true);
     ui->label_duration_2->setText(sliderduration);
     m_trackDuration=sliderduration;
  }

}

void MainWindow::on_horizontalSlider_sliderPressed()
{
    moving = true;
}

void MainWindow::on_horizontalSlider_sliderReleased()
{
    moving = false;
    audio->changePosition(ui->horizontalSlider_2->value());
}

void MainWindow::on_pushButton_play_clicked()
{
    if (hasNextTrack()) {
        audio->playOrPause(tracklist.at(position)->path);
    }
}

bool MainWindow::hasNextTrack() {
    if (tracklist.size()<=0) return false;
    if (position<0) return false;
    if (position>(tracklist.size()-1)) return false;
    return true;
}

void MainWindow::setRepeatModeIcon() {
    if (repeatMode>2) {
        repeatMode = 0;
    }
    switch (repeatMode) {
    case 1:
        ui->pushButton_repeat->setIcon(QIcon(":/Resource/img/btn_repeat_one.png"));
        break;
    case 2:
        ui->pushButton_repeat->setIcon(QIcon(":/Resource/img/btn_repeat_all_highlighted.png"));
        break;
    default:
        ui->pushButton_repeat->setIcon(QIcon(":/Resource/img/btn_repeat_off.png"));
        break;
    }
}

void MainWindow::on_pushButton_repeat_clicked()
{
    repeatMode++;
    setRepeatModeIcon();
}

void MainWindow::on_pushButton_shuffle_clicked()
{
    shuffle = !shuffle;
    if (shuffle) {
        ui->pushButton_shuffle->setIcon(QIcon(":/Resource/img/btn_shuffle_highlighted.png"));
    }
    else {
        ui->pushButton_shuffle->setIcon(QIcon(":/Resource/img/btn_shuffle_off.png"));
    }
}

void MainWindow::on_pushButton_prev_clicked()
{
    nextTrack(false);
}

void MainWindow::on_pushButton_next_clicked()
{
    nextTrack(true);
}

void MainWindow::onFileAdded(QString file) {
    addItem(file);
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    position = index.row();
    if (hasNextTrack()) {
        audio->play(tracklist.at(position)->path);
    }
}

