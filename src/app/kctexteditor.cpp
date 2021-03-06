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

#include "kctexteditor.h"

//static int elideWidth=500;

KCTextEditor::KCTextEditor(QWidget *parent) :
    QPlainTextEdit(parent)
{
    setContentsMargins(0,0,0,0);
    setObjectName("KCTextEditor");

    setFrameStyle(QFrame::NoFrame);
    setFont(QString("Monaco"));
    setAcceptDrops(false);

    configureInstance=KCEditorConfigure::getInstance();

    /*clipboard=KCClipboard::getInstance();
    clipboardHistoryMenuSignalMapper=new QSignalMapper(this);
    connect(clipboardHistoryMenuSignalMapper,SIGNAL(mapped(QString)),
            this,SLOT(insertPlainText(QString)));*/

    //Set Customize TextEditor Properties.
    //Set Tab Width.
    setTabStopWidth(fontMetrics().width(' ')*configureInstance->getTabWidth());
    //Set WordWarp Mode.
    setWordWrapMode(configureInstance->getWrapMode());
    //Set Cursor Width.
    setCursorWidth(configureInstance->getCursorWidth());
    //Set OverWrite Mode.
    setOverwriteMode(configureInstance->getOverwriteMode());

    QPalette pal = palette();
    KCColorConfigure::getInstance()->getPalette(pal,objectName());
    setPalette(pal);
    setFrameStyle(0);

    lineColor = QColor(0x64,0x64,0x64);
    searchResultColor = QColor(98,147,221);
    searchResultColor = QColor(0x5A,0x86,0xCA);
    noMatchedParenthesesColor = QColor(0xc8,0x0,0x0);
    matchedParenthesesColor = QColor(0xfd,0x95,0x00);

    searchCode=0;

    //Solve the default line's bug.
    updateHighlights();

    connect(this, &KCTextEditor::cursorPositionChanged,
            this, &KCTextEditor::updateHighlights);
    connect(this, &KCTextEditor::textChanged,
            this, &KCTextEditor::updateHighlights);
    connect(verticalScrollBar(),SIGNAL(valueChanged(int)),
            this,SLOT(updateSearchResults()));
    connect(verticalScrollBar(),SIGNAL(valueChanged(int)),
            this,SLOT(updateHighlights()));

    connect(configureInstance, &KCEditorConfigure::tabWidthChanged,
            this, &KCTextEditor::setTabWidth);
    connect(configureInstance, &KCEditorConfigure::wrapModeChanged,
            this, &KCTextEditor::setWordWrap);
    connect(configureInstance, &KCEditorConfigure::cursorWidthChanged,
            this, &KCTextEditor::setTheCursorWidth);
}

void KCTextEditor::paintEvent(QPaintEvent *e)
{
    QPlainTextEdit::paintEvent(e);
    emit updated();
}

void KCTextEditor::checkWhetherBlockSearchedAndDealWith(
        const QTextBlock& block)
{
    KCTextBlockData *data=(KCTextBlockData *)block.userData();
    //check whether the block has been searched
    data->beginUsingSearchDatas();
    bool hasSearched=data->isSearched(searchCode);
    if(!hasSearched)
    {
        data->endUsingSearchDatas();
        generalSearch(block,50,true);    //search 50 lines
        data->beginUsingSearchDatas();
    }
    data->endUsingSearchDatas();
}

bool KCTextEditor::showPreviousSearchResult()
{
    return findString(false);
}

bool KCTextEditor::showNextSearchResult()
{
    return findString(true);
}

void KCTextEditor::setTabWidth(int width)
{
    setTabStopWidth(fontMetrics().width(' ')*width);
}

