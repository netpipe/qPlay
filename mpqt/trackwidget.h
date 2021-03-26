#ifndef TRACKWIDGET_H
#define TRACKWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class TrackWidget : public QWidget
{
    Q_OBJECT
public:
    QLabel *title;
    QLabel *description;
    QLabel *duration;
    QVBoxLayout *layout;
    QHBoxLayout *hlayout;
    explicit TrackWidget(QWidget *parent = 0);
    ~TrackWidget();
signals:

public slots:
};

#endif // TRACKWIDGET_H
