#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QTextStream>
#include <QTimer>

#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDirIterator>
#include <QDebug>

//#include <player.h>

int presetloaded=0;
bool writeconfig=0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->pushButton_play->setIcon(QIcon("./Resource/img/btn_play.png"));
  ui->pushButton_play->setIcon(QIcon("./Resource/img/btn_play.png"));
  ui->label_3->setPixmap(QPixmap("./Resource/img/freemp.png"));
  ui->pushButton_shuffle->setIcon(QIcon("./Resource/img/btn_shuffle_off.png"));
  ui->pushButton_prev->setIcon(QIcon("./Resource/img/btn_previous.png"));
  ui->pushButton_next->setIcon(QIcon("./Resource/img/btn_next.png"));
  ui->pushButton_repeat->setIcon(QIcon("./Resource/img/btn_repeat.png"));

  repeatMode = 0;
  moving = false;
  setRepeatModeIcon();
  shuffle = false;
  position = 0;

  m_playlistDB = new PlaylistManager(this);

  ui->listView->setEditTriggers(0);
  ui->listView->setAttribute(Qt::WA_MacShowFocusRect, false);
  ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->listView, SIGNAL(customContextMenuRequested(const QPoint &)),
          SLOT(showContextMenuForWidget(const QPoint &)));

  // ui->horizontalSlider_2->setMaximum(0);
  // ui->horizontalSlider_2->setSliderDown(true);
  trackModel = new TrackModel;
  trackModel->list = tracklist;

  ui->listView->setModel(trackModel);
  ui->listView->setItemDelegate(new TrackDelegate);
  setAcceptDrops(true);
  scanner = NULL;

  // audiothread
  // Added By Rajiv
  audio = new AudioThread(this);
  // connect(audio, SIGNAL(startOfPlayback(double )), this,
  // SLOT(onStartOfPlayback(double)));
  connect(audio, SIGNAL(startOfPlayback()), this, SLOT(onStartOfPlayback()));
  connect(audio, SIGNAL(endOfPlayback()), this, SLOT(onEndOfPlayback()));
  connect(audio, SIGNAL(pauseOfPlayback()), this, SLOT(onPauseOfPlayback()));
  connect(audio, SIGNAL(curPos(double, double)), this,
          SLOT(onCurPos(double, double)));
  connect(audio, SIGNAL(sliderTime(QString, qint64, qint64)), this,
          SLOT(onSlidertime(QString, qint64, qint64)));

 player = new QMediaPlayer(this);
   // QVideoWidget*
     //       vidplayer = new QVideoWidget;
//vidwid
  audio->setVideoOutput(ui->vidwid);

 // ui->vidwid
//this->setCentralWidget(vidplayer);
 // vidplayer->show();

  dlmanager = new DownloadManager;


  QDirIterator it("./Resource/themes/", QStringList() << "*.qss", QDir::Files, QDirIterator::Subdirectories);
  while (it.hasNext()){
    //  QFileInfo fileInfo(f.fileName());
      ui->cmbTheme->addItem(it.next().toLatin1());
  }


  QFile MyFile("themes.txt");
  if(MyFile.exists()){
      MyFile.open(QIODevice::ReadWrite);
      QTextStream in (&MyFile);
      QString line;
      QStringList list;
       //   QList<QString> nums;
      QStringList nums;
      QRegExp rx("[:]");
      line = in.readLine();
QString stylesheet;
      if (line.contains(":")) {
          list = line.split(rx);
              qDebug() << "theme" <<  list.at(1).toLatin1();
              stylesheet =  list.at(1).toLatin1();
        //loadStyleSheet( list.at(1).toLatin1());

              MyFile.close();
      }

    fileName=stylesheet;
      QFile file(stylesheet);
}
loaded=true;


ui->dockWidget_5->setFixedWidth(300);

// QWidget *pic = new QWidget(this);
// pic->setStyleSheet("background-image: url(\"test.png\")");
// pic->setGeometry(QRect(0,0,220,484));

//QPixmap bkgnd("/home/netpipe/Pictures/test.png");
//bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
//QPalette palette;
//palette.setBrush(QPalette::Background, bkgnd);
//this->setPalette(palette);

//this->ui->uitabwidget_2->setStyleSheet(     "background-image:url(\"test.png\"); background-position: center;" );
//this->setStyleSheet(            "QWidget# { "            " background-image: url(:/test.png) 0 0 0 0 stretch stretch;"            "}");
//this->setStyleSheet(     "QTabWidget { background-image:url(\"test.png\"); background-position: center;}" );

//QPixmap pixmap("./test.png");
//        QListView w;
//        QPalette p = w.palette();
//        p.setBrush(QPalette::Base, pixmap);
//        w.setPalette(p);
//        w.resize(pixmap.size());

pixmap.load("./test.png");