bool KCTextEditor::findString(bool forward)
{
    QTextCursor _textCursor;

    bool hasMatch=false;
    _textCursor=textCursor();
    if(!forward)
    {
        _textCursor.setPosition(_textCursor.selectionStart());
    }

    for(QTextBlock i=_textCursor.block();
        i.isValid() && !hasMatch;
        i=(forward==true)?i.next():i.previous())
    {
        KCTextBlockData *blockData=(KCTextBlockData *)i.userData();

        checkWhetherBlockSearchedAndDealWith(i);

        blockData->beginUsingSearchDatas();
        if(blockData->hasMatched())
        {
            if(forward)
            {
                auto end=blockData->getEndMatchedTextPosition();
                for(auto j=blockData->getFirstMatchedTextPosition();
                    j<end;
                    j++)
                {
                    if(j->pos >= ((i.blockNumber() == _textCursor.blockNumber())?
                                  _textCursor.positionInBlock():0))
                    {
                        hasMatch=true;
                        _textCursor.setPosition(i.position()+j->pos);
                        _textCursor.movePosition(QTextCursor::NextCharacter,
                                                 QTextCursor::KeepAnchor,
                                                 j->matchedLength);
                        break;
                    }
                }
            }
            else
            {
                auto end=blockData->getFirstMatchedTextPosition();
                for(auto j=blockData->getEndMatchedTextPosition()-1;
                    j>=end;
                    j--)
                {
                    if((i.blockNumber() == _textCursor.blockNumber() &&
                        j->pos < _textCursor.positionInBlock()) ||
                       i.blockNumber() != _textCursor.blockNumber())
                    {
                        hasMatch=true;
                        _textCursor.setPosition(i.position()+j->pos);
                        _textCursor.movePosition(QTextCursor::NextCharacter,
                                                 QTextCursor::KeepAnchor,
                                                 j->matchedLength);
                        break;
                    }
                }
            }
        }
        blockData->endUsingSearchDatas();
    }

    if(!hasMatch)
    {
        int endBlockNumber=_textCursor.blockNumber();
        for(QTextBlock i= (forward==true)?
                          document()->firstBlock():
                          document()->lastBlock().previous();

            i.isValid() &&
            (forward==true?
             i.blockNumber()<endBlockNumber:
             i.blockNumber()>endBlockNumber) &&
            !hasMatch;

            i=(forward==true)?i.next():i.previous())
        {
            KCTextBlockData *blockData=(KCTextBlockData *)i.userData();
            checkWhetherBlockSearchedAndDealWith(i);

            blockData->beginUsingSearchDatas();
            if(blockData->hasMatched())
            {
                if(forward)
                {
                    auto end=blockData->getEndMatchedTextPosition();
                    for(auto j=blockData->getFirstMatchedTextPosition();
                        j<end;
                        j++)
                    {
                        if(j->pos >= (i.blockNumber() == _textCursor.blockNumber())?
                           _textCursor.positionInBlock():0)
                        {
                            hasMatch=true;
                            _textCursor.setPosition(i.position()+j->pos);
                            _textCursor.movePosition(QTextCursor::NextCharacter,
                                                     QTextCursor::KeepAnchor,
                                                     j->matchedLength);
                            break;
                        }
                    }
                }
                else
                {
                    auto end=blockData->getFirstMatchedTextPosition();
                    for(auto j=blockData->getEndMatchedTextPosition()-1;
                        j>=end;
                        j--)
                    {
                        if((i.blockNumber() == _textCursor.blockNumber() &&
                            j->pos < _textCursor.positionInBlock()) ||
                           i.blockNumber() != _textCursor.blockNumber())
                        {
                            hasMatch=true;
                            _textCursor.setPosition(i.position()+j->pos);
                            _textCursor.movePosition(QTextCursor::NextCharacter,
                                                     QTextCursor::KeepAnchor,
                                                     j->matchedLength);
                            break;
                        }
                    }
                }
            }
            blockData->endUsingSearchDatas();
        }
    }

    if(hasMatch)
    {
        setTextCursor(_textCursor);
        return true;
    }
    return false;
}

