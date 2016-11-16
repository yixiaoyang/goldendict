#ifndef WORDBOOK_H
#define WORDBOOK_H

#include <QString>
#include <QDateTime>
#include <QMap>
#include <QObject>

#define WB_TIME_STR ("yyyy-MM-dd hh:mm:ss")

enum{
    WBOOK_START_DEFAUT  = 1,
    WBOOK_START0 = 0,
    WBOOK_START1,
    WBOOK_START2,
    WBOOK_START3,
    WBOOK_START4,
    WBOOK_START5,

    WBOOk_SETID_DEFAULT = 0,
};

class WordBookItem{
public:
    WordBookItem(QString w, char s, QDateTime t,QString d="")
        :word(w),star(s),addedTime(t),defination(d){}
    QString serialization(){
        QString str(word);
        str += ",";
        str += QString::number(star);
        str += ",";
        str += addedTime.toString(WB_TIME_STR);
        str += "\n";
        return str;
    }

    QString word;
    char star;
    QDateTime addedTime;
    QString defination;
};

typedef QMap<QString, WordBookItem> WBookWordMap;

class WordBookSet{
public:
    WordBookSet(int _id, QString _name):id(_id),name(_name){}
    int id;
    QString name;
    WBookWordMap wlist;

    bool save();
    bool exportToFile(QString filename);
    bool importFrom(QString file);
    bool rename(QString nstr);
};

typedef QMap<int, WordBookSet> WBookMap;
typedef QMap<QString, int> WBookNameMap;

class WordBook : public QObject
{
    Q_OBJECT

private:
    // id : WordBookSet
    WBookMap wbook;
    // WordBookSet name: id
    WBookNameMap wbookNames;
    int defaultSetId;

    QString newWordlistName(QString name, int count);
signals:
    void sig_wordlist_removed(int);
    void sig_wordlist_new(int);
public:
    WordBook(QObject* parent=0);

    WBookMap* getWbookMap();

    bool appendToSet(QString word, char star, QString wordlistName,QString defination="");
    bool removeFromSet(QString word, QString wordlistName);
    bool search(QString word, char& star, QString &curName);
    bool search_in(QString word, QString wordListName, char& star);
    bool removeWordList(int id);
    int appendWordList(QString& name);
    int getWordListIdByName(QString wordlistName);

    QStringList getBookNameList();
    bool exportToFile();
    void save();
    bool wordlistRename(QString oldName, QString newName);
};

#endif // WORDBOOK_H
