#ifndef TRACKMODEL_H
#define TRACKMODEL_H
#include <QAbstractListModel>
#include <QList>
#include "track.h"

class TrackModel: public QAbstractListModel
{
    Q_OBJECT
public:
    explicit TrackModel(QObject *parent = 0);
    ~TrackModel();
    QList<Track *> list;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void appendTrack(Track * const &track);
    void deleteAllTracks();
signals:

public slots:
};

#endif // TRACKMODEL_H