void KCTextEditor::searchString(QString searchTextSets,
                                bool regularExpressionSets,
                                bool caseSensitivelySets,
                                bool wholeWordSets)
{
    searchText=searchTextSets;
    searchRegularExpression=regularExpressionSets;
    searchCaseSensitively=caseSensitivelySets;
    searchWholeWord=wholeWordSets;
    searchCode++;

    if(searchText.isEmpty())
    {
        updateSearchResults();
        searchBackupCursor.clearSelection();
        setTextCursor(searchBackupCursor);
    }
    else
    {
        setTextCursor(searchBackupCursor);
        if(wholeWordSets)
        {
            if(!searchRegularExpression)
            {
                searchText=QRegularExpression::escape(searchTextSets);
            }
            searchText.prepend(QString("\\b("));
            searchText.append(QString(")\\b"));
        }
        updateSearchResults();
        searchOnOtherThread(searcherForNext,threadNext,firstVisibleBlock(),true);
        searchOnOtherThread(searcherForPrev,threadPrev,firstVisibleBlock(),false);
        findString(true);
    }
}

void KCTextEditor::updateSearchResults()
{
    generalSearch(firstVisibleBlock(),
                  height()/fontMetrics().lineSpacing()+2,
                  true);
    updateHighlights();
}

void KCTextEditor::generalSearch(const QTextBlock &block,
                                  const int &lines,
                                  const bool forward)
{
    QScopedPointer<KCTextSearcher> searcher;
    initTextSearcher(searcher);
    searcher->search(block,
                     lines,
                     searchCode,
                     forward);
}

void KCTextEditor::searchOnOtherThread(QScopedPointer<KCTextSearcher> &searcher,
                                        QFuture<void> &thread,
                                        const QTextBlock &block,
                                        const bool forward)
{
    if(!searcher.isNull())
    {
        searcher->requireStop();
        thread.waitForFinished();
    }

    initTextSearcher(searcher);

    thread=QtConcurrent::run(searcher.data(),
                             &KCTextSearcher::search,
                             block,
                             SEARCH_UNTIL_END_MARK,
                             searchCode,
                             forward);
}

void KCTextEditor::initTextSearcher(QScopedPointer<KCTextSearcher> &searcher)
{
    if(searchRegularExpression || searchWholeWord)
    {
        searcher.reset(new KCTextSearcherRegexp);
    }
    else
    {
        searcher.reset(new KCTextSearcherStringMatcher);
    }

    searcher->setPatternString(searchText);
    searcher->setIsCaseSensitive(searchCaseSensitively);
}

bool KCTextEditor::replace(const QString &oldText, const QString &newText)
{
    QTextCursor _cursor=textCursor();

    if(_cursor.hasSelection() && _cursor.selectedText() == oldText)
    {
        _cursor.beginEditBlock();
        _cursor.removeSelectedText();
        _cursor.insertText(newText);
        _cursor.endEditBlock();
        setTextCursor(_cursor);
        return true;
    }

    return false;
}

bool KCTextEditor::replaceAndFind(const QString &oldText,
                                   const QString &newText)
{
    bool ret=replace(oldText,newText);
    return ret|showNextSearchResult();
}

bool KCTextEditor::replaceAll(const QString &oldText, const QString &newText)
{
    bool ret=replaceAndFind(oldText,newText);
    while(replaceAndFind(oldText,newText))
    {
        ;
    }
    return ret;
}

void KCTextEditor::autoIndent()
{
    QTextCursor _textCursor=textCursor();
    QTextBlock currBlock=_textCursor.block();
    QTextBlock prevBlock=currBlock.previous();
    _textCursor.setPosition(currBlock.position());
    int basePos=findFirstCharacter(prevBlock);
    int pos=findFirstCharacter(currBlock);

    QTextCursor _prevTextCursor(prevBlock);
    _prevTextCursor.movePosition(QTextCursor::Right,
                                 QTextCursor::KeepAnchor,
                                 basePos);
    QString tabs=_prevTextCursor.selectedText();

    _textCursor.setPosition(currBlock.position()+pos);
    _textCursor.movePosition(QTextCursor::StartOfBlock,QTextCursor::KeepAnchor);
    _textCursor.removeSelectedText();
    _textCursor.insertText(tabs);

    KCTextBlockData *prevData=static_cast<KCTextBlockData *>(prevBlock.userData());
    KCTextBlockData *currData=static_cast<KCTextBlockData *>(currBlock.userData());
    int baseLevel=prevData->getCodeLevel();
    int currLevel=currData->getCodeLevel();

    if(currLevel>=baseLevel)
    {
        insertTab(_textCursor,currLevel-baseLevel);
    }
    else
    {
        removeTab(_textCursor,baseLevel-currLevel);
    }
}

