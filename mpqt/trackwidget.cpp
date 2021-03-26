#include "trackwidget.h"

TrackWidget::TrackWidget(QWidget *parent) :
    QWidget(parent)
{
    const int maxWidth = 325;
    title = new QLabel;
    QPalette mainRowPalette;
    mainRowPalette.setColor(QPalette::WindowText, Qt::white);
    title->setPalette(mainRowPalette);
    title->setMinimumWidth(maxWidth);
    title->setMaximumWidth(maxWidth);

    description = new QLabel;
    QPalette slaveRowPalette;
    slaveRowPalette.setColor(QPalette::WindowText, QColor(180,180,180));
    description->setPalette(slaveRowPalette);
    description->setMinimumWidth(maxWidth);
    description->setMaximumWidth(maxWidth);

    layout = new QVBoxLayout;
    layout->addWidget(title);
    layout->addWidget(description);

    hlayout = new QHBoxLayout;
    hlayout->addLayout(layout);

    duration = new QLabel;
    duration->setText("00:00");
    duration->setMaximumWidth(45);
    duration->setPalette(slaveRowPalette);

    hlayout->addWidget(duration);
    this->setLayout(hlayout);
}

TrackWidget::~TrackWidget()
{

}
