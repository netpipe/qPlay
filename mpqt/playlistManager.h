#pragma once

/*
 *
File added by @YacineBENAFFANE (Styleoshin)

For saving the playlist and add more features in future, it was preferred to
save the Songs in the database for ease of use
*/

#include "track.h"

#include <QObject>
#include <QSqlDatabase>

class PlaylistManager : public QObject {
  Q_OBJECT
public:
  explicit PlaylistManager(QObject *parent = nullptr);

  // This function is used for add one track (music) to the database
  void saveTrack(const QString &artist, const QString &title,
                 const QString &durationStr, const QString &path);

  // This function is used for get all music in map
  // map["artiste"], map["title"], map["durationStr"], map["path"]
  QList<QMap<QString, QString>> loadTracks() const;

private:
  // These functions are used for debugging
  void clearTablePlaylist() const;  // Clear contents of playlist
  void deleteTablePlaylist() const; // Delete table playlist
  void showTablePlaylist() const;   // Show content of playlist
  bool isOpen() const; // Test if the databse is opened, use this function
                       // everytime when sql query is executed.

  void debugError(const QString &title, const QSqlQuery &query) const;

private:
  QSqlDatabase m_db;
};
