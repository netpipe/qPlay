#include "mainwindow.h"
#include "ui_mainwindow.h"

//Global variables used for use in variou functions
//Start
QUrl dir,dir1;
QList<QUrl> files;
QList<QMediaContent> content;
QList<QListWidgetItem *> itemlist;
qint64 save;
//End

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    player = new QMediaPlayer(this);                                        //Setting Up New MediaPlayer

    //Creating and adding necessary widgets to connect to QMediaPlayer Start

    vw = new QVideoWidget(this);                                            //Creating Video Output Widget

    QPalette pal(palette());                                                //Creating and adding Pallete to set Video Output Background
    pal.setColor(QPalette::Background, Qt::black);
    vw->setAutoFillBackground(true);
    vw->setPalette(pal);

    player->setVideoOutput(vw);                                             //Adding Video Output to MediaPlayer

    playlist = new QMediaPlaylist(player);                                  //Creating Playlist
    player->setPlaylist(playlist);                                          //Connecting Playlist to Media Player
    player->playlist();

    listWidget = new QListWidget(this);                                     //Creating a list widget to view playlist
    listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);     //Setting up selection mode for plylist
    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);                //Setting up context menu for playlist

    vw->installEventFilter(this);                                           //Setting up Video Output Widget to accept input events such as mouse double click

    sw = new QSplitter(this);                                               //Setting up a splitter widget to split Video Output and playlist
    sw->addWidget(vw);
    sw->addWidget(listWidget);
    sw->setStretchFactor(0,1);                                              //Setting up the stretch factor of Video Output to be greater than playlist

    ui->screenlayout->addWidget(sw);                                        //Adding the compact splitter widget to ui layout

    mw = new QWidget(this);                                                 //Creating a primary widget
    mw->setLayout(ui->screenlayout);                                        //Setting the layout to to a primary widget

    this->setCentralWidget(mw);                                             //Setting the primary widget as the central widget

    slider = new QSlider(this);                                             //Creating Sliders for volume change and position change
    volumeslider = new QSlider(this);

    slider->setOrientation(Qt::Horizontal);                                 //Setting the orientation of the sliders
    volumeslider->setOrientation(Qt::Horizontal);

    slider->setRange(0, player->duration() / 1000);                         //Setting the media position sliders's range
    volumeslider->setValue(100);                                            //Setting the volume slider's default value

    labelDuration = new QLabel(this);                                       //Creating a Label (to be used as duration viewer)
    colorDialog = new QDialog(this);                                        //Creaig a dialog (to be used as color options)

    //Adding widgets to the ui form by code Start

    ui->toolBar->addWidget(slider);
    ui->toolBar->addWidget(labelDuration);
    ui->mainToolBar->addWidget(volumeslider);

    //Adding widgets to the ui form by code End

    //Connecting the signals to the appropiate slots Start
    //Most of the connections are self-explanatory

    connect(player, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));            //Signal of change in Media
    connect(player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));            //Signal of position change of Media
    connect(slider, SIGNAL(sliderMoved(int)), SLOT(seek(int)));                                 //Signal of Media sider moved

    connect(volumeslider,&QSlider::sliderMoved,player,&QMediaPlayer::setVolume);                //Signal of volume slider moved
    connect(player,&QMediaPlayer::volumeChanged,volumeslider,&QSlider::setValue);               //Signal of volume change

    connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(listwidgetdoubleclicked(QListWidgetItem*)) );             //Signal of mouse double click on playlist window
    connect(listWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));             //Signal of context menu called on playlist window

    connect(player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(displayErrorMessage()));     //Signal of Media playback error

    //Creating shortcuts and connecting them Start

    s1 = new QShortcut(this);

    s1->setKey(Qt::Key_Space);                                        //Shortcut to Play or Pause (Space)

    connect(s1, &QShortcut::activated, [&](){
        if(player->state() == QMediaPlayer::PausedState || player->state() == QMediaPlayer::StoppedState)
            player->play();
        else
            player->pause();
    });

    s2 = new QShortcut(this);

    s2->setKey(Qt::Key_Escape);                                       //Shortcut to Leave Fullscreen (Escape)

    connect(s2, &QShortcut::activated, [&](){

        bool isFullScr = isFullScreen();
        if(isFullScr)
        {
            ui->menuBar->setVisible(isFullScr);
            ui->statusBar->setVisible(isFullScr);
            ui->mainToolBar->setVisible(isFullScr);
            ui->toolBar->setVisible(isFullScr);
            listWidget->setVisible(isFullScr);

            this->setWindowState(this->windowState()^Qt::WindowFullScreen);
         }
    });

    s3 = new QShortcut(this);

    s3->setKey( Qt::ALT + Qt::Key_Return);                          //Shortcut to Enter Fullscreen (Alt+Enter)

    connect(s3, &QShortcut::activated, [&](){

        bool isFullScr = isFullScreen();
        if(!isFullScr)
        {
            ui->menuBar->setVisible(isFullScr);
            ui->statusBar->setVisible(isFullScr);
            ui->mainToolBar->setVisible(isFullScr);
            ui->toolBar->setVisible(isFullScr);
            listWidget->setVisible(isFullScr);

            this->setWindowState(this->windowState()^Qt::WindowFullScreen);
         }
    });

    s4 = new QShortcut(this);

    s4->setKey(Qt::Key_Right);                                      //Shortcut to Seek forward (Right Arrow)

    connect(s4, &QShortcut::activated, [&](){
        curdur = player->duration();
        position = player->position();

        if(position+5000<=curdur)
        {
               player->setPosition(position+5000);
        }

    });

    s5 = new QShortcut(this);

    s5->setKey(Qt::Key_Left);                                      //Shortcut to Seek backward (Left Arrow)

    connect(s5, &QShortcut::activated, [&](){
        curdur = player->duration();
        position = player->position();

        if(position-5000>=0)
        {
               player->setPosition(position-5000);
        }

    });

    s6 = new QShortcut(this);

    s6->setKey(Qt::Key_Up);                                      //Shortcut to Volume Up (Up Arrow)

    connect(s6, &QShortcut::activated, [&](){

        int now = player->volume();

        if(now+5<=100)
        {
               player->setVolume(now+5);
        }

    });

    s7 = new QShortcut(this);

    s7->setKey(Qt::Key_Down);                                     //Shortcut to Volume Down (Down Arrow)

    connect(s7, &QShortcut::activated, [&](){
        int now = player->volume();

        if(now-5>=0)
        {
               player->setVolume(now-5);
        }

    });

    s8 = new QShortcut(this);

    s8->setKey(Qt::Key_Delete);                                    //Shortcut to delete from currenr playlist on view (Delete)

    connect(s8, &QShortcut::activated, [&](){
        itemlist.clear();

        if(listWidget->count()>0)
        {
            itemlist = listWidget->selectedItems();

            if(itemlist.count() == 1 && listWidget->currentRow()!=playlist->currentIndex())
            {
                itemlist.clear();
                itemlist.append(listWidget->currentItem());
            }
            deletefromPlaylist();
        }
    });

    s9 = new QShortcut(this);

    s9->setKey(Qt::Key_Return);                                      //Shortcut to play the current selection in playlist (Enter)

    connect(s9, &QShortcut::activated, [&](){
        QModelIndex pos = listWidget->currentIndex();
        int loc = pos.row();
        if(playlist->currentIndex()!= loc)
        {
            playlist->setCurrentIndex(loc);
            listWidget->setCurrentRow(playlist->currentIndex());
            on_actionPlay_triggered();
        }
    });

    s10 = new QShortcut(this);

    s10->setKey(Qt::CTRL+Qt::Key_S);                                  //Shortcut to Save Playlist (Ctrl+S)

    connect(s10, &QShortcut::activated, [&](){
        savePlaylist();
    });

    //Creating shortcuts and connecting them End

    //Connecting the signals to the appropiate slots End

    //Creating and adding necessary widgets to connect to QMediaPlayer End

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    //Open files slot (UI linked)
    files = QFileDialog::getOpenFileUrls(this,"Select one or more files to open",dir,"Media files (*.webm *.ogg *.mp3 *.wmv *.mp4 *.mkv *.flv *.rmvb *.3gp *.avi *.mpeg)");

    content.clear();

    for(const QUrl& f:files)
    {
        content.push_back(f);
        listWidget->addItem(f.fileName());
    }

    position=playlist->mediaCount();

    playlist->addMedia(content);

    listWidget->setCurrentRow(playlist->currentIndex() != -1? position:0);

    if(player->state()!=QMediaPlayer::PausedState)
        on_actionPlay_triggered();
}

