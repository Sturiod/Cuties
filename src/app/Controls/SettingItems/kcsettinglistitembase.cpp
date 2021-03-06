#include "kcsettinglistitembase.h"

KCSettingListItemBase::KCSettingListItemBase(QWidget *parent) :
    QWidget(parent)
{
    //Set Auto Fill Background.
    setAutoFillBackground(true);
    //Set Palette.
    pal=this->palette();
    pal.setColor(QPalette::Window, QColor(0xf7,0xcf,0x3d,0));
    setPalette(pal);
    //Set Label.
    Caption=new QLabel(this);
    //Set Default Height.
    setFixedHeight(27);

    //Set Selected Status.
    itemSelected=false;

    //Set Fade Out Anime.
    animeFadeOut=new QTimeLine(200,this);
    connect(animeFadeOut,SIGNAL(frameChanged(int)),
            this,SLOT(changeBackgroundAlpha(int)));
}

void KCSettingListItemBase::enterEvent(QEvent *e)
{
    if(!itemSelected)
    {
        //Stop Anime AnyWay.
        animeFadeOut->stop();
        //Set New Background.
        pal.setColor(QPalette::Window,QColor(0xf7,0xcf,0x3d,200));
        setPalette(pal);
    }
    e->accept();
}

void KCSettingListItemBase::leaveEvent(QEvent *e)
{
    if(!itemSelected)
    {
        int nowAlpha=pal.color(QPalette::Window).alpha();
        animeFadeOut->stop();
        animeFadeOut->setFrameRange(nowAlpha,0);
        animeFadeOut->start();
    }
    e->accept();
}

void KCSettingListItemBase::changeBackgroundAlpha(int alpha)
{
    pal.setColor(QPalette::Window,QColor(0xf7,0xcf,0x3d,alpha));
    setPalette(pal);
}

void KCSettingListItemBase::mousePressEvent(QMouseEvent *e)
{
    changeBackgroundAlpha(255);
    e->accept();
    emit ItemGetFocus();
}

void KCSettingListItemBase::enableEditMode()
{
    blnEditMode=true;
}

void KCSettingListItemBase::disableEditMode()
{
    blnEditMode=false;
}
