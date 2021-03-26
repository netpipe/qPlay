#ifndef SCANNER
#define SCANNER
#include <QThread>
#include <QDir>
#include <QDirIterator>
#include <QDebug>
#include <QString>

class Scanner : public QThread
{
  Q_OBJECT

public:
  explicit Scanner(const QDir &dir, QObject *parent = 0)
    : iterator(dir, QDirIterator::Subdirectories), QThread(parent), stopped(false)
  {
  }

  void run()
  {
    while(!stopped && iterator.hasNext()) {
      QString entry(iterator.next());
      if(iterator.fileInfo().suffix().toLower() == "mp3") {
          qDebug() << "scanner" << iterator.fileInfo().absoluteFilePath();
          fileList << entry;
          emit fileCountChanged(fileList.count());
          emit fileAdded(iterator.fileInfo().absoluteFilePath());
      }
      else {
          //qDebug() << QFileInfo(entry).fileName();
      }
    }
  }
  bool stopped;

  QStringList files() const
  {
    return fileList;
  }

public slots:
  void stop()
  {
    stopped = true;
  }
  bool isScannerRunning()
  {
      return stopped;
  }

signals:
  void fileCountChanged(int);
  void fileAdded(QString);

private:
  QDirIterator iterator;
  QStringList fileList;
};
#endif // SCANNER