this->ui->uitabwidget_2->setStyleSheet(     "QTabWidget::pane { background: transparent; border:0;}"   );

}

void MainWindow::paintEvent(QPaintEvent *pe)
 {


    QPainter paint(this);
    int widWidth = this->ui->uitabwidget_2->width();
    int widHeight = this->ui->uitabwidget_2->height();
    pixmap = pixmap.scaled(widWidth, widHeight, Qt::KeepAspectRatioByExpanding);
    paint.drawPixmap(0, 0, pixmap);
    QWidget::paintEvent(pe);

//    QPainter* pPainter = new QPainter(this);
//    pPainter->drawPixmap(rect(), QPixmap("./test.png"));
//    delete pPainter;
//    QWidget::paintEvent(pe);

 }

MainWindow::~MainWindow() { delete ui; }

void MainWindow::showContextMenuForWidget(const QPoint &pos) {
  QMenu contextMenu(tr("Context menu"), this);
  contextMenu.addAction(clearAction);
  contextMenu.addAction(scanAction);
  contextMenu.exec(ui->listView->mapToGlobal(pos));
}

void MainWindow::dropEvent(QDropEvent *ev) {
  QList<QUrl> urls = ev->mimeData()->urls();
  stopScanner();

  foreach (QUrl url, urls) {
    qInfo() << "path:" << url.path();
    QFileInfo fi(url.path());
    if (fi.isDir()) {
      scanner = new Scanner(fi.filePath(),
                            this); // go inside the directory if it is one.
      connect(scanner, SIGNAL(fileAdded(QString)), this,
              SLOT(onFileAdded(QString)));
      scanner->start();
    }
    if (fi.isFile()) {
      addItem(url.path());
    }
  }
}



void MainWindow::loadStyleSheet( QString sheet_name)
{
    QFile file(sheet_name);
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);
}

void MainWindow::on_cmbTheme_currentIndexChanged(const QString &arg1)
{
    if (loaded==true)
    {
    fileName=ui->cmbTheme->currentText();
    QFile file(fileName);

    file.open(QIODevice::Text | QIODevice::ReadOnly);
    QString content;
  //  while(!file.atEnd())
    //   content.append(file.readLine());
//    ui->code->setPlainText(content);
  //  file.close();


    loadStyleSheet(ui->cmbTheme->currentText());

    QFile file2("themes.txt");
        if(file2.open(QIODevice::ReadWrite | QIODevice::Text))// QIODevice::Append |
        {
                QTextStream stream(&file2);
                file2.seek(0);
               stream << "theme:" << ui->cmbTheme->currentText().toLatin1()<< endl;
                for (int i = 0; i < ui->cmbTheme->count(); i++)
                {
                 stream << "theme:" << ui->cmbTheme->itemText(i) << endl;
                }
            //                file.write("\n");
               file2.close();
        }

    if (ui->cmbTheme->currentText().toLatin1() != ""){
      //   ui->cmbTheme->currentText().toLatin1();
    }
}
}


void MainWindow::dragEnterEvent(QDragEnterEvent *ev) { ev->accept(); }

void MainWindow::onClearList() {
  stopScanner();
  tracklist.clear();
  trackModel->deleteAllTracks();
  audio->stop();
  ui->horizontalSlider_2->setMaximum(0);
}

void MainWindow::stopScanner() {
  if (scanner != NULL && scanner->isRunning()) {
    scanner->stopped = true;
    if (!scanner->wait(2000)) {
      scanner->terminate();
      scanner->wait(); // Note: We have to wait again here!
    }
    scanner->deleteLater();
    scanner = NULL;
  }
}

void MainWindow::onFindMusic(QString directory) {
  stopScanner();
  QString dirMusic = directory;

//          QDir::homePath() + "/" +
//                     QStandardPaths::displayName(QStandardPaths::MusicLocation);

  scanner = new Scanner(dirMusic, this);
  connect(scanner, SIGNAL(fileAdded(QString)), this,
          SLOT(onFileAdded(QString)));
  scanner->start();
}

