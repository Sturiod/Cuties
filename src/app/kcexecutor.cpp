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


#include "kcexecutor.h"

#ifdef Q_OS_WIN32
const QString consoleRunnerPath("KciConsoleRunner.exe");
#else
const QString consoleRunnerPath("KciConsoleRunner");
#endif

#ifdef Q_OS_UNIX
static const int terminalCount = 8;
static const Terminal knownTerminals[terminalCount] =
{
    {"xterm", "-e"},
    {"gnome-terminal", "-x"},
    {"konsole", "-e"},
    {"aterm", "-e"},
    {"Eterm", "-e"},
    {"rxvt", "-e"},
    {"urxvt", "-e"},
    {"xfce4-terminal", "-x"}
};
#else
static const int terminalCount = 1;
static const Terminal knownTerminals[terminalCount] =
{
    {"cmd", "/c"}
};
#endif

Terminal KCRunner::getDefaultTerminal()
{
    QSettings settings(KCGlobal::getInstance()->getSettingsFileName(),QSettings::IniFormat);
    settings.beginGroup("DefaultTerminal");
    Terminal ret(
        settings.value("name", knownTerminals[0].terminal_name).toByteArray().constData(),
        settings.value("arg",knownTerminals[0].arg).toByteArray().constData());
    settings.endGroup();

    return ret;
}

QStringList KCExecutor::getSupportTerminalList()
{
    QStringList ret;
    for(int i=0; i<terminalCount; i++)
    {
        ret<<knownTerminals[i].terminal_name;
    }
    return ret;
}

void KCExecutor::setDefaultTerminal(const int &num)
{
    if(Q_LIKELY(num<terminalCount))
    {
        QSettings settings(KCGlobal::getInstance()->getSettingsFileName(),QSettings::IniFormat);
        settings.beginGroup("DefaultTerminal");
        settings.setValue("name",knownTerminals[num].terminal_name);
        settings.value("arg",knownTerminals[num].arg);
        settings.endGroup();
    }
}

KCRunner::KCRunner(QObject *parent):
    QThread(parent)
{
    enabledAutoInput=false;
    enabledBackExec=true;
    executeProcess=NULL;
}

KCRunner::~KCRunner()
{
    if(executeProcess==NULL)
    {
        return ;
    }

    if(executeProcess->state() != QProcess::NotRunning)
    {
        executeProcess->kill();
        executeProcess->waitForFinished();
    }
}

void KCRunner::run()
{
    executeProcess=new QProcess;
    executeProcess->setReadChannelMode(QProcess::MergedChannels);
    if(enabledBackExec)
    {
        connect(executeProcess,SIGNAL(readyRead()),
                this,SLOT(onReadyRead()));
        executeProcess->start(executeFilePath);
    }
    else
    {
        QStringList executorArgv;
        QFileInfo info(executeFilePath);

        executeProcess->setWorkingDirectory(info.absoluteDir().absolutePath());
        /*
         * Here we have to launch terminal program.
         *
         * For Windows: launch cmd.exe
         * For Mac OS X: we have a .command file to launch terminal.
         * For Linux: launch Different Terminal program and send them args.
         */
        QString argExecuteFilePath=executeFilePath;

        /*
         * For Mac OS X, no terminal to execute. For others, use terminal to
         * launch terminal and solve the Qt Background Running problem.
         */
#ifdef Q_OS_MACX
        executorArgv
#else
        Terminal terminal=getDefaultTerminal();
        executorArgv<<terminal.arg
#endif

#ifdef Q_OS_WIN32
                    //For Windows, we have to launch terminal by:
                    // cmd /c start cmd /c kciExecutor.exe C:\cppName.exe
                    <<"start"<<terminal.terminal_name<<terminal.arg
#endif
                    <<qApp->applicationDirPath()+'/'+consoleRunnerPath<<argExecuteFilePath;

        connect(executeProcess,SIGNAL(finished(int)),
                this,SLOT(quit()));
        connect(this,SIGNAL(finished()),
                this,SLOT(deleteLater()));
#ifdef Q_OS_MACX
        executeProcess->start(qApp->applicationDirPath() + "/openTerminal.command",executorArgv);
#else
        executeProcess->start(QLatin1String(terminal.terminal_name),executorArgv);
#endif
    }

    if(enabledAutoInput)
    {
        executeProcess->write(m_input);
    }

    exec();
}

void KCRunner::onReadyRead()
{
    output_lock.lockForWrite();

    user_output+=executeProcess->readAll();

    output_lock.unlock();
}

void KCRunner::setTestCase(const QByteArray &input, const QByteArray &output)
{
    m_input=input;
    m_output=output;
}

bool KCRunner::isBackgroundExec() const
{
    return enabledBackExec;
}

void KCRunner::setBackgroundExec(bool value)
{
    lock.lockForWrite();
    enabledBackExec = value;
    lock.unlock();
}

bool KCRunner::isEnabledAutoInput() const
{
    return enabledAutoInput;
}

void KCRunner::setEnabledAutoInput(bool value)
{
    lock.lockForWrite();
    enabledAutoInput = value;
    lock.unlock();
}

QByteArray KCRunner::getUserOutput()
{
    output_lock.lockForRead();
    QByteArray ret = user_output;
    output_lock.unlock();
    return ret;
}

QString KCRunner::getPath() const
{
    return executeFilePath;
}

void KCRunner::setPath(const QString &value)
{
    executeFilePath = value;
}

KCExecutor *KCExecutor::instance=NULL;

KCExecutor::KCExecutor(QObject *parent) :
    QObject(parent)
{
}

KCExecutor::~KCExecutor()
{
    /*for(auto i=runnerList.begin(),e=runnerList.end();
        i!=e;
        i++)
    {
        delete i;
    }*/
}

void KCExecutor::exec(const QString &programPath)
{
    KCRunner *runner=new KCRunner;
#ifdef Q_OS_WIN32
    QString winPath;
    winPath=programPath;
    winPath=winPath.replace("//","/");
    runner->setPath(winPath);
#else
    runner->setPath(programPath);
#endif
    runner->setBackgroundExec(false);
    runner->setEnabledAutoInput(false);
    runner->start();
}

KCExecutor *KCExecutor::getInstance()
{
    if(instance==NULL)
    {
        return instance=new KCExecutor;
    }
    return instance;
}
