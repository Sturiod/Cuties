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


#include "kcstatusbar.h"

KCStatusBar::KCStatusBar(QWidget *parent):
    QStatusBar(parent)
{
    //Set Margins
    setContentsMargins(0,0,0,0);

    //Add File Type Display Widget
    //kscFileTypeDisplay=new KCFileType(this);

    //Add Rewrite Mode Display Widget
    lblRewriteMode=new KCStatusRewriteInfo(this);
    lblRewriteMode->hide();

    //Add Cursor Position Widget
    kscCursorPosition=new KCStatusCursorInfo(this);
    connect(kscCursorPosition,SIGNAL(ToLineNum(int)),
            this, SIGNAL(ToNewPosition(int)));

    //addPermanentWidget(kscFileTypeDisplay);
    addPermanentWidget(lblRewriteMode);
    addPermanentWidget(kscCursorPosition);
}

void KCStatusBar::showGotoBar(int currentValue, int MaxValue)
{
    kscCursorPosition->ShowGotoBox(currentValue, MaxValue);
}

void KCStatusBar::updateCursorPosition(int LineNum, int ColNum)
{
    kscCursorPosition->updateCursorPosition(LineNum, ColNum);
}

void KCStatusBar::updateRewriteMode(bool NewValue)
{
    lblRewriteMode->show();
    lblRewriteMode->setTextMode(NewValue);
}

void KCStatusBar::hideRewriteDisplay()
{
    lblRewriteMode->hide();
}
