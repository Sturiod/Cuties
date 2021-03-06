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

#include "kchistoryconfigure.h"

KCHistoryConfigure *KCHistoryConfigure::instance = nullptr;
QString KCHistoryConfigure::historyFileName;

KCHistoryConfigure::KCHistoryConfigure()
{
    setHistoryFileName(QString("Settings/History.db"));

    //Load Default Value.
    cleanMark=false;
    trackUserHistory=true;
    maxRecentFilesSize=10;
    recentOpenedFileModel=new QStandardItemModel(this);

    //Load File Icon.
    cFileIcon=QIcon(":/Sidebar/image/Sidebar/source_c.png");
    cppFileIcon=QIcon(":/Sidebar/image/Sidebar/source_cpp.png");
    otherFileIcon=QIcon(":/Sidebar/image/Sidebar/source_others.png");
    pasFileIcon=QIcon(":/Sidebar/image/Sidebar/source_pas.png");
}

KCHistoryConfigure *KCHistoryConfigure::getInstance()
{
    return instance==nullptr?
           instance=new KCHistoryConfigure:
    instance;
}

QIcon KCHistoryConfigure::getFileIcon(const QString &fileExtName) const
{
    if(fileExtName==QString("c"))
    {
        return cFileIcon;
    }
    else if(fileExtName==QString("cpp"))
    {
        return cppFileIcon;
    }
    else if(fileExtName==QString("pas"))
    {
        return pasFileIcon;
    }
    else
    {
        return otherFileIcon;
    }
}

void KCHistoryConfigure::readConfigure()
{
    QSettings settings(getHistoryFileName(),
                       QSettings::IniFormat);

    settings.beginGroup("History");
    historyDirPath=settings.value("historyDir").toString();

    unClosedCurrent=settings.value("CurrIndex", -1).toInt();
    int unClosedFilePathsSize=settings.beginReadArray("unClosedFilePaths");

    for(int i=0; i<unClosedFilePathsSize; i++)
    {
        settings.setArrayIndex(i);

        QFileInfo _fileInfo(settings.value("filePath").toString());
        if(_fileInfo.exists())
        {
            unClosedFilePaths.append(_fileInfo.absoluteFilePath());
            unClosedFileH.append(settings.value("hPosition").toInt());
            unClosedFileV.append(settings.value("vPosition").toInt());
        }
    }
    settings.endArray();

    QString filePath;
    maxRecentFilesSize=settings.value("maxRecentFilesSize", 10).toInt();
    int recentOpenedFilesSize=settings.beginReadArray("recentOpenedFiles");
    if(recentOpenedFilesSize > maxRecentFilesSize)
    {
        recentOpenedFilesSize=maxRecentFilesSize;
    }
    for(int i=0; i<recentOpenedFilesSize; i++)
    {
        settings.setArrayIndex(i);
        filePath=settings.value("filePath").toString();

        //check whether the file is exist first
        QFile *historyFileItem=new QFile(filePath);
        if(historyFileItem->exists())
        {
            QFileInfo historyFileInfo(filePath);
            QStandardItem *item=new QStandardItem(historyFileInfo.fileName());
            item->setToolTip(filePath);
            item->setEditable(false);
            item->setIcon(getFileIcon(historyFileInfo.suffix().toLower()));
            recentOpenedFileModel->insertRow(0,item);
        }
    }
    settings.endArray();

    settings.endGroup();
}

