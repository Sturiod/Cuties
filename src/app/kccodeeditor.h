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

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QErrorMessage>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QMessageBox>
#include <QPalette>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QSettings>
#include <QString>
#include <QTimeLine>
#include <QTextCursor>
#include <QTextStream>

#include <QDebug>

#include "kchistoryconfigure.h"
#include "kcgeneralconfigure.h"
#include "kccolorconfigure.h"

#include "kctexteditor.h"
#include "kclinenumpanel.h"
#include "kcmarkpanel.h"
#include "kcsearchwindow.h"
#include "kclanguagemode.h"
#include "kcreplacewindow.h"
#include "kcconnectionhandler.h"

class KCSearchWindow;
class KCLanguageMode;

class KCCodeEditor : public QWidget
{
    Q_OBJECT
public:
    explicit KCCodeEditor(QWidget *parent = 0);
    ~KCCodeEditor();

    QFileDevice::FileError error();
    void setDocumentTitle(const QString &title);
    QString getDocumentTitle();
    void setDocumentCursor(int nLine, int linePos);
    void setTextFocus();
    QString getFilePath();
    QString getExecFileName();
    QTextCursor getTextCursor();

    int getTextLines();
    bool isModified();
    QTextDocument* document();
    KCLanguageMode *langMode() const;
    bool getOverwriteMode();

    void insertTextAtCursor(QString insertText);

signals:
    void filenameChanged(QString newName);
    void fileTextCursorChanged();
    void rewriteStateChanged(bool nowValue);

public slots:
    bool open(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveAs(const QString &fileName);
    void redo();
    void undo();
    void copy();
    void cut();
    void paste();
    void selectAll();
    void cursorChanged();
    QString getSelectedText();
    void showSearchBar();
    void showReplaceBar();
    void hideSearchBar();
    void setOverwriteMode(bool newValue);

private slots:
    void onModificationChanged(bool changed);

protected:
    void closeEvent(QCloseEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    bool processSaveAsAction(const QString &dialogCaption);
    bool requireSaveAs(const QString &Caption);
    void computeExecFileName();
    void fileInfoChanged(const QFile &file);
    void connectSearchWidgetWithEditor(KCSearchWidget *widget);

    KCEditorConfigure *configureInstance;

    KCLanguageMode *languageMode;

    QVBoxLayout *replaceLayout;
    QHBoxLayout *mainLayout;
    KCTextEditor *editor;
    KCLinenumPanel *linePanel;
    KCMarkPanel *markPanel;

    QString filePath;
    QString execFileName;
    QFileDevice::FileError fileError;
    QTextCursor fileTextCursor;

    KCSearchWindow *searchBar;
    KCReplaceWindow *replaceBar;

    KCConnectionHandler searcherConnections;

    friend class KCLanguageMode;
};

#endif // TEXTEDITOR_H
