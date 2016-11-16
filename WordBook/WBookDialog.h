#ifndef WBOOKDIALOG_H
#define WBOOKDIALOG_H

#include <QDialog>
#include "WordBook.h"

#include <QMap>
#include <QTreeWidget>
#include <QTableWidget>
#include <QMenu>
#include <QVector>

namespace Ui {
class WBookDialog;
}

class WBookDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WBookDialog(QWidget *parent = 0);
    ~WBookDialog();

    enum{
        TREE_ROOT_BOOKS,
        TREE_ROOT_MAX
    };
    enum{
        TREE_COL_NAME,
        TREE_COL_COUNT,
        TREE_COL_MAX
    };
    enum{
        TABLE_COL_WORD,
        TABLE_COL_STAR,
        TABLE_COL_TIME,
        TABLE_COL_MAX
    };


    void setWbook(WordBook *value);
    void updateBook();
private slots:
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

    void on_action_Remove_triggered();

    void on_action_New_triggered();

    void on_action_export_triggered();

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_treeWidget_itemChanged(QTreeWidgetItem *item, int column);

    void on_tableWidget_wordlist_customContextMenuRequested(const QPoint &pos);
    void on_tableWidget_wordlist_cellClicked(int row, int column);

    void on_action_RemoveWord_triggered();

    void on_action_Star1_triggered();

    void on_action_Star2_triggered();

    void on_action_Star3_triggered();

    void on_action_Star4_triggered();

    void on_action_Star5_triggered();


signals:
    void sig_wordlist_changed();
    void sig_scanpopup_show(QString word);
private:
    Ui::WBookDialog *ui;

    WordBook* wbook;
    // book id : tree item row
    QMap<int,int> idRowMap;
    //QMap<int,int> rowIdMap;
    QList<int> rowIdList;
    QString curWord;

    QMenu* treeWidgetSelectedMenu;
    QMenu* treeWidgetEmptyMenu;
    QMenu* tableItemMenu;

    QTreeWidgetItem* lastSelectTreeSetItem;
    QTableWidgetItem* lastSelectTableItem;

    WordBookSet *getSelectedWordList(QTreeWidgetItem* item=NULL);
    void starSelectedTableItemTo(char star);
};

#endif // WBOOKDIALOG_H
