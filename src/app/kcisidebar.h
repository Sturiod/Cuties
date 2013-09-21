#ifndef KCISIDEBAR_H
#define KCISIDEBAR_H

#include <QDockWidget>
#include <QLabel>
#include <QTimeLine>
#include <QIcon>
#include <QToolButton>
#include <QPushButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QStackedWidget>
#include <QListView>

#include "kcihistoryconfigure.h"
#include "kciclipboard.h"

class kciClipboardHistoryStack : public QListView
{
    Q_OBJECT
public:
    explicit kciClipboardHistoryStack(QWidget *parent = 0);

signals:
    void requiredInsertText(QString insertText);

private slots:
    void dblClickClipboardItems(QModelIndex ItemID);

private:

};

class kciHistoryStack : public QListView
{
    Q_OBJECT
public:
    explicit kciHistoryStack(QWidget *parent = 0);

signals:
    void requiredOpenFiles(QString filePath);

private slots:
    void dblClickHistoryItems(QModelIndex ItemID);

private:

};

class kciSideBarContent : public QWidget
{
    Q_OBJECT
public:
    explicit kciSideBarContent(QWidget *parent = 0);
    ~kciSideBarContent();

signals:
    void historyRequiredOpenFiles(QString filePath);
    void clipRequiredInsertText(QString insertText);

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonGroupLayout;
    QToolButton *buttonRecent, *buttonClipboard;

    QStackedWidget *contents;
    kciHistoryStack *historyStack;
    kciClipboardHistoryStack *clipboardStack;
};

class kciSideBar : public QDockWidget
{
    Q_OBJECT
public:
    explicit kciSideBar(QWidget *parent = 0);
    
signals:
    void historyRequiredOpenFiles(QString filePath);
    void clipboardRequiredInsertText(QString insertText);

public slots:
    void showAnime();
    void hideAnime();

private slots:
    void resizeDock(int newWidth);

private:
    kciSideBarContent *CentralWidget;
};

#endif // KCISIDEBAR_H