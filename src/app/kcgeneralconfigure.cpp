#include "kcgeneralconfigure.h"

KCGeneralConfigure *KCGeneralConfigure::instance=nullptr;

KCGeneralConfigure *KCGeneralConfigure::getInstance()
{
    return instance==nullptr?instance=new KCGeneralConfigure:instance;
}

KCGeneralConfigure::KCGeneralConfigure()
{
    asfFilter = tr("All Support Files")+
                "(*.txt *.h *.hpp *.rh *.hh *.c *.cpp *.cc *.cxx *.c++ *.cp *.pas)";
    ptfFilter = tr("Plain Text Files")+"(*.txt)";
    hfFilter = tr("Hearder Files")+"(*.h *.hpp *.rh *.hh)";
    cfFilter = tr("C Source Files")+"(*.c)";
    cppfFilter = tr("C++ Source Files")+"(*.cpp *.cc *.cxx *.c++ *.cp)";
    pasfFilter = tr("Pascal Source Files")+"(*.pas)";
    afFilter = tr("All Files")+"(*.*)";

    strFileFilter = asfFilter + ";;" +
                    ptfFilter + ";;" +
                    hfFilter + ";;" +
                    cfFilter + ";;" +
                    cppfFilter + ";;" +
                    pasfFilter + ";;" +
                    afFilter;

    defaultLanguageMode=1;
    rememberUnclosedFile=true;
    useDefaultLanguageWhenOpen=false;
    useDefaultLanguageWhenSave=true;

    searchEngine defaultEngineGoogleAddor;
    defaultEngineGoogleAddor.name=tr("Google");
    defaultEngineGoogleAddor.engineURL="https://www.google.com.hk/#newwindow=1&safe=strict&q=";
    searchEngineList.append(defaultEngineGoogleAddor);
    searchEngine defaultEngineYahooAddor;
    defaultEngineYahooAddor.name=tr("Yahoo!");
    defaultEngineYahooAddor.engineURL="http://search.yahoo.com/search?toggle=1&cop=mss&ei=UTF-8&fr=yfp-t-900&p=";
    searchEngineList.append(defaultEngineYahooAddor);
    searchEngine defaultEngineBingAddor;
    defaultEngineBingAddor.name=tr("Bing");
    defaultEngineBingAddor.engineURL="http://cn.bing.com/search?form=MXBTDF&pc=MXBR&q=";
    searchEngineList.append(defaultEngineBingAddor);
    searchEngine defaultEngineBaiduAddor;
    defaultEngineBaiduAddor.name=tr("Baidu");
    defaultEngineBaiduAddor.engineURL="http://www.baidu.com/s?wd=";
    searchEngineList.append(defaultEngineBaiduAddor);
    searchEngineIndex=0;
}

void KCGeneralConfigure::readConfigure()
{
    QSettings settings(getCfgFileName(), QSettings::IniFormat);
    settings.beginGroup("General");
    QString strDefaultLanguageMode=settings.value("DefaultLanguageMode",
                                   "cpp").toString();
    if(strDefaultLanguageMode=="c")
    {
        defaultLanguageMode=1;
    }
    else if(strDefaultLanguageMode=="cpp")
    {
        defaultLanguageMode=2;
    }
    else if(strDefaultLanguageMode=="pas")
    {
        defaultLanguageMode=3;
    }
    else
    {
        defaultLanguageMode=0;
    }
    rememberUnclosedFile=settings.value("RememberUnclosed",
                                        rememberUnclosedFile).toBool();
    useDefaultLanguageWhenOpen=settings.value("UseDefaultLanguageModeWhenOpen",
                               useDefaultLanguageWhenOpen).toBool();
    useDefaultLanguageWhenSave=settings.value("UseDefaultLanguageModeWhenSave",
                               useDefaultLanguageWhenSave).toBool();
    searchEngineIndex=settings.value("SearchEngineIndex",
                                     searchEngineIndex).toInt();
    settings.endGroup();
}