void KCTextEditor::insertTab(QTextCursor insertTabCursor, int tabCount)
{
    insertTabCursor.clearSelection();
    QString spaceChar=configureInstance->usingBlankInsteadTab()?
                      QString(" ").repeated(configureInstance->getTabWidth()):
                      "\t";
    insertTabCursor.insertText(spaceChar.repeated(tabCount));
}

void KCTextEditor::removeTab(QTextCursor removeTabCursor, int tabCount)
{
    removeTabCursor.clearSelection();
    while(tabCount-- && !removeTabCursor.atBlockStart())
    {
        if(configureInstance->usingBlankInsteadTab())
        {
            removeTabCursor.movePosition(QTextCursor::Left,
                                         QTextCursor::KeepAnchor,
                                         configureInstance->getTabWidth());
        }
        else
        {
            removeTabCursor.movePosition(QTextCursor::Left,
                                         QTextCursor::KeepAnchor);
        }
    }

    removeTabCursor.removeSelectedText();
}

void KCTextEditor::tabPressEvent(QTextCursor tabPressCursor)
{
    if(tabPressCursor.selectedText().isEmpty())
    {
        insertTab(tabPressCursor);
    }
}

int KCTextEditor::findFirstCharacter(const QTextBlock &block)
{
    QString text=block.text();
    int ret=text.indexOf(QRegularExpression("\\S"));
    return ret==-1?block.text().length():ret;
}

void KCTextEditor::setDocumentCursor(int nLine, int linePos)
{
    QTextCursor cursor = textCursor();
    cursor.setPosition(document()->findBlockByNumber(nLine).position());
    cursor.movePosition(QTextCursor::NextCharacter,
                        QTextCursor::MoveAnchor,
                        linePos);
    setTextCursor(cursor);
}

void KCTextEditor::backupSearchTextCursor()
{
    searchBackupCursor=textCursor();
}

void KCTextEditor::highlightParenthesis(QList<QTextEdit::ExtraSelection> &selections)
{
    QTextCursor cursor = textCursor();
    cursor.clearSelection();
    int pos=cursor.position()-cursor.block().position();

    char all[]="{[()]}";
    int len=strlen(all);

    KCTextBlockData *blockData=static_cast<KCTextBlockData *>(cursor.block().userData());
    if(blockData!=NULL)
    {
        int matchedParentheses=-2;

        for(auto i=blockData->getFirstParenthesesInfo(),
            l=blockData->getEndParenthesesInfo();
            i<l;
            i++)
        {
            if(i->pos == pos-1)
            {
                for(int j=0; j<(len>>1); j++)
                    if(i->character == all[j])
                    {
                        cursor.movePosition(QTextCursor::Left,
                                            QTextCursor::KeepAnchor);
                        matchedParentheses=matchParentheses(
                                               all[j],
                                               all[len-j-1],
                                               i,
                                               cursor.block(),
                                               true);
                        break;
                    }
            }
            else if(i->pos == pos)
            {
                for(int j=(len>>1); j<len; j++)
                    if(i->character == all[j])
                    {
                        cursor.movePosition(QTextCursor::Right,
                                            QTextCursor::KeepAnchor);
                        matchedParentheses=matchParentheses(
                                               all[j],
                                               all[len-j-1],
                                               i,
                                               cursor.block(),
                                               false);
                        break;
                    }
            }
            else
            {
                continue;
            }

            switch(matchedParentheses)
            {
            case -2:
                //no parenthesis in the block
                break;
            case -1:
            {
                QTextEdit::ExtraSelection selection;

                selection.cursor = cursor;
                selection.format.setBackground(noMatchedParenthesesColor);
                selections.append(selection);

                break;
            }
            default:
            {
                QTextEdit::ExtraSelection selection;

                selection.cursor = cursor;
                selection.format.setBackground(matchedParenthesesColor);
                selections.append(selection);

                cursor.setPosition(matchedParentheses);
                cursor.movePosition(QTextCursor::Right,
                                    QTextCursor::KeepAnchor);
                selection.cursor = cursor;
                selection.format.setBackground(matchedParenthesesColor);
                selections.append(selection);

                break;
            }
            }

            cursor=textCursor();
            cursor.clearSelection();
        }
    }
}