void MainWindow::addItem(QString s) {

  for (auto const &track : qAsConst(tracklist)) {
    if (s == track->path) {
      return;
    }
  }

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

void MainWindow::on_closeButton_clicked() { MainWindow::close(); }

/*void MainWindow::onStartOfPlayback(double total)*/
void MainWindow::onStartOfPlayback() {
  // ui->horizontalSlider_2->setMaximum(total);
  ui->pushButton_play->setIcon(QIcon("./Resource/img/btn_pause.png"));
  QModelIndex index = trackModel->index(position, 0);

  if (index.isValid()) {
    ui->listView->selectionModel()->clear();
    ui->listView->selectionModel()->select(index, QItemSelectionModel::Select);
    ui->listView->scrollTo(index);
    ui->label_artist->setText(tracklist.at(position)->artist);
    ui->label_title->setText(tracklist.at(position)->title);
  }
}

void MainWindow::onEndOfPlayback() {
  //  ui->pushButton_play->setIcon(QIcon(":/Resource/img/btn_play.png"));
  nextTrack(true);
}

void MainWindow::nextTrack(bool next) {
  short add = 1;
  if (!next)
    add = -1;
  switch (repeatMode) {
  case 1:
    // looped track
    break;
  case 2:
    // looped playlist
    position = position + add;
    if (position >= tracklist.size()) {
      position = 0;
    }
    if (position < 0) {
      position = tracklist.size() - 1;
    }
    break;
  default:
    position = position + add;
    break;
  }
  if (hasNextTrack()) {
    audio->play(tracklist.at(position)->path,0);
  } else {
    audio->stop();
  }
}

void MainWindow::onPauseOfPlayback() {
  ui->pushButton_play->setIcon(QIcon("./Resource/img/btn_play.png"));
  ui->pushButton_play->setIcon(QIcon("./Resource/img/btn_play.png"));
}

void MainWindow::onCurPos(double position, double total) {
  // qDebug() << "Position" << position << "Total" << total;
  if (!moving) {
    // ui->horizontalSlider_2->setValue(position*1000);
    // ui->horizontalSlider_2->setRange(0, position / 1000);
    // ui->label_duration_2->setText(audio->formattedTime(position));
  }
}
void MainWindow::onSlidertime(QString sliderduration, qint64 duration,
                              qint64 progress) {
  if (!moving) {
    // qDebug() << "Slider position" << sliderduration;
    ui->horizontalSlider_2->setMaximum(duration);
    ui->horizontalSlider_2->setValue(progress);
    //ui->label_bgr->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    ui->horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
    ui->label_duration_2->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui->label_duration_2->setContentsMargins(0, 0, 0, 0);
    ui->label_duration_2->setSizePolicy(QSizePolicy::Preferred,
                                        QSizePolicy::Minimum);
    ui->label_duration_2->setWordWrap(true);
    ui->label_duration_2->setText(sliderduration);
    m_trackDuration = sliderduration;
  }
}

bool MainWindow::hasNextTrack() {
  if (tracklist.size() <= 0)
    return false;
  else if (position < 0)
    return false;
  else if (position > (tracklist.size() - 1))
    return false;
  return true;
}

void MainWindow::setRepeatModeIcon() {
  if (repeatMode > 2) {
    repeatMode = 0;
  }
  switch (repeatMode) {
  case 1:
    ui->pushButton_repeat->setIcon(QIcon("./Resource/img/btn_repeat_one.png"));
    break;
  case 2:
    ui->pushButton_repeat->setIcon(
        QIcon("./Resource/img/btn_repeat_all_highlighted.png"));
    break;
  default:
    ui->pushButton_repeat->setIcon(QIcon("./Resource/img/btn_repeat_off.png"));
    break;
  }
}

void MainWindow::on_pushButton_repeat_clicked() {
  repeatMode++;
  setRepeatModeIcon();
}

void MainWindow::on_pushButton_shuffle_clicked() {
  shuffle = !shuffle;
  if (shuffle) {
    ui->pushButton_shuffle->setIcon(
        QIcon("./Resource/img/btn_shuffle_highlighted.png"));
  } else {
    ui->pushButton_shuffle->setIcon(
        QIcon("./Resource/img/btn_shuffle_off.png"));
  }
}

void MainWindow::on_pushButton_prev_clicked() { nextTrack(false); }

void MainWindow::on_pushButton_next_clicked() { nextTrack(true); }

void MainWindow::onFileAdded(QString file) { addItem(file); }

void MainWindow::on_listView_clicked(const QModelIndex &index) {
  position = index.row();
  if (hasNextTrack()) {
    audio->play(tracklist.at(position)->path,0);
  }
}

void MainWindow::on_SavePlaylist_clicked() {

  for (auto const track : qAsConst(tracklist)) {
    m_playlistDB->saveTrack(track->artist, track->title, track->durationStr,
                            track->path);
  }
}

void MainWindow::on_LoadPlaylist_clicked() {

  QList<QMap<QString, QString>> listTrackMap = m_playlistDB->loadTracks();

  for (auto const &trackMap : qAsConst(listTrackMap)) {
    Track *track = new Track;
    track->artist = trackMap["artist"];
    track->title = trackMap["title"];
    track->durationStr = trackMap["durationStr"];
    track->path = trackMap["path"];

    tracklist.append(track);
    trackModel->appendTrack(track);
  }
}

//void MainWindow::on_SavePlaylist_clicked()
//{

//    QFile file2("./presets/"+ ui->savePreset->text().toLatin1() + ".preset");
//        if(file2.open(QIODevice::ReadWrite | QIODevice::Text))// QIODevice::Append |
//        {
//                QTextStream stream(&file2);
//           //     file2.seek(0);

//                   stream << "CompAttLE:" << ui->CompAttLE->text().toLatin1()<< endl;

//                   file2.close();
//            }
//}

//void MainWindow::on_LoadPlaylist_clicked()
//{
//    //https://stackoverflow.com/questions/31384273/how-to-search-for-a-string-in-a-text-file-using-qt
//        QString searchString(":");
//        QFile MyFile(ui->SelectPresetCMB->currentText());
//        MyFile.open(QIODevice::ReadWrite);
//        QTextStream in (&MyFile);
//        QString line;
//      //  int ii=0;
//        QStringList list;
//         //   QList<QString> nums;
//        QStringList nums;


//        do {
//            line = in.readLine();
//            searchString=":";
//            if (line.contains(searchString)) { //, Qt::CaseSensitive
//                // do something
//                QRegExp rx("[:]");// match a comma or a space
//                list = line.split(rx);
//                nums.append(list.at(1).toLatin1());
//            }
//        } while (!line.isNull());
      //ui->CompAttLE->setText(nums.at(0));
//}



void MainWindow::on_actionScan_triggered() { onFindMusic( QDir::homePath() + "/Music" );  //+  QStandardPaths::displayName(QStandardPaths::MusicLocation)
                                           }

void MainWindow::on_actionClear_triggered() { onClearList(); }

void MainWindow::on_Play_clicked() {

  if (hasNextTrack()) {
    audio->playOrPause(tracklist.at(position)->path);
    // audio->playOrPause(tracklist.at(20.0f)->path);
  }

  audio->changePosition(ui->horizontalSlider_2->value() * 1000);
}

void MainWindow::on_horizontalSlider_2_sliderPressed() { moving = true; }

void MainWindow::on_horizontalSlider_2_sliderReleased() {
  moving = false;
  audio->changePosition(ui->horizontalSlider_2->value() * 1000);
}

void MainWindow::on_horizontalSlider_2_sliderMoved(int position) {
  audio->changePosition(position * 1000);
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
audio->setvolume(position);
}

void MainWindow::on_pushButton_clicked()
{
//    audio->play("https://usa19.fastcast4u.com:2000/");

audio->play("http://185.33.21.112:80/rockclassics_64",1);
//audio->play("http://136.32.49.56:8000",1);


}

void MainWindow::on_pushButton_play_clicked()
{
    if (hasNextTrack()) {
      audio->playOrPause(tracklist.at(position)->path);
      // audio->playOrPause(tracklist.at(20.0f)->path);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    onFindMusic("./");
    //          QDir::homePath() + "/" +
    //                     QStandardPaths::displayName(QStandardPaths::MusicLocation);
}

void MainWindow::on_pushButton_3_clicked()
{

    //http://api.shoutcast.com/legacy/genresearch?k=F5NaL5p4avjtIaoy&genre=classic&limit=X,Y

QString url2 = "http://api.shoutcast.com/legacy/stationsearch?k=F5NaL5p4avjtIaoy&search=";  //ambient+beats
        url2+= ui->stationSearch->text().toLatin1();
        url2+= "&limit=1,10";

        QString fileName = "./stationsearch";
        QFile file2(fileName);
            file2.remove();
            file2.close();

   // dlmanager->Download("http://api.shoutcast.com/legacy/Top500?k=F5NaL5p4avjtIaoy");
    dlmanager->Download(url2.toLatin1());

//    QThread::sleep(3);
//qDebug() << "testing\n";
    QTime dieTime = QTime::currentTime().addMSecs( 1000 );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }

parseSearch();

}

void MainWindow::parseSearch(){

    ui->txtIds->clear();
    ui->txtNames->clear();
    ui->stationurls->clear();

QString fileName = "./stationsearch";
QFile file(fileName);
  //  file.remove();
    file.open(QIODevice::ReadOnly);
    QString s;

    QTextStream s1(&file);
    s.append(s1.readAll());


    QString inputstring = s; //ui->txtInput->toPlainText();

    QRegExp regex;
    regex.setPattern("name=\\\"([^\\\"]+)\\\"");

    QStringList namelist, idlist;
    int pos = 0;
    while (pos >= 0)
    {
        pos = regex.indexIn(inputstring, pos);

        if (pos >= 0)
        {
            QString strName = regex.cap(0);
            strName = strName.mid(5);
            namelist.push_back(strName);
            pos += strName.length();
        }
    }

    regex.setPattern("id=\\\"([0-9]+)\\\"");
    pos = 0;
    while (pos >= 0)
    {
        pos = regex.indexIn(inputstring, pos);
        if (pos >= 0)
        {
            QString strId = regex.cap(0);
            strId = strId.mid(3);
            idlist.push_back(strId);
            pos += strId.length();
        }
    }
ui->txtNames->clear();
    QString buf = "";
    int count = namelist.count();
    for (int i = 0; i < count; i ++)
    {
        buf = namelist.at(i);
          buf.remove('"');
        ui->txtNames->addItem(buf);

   //     buf += "\r\n";
    }
   // ui->txtNames->setPlainText(buf);
  //  ui->txtNames->addItem(buf);
  //  ui->txtNames->(buf);

ui->txtIds->clear();
    buf = "";
    count = idlist.count();
    for (int i = 0; i < count; i ++)
    {
        buf = idlist.at(i);
      //  buf += "\r\n";
        buf.remove('"');
           ui->txtIds->addItem(buf);

    }
  //  ui->txtIds->setPlainText(buf);


}

void MainWindow::on_chooseStationbtn_clicked()
{
  //     QFile::remove("tunein-station.pls");
ui->stationurls->clear();
   // ui->txtIds->addItem(buf);
 //   QString url2 = "http://yp.shoutcast.com/sbin/tunein-station.pls?id=";
 //           url2 +=    ui->txtIds->currentItem()->text().toLatin1();

 //   dlmanager->Download(url2);

  //  QThread::sleep(1);

    QString filename="tunein-station.pls";
    QFile file(filename);
    QString line;
    ui->textEdit->clear();
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){
            line = stream.readLine();
            ui->textEdit->setText(ui->textEdit->toPlainText()+line+"\n");
          //  qDebug() << "linea: "<<line;
        }
    }
    file.close();


    QString inputstring = ui->textEdit->toPlainText();

    QRegExp regex;
    //regex.setPattern("name=\\\"([^\\\"]+)\\\"");

     regex.setPattern("(http:|http)//[a-zA-Z0-9./?=_%:-]*"); //https
    QStringList namelist, idlist;
    int pos = 0;
    while (pos >= 0)
    {
        pos = regex.indexIn(inputstring, pos);

        if (pos >= 0)
        {
            QString strName = regex.cap(0);
            strName = strName.mid(5);
            namelist.push_back(strName);
                ui->stationurls->addItem(strName);
            pos += strName.length();
        }
    }


    //QString fileName = "tunein-station.pls";
    //QFile file2(fileName);
    //    file2.remove();
     //   file2.close();

    //    QFile::remove("tunein-station.pls");

