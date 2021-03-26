#include "trackmodel.h"

TrackModel::TrackModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

TrackModel::~TrackModel()
{

}

int TrackModel::rowCount(const QModelIndex &parent) const
{
    return list.size();
}

void TrackModel::deleteAllTracks() {
    beginResetModel();
    list.clear();
    endResetModel();
}

void TrackModel::appendTrack(Track * const &track)
{
    //if(std::find(mydata.begin(),mydata.end(),person)!=mydata.end())
    //    return;
    beginInsertRows(QModelIndex(),list.size(),list.size());
    list.insert(list.size(),track);//.size();
    endInsertRows();
}

QVariant TrackModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() >= list.size())
        return QVariant();
    if (role == Qt::DisplayRole){
        /* Чтобы произвести преобразования QVariant::fromValue(list.at(index.row()))
         * нужно было регистрировать наш тип макросом Q_DECLARE_METATYPE(MyItem *)
         */
        return QVariant::fromValue(list.at(index.row()));
    }else{
        return QVariant();
    }
}

Qt::ItemFlags TrackModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled ;
}
