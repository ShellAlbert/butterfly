#include "zmainui.h"
#include <QDebug>
ZMainUI::ZMainUI(QWidget *parent)
    : QWidget(parent)
{
    this->m_UIMain=NULL;
    this->m_UIAux=NULL;
    this->m_hLayout=NULL;
}

ZMainUI::~ZMainUI()
{
    this->ZDoClean();
}
ZImgDispUI* ZMainUI::ZGetDispUI(qint32 index)
{
    switch(index)
    {
    case 0:
        return this->m_UIMain;
        break;
    case 1:
        return this->m_UIAux;
        break;
    default:
        break;
    }
    return NULL;
}
qint32 ZMainUI::ZDoInit()
{
    try{
        this->m_UIMain=new ZImgDispUI;
        this->m_UIAux=new ZImgDispUI;
        this->m_hLayout=new QHBoxLayout;
    }catch(...)
    {
        qDebug()<<"<error>:new failed,low memory.";
        return -1;
    }

    if(this->m_UIMain->ZDoInit()<0)
    {
        return -1;
    }
    if(this->m_UIAux->ZDoInit()<0)
    {
        return -1;
    }

    this->m_hLayout->addWidget(this->m_UIMain);
    this->m_hLayout->addWidget(this->m_UIAux);
    this->setLayout(this->m_hLayout);

    return 0;
}
qint32 ZMainUI::ZDoClean()
{
    if(this->m_UIMain)
    {
        delete this->m_UIMain;
    }
    if(this->m_UIAux)
    {
        delete this->m_UIAux;
    }
    if(this->m_hLayout)
    {
        delete this->m_hLayout;
    }
    return 0;
}