//audio->play("http://185.33.21.112:80/rockclassics_64",1);
  //  audio->play("http://185.33.21.112:80/rockclassics_64",1);

}


void MainWindow::on_txtIds_currentRowChanged(int currentRow)
{
//    ui->stationurls->clear();
// //download playlist
//    QString url2 = "https://yp.shoutcast.com/sbin/tunein-station.pls?id=";
//            url2 +=    ui->txtIds->currentItem()->text().toLatin1();
//QFile::remove("tunein-station.pls");
//    dlmanager->Download(url2);

//   //  QThread::sleep(2);

//    QString filename="tunein-station.pls";
//    QFile file(filename);
//    QString line;
//   // ui->textEdit->clear();
//    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
//        QTextStream stream(&file);
//        while (!stream.atEnd()){
//            line = stream.readLine();
//            ui->textEdit->setText(ui->textEdit->toPlainText()+line+"\n");
//          //  qDebug() << "linea: "<<line;
//        }
//    }
//    file.close();


//    QString inputstring = ui->textEdit->toPlainText();

//    QRegExp regex;
//    //regex.setPattern("name=\\\"([^\\\"]+)\\\"");

//     regex.setPattern("(http|https)://[a-zA-Z0-9./?=_%:-]*");
//    QStringList namelist, idlist;
//    int pos = 0;
//    while (pos >= 0)
//    {
//        pos = regex.indexIn(inputstring, pos);

//        if (pos >= 0)
//        {
//            QString strName = regex.cap(0);
//            strName = strName.mid(5);
//            namelist.push_back(strName);
//                ui->stationurls->addItem(strName);
//            pos += strName.length();
//        }
//    }

//QFile::remove("tunein-station.pls");
//QString url2 = "http://yp.shoutcast.com/sbin/tunein-station.pls?id=";
//        url2 +=    ui->txtIds->currentItem()->text().toLatin1();

//dlmanager->Download(url2);
}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{

    //http://api.shoutcast.com/legacy/genresearch?k=F5NaL5p4avjtIaoy&genre=classic&limit=X,Y


    QString url2 = "http://api.shoutcast.com/legacy/stationsearch?k=F5NaL5p4avjtIaoy&search=";  //ambient+beats
            url2+= ui->stationSearch->text().toLatin1();
            url2+= "&limit=1,10";

       // dlmanager->Download("http://api.shoutcast.com/legacy/Top500?k=F5NaL5p4avjtIaoy");
      //  dlmanager->Download(url2.toLatin1());
}