int KCTextEditor::matchParentheses(const char &parenthesesA,
                                    const char &parenthesesB,
                                    QList<parenthesesInfo>::iterator startPos,
                                    QTextBlock block,
                                    bool forward)
{
    int count=0;

    KCTextBlockData *blockData=static_cast<KCTextBlockData *>(block.userData());
    if(blockData!=NULL)
    {
        auto i=startPos,
             l= forward?
                blockData->getEndParenthesesInfo() :
                blockData->getFirstParenthesesInfo() - 1;
        while(block.isValid())
        {
            for(;
                i!=l;
                i+= forward?1:-1)
            {
                if(i->character == parenthesesA)
                {
                    count++;
                }
                else if(i->character == parenthesesB)
                {
                    count--;
                }

                if(count == 0)
                {
                    return block.position()+i->pos;
                }
            }

            block= forward? block.next() : block.previous();
            blockData=static_cast<KCTextBlockData *>(block.userData());
            if(blockData == NULL)
            {
                break;
            }
            if(forward)
            {
                i= blockData->getFirstParenthesesInfo();
                l= blockData->getEndParenthesesInfo();
            }
            else
            {
                i= blockData->getEndParenthesesInfo()-1;
                l= blockData->getFirstParenthesesInfo()-1;
            }
        }
    }

    return -1;
}

void KCTextEditor::updateHighlights()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    highlightCurrentLine(extraSelections);
    highlightSearchResult(extraSelections);
    highlightParenthesis(extraSelections);

    setExtraSelections(extraSelections);
}

void KCTextEditor::highlightCurrentLine(QList<QTextEdit::ExtraSelection>& selections)
{
    if(!isReadOnly())
    {
        QTextEdit::ExtraSelection selection;

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        selections.append(selection);
    }
}

void KCTextEditor::highlightSearchResult(QList<QTextEdit::ExtraSelection>& selections)
{
    QTextCursor _cursor(document());

    QTextBlock block=firstVisibleBlock();
    int bottom=height()/fontMetrics().lineSpacing()+block.lineCount();

    for(; block.isValid() && bottom>0; block=block.next())
    {
        bottom-=block.lineCount();
        KCTextBlockData *currBlockData=static_cast<KCTextBlockData *>(block.userData());
        if(currBlockData==NULL)
        {
            continue;
        }
        checkWhetherBlockSearchedAndDealWith(block);
        currBlockData->beginUsingSearchDatas();
        if(currBlockData->hasMatched())
        {
            for(auto i=currBlockData->getFirstMatchedTextPosition(),
                end=currBlockData->getEndMatchedTextPosition();
                i<end;
                i++)
            {
                QTextEdit::ExtraSelection selection;

                _cursor.clearSelection();
                _cursor.setPosition(block.position()+i->pos);
                _cursor.movePosition(QTextCursor::NextCharacter,
                                     QTextCursor::KeepAnchor,
                                     i->matchedLength);
                selection.cursor=_cursor;

                selection.format.setBackground(searchResultColor);
                selections.append(selection);
            }
        }
        currBlockData->endUsingSearchDatas();
    }
}

/*void KCTextEditor::pasteFromeHistory()
{
    QMenu* menu=new QMenu(this);

    QStringList texts=clipboard->getClipboardTexts();
    for(int i=0,l=texts.size();
        i<l;
        i++)
    {
        QString elidedText=fontMetrics().elidedText(texts.at(i),
                                                    Qt::ElideRight,
                                                    elideWidth);
        QAction *action=menu->addAction(elidedText,
                                        clipboardHistoryMenuSignalMapper,
                                        SLOT(map()));

        clipboardHistoryMenuSignalMapper->setMapping(action,texts.at(i));
    }

    menu->exec(contextMenuPos);

    delete menu;
}*/

