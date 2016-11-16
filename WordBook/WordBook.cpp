#include "WordBook.h"
#include "config.hh"

#include <QFile>
#include <QDir>
#include <QTextStream>

#include "gddebug.hh"

static int _wordlist_id = 0;

WBookMap* WordBook::getWbookMap()
{
    return &wbook;
}

WordBook::WordBook(QObject *parent):
    QObject(parent)
{
    QFile file( Config::getWbookFileName());
    bool ret = false;
    if(file.exists()){
        ret = file.open( QFile::ReadOnly | QIODevice::Text );
    }else{
        ret = file.open( QFile::ReadWrite | QIODevice::Text | QIODevice::Truncate );
    }

    if(!ret){
        gdWarning("wordbook file open failed");
        return ;
    }else{
        int id = _wordlist_id++;
        QTextStream stream(&file);
        stream.setCodec("UTF-8");

        QString defaultSetIdStr = stream.readLine();
        if(defaultSetIdStr.isEmpty()){
            defaultSetId = WBOOk_SETID_DEFAULT;
        }else{
            defaultSetId = defaultSetIdStr.toInt();
        }

        while(!stream.atEnd()){
            QString line = stream.readLine();
            WBookMap::iterator it = wbook.insert(id,WordBookSet(id,line));

            wbookNames.insert(it.value().name,id);

            // TODO:import old words
            if(it != wbook.end()){
                it.value().importFrom(line);
            }

            id = _wordlist_id++;
        }
        file.close();

        if(wbook.find(defaultSetId) == wbook.end()){
            wbook.insert(defaultSetId,WordBookSet(defaultSetId,"default"));
            save();
        }
    }


}
bool WordBook::appendToSet(QString word, char star, QString wordlistName, QString defination){
    int setId = defaultSetId;
    if(!wordlistName.isEmpty()){
        int temp = getWordListIdByName(wordlistName);
        if(temp >= 0){
            setId = temp;
        }else{
            return false;
        }
    }

    WBookMap::iterator it = wbook.find(setId);
    if( it == wbook.end()){
        return false;
    }

    WBookWordMap::iterator wlistIt = it.value().wlist.find(word) ;
    if(wlistIt != it.value().wlist.end()){
        wlistIt.value().star = star;
        // TODO: optimizing it
        it.value().save();
        return true;
    }

    WordBookItem newItem(word,star,QDateTime::currentDateTime(),defination);
    it.value().wlist.insert(word,newItem);

    // TODO: append to file
    QString name = it.value().name;
    QFile file( Config::getWbookSetName(name));

    if ( !file.open( QFile::Append | QIODevice::Text | QIODevice::Truncate ) ){
        // no such file, create it
    }else{
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        stream << newItem.serialization();
    }

    file.close();
    return true;
}

bool WordBook::removeFromSet(QString word, QString wordlistName)
{
    int id = getWordListIdByName(wordlistName);
    if(id < 0){
        return false;
    }

    WBookMap::iterator it = wbook.find(id);
    if( it == wbook.end()){
        return false;
    }

    if(it.value().wlist.find(word) == it.value().wlist.end()){
        return false;
    }

    it.value().wlist.remove(word);

    // TODO: optimizing, remove from file
    it.value().save();

    return true;
}

bool WordBook::search(QString word, char &star, QString &curName)
{
    for(WBookMap::iterator it = wbook.begin(); it != wbook.end(); it++){
        WBookWordMap::iterator wlistIt = it.value().wlist.find(word);
        if(wlistIt != it.value().wlist.end()){
            star = wlistIt.value().star;
            curName = it.value().name;
            return true;
        }
    }
    return false;
}

bool WordBook::search_in(QString word, QString wordListName, char &star)
{
    int wotdListId = getWordListIdByName(wordListName);
    if(wotdListId < 0)
        return false;
    WBookMap::iterator it = wbook.find(wotdListId);
    if(it != wbook.end()){
        WBookWordMap::iterator wlistIt = it.value().wlist.find(word);
        if(wlistIt != it.value().wlist.end()){
            star = wlistIt.value().star;
            return true;
        }
    }
    return false;
}

bool WordBook::removeWordList(int id)
{
    WBookMap::iterator it = wbook.find(id);
    if( it == wbook.end()){
        return false;
    }

    if(wbook.size() ==1){
        return false;
    }

    it = wbook.erase(it);

    save();

    emit sig_wordlist_removed(id);
    return true;
}