void MainWindow::on_refreshbtn_clicked()
{
    parseSearch();
}

void MainWindow::on_txtNames_currentRowChanged(int currentRow)
{
        ui->txtIds->setCurrentRow(currentRow);
}

void MainWindow::on_playstationbtn_clicked()
{
      audio->play("http:" + ui->stationurls->currentItem()->text().toLatin1(),1);
//ui->treeWidget->currentItem()->text(0);
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{

   ui->stationSearch->setText(ui->treeWidget->currentItem()->text(0) )  ;

   on_pushButton_3_clicked();

  // parseSearch();
}

void MainWindow::on_stationurls_itemDoubleClicked(QListWidgetItem *item)
{
    audio->play("http:" + ui->stationurls->currentItem()->text().toLatin1(),1);

}

void MainWindow::on_txtNames_itemDoubleClicked(QListWidgetItem *item)
{
    QFile::remove("tunein-station.pls");
    QString url2 = "http://yp.shoutcast.com/sbin/tunein-station.pls?id=";
            url2 +=    ui->txtIds->currentItem()->text().toLatin1();

    dlmanager->Download(url2);

    QTime dieTime = QTime::currentTime().addMSecs( 1000 );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }

    on_chooseStationbtn_clicked();
}

void MainWindow::on_pushButton_4_clicked()
{


    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "/home",
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);

       onFindMusic(dir.toLatin1());
}