void MainWindow::on_actionPlay_triggered()
{
    //Play slot (UI linked)
    if(player->error() == QMediaPlayer::NoError)
    {
        player->play();
        if(this->windowTitle() == "RockPlayer")
        {
            QMediaContent media = player->currentMedia();
            dir1 = media.canonicalUrl();
            this->setWindowTitle(dir1.fileName());
        }
        ui->statusBar->showMessage("Playing");
    }
    else
    {
        ui->statusBar->showMessage(player->errorString());
        ui->statusBar->showMessage("Error Playing Media");
    }
}

void MainWindow::on_actionPause_triggered()
{
    //Pause slot (UI linked)
    player->pause();
    ui->statusBar->showMessage("Paused . . . ");
}

void MainWindow::on_actionStop_triggered()
{
    //Stop slot (UI linked)
    player->stop();
    ui->statusBar->showMessage("Stopped");
    this->setWindowTitle("RockPlayer");
}

void MainWindow::on_actionMute_triggered()
{
    //Mute slot (UI linked)
    if(player->isMuted()==0)
    {
        save = volumeslider->value();
        volumeslider->setValue(0);
        player->setMuted(!player->isMuted());
    }
    else
    {
        volumeslider->setValue(save);
        player->setMuted(!player->isMuted());
    }
}

