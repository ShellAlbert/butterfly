#include "zimgdispui.h"
#include <QPainter>
ZImgDispUI::ZImgDispUI(QWidget *parent) : QWidget(parent)
{

}
ZImgDispUI::~ZImgDispUI()
{

}
qint32 ZImgDispUI::ZDoInit()
{
    return 0;
}
qint32 ZImgDispUI::ZDoClean()
{
    return 0;
}
void ZImgDispUI::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if(this->m_img.isNull())
    {
        return;
    }

    QPainter painter(this);
    painter.drawImage(QRectF(0,0,this->width(),this->height()),this->m_img);
}
void ZImgDispUI::ZSlotFlushImg(const QImage &img)
{
    this->m_img=img;
    this->update();
}
