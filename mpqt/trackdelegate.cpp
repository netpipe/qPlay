#include "trackdelegate.h"

TrackDelegate::TrackDelegate()
{
    wdg = new TrackWidget;
    wdg->setAttribute(Qt::WA_TranslucentBackground);
}


TrackDelegate::~TrackDelegate()
{

}

void TrackDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    /* Чтобы произвести данные преобразования нужно было регистрировать
     * наш тип макросом Q_DECLARE_METATYPE(MyItem *)
     */
    Track * track = index.data().value<Track *>();
    wdg->title->setText(QString::number(index.row()+1)+". "+ track->artist);

    wdg->description->setText(track->title);
    wdg->duration->setText(track->durationStr);
    QPalette slaveRowPaletteActive;
    slaveRowPaletteActive.setColor(QPalette::WindowText, QColor(255,195,50));
    QPalette slaveRowPalette;
    slaveRowPalette.setColor(QPalette::WindowText, QColor(180,180,180));
    //QPalette pal;
    //QLinearGradient gradient(0, 0, 0, 100);

    /* Если элемент выделен то устанавливаем определенный цвет,
     * если нет, то устанавливаем градиент
     */
    if ((option.state & QStyle::State_Selected) == QStyle::State_Selected){
        wdg->description->setPalette(slaveRowPaletteActive);
        //pal.setBrush(QPalette::Window, QBrush(QColor(0, 255, 200)));
    }else{

        wdg->description->setPalette(slaveRowPalette);
        //pal.setBrush(QPalette::Window, QBrush(QColor(56,64,68)));
        /*
        gradient.setColorAt(0.0, QColor(56,64,68));
        gradient.setColorAt(1.0, QColor(29,33,37));
        pal.setBrush(QPalette::Window, QBrush(QColor(Qt::transparent)));
        pal.setBrush(QPalette::Window, QBrush(gradient));
        */
    }
    //wdg->setPalette(pal);
    wdg->resize(option.rect.size());
    /* сохраняем текущее состояние painter'а, для того
     * чтобы произветси изменения над ним */
    painter->save();
    //painter->setRenderHint(QPainter::Antialiasing, true);
    painter->translate(option.rect.topLeft());
    wdg->render(painter);
    /*востанавливаем состояние пеинтера*/
    painter->restore();
}

QSize TrackDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    /*указываем любой нужный для нас размер*/
    int width = 400;
    int height = 60;
    return QSize(width, height);
}