void MainWindow::on_actionNext_triggered()
{
    //Play Next slot (UI linked)
    if(playlist->currentIndex()!=playlist->mediaCount()-1)
    {
        playlist->next();
        listWidget->setCurrentRow(playlist->currentIndex() != -1? playlist->currentIndex():0);
        on_actionPlay_triggered();
    }
    else
    {
        on_actionStop_triggered();
    }
}

void MainWindow::on_actionPrevious_triggered()
{
    //Play Previous slot (UI linked)
    playlist->previous();

    if(playlist->currentIndex() == -1)
    {
        listWidget->setCurrentRow(0);
        on_actionStop_triggered();
    }
    else
    {
        listWidget->setCurrentRow(playlist->currentIndex());
        on_actionPlay_triggered();
    }
}

void MainWindow::durationChanged(qint64 duration)
{
    //Media duration cange slot (occurs when a new media is played)
    this->duration = duration/1000;
    slider->setMaximum(duration / 1000);

    QMediaContent media = player->currentMedia();
    dir1 = media.canonicalUrl();
    this->setWindowTitle(dir1.fileName());
}

void MainWindow::seek(int seconds)
{
    //Seek slot(correspods to media position slider)
    player->setPosition(seconds * 1000);
}

void MainWindow::positionChanged(qint64 progress)
{
    //Media position change slot
    if (!slider->isSliderDown())
    {
        slider->setValue(progress / 1000);
    }
    updateDurationInfo(progress / 1000);
}

