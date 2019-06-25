#ifndef ZIMGDISPUI_H
#define ZIMGDISPUI_H

#include <QWidget>
class ZImgDispUI : public QWidget
{
    Q_OBJECT
public:
    explicit ZImgDispUI(QWidget *parent = nullptr);
    ~ZImgDispUI();
public:
    qint32 ZDoInit();
    qint32 ZDoClean();
protected:
    void paintEvent(QPaintEvent *event);
public slots:
    void ZSlotFlushImg(const QImage &img);
private:
    QImage m_img;
};

#endif // ZIMGDISPUI_H
