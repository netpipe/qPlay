#ifndef TRACKDELEGATE_H
#define TRACKDELEGATE_H

#include <QItemDelegate>
#include <QPainter>
#include "track.h"
#include "trackwidget.h"

class TrackDelegate : public QItemDelegate
{
public:
    TrackDelegate();
    TrackWidget *wdg;
    /*Метод для отрисовки нашего виджета*/
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    /*Возвращает нужный размер нашего виджета*/
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    ~TrackDelegate();
};

#endif // TRACKDELEGATE_H