void KCTextEditor::contextMenuEvent(QContextMenuEvent *event)
{
    /*contextMenuPos=event->globalPos();

    QMenu *menu=createStandardContextMenu();
    menu->addAction(tr("paste from clipboard history"),
                    this,SLOT(pasteFromeHistory()));
    menu->exec(event->globalPos());
    delete menu;*/
    QPlainTextEdit::contextMenuEvent(event);
}

/*void KCTextEditor::autoCompleteParentheses(QKeyEvent *e,
                                            QTextCursor& currTextCursor,
                                            const QChar& rightParentheses)
{
    insertPlainText(QString(rightParentheses));
    currTextCursor.movePosition(QTextCursor::Left);
    setTextCursor(currTextCursor);
}*/

void KCTextEditor::keyPressEvent(QKeyEvent *e)
{
    QTextCursor _textCursor=textCursor();

    switch(e->key())
    {
    case Qt::Key_BracketLeft:
    {
        QPlainTextEdit::keyPressEvent(e);
        qDebug()<<"BracketLeft";
        break;
    }
        /*case Qt::Key_ParenLeft:
        {
            if(!(e->modifiers()&Qt::ShiftModifier))
            {
                QPlainTextEdit::keyPressEvent(e);
                autoCompleteParentheses(e,_textCursor,')');
            }
            break;
        }
        case Qt::Key_Apostrophe:
        {
            if(!(e->modifiers()&Qt::ShiftModifier))
            {
                QPlainTextEdit::keyPressEvent(e);
                autoCompleteParentheses(e,_textCursor,'\'');
            }
            else
            {
                QString text=_textCursor.selectedText();
                if(text.isEmpty())
                {
                    QPlainTextEdit::keyPressEvent(e);
                    autoCompleteParentheses(e,_textCursor,'\"');
                }
                else
                {
                    int start=_textCursor.selectionStart(),
                            end=_textCursor.selectionEnd();
                    _textCursor.beginEditBlock();
                    _textCursor.clearSelection();
                    _textCursor.setPosition(start);
                    _textCursor.insertText("\"");
                    _textCursor.setPosition(end+1);
                    _textCursor.insertText("\"");
                    _textCursor.endEditBlock();
                    setTextCursor(_textCursor);
                }
            }
            break;
        }
        case Qt::Key_BracketLeft:
        {
            QPlainTextEdit::keyPressEvent(e);
            if(!(e->modifiers()&Qt::ShiftModifier))
            {
                autoCompleteParentheses(e,_textCursor,']');
            }

            break;
        }*/
    case Qt::Key_Tab:
    {
        tabPressEvent(_textCursor);
        break;
    }
    case Qt::Key_Backspace:
    {
        int pos=findFirstCharacter(_textCursor.block());
        if(_textCursor.selectedText().length()>0)
        {
            QPlainTextEdit::keyPressEvent(e);
            break;
        }
        if(_textCursor.positionInBlock()<=pos && pos!=0 && (!pos<configureInstance->getTabWidth()))
        {
            removeTab(_textCursor);
        }
        else
        {
            QPlainTextEdit::keyPressEvent(e);
        }
        break;
    }
    case Qt::Key_Return:
    case Qt::Key_Enter:
    {
        QPlainTextEdit::keyPressEvent(e);
        autoIndent();
        break;
    }
    case Qt::Key_Insert:
    {
        setOverwriteMode(!overwriteMode());
        emit overwriteModeChanged(overwriteMode());
        break;
    }
    default:
        QPlainTextEdit::keyPressEvent(e);
        break;
    }
}

void KCTextEditor::setWordWrap(QTextOption::WrapMode wrapMode)
{
    setWordWrapMode(wrapMode);
}

void KCTextEditor::setTheCursorWidth(int width)
{
    setCursorWidth(width);
}