void MainWindow::on_pushButton_5_clicked()
{

    QString fileName = QFileDialog::getOpenFileName(this, ("Open File"),
                                                    "/home",
                                                    ("Images (*.mp3 *.ogg *.wav)"));

    audio->play(fileName,0);
}

void MainWindow::on_txtIds_itemDoubleClicked(QListWidgetItem *item)
{
    QFile::remove("tunein-station.pls");
    QString url2 = "http://yp.shoutcast.com/sbin/tunein-station.pls?id=";
            url2 +=    ui->txtIds->currentItem()->text().toLatin1();

    dlmanager->Download(url2);

    QTime dieTime = QTime::currentTime().addMSecs( 1000 );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }

    on_chooseStationbtn_clicked();
}




//////

void MainWindow::WritePresetFile(QString fileName)
{
    QString pream_str, presetName_str;

    float kValue = ui->preamp_slider->value();
    kValue = kValue / 10;
    pream_str = QString::number(kValue);
    presetName_str = ui->PresetName->toPlainText();

    QFile file(fileName);
    file.open(QIODevice::Text | QIODevice::WriteOnly);
    QTextStream stream(&file);
    stream << "mbeq_1197" << endl;
    stream << "mbeq" << endl;
    stream << "Multiband EQ" << endl;


    stream << pream_str.toFloat() << endl;
    stream << presetName_str << endl;
    stream << "1" << endl; //equalizer status
        stream << "1" << endl; //persistance
        stream << "-30" << endl;
            stream << "30" << endl;
    stream << "15" << endl;
    stream <<  (float)ui->s50hz->value() / 10 << endl;
    stream <<  (float)ui->s100hz->value()/ 10 << endl;
    stream <<  (float)ui->s156hz->value()/ 10 << endl;
    stream <<  (float)ui->s220hz->value()/ 10 << endl;
    stream <<  (float)ui->s311hz->value()/ 10 << endl;
    stream <<  (float)ui->s440hz->value()/ 10 << endl;
    stream <<  (float)ui->s622hz->value()/ 10 << endl;
    stream <<  (float)ui->s880hz->value() / 10<< endl;
    stream <<  (float)ui->s125khz->value()/ 10 << endl;
    stream <<  (float)ui->s175khz->value()/ 10 << endl;
    stream <<  (float)ui->s25khz->value()/ 10 << endl;
    stream <<  (float)ui->s35khz->value()/ 10 << endl;
    stream <<  (float)ui->s5khz->value() / 10<< endl;
    stream <<  (float)ui->s10khz->value()/ 10 << endl;
    stream <<  (float)ui->s20khz->value()/ 10 << endl;
    stream << 50 << endl;
    stream << 100 << endl;
    stream << 156 << endl;
    stream << 220 << endl;
    stream << 311 << endl;
    stream << 440 << endl;
    stream << 622 << endl;
    stream << 880 << endl;
    stream << 1250 << endl;
    stream << 1750 << endl;
    stream << 2500 << endl;
    stream << 3500 << endl;
    stream << 5000 << endl;
    stream << 10000 << endl;
    stream << 20000 << endl;
    file.close();
    qDebug() << fileName.toLatin1() << "wrote";

    if (writeconfig){
     presetloaded=0;

     ui->preset->clear();
    ui->preset->clear();

    ui->preset->addItem(QDir::homePath()+"/.config/pulse/equalizerrc");

    QDirIterator it2("./", QStringList() << "*.preset", QDir::Files, QDirIterator::Subdirectories);
    while (it2.hasNext()){
        ui->preset->addItem(it2.next().toLatin1());
    }

    writeconfig=0;

    //ui->preset->setCurrentIndex(0);
    on_preset_currentIndexChanged(0);
        presetloaded=1;
    on_applybtn_clicked();
    }
}