void KCHistoryConfigure::writeConfigure()
{
    QSettings settings(getHistoryFileName(),
                       QSettings::IniFormat);

    settings.beginGroup("History");
    if(Q_UNLIKELY(cleanMark || !trackUserHistory))
    {
        settings.remove("");
    }
    else
    {
        settings.setValue("historyDir",historyDirPath);
        settings.setValue("CurrIndex", unClosedCurrent);
        settings.beginWriteArray("unClosedFilePaths");

        int unCloseCount=unClosedFilePaths.size()-1;
        int itemCount;
        for(int i=0; i<=unCloseCount; i++)
        {
            itemCount=unCloseCount - i;
            settings.setArrayIndex(i);
            settings.setValue("filePath",unClosedFilePaths.at(itemCount));
            settings.setValue("hPosition", unClosedFileH.at(itemCount));
            settings.setValue("vPosition", unClosedFileV.at(itemCount));
        }
        settings.endArray();

        settings.setValue("maxRecentFilesSize", maxRecentFilesSize);
        settings.beginWriteArray("recentOpenedFiles");

        int recentOpenedCounts=recentOpenedFileModel->rowCount()-1;
        for(int i=0; i<=recentOpenedCounts; i++)
        {
            settings.setArrayIndex(recentOpenedCounts-i);
            settings.setValue("filePath",recentOpenedFileModel->item(i)->toolTip());
        }
        settings.endArray();
    }
    settings.endGroup();
}

void KCHistoryConfigure::setHistoryDir(const QString &dirPath)
{
    historyDirPath=dirPath;
}

QString KCHistoryConfigure::getHistoryDir() const
{
    return historyDirPath;
}

void KCHistoryConfigure::cleanAllHistorys()
{
    cleanMark=true;
}

void KCHistoryConfigure::setTrackUserHistoryEnabled(bool enabled)
{
    trackUserHistory=enabled;
}

bool KCHistoryConfigure::isTrackUserHistoryEnabled() const
{
    return trackUserHistory;
}

void KCHistoryConfigure::clearAllUnClosedFilePaths()
{
    unClosedFilePaths.clear();
    unClosedFileH.clear();
    unClosedFileV.clear();
}

QStringList KCHistoryConfigure::getAllUnClosedFilePaths() const
{
    return unClosedFilePaths;
}

void KCHistoryConfigure::addUnClosedFilePath(const QString &path,
                                             const int &HValue,
                                             const int &VValue)
{
    unClosedFilePaths.append(path);
    unClosedFileH.append(HValue);
    unClosedFileV.append(VValue);
}

QList<int> KCHistoryConfigure::getAllUnClosedFileHs() const
{
    return unClosedFileH;
}

QList<int> KCHistoryConfigure::getAllUnClosedFileVs() const
{
    return unClosedFileV;
}

int KCHistoryConfigure::getUnClosedCurrent() const
{
    return unClosedCurrent;
}

void KCHistoryConfigure::setUnClosedCurrent(int value)
{
    unClosedCurrent = value;
}

int KCHistoryConfigure::getMaxRecentFilesSize() const
{
    return maxRecentFilesSize;
}

void KCHistoryConfigure::setMaxRecentFilesSize(int value)
{
    maxRecentFilesSize = value;
}

void KCHistoryConfigure::clearAllRecentFilesRecord()
{
    recentOpenedFileModel->clear();
}

void KCHistoryConfigure::addRecentFileRecord(const QString &path)
{
    QFileInfo historyFileInfo(path);

    QList<QStandardItem *> results=recentOpenedFileModel->findItems(historyFileInfo.fileName());
    QStandardItem *item=nullptr;
    item=new QStandardItem(historyFileInfo.fileName());
    item->setToolTip(path);
    item->setEditable(false);
    item->setIcon(getFileIcon(historyFileInfo.suffix().toLower()));
    if(!results.isEmpty())
    {
        for(auto i=results.begin(); i!=results.end(); i++)
        {
            recentOpenedFileModel->removeRow((*i)->row());
        }
    }
    recentOpenedFileModel->insertRow(0,item);

    if(recentOpenedFileModel->rowCount() >= maxRecentFilesSize)
    {
        recentOpenedFileModel->removeRow(recentOpenedFileModel->rowCount() - 1);
    }
}

QStandardItemModel *KCHistoryConfigure::getRecentOpenedFileModel() const
{
    return recentOpenedFileModel;
}

void KCHistoryConfigure::setRecentOpenedFileModel(QStandardItemModel *value)
{
    recentOpenedFileModel = value;
}

QString KCHistoryConfigure::getHistoryFileName()
{
    return historyFileName;
}

void KCHistoryConfigure::setHistoryFileName(const QString &value)
{
    historyFileName = value;
}
