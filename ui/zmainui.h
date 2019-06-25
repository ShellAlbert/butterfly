#ifndef ZMAINUI_H
#define ZMAINUI_H

#include <QWidget>
#include <QHBoxLayout>
#include <ui/zimgdispui.h>
class ZMainUI : public QWidget
{
    Q_OBJECT

public:
    ZMainUI(QWidget *parent = 0);
    ~ZMainUI();
public:
    qint32 ZDoInit();
    qint32 ZDoClean();

    ZImgDispUI* ZGetDispUI(qint32 index);
private:
    ZImgDispUI *m_UIMain;
    ZImgDispUI *m_UIAux;
    QHBoxLayout *m_hLayout;
};

#endif // ZMAINUI_H
