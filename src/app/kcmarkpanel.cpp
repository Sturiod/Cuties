/*
 *  Copyright 2013 Kreogist Dev Team
 *
 *  This file is part of Kreogist-Cuties.
 *
 *    Kreogist-Cuties is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *    Kreogist-Cuties is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kreogist-Cuties.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "kcmarkpanel.h"

KCMarkPanel::KCMarkPanel(QWidget *parent) :
    KCPanel(parent)
{
    markPix.load(":/img/image/BreakPoint.png");
    setAutoAdaptWidth(false);
    setFixedWidth(25);
    isPressed=false;
}

QPixmap KCMarkPanel::getMarkPix() const
{
    return markPix;
}

void KCMarkPanel::setMarkPix(const QPixmap &value)
{
    markPix = value;
}

void KCMarkPanel::draw(QPainter *painter, QTextBlock *block,
                        int x, int y, int w, int h,
                        bool isCurrentLine)
{
    Q_UNUSED(isCurrentLine);

    int blockNum=block->blockNumber();

    if(blockNum>=vecMark.size())
    {
        vecMark.resize(blockNum+1);
    }

    QPoint _global(x,y);
    vecMark[blockNum].rect.setTopLeft(mapToGlobal(_global));
    vecMark[blockNum].rect.setWidth(w);
    vecMark[blockNum].rect.setHeight(h);

    if(vecMark[blockNum].marked)
    {
        painter->drawPixmap(x,
                            y - 3,
                            markPix.width(),
                            markPix.height(),
                            markPix);
    }
}

void KCMarkPanel::mousePressEvent(QMouseEvent *e)
{
    if(e->buttons() == Qt::LeftButton)
    {
        isPressed=true;
        pressedPos=e->globalPos();
    }
}

void KCMarkPanel::mouseReleaseEvent(QMouseEvent *e)
{
    if(isPressed)
    {
        int i=getFirstVisiableBlockNumber(),
            l=getLastVisiableBlockNumber();

        l=(l==-1)?vecMark.size():l;
        // l==-1 means that the document is reach at the end.
        // So l should be vecMark.size().

        for(; i<l; i++)
        {
            if(vecMark[i].rect.contains(pressedPos,true))
            {
                if(vecMark[i].rect.contains(e->globalPos(),true))
                {
                    vecMark[i].marked^=1;   //exchange the state
                    update();
                }

                break;  //mouse press in a rect and release in another rect
            }
        }
        isPressed=false;
    }
}
