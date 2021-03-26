#include <mainwindow.h>
#include "ui_mainwindow.h"

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
    //audio->stop();
    player->stop();
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

    track->durationStr = player->duration();
    tracklist.append(track);
    trackModel->appendTrack(track);
}

void MainWindow::on_closeButton_clicked()
{
    MainWindow::close();
}

void MainWindow::onStartOfPlayback(double total) {
    ui->horizontalSlider_2->setMaximum(total);
    ui->pushButton_play->setIcon(QIcon("./Resource/img/btn_pause.png"));
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
    ui->pushButton_play->setIcon(QIcon("./Resource/img/btn_play.png"));
    nextTrack(true);
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
        player->play();
        //audio->play(tracklist.at(position)->path);
    }
    else {
      //  audio->stop();
        player->stop();
    }
}

void MainWindow::onPauseOfPlayback() {
    ui->pushButton_play->setIcon(QIcon("./Resource/img/btn_play.png"));
}

void MainWindow::onCurPos(double position, double total) {
    //qDebug() << "Position" << position << "Total" << total;
    if (!moving) {
        ui->horizontalSlider_2->setValue(position);
//        ui->label_duration->setText(audio->formattedTime(position));
    }
}

void MainWindow::on_horizontalSlider_sliderPressed()
{
    moving = true;
}

void MainWindow::on_horizontalSlider_sliderReleased()
{
    moving = false;
    //audio->changePosition(ui->horizontalSlider_2->value());
    player->setPosition(ui->horizontalSlider_2->value());
}

void MainWindow::on_pushButton_play_clicked()
{
    if (hasNextTrack()) {
        player->play();
 //       audio->playOrPause(tracklist.at(position)->path);
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
        ui->pushButton_repeat->setIcon(QIcon("./Resource/img/btn_repeat_one.png"));
        break;
    case 2:
        ui->pushButton_repeat->setIcon(QIcon("./Resource/img/btn_repeat_all_highlighted.png"));
        break;
    default:
        ui->pushButton_repeat->setIcon(QIcon("./Resource/img/btn_repeat_off.png"));
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
        ui->pushButton_shuffle->setIcon(QIcon("./Resource/img/btn_shuffle_highlighted.png"));
    }
    else {
        ui->pushButton_shuffle->setIcon(QIcon("./Resource/img/btn_shuffle_off.png"));
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
  //      audio->play(tracklist.at(position)->path);
    }
}