void MainWindow::on_s50hz_sliderMoved(int position)
{
    float k = position;
    k = k / 10;
    ui->l50hz->setText(QString::number(k)+" db");
    if(presetloaded)
    on_applybtn_clicked();
}

void MainWindow::on_s100hz_sliderMoved(int position)
{
    float k = position;
    k = k / 10;
    ui->l100hz->setText(QString::number(k)+" db");
        if(presetloaded)
    on_applybtn_clicked();
}

void MainWindow::on_s156hz_sliderMoved(int position)
{
    float k = position;
    k = k / 10;
    ui->l156hz->setText(QString::number(k)+" db");
        if(presetloaded)
    on_applybtn_clicked();
}

void MainWindow::on_s220hz_sliderMoved(int position)
{
    float k = position;
    k = k / 10;
    ui->l220hz->setText(QString::number(k)+" db");
        if(presetloaded)
on_applybtn_clicked();
}

void MainWindow::on_s311hz_sliderMoved(int position)
{
    float k = position;
    k = k / 10;
    ui->l311hz->setText(QString::number(k)+" db");
        if(presetloaded)
on_applybtn_clicked();
}


void MainWindow::on_s440hz_sliderMoved(int position)
{
    float k = position;
    k = k / 10;
    ui->l440hz->setText(QString::number(k)+" db");
        if(presetloaded)
on_applybtn_clicked();
}

void MainWindow::on_s622hz_sliderMoved(int position)
{
    float k = position;
    k = k / 10;
    ui->l622hz->setText(QString::number(k)+" db");
        if(presetloaded)
on_applybtn_clicked();
}

void MainWindow::on_s880hz_sliderMoved(int position)
{
    float k = position;
    k = k / 10;
    ui->l880hz->setText(QString::number(k)+" db");
        if(presetloaded)
on_applybtn_clicked();
}

void MainWindow::on_s125khz_sliderMoved(int position)
{
    float k = position;
    k = k / 10;
    ui->l125khz->setText(QString::number(k)+" db");
        if(presetloaded)
on_applybtn_clicked();
}

void MainWindow::on_s175khz_sliderMoved(int position)
{
    float k = position;
    k = k / 10;
    ui->l175khz->setText(QString::number(k)+" db");
        if(presetloaded)
on_applybtn_clicked();
}

void MainWindow::on_s25khz_sliderMoved(int position)
{
    float k = position;
    k = k / 10;
    ui->l25khz->setText(QString::number(k)+" db");
        if(presetloaded)
    on_applybtn_clicked();
}

void MainWindow::on_s35khz_sliderMoved(int position)
{
    float k = position;
    k = k / 10;
    ui->l35khz->setText(QString::number(k)+" db");
        if(presetloaded)
    on_applybtn_clicked();
}

void MainWindow::on_s5khz_sliderMoved(int position)
{
    float k = position;
    k = k / 10;
    ui->l5khz->setText(QString::number(k)+" db");
        if(presetloaded)
    on_applybtn_clicked();
}

void MainWindow::on_s10khz_sliderMoved(int position)
{
    float k = position;
    k = k / 10;
    ui->l10khz->setText(QString::number(k)+" db");
        if(presetloaded)
on_applybtn_clicked();
}

void MainWindow::on_s20khz_sliderMoved(int position)
{
    float k = position;
    k = k / 10;
    ui->l20khz->setText(QString::number(k)+" db");
        if(presetloaded)
    on_applybtn_clicked();
}

void MainWindow::on_preamp_slider_sliderMoved(int position)
{
    float k = position;
    k = k / 10;
    ui->lpreamp->setText(QString::number(k)+" X");
        if(presetloaded)
    on_applybtn_clicked();
}

void MainWindow::on_applybtn_clicked()
{
    //save QDir::homePath()+"/.config/pulse/equalizerrc"
    WritePresetFile(QDir::homePath()+"/.config/pulse/equalizerrc");
    QProcess::execute ("bash", QStringList() << "pulseaudio-equalizer" << "interface.applysettings");
}


//void MainWindow::on_enableBTN_clicked()
//{
// //   QProcess::execute ("pulseaudio-equalizer enable");
//    QProcess::execute("bash", QStringList() << "/usr/bin/pulseaudio-equalizer" << "enable");
//}


