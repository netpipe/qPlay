#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <player.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

ui->pushButton_play->setIcon(QIcon("./Resource/img/btn_play.png"));
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

ui->horizontalSlider_2->setMaximum(0);

trackModel = new TrackModel;
trackModel->list = tracklist;

ui->listView->setModel(trackModel);
ui->listView->setItemDelegate(new TrackDelegate);
setAcceptDrops(true);

scanner = NULL;
 //audiothread

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