void KCGeneralConfigure::writeConfigure()
{
    QSettings settings(getCfgFileName(), QSettings::IniFormat);
    settings.beginGroup("General");
    settings.setValue("DefaultLanguageMode",getDefaultLanguageModeString());
    settings.setValue("RememberUnclosed", rememberUnclosedFile);
    settings.setValue("UseDefaultLanguageModeWhenOpen", useDefaultLanguageWhenOpen);
    settings.setValue("UseDefaultLanguageModeWhenSave", useDefaultLanguageWhenSave);
    settings.setValue("SearchEngineIndex", searchEngineIndex);
    settings.endGroup();
}

int KCGeneralConfigure::getDefaultLanguageMode() const
{
    return defaultLanguageMode;
}

QString KCGeneralConfigure::getDefaultLanguageModeString()
{
    switch(defaultLanguageMode)
    {
    case 1:
        return QString("c");
    case 2:
        return QString("cpp");
    case 3:
        return QString("pas");
    default:
        return QString("txt");
    }
}

void KCGeneralConfigure::setDefaultLanguageMode(const int &value)
{
    defaultLanguageMode = value;
}

bool KCGeneralConfigure::getRememberUnclosedFile() const
{
    return rememberUnclosedFile;
}

void KCGeneralConfigure::setRememberUnclosedFile(bool value)
{
    rememberUnclosedFile = value;
}

bool KCGeneralConfigure::getUseDefaultLanguageWhenSave() const
{
    return useDefaultLanguageWhenSave;
}

void KCGeneralConfigure::setUseDefaultLanguageWhenSave(bool value)
{
    useDefaultLanguageWhenSave = value;
}

bool KCGeneralConfigure::getUseDefaultLanguageWhenOpen() const
{
    return useDefaultLanguageWhenOpen;
}

void KCGeneralConfigure::setUseDefaultLanguageWhenOpen(bool value)
{
    useDefaultLanguageWhenOpen = value;
}

QString KCGeneralConfigure::getStrFileFilter() const
{
    return strFileFilter;
}

void KCGeneralConfigure::setStrFileFilter(const QString &value)
{
    strFileFilter = value;
}

QString KCGeneralConfigure::getAfFilter() const
{
    return afFilter;
}

void KCGeneralConfigure::setAfFilter(const QString &value)
{
    afFilter = value;
}

QString KCGeneralConfigure::getPasfFilter() const
{
    return pasfFilter;
}

void KCGeneralConfigure::setPasfFilter(const QString &value)
{
    pasfFilter = value;
}

QString KCGeneralConfigure::getCppfFilter() const
{
    return cppfFilter;
}

void KCGeneralConfigure::setCppfFilter(const QString &value)
{
    cppfFilter = value;
}

QString KCGeneralConfigure::getCfFilter() const
{
    return cfFilter;
}

void KCGeneralConfigure::setCfFilter(const QString &value)
{
    cfFilter = value;
}

QString KCGeneralConfigure::getHfFilter() const
{
    return hfFilter;
}

void KCGeneralConfigure::setHfFilter(const QString &value)
{
    hfFilter = value;
}

QString KCGeneralConfigure::getPtfFilter() const
{
    return ptfFilter;
}

void KCGeneralConfigure::setPtfFilter(const QString &value)
{
    ptfFilter = value;
}

QString KCGeneralConfigure::getAsfFilter() const
{
    return asfFilter;
}

void KCGeneralConfigure::setAsfFilter(const QString &value)
{
    asfFilter = value;
}

int KCGeneralConfigure::getSearchEngineIndex() const
{
    return searchEngineIndex;
}

void KCGeneralConfigure::setSearchEngineIndex(int value)
{
    searchEngineIndex = value;
}

QList<searchEngine> KCGeneralConfigure::getSearchEngineList() const
{
    return searchEngineList;
}

void KCGeneralConfigure::setSearchEngineList(const QList<searchEngine> &value)
{
    searchEngineList = value;
}