//void MainWindow::on_disableBTN_clicked()
//{
//    QProcess::execute ("bash", QStringList() << "/usr/bin/pulseaudio-equalizer" << "disable");
//}

//void MainWindow::on_toggleBTN_clicked()
//{
//    QProcess::execute ("bash", QStringList() << "/usr/bin/pulseaudio-equalizer" << "toggle");

//}

void MainWindow::on_preset_currentIndexChanged(int index)
{
          presetloaded=0;
    if( index == 0 )
    {
//        ui->s50hz->setValue(4); on_s50hz_sliderMoved(4);
//        ui->s100hz->setValue(4); on_s100hz_sliderMoved(4);
//        ui->s156hz->setValue(4); on_s156hz_sliderMoved(4);
//        ui->s220hz->setValue(4); on_s220hz_sliderMoved(4);
//        ui->s311hz->setValue(4); on_s311hz_sliderMoved(4);
//        ui->s440hz->setValue(4); on_s440hz_sliderMoved(4);
//        ui->s622hz->setValue(4); on_s622hz_sliderMoved(4);
//        ui->s880hz->setValue(4); on_s880hz_sliderMoved(4);
//        ui->s125khz->setValue(4); on_s125khz_sliderMoved(4);
//        ui->s175khz->setValue(4); on_s175khz_sliderMoved(4);
//        ui->s156hz->setValue(4); on_s156hz_sliderMoved(4);
//        ui->s25khz->setValue(4); on_s25khz_sliderMoved(4);
//        ui->s35khz->setValue(4); on_s35khz_sliderMoved(4);
//        ui->s5khz->setValue(4); on_s5khz_sliderMoved(4);
//        ui->s10khz->setValue(4); on_s10khz_sliderMoved(4);
//        ui->s20khz->setValue(4); on_s20khz_sliderMoved(4);
//        ui->preamp_slider->setValue(20); on_preamp_slider_sliderMoved(20);
//        ui->PresetName->setPlainText("default");
    }
    else
    {
        QString fileName = ui->preset->itemText(index);

        QFile PresetFile(fileName);
        if(PresetFile.exists())
        {
            PresetFile.open(QIODevice::ReadOnly);
            QTextStream in (&PresetFile);
            QString strBuffer = in.readAll();
            QStringList strList = strBuffer.split('\n');

            ui->preamp_slider->setValue(strList[3].toFloat()*10);
            on_preamp_slider_sliderMoved(strList[3].toFloat()*10);
            ui->PresetName->setPlainText(strList[4]);


            double kValue = strList[10].toFloat()*10;
            ui->s50hz->setValue((int)kValue); on_s50hz_sliderMoved((int)kValue);

            kValue = strList[11].toFloat()*10;
            ui->s100hz->setValue((int)kValue); on_s100hz_sliderMoved((int)kValue);

            kValue = strList[12].toFloat()*10;
            ui->s156hz->setValue((int)kValue); on_s156hz_sliderMoved((int)kValue);

            kValue = strList[13].toFloat()*10;
            ui->s220hz->setValue((int)kValue); on_s220hz_sliderMoved((int)kValue);

            kValue = strList[14].toFloat()*10;
            ui->s311hz->setValue((int)kValue); on_s311hz_sliderMoved((int)kValue);

            kValue = strList[15].toFloat()*10;
            ui->s440hz->setValue((int)kValue); on_s440hz_sliderMoved((int)kValue);

            kValue = strList[16].toFloat()*10;
            ui->s622hz->setValue((int)kValue); on_s622hz_sliderMoved((int)kValue);

            kValue = strList[17].toFloat()*10;
            ui->s880hz->setValue((int)kValue); on_s880hz_sliderMoved((int)kValue);

            kValue = strList[18].toFloat()*10;
            ui->s125khz->setValue((int)kValue); on_s125khz_sliderMoved((int)kValue);

            kValue = strList[19].toFloat()*10;
            ui->s175khz->setValue((int)kValue); on_s175khz_sliderMoved((int)kValue);

            kValue = strList[20].toFloat()*10;
            ui->s25khz->setValue((int)kValue); on_s25khz_sliderMoved((int)kValue);

            kValue = strList[21].toFloat()*10;
            ui->s35khz->setValue((int)kValue); on_s35khz_sliderMoved((int)kValue);

            kValue = strList[22].toFloat()*10;
            ui->s5khz->setValue((int)kValue); on_s5khz_sliderMoved((int)kValue);

            kValue = strList[23].toFloat()*10;
            ui->s10khz->setValue((int)kValue); on_s10khz_sliderMoved((int)kValue);

            kValue = strList[24].toFloat()*10;
            ui->s20khz->setValue((int)kValue); on_s20khz_sliderMoved((int)kValue);

            PresetFile.close();
             presetloaded=1;
        }
        if (presetloaded){
          on_applybtn_clicked();

        }
}
}

