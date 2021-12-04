#include "playlistManager.h"

#include <QSqlError>
#include <QSqlQuery>

#include <QVariant>

PlaylistManager::PlaylistManager(QObject *parent) : QObject(parent) {

  m_db = QSqlDatabase::addDatabase("QSQLITE");
  m_db.setHostName("Clay");
  m_db.setDatabaseName("playlist");
  m_db.setUserName("Clay");
  m_db.setPassword("Clay");

  if (m_db.open()) {
    qInfo() << "Database is open!";
    QSqlQuery query;
    const bool &ok = query.exec(
        "CREATE TABLE IF NOT EXISTS playlist (id INT primary key,"
        "artist VARCHAR(255), title VARCHAR(255), durationStr VARCHAR (255), "
        "path VARCHAR(255), UNIQUE (path));");

    if (ok) {
      qInfo() << "Tables:" << m_db.tables();
    } else {
      debugError("Error table", query);
    }
  } else {
    qDebug() << m_db.isOpenError()
             << "Native error code:" << m_db.lastError().nativeErrorCode()
             << "Error text:" << m_db.lastError().text();
  }
}

void PlaylistManager::saveTrack(const QString &artist, const QString &title,
                                const QString &durationStr,
                                const QString &path) {

  if (isOpen()) {
    qInfo() << "Prepare to save the playlist into database.";

    QSqlQuery query;
    query.prepare("INSERT INTO playlist(artist, title, durationStr, path)"
                  "VALUES(:artist, :title, :durationStr, :path)");

    query.bindValue(":artist", QVariant(artist));
    query.bindValue(":title", QVariant(title));
    query.bindValue(":durationStr,", QVariant(durationStr));
    query.bindValue(":path", QVariant(path));

    if (query.exec()) {
      qInfo() << "Insertion done!"
              << "Artist: " << artist << "| Title: " << title
              << "| Duration: " << durationStr << "| Path: " << path;
    } else {
      debugError("Failed to save playlist into database", query);
    }
  }
}

QList<QMap<QString, QString>> PlaylistManager::loadTracks() const {

  if (isOpen()) {
    QList<QMap<QString, QString>> listTracks;

    QSqlQuery query;
    query.exec("SELECT artist, title, durationStr, path FROM playlist");

    while (query.next()) {

      QMap<QString, QString> track;

      track["artist"] = query.value(0).toString();
      track["title"] = query.value(1).toString();
      track["durationStr"] = query.value(2).toString();
      track["path"] = query.value(3).toString();

      listTracks.append(track);
    }

    return listTracks;
  }

  return QList<QMap<QString, QString>>();
}

void PlaylistManager::clearTablePlaylist() const {
  if (isOpen()) {
    QSqlQuery query;
    const bool &ok = query.exec("DELETE FROM playlist");
    if (ok) {
      qInfo() << "Table playlist cleared." << m_db.tables();
    } else {
      debugError("Error clear playlist table", query);
    }
  }
}

void PlaylistManager::deleteTablePlaylist() const {
  QSqlQuery query;
  const bool &ok = query.exec("DROP TABLE playlist");

  if (ok) {
    qInfo() << "Table playlist deleted." << m_db.tables();
  } else {
    debugError("Error delete playlist table", query);
  }
}

void PlaylistManager::showTablePlaylist() const {

  QSqlQuery query;
  const bool &ok =
      query.exec("SELECT artist, title, durationStr, path FROM playlist");

  if (ok) {
    int i{1};
    while (query.next()) {
      QString artist = query.value(0).toString();
      QString title = query.value(1).toString();
      QString durationStr = query.value(2).toString();
      QString path = query.value(3).toString();

      qInfo() << "Song number:" << i++ << "| Artist:" << artist
              << "| Title:" << title << "| Duration:" << durationStr
              << "| path:" << path;
    }
  } else {
    debugError("delete playlist table", query);
  }
}

bool PlaylistManager::isOpen() const { return m_db.isOpen(); }

void PlaylistManager::debugError(const QString &title,
                                 const QSqlQuery &query) const {
  qDebug() << "\n"
           << title << ": " << query.lastError().text()
           << "Native error code: " << query.lastError().nativeErrorCode();
}