void MainWindow::updateDurationInfo(qint64 currentInfo)
{
    //Duration info update slot
    QString tStr;
    if (currentInfo || duration)
    {
        QTime currentTime((currentInfo/3600)%60, (currentInfo/60)%60, currentInfo%60, (currentInfo*1000)%1000);
        QTime totalTime((duration/3600)%60, (duration/60)%60, duration%60, (duration*1000)%1000);
        QString format = "mm:ss";
        if (duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    labelDuration->setText(tStr);
}

void MainWindow::listwidgetdoubleclicked(QListWidgetItem *item)
{
    //Start selected media from playlist on view
    playlist->setCurrentIndex(listWidget->row(item));
    listWidget->setCurrentRow(playlist->currentIndex());
    on_actionPlay_triggered();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
 {
     //Mouse event handler for video output widget
     if (event->type() == QMouseEvent::MouseButtonDblClick)
     {
         if(obj == vw)
         {
             bool isFullScr = isFullScreen();

             ui->menuBar->setVisible(isFullScr);
             ui->statusBar->setVisible(isFullScr);
             ui->mainToolBar->setVisible(isFullScr);
             ui->toolBar->setVisible(isFullScr);
             listWidget->setVisible(isFullScr);
         }
         this->setWindowState(this->windowState()^Qt::WindowFullScreen);
      }
      return QMainWindow::eventFilter(obj,event);
 }

void MainWindow::on_actionPlaylists_triggered()
{
    //Playlist open slot (UI linked)
    openPlaylist();
}

void MainWindow::openPlaylist()
{
    //definition of open playlist slot
    QString fn;
    fn.clear();

    if(QDir("Playlist").exists() && QDir("Playlist").count()>0)
    {
        fn=QFileDialog::getOpenFileName(this,"Open playlist",QDir("Playlist").path() ,
        "Playlist files (*.m3u)");

        int cur,tot;
        QMediaContent x;

        cur = playlist->mediaCount();

        playlist->load(QUrl::fromLocalFile(fn));

        tot=playlist->mediaCount();

        for(int i=cur; i<tot; i++)
        {
            x = playlist->media(i);

            listWidget->addItem(x.canonicalUrl().fileName());
        }
        if(player->state()!=QMediaPlayer::PausedState)
            on_actionPlay_triggered();
    }
    else
    {
        QDir().mkdir("Playlist");
        fn=QFileDialog::getOpenFileName(this,"Open playlist",QDir("Playlist").path() ,
        "Playlist files (*.m3u)");
    }
}
void MainWindow::savePlaylist()
{
    //definition of save playlist slot
    if(playlist->mediaCount()>0)
    {
        QString fn;
        fn.clear();

        if(QDir("Playlist").exists())
        {
            fn=QFileDialog::getSaveFileName(this,"Save file",QDir("Playlist").path() ,
            "Playlist files (*.m3u) ;; All files (*.)");
        }
        else
        {
            QDir().mkdir("Playlist");
            fn=QFileDialog::getSaveFileName(this,"Save playlist",QDir("Playlist").path() ,
            "Playlist files (*.m3u) ;; All files (*.)");
        }
        qDebug() << playlist->save(QUrl::fromLocalFile(fn),"m3u");
    }
}

void MainWindow::deletefromPlaylist()
{
    //definition of delete playlist slot
    for(QListWidgetItem *i:itemlist)
    {
        playlist->removeMedia(listWidget->row(i),listWidget->row(i));
        delete listWidget->takeItem(listWidget->row(i));
    }
    if(playlist->mediaCount()>0)
    {
        listWidget->setCurrentRow(playlist->currentIndex());
        if(player->state() == QMediaPlayer::PlayingState)
            on_actionPlay_triggered();
    }
}

void MainWindow::onCustomContextMenu(const QPoint &point)
{
    //definition of custom contect menu
    QModelIndex pos = listWidget->indexAt(point);

    QMenu contextMenu(tr("Context menu"), this);

    QAction *action1;
    action1 = new QAction(this);
    action1->setText("Play");                           //Play

    connect(action1, &QAction::triggered, [&](){
        if(pos.isValid())
        {
            int loc = pos.row();
            playlist->setCurrentIndex(loc);
            listWidget->setCurrentRow(playlist->currentIndex());
            on_actionPlay_triggered();
        }
    });
    contextMenu.addAction(action1);

    QAction *action2;
    action2 = new QAction(this);
    action2->setText("Playlists");                           //Open Playlist

    connect(action2, &QAction::triggered, [&](){
        openPlaylist();
    });
    contextMenu.addAction(action2);

    QAction *action3;
    action3 = new QAction(this);
    action3->setText("Save");                           //Save Playlist

    connect(action3, &QAction::triggered, [&](){
        savePlaylist();
    });
    contextMenu.addAction(action3);

    QAction *action4;
    action4 = new QAction(this);
    action4->setText("Add");                            //Add to Playlist

    connect(action4, SIGNAL(triggered()), this, SLOT(on_actionOpen_triggered()));
    contextMenu.addAction(action4);

    QAction *action5;
    action5 = new QAction(this);
    action5->setText("Delete");                         //Delete from Playlist

    connect(action5, &QAction::triggered, [&](){
        if(pos.isValid())
        {
            itemlist.clear();

            if(listWidget->count()>0)
            {
                itemlist = listWidget->selectedItems();

                deletefromPlaylist();
            }
        }
    });
    contextMenu.addAction(action5);

    contextMenu.exec(listWidget->mapToGlobal(point));
}

void MainWindow::displayErrorMessage()
{
    //Error message slot
    ui->statusBar->showMessage(player->errorString());
}

void MainWindow::on_actionOpen_2_triggered()
{
    //Open files slot2 (UI linked)
    on_actionOpen_triggered();
}

void MainWindow::on_actionQuit_triggered()
{
    //Quit Application (UI linked)
    QCoreApplication::quit();
}

//Set Window Opacity Start (UI linked)

void MainWindow::on_action10_triggered()
{
    qreal v = 0.1;
    setWindowOpacity(v);
}

void MainWindow::on_action20_triggered()
{
    qreal v = 0.2;
    setWindowOpacity(v);
}

void MainWindow::on_action30_triggered()
{
    qreal v = 0.3;
    setWindowOpacity(v);
}

void MainWindow::on_action40_triggered()
{
    qreal v = 0.4;
    setWindowOpacity(v);
}

void MainWindow::on_action50_triggered()
{
    qreal v = 0.5;
    setWindowOpacity(v);
}

void MainWindow::on_action60_triggered()
{
    qreal v = 0.6;
    setWindowOpacity(v);
}

void MainWindow::on_action70_triggered()
{
    qreal v = 0.7;
    setWindowOpacity(v);
}

void MainWindow::on_action80_triggered()
{
    qreal v = 0.8;
    setWindowOpacity(v);
}

void MainWindow::on_action90_triggered()
{
    qreal v = 0.9;
    setWindowOpacity(v);
}

void MainWindow::on_action100_triggered()
{
    qreal v = 1.0;
    setWindowOpacity(v);
}

//Set Window Opacity End (UI linked)

void MainWindow::on_actionColor_Options_triggered()
{
     //Set Color Options slot (UI linked)

     QSlider *brightnessSlider = new QSlider(Qt::Horizontal);                       //Brightness slider
     brightnessSlider->setRange(-100, 100);
     brightnessSlider->setValue(vw->brightness());
     connect(brightnessSlider, SIGNAL(sliderMoved(int)), vw, SLOT(setBrightness(int)));
     connect(vw, SIGNAL(brightnessChanged(int)), brightnessSlider, SLOT(setValue(int)));

     QSlider *contrastSlider = new QSlider(Qt::Horizontal);                         //Contrast slider
     contrastSlider->setRange(-100, 100);
     contrastSlider->setValue(vw->contrast());
     connect(contrastSlider, SIGNAL(sliderMoved(int)), vw, SLOT(setContrast(int)));
     connect(vw, SIGNAL(contrastChanged(int)), contrastSlider, SLOT(setValue(int)));

     QSlider *hueSlider = new QSlider(Qt::Horizontal);                              //Hue slider
     hueSlider->setRange(-100, 100);
     hueSlider->setValue(vw->hue());
     connect(hueSlider, SIGNAL(sliderMoved(int)), vw, SLOT(setHue(int)));
     connect(vw, SIGNAL(hueChanged(int)), hueSlider, SLOT(setValue(int)));

     QSlider *saturationSlider = new QSlider(Qt::Horizontal);                       //Saturation slider
     saturationSlider->setRange(-100, 100);
     saturationSlider->setValue(vw->saturation());
     connect(saturationSlider, SIGNAL(sliderMoved(int)), vw, SLOT(setSaturation(int)));
     connect(vw, SIGNAL(saturationChanged(int)), saturationSlider, SLOT(setValue(int)));

     //Creating From Layout and adding options to show in dialog Start

     QFormLayout *layout = new QFormLayout;
     layout->addRow(tr("Brightness"), brightnessSlider);
     layout->addRow(tr("Contrast"), contrastSlider);
     layout->addRow(tr("Hue"), hueSlider);
     layout->addRow(tr("Saturation"), saturationSlider);

     QPushButton *button = new QPushButton(tr("Close"));                            //New button to add to Layout

     layout->addRow(button);

     //Creating From Layout and adding options to show in dialog End

     //Setting necessay information, layout and signals to color option dialog

     colorDialog->setWindowTitle(tr("Color Options"));
     colorDialog->setLayout(layout);

     connect(button, SIGNAL(clicked()), colorDialog, SLOT(close()));

     colorDialog->show();
}


void MainWindow::on_actionAbout_triggered()
{
    //About slot (UI linked)
    QMessageBox *message = new QMessageBox(this);                                   //New Messagebox to show "About"

    message->setWindowTitle("About Rock Player");
    message->setText("Rock Player v1.3 is a simple basic multimedia player\nRock Player is brought to you by Nasif Mahbub");
    message->setDetailedText("Rock Player is a free software built upon using other software products licensed under The GNU Lesser General Public License (LGPL).\nRock Player is explicitly dependent on K-Lite_Standard_Codec Pack\nThis software is a 32-bit software");

    message->show();
}

void MainWindow::on_actionShortcut_Preview_triggered()
{
    //Hot key slot (UI linked)
    QMessageBox *message = new QMessageBox(this);                                    //New Messagebox to show "Hot keys"

    message->setWindowTitle("Hot Keys");
    message->setText("Open\t\t\tCtrl+O\nPlay/Pause\t\tSpace\nStop\t\t\tS\nSeek forward\t\tRight Arrow\nSeek Backkward\tLeft Arrow\nVolume Up\t\tUp Arrow\nVolume Down\t\tDown Arrow\nPlay Next\t\tAlt+Right\nPlay Previous\t\tAlt Left\nMute\t\t\tM\nOpen Playlists\t\tCtrl+L\nSave(Playlist)\t\tCtrl+S\nDelete(from playlist)\tDelete\nFullscreen Enter\tAlt+Enter\nFullscreen Leave\tEscape");

    message->show();
}