int WordBook::appendWordList(QString &name)
{
    int id = _wordlist_id++;
    name = newWordlistName(tr("List"),0);
    WBookMap::iterator it = wbook.insert(id,WordBookSet(id,name));
    wbookNames.insert(it.value().name,id);

    save();

    emit sig_wordlist_new(id);
    return id;
}

int WordBook::getWordListIdByName(QString wordlistName)
{
    WBookNameMap::iterator namesIt  = wbookNames.find(wordlistName);
    if(namesIt == wbookNames.end())
        return -1;
    return namesIt.value();

}

QString WordBook::newWordlistName(QString name, int count)
{
    QString newName;
    if(count == 0){
        newName = name;
    }else{
        newName = name + QString("%1").arg(count);
    }
    if(wbookNames.find(newName) != wbookNames.end()){
        return newWordlistName(name,count+1);
    }else{
        return newName;
    }
    return newName;
}

QStringList WordBook::getBookNameList()
{
    QStringList list;
    for(WBookMap::iterator it = wbook.begin(); it != wbook.end(); it++){
        list << it.value().name;
    }
    return list;
}

void WordBook::save()
{
    QFile file( Config::getWbookFileName());

    if ( !file.open( QFile::ReadWrite | QIODevice::Text | QIODevice::Truncate ) ){
        // no such file, create it
    }else{
        QTextStream stream(&file);
        stream.setCodec("UTF-8");

        stream << defaultSetId << "\n";
        for(WBookMap::iterator it = wbook.begin(); it != wbook.end(); it++){
            QString str(it.value().name);
            str += "\n";
            stream << str;
        }
    }

    file.close();
}

bool WordBook::wordlistRename(QString oldName, QString newName)
{
    WBookNameMap::iterator namesIt  = wbookNames.find(oldName);
    if(namesIt != wbookNames.end()){
        if(wbookNames.find(newName) != wbookNames.end()){
            return false;
        }

        WBookMap::iterator it = wbook.find(namesIt.value());
        if(it != wbook.end()){
            it.value().rename(newName);
            wbookNames.erase(namesIt);
            wbookNames.insert(newName,it.value().id);
            return true;
        }
    }
    return false;
}

bool WordBookSet::save()
{
    QFile file( Config::getWbookSetName(name));

    if ( !file.open( QFile::ReadWrite | QIODevice::Text | QIODevice::Truncate ) ){
        return false;
    }else{
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        for(WBookWordMap::iterator it =  wlist.begin(); it != wlist.end(); it++){
            stream << it.value().serialization();
        }
    }

    file.close();
    return true;
}

bool WordBookSet::exportToFile(QString filename)
{
    QFile file( filename );

    if ( !file.open( QFile::ReadWrite | QIODevice::Text | QIODevice::Truncate ) ){
        return false;
    }else{
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        //stream.setGenerateByteOrderMark(false);
        for(WBookWordMap::iterator it =  wlist.begin(); it != wlist.end(); it++){
            stream << it.value().word.toUtf8()  << "\r\n";
        }
    }

    file.close();
    return true;
}

bool WordBookSet::importFrom(QString filename)
{
    QFile file( Config::getWbookSetName( filename ));

    if ( !file.open( QFile::ReadOnly | QIODevice::Text | QIODevice::Truncate ) ){
        gdWarning("wordset file open failed");
        return false;
    }else{
        QTextStream stream(&file);
        QDateTime time;
        QString line;
        QStringList list;

        stream.setCodec("UTF-8");
        //stream.setGenerateByteOrderMark(false);
        while(!stream.atEnd()){
            line = stream.readLine();
            list = line.split(',');
            if(list.size() >= 3){
                // word,star,time
                QString word = list[0];
                char star = list[1].toInt();
                time = QDateTime::fromString(list[2],WB_TIME_STR);
                wlist.insert(word,WordBookItem(word,star,time));
            }
        }
        gdDebug(QString("import %1 words from wordbook %2").arg(wlist.size()).arg("name").toStdString().c_str());
    }
    return true;
}

bool WordBookSet::rename(QString nstr)
{
    QFile file( Config::getWbookSetName(name));

    if ( file.exists() ){
        file.rename(Config::getWbookSetName(nstr));
    }

    file.close();
    this->name = nstr;
    return true;
}
