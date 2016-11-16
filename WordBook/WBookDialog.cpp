#include "WBookDialog.h"
#include "ui_WBookDialog.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QLineEdit>

#include "WordBook/QTableLineEditItem.h"
#include "articleview.hh"

WBookDialog::WBookDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WBookDialog)
{
    ui->setupUi(this);

    treeWidgetSelectedMenu = new QMenu(this);
    treeWidgetEmptyMenu = new QMenu(this);
    tableItemMenu = new QMenu(this);
    treeWidgetSelectedMenu->addAction(ui->action_Remove);
    treeWidgetSelectedMenu->addAction(ui->action_New);
    treeWidgetSelectedMenu->addAction(ui->action_export);
    tableItemMenu->addAction(ui->action_RemoveWord);
    tableItemMenu->addAction(ui->action_Star1);
    tableItemMenu->addAction(ui->action_Star2);
    tableItemMenu->addAction(ui->action_Star3);
    tableItemMenu->addAction(ui->action_Star4);
    tableItemMenu->addAction(ui->action_Star5);

    treeWidgetEmptyMenu->addAction(ui->action_New);
    lastSelectTreeSetItem = NULL;
    lastSelectTableItem = NULL;

    // init the view style
    QHeaderView* hHeader = ui->tableWidget_wordlist->horizontalHeader();
#if QT_VERSION < 0x050000
    hHeader->setResizeMode(TABLE_COL_TIME, QHeaderView::Stretch);
#else
    hHeader->setSectionResizeMode(TABLE_COL_TIME, QHeaderView::Stretch);
#endif
    hHeader->show();
    ui->treeWidget->setItemDelegateForColumn(TREE_COL_NAME,  new QTableLineEditItem(ui->treeWidget));

}

WBookDialog::~WBookDialog()
{
    delete ui;
    this->deleteLater();
}

void WBookDialog::setWbook(WordBook *value)
{
    wbook = value;
    idRowMap.clear();
    updateBook();
}

void WBookDialog::updateBook()
{
    if(wbook){
        QTreeWidgetItem* root = ui->treeWidget->topLevelItem(TREE_ROOT_BOOKS);
        WBookMap* map = wbook->getWbookMap();

        if(!root){
            ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(QStringList(QString::fromUtf8("Word Book")) << "") );
            root = ui->treeWidget->topLevelItem(TREE_ROOT_BOOKS);
            if(!root){
                return ;
            }
        }

        int row = 0;
        for(WBookMap::iterator it = map->begin(); it != map->end(); it++){
            if(root->childCount() > row){
                QTreeWidgetItem* item = root->child(row);
                if(!item){
                    continue;
                }
                item->setText(TREE_COL_NAME,it.value().name);
                item->setText(TREE_COL_COUNT, QString::number(it.value().wlist.size()));
            }else{
                root->addChild(new QTreeWidgetItem(QStringList(it.value().name) << QString::number(it.value().wlist.size())));
            }
            idRowMap.insert(it.key(),row);
            rowIdList.append(it.key());

            row++;
        }
    }

    ui->treeWidget->expandAll();
}

void WBookDialog::on_treeWidget_itemClicked(QTreeWidgetItem *item, int )
{
    lastSelectTreeSetItem = item;

    WordBookSet* wordlist = getSelectedWordList(item);
    if(wordlist){
        int tabRow = 0;

        ui->tableWidget_wordlist->clearContents();
        //ui->tableWidget_wordlist->setHorizontalHeaderLabels(QStringList(tr("word")) << tr("defination") << tr("star") << tr("time"));
        for(WBookWordMap::iterator wordIt = wordlist->wlist.begin(); wordIt != wordlist->wlist.end(); wordIt++){
            if(tabRow >= ui->tableWidget_wordlist->rowCount()){
                ui->tableWidget_wordlist->insertRow(tabRow);
            }

            ui->tableWidget_wordlist->setItem(tabRow,TABLE_COL_WORD,new QTableWidgetItem(wordIt.value().word));
            ui->tableWidget_wordlist->setItem(tabRow,TABLE_COL_STAR,new QTableWidgetItem(QString::number(wordIt.value().star)));
            ui->tableWidget_wordlist->setItem(tabRow,TABLE_COL_TIME,new QTableWidgetItem(wordIt.value().addedTime.toString(WB_TIME_STR)));
            tabRow++;
        }
        for(int idx = tabRow; idx > ui->tableWidget_wordlist->rowCount(); idx++){
            ui->tableWidget_wordlist->removeRow(idx);
        }
    }
}

void WBookDialog::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem* curItem = ui->treeWidget->itemAt(pos);
    if(!curItem){
        // 空白处点击，显示测试集操作菜单
        treeWidgetEmptyMenu->exec(QCursor::pos());
        lastSelectTreeSetItem = NULL;
        return ;
    }else{
        QTreeWidgetItem* root = ui->treeWidget->topLevelItem(TREE_ROOT_BOOKS);
        if((!root) || (!wbook) || (curItem->parent() != root)){
            treeWidgetEmptyMenu->exec(QCursor::pos());
            lastSelectTreeSetItem = NULL;
            return ;
        }

        lastSelectTreeSetItem = curItem;
        treeWidgetSelectedMenu->exec(QCursor::pos());
    }
}

void WBookDialog::on_action_Remove_triggered()
{
    if(lastSelectTreeSetItem){
        QTreeWidgetItem* root = ui->treeWidget->topLevelItem(TREE_ROOT_BOOKS);
        if((!root) || (!wbook) || (lastSelectTreeSetItem->parent() != root)){
            return ;
        }

        int row = root->indexOfChild(lastSelectTreeSetItem);
        if(row >= rowIdList.size())
            return ;

        QList<int>::iterator it = rowIdList.begin()+row;
        if(it == rowIdList.end()){
            return ;
        }

        int id = *it;
        WBookMap* map =wbook->getWbookMap();
        WBookMap::iterator wbIt = map->find(id);
        if(wbIt != map->end()){
            switch(QMessageBox::warning(this,"Warning",
                                        tr("Do you want to remove word list %1").arg(wbIt.value().name),
                                        QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel))
            {
            case QMessageBox::Ok:
                if(wbook->removeWordList(id)){
                    QMap<int,int>::iterator idRowIt;

                    root->removeChild(lastSelectTreeSetItem);
                    idRowMap.remove(id);
                    it = rowIdList.erase(it);

                    // update the row
                    for(int newRow = row; it != rowIdList.end(); it++,newRow++){
                        idRowIt = idRowMap.find(*it);
                        if(idRowIt != idRowMap.end()){
                            idRowIt.value() = newRow;
                        }
                    }
                }
                break;
            case QMessageBox::Cancel:
                break;
            default:
                break;
            }
        }
    }
}

void WBookDialog::on_action_New_triggered()
{
    QTreeWidgetItem* root = ui->treeWidget->topLevelItem(TREE_ROOT_BOOKS);
    if((!root) || (!wbook)){
        return ;
    }
    QString name;
    int row = root->childCount();
    int id = wbook->appendWordList(name);
    root->addChild(new QTreeWidgetItem(QStringList(name) << "0"));
    idRowMap.insert(id,row);
    rowIdList.append(id);
    emit sig_wordlist_changed();
}

void WBookDialog::on_action_export_triggered()
{
    WordBookSet* wordlist = getSelectedWordList();
    if(wordlist)
    {
        QString fileName = QFileDialog::getSaveFileName(this,
                            tr("Export to File"), "", tr("txt Files (*.txt);;"));
        if (!fileName.isEmpty()){
            if(wordlist->exportToFile(fileName)){
                QMessageBox::information(this,"Export Succeed",tr("Export word list to %1 succeed").arg(fileName));
            }else{
                QMessageBox::warning(this,"Export Failed",tr("Export word list to %1 Failed").arg(fileName));
            }
        }
    }
}

WordBookSet* WBookDialog::getSelectedWordList(QTreeWidgetItem *item){
    if(!item){
        item = lastSelectTreeSetItem;
    }
    if(item){
        QTreeWidgetItem* root = ui->treeWidget->topLevelItem(TREE_ROOT_BOOKS);
        if((!root) || (!wbook) || (item->parent() != root)){
            goto failed;
        }

        int row = root->indexOfChild(item);
        if(row >= rowIdList.size())
            goto failed;

        QList<int>::iterator it = rowIdList.begin()+row;
        if(it == rowIdList.end()){
            goto failed;
        }

        int id = *it;
        WBookMap* map =wbook->getWbookMap();
        WBookMap::iterator wbIt = map->find(id);
        if(wbIt != map->end()){
            return &wbIt.value();
        }
    }

failed:
    return NULL;
}

void WBookDialog::starSelectedTableItemTo(char star)
{
    if(lastSelectTableItem){
        int row = ui->tableWidget_wordlist->row(lastSelectTableItem);
        QTableWidgetItem* item = ui->tableWidget_wordlist->item(row,TABLE_COL_WORD);
        if(item){
            QString word = item->text();
            WordBookSet* wordlist = getSelectedWordList();
            if(wordlist){
                if(wbook->appendToSet(word,star,wordlist->name)){
                    ui->tableWidget_wordlist->setItem(row,TABLE_COL_STAR, new QTableWidgetItem(QString::number(star)));
                }
            }
        }
    }
}

void WBookDialog::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(column != TREE_COL_NAME)
        return ;
    Qt::ItemFlags tmp = item->flags();
    item->setFlags(tmp | Qt::ItemIsEditable);
}

void WBookDialog::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    if(column != TREE_COL_NAME)
        return ;
    WordBookSet* wordlist = getSelectedWordList(item);
    if(wordlist){
        QString newName = item->text(column);
        if(newName != wordlist->name){
            if(wbook->wordlistRename(wordlist->name,newName)){
                wbook->save();
                emit sig_wordlist_changed();
            }
        }
    }
}

void WBookDialog::on_tableWidget_wordlist_customContextMenuRequested(const QPoint &pos)
{
    QTableWidgetItem* item = ui->tableWidget_wordlist->itemAt(pos);
    if(item){
        QPoint p = ui->tableWidget_wordlist->mapToGlobal(pos);
        lastSelectTableItem = item;
        tableItemMenu->exec(p);
    }
}

void WBookDialog::on_action_RemoveWord_triggered()
{
    if(lastSelectTableItem){
        int row = ui->tableWidget_wordlist->row(lastSelectTableItem);
        QTableWidgetItem* item = ui->tableWidget_wordlist->item(row,TABLE_COL_WORD);
        if(item){
            QString word = item->text();
            WordBookSet* wordlist = getSelectedWordList();
            if(wordlist){
                if(wbook->removeFromSet(word,wordlist->name)){
                    ui->tableWidget_wordlist->removeRow(row);
                    emit sig_wordlist_changed();
                }
            }
        }
    }

}

void WBookDialog::on_action_Star1_triggered()
{
    starSelectedTableItemTo(WBOOK_START1);
}

void WBookDialog::on_action_Star2_triggered()
{
   starSelectedTableItemTo(WBOOK_START2);
}

void WBookDialog::on_action_Star3_triggered()
{
   starSelectedTableItemTo(WBOOK_START3);
}

void WBookDialog::on_action_Star4_triggered()
{
   starSelectedTableItemTo(WBOOK_START4);
}

void WBookDialog::on_action_Star5_triggered()
{
   starSelectedTableItemTo(WBOOK_START5);
}

void WBookDialog::on_tableWidget_wordlist_cellClicked(int row, int column)
{
    if(column != 0)
        return ;

    QTableWidgetItem* item = ui->tableWidget_wordlist->item(row,column);
    if(item){
        curWord = item->text();
        emit sig_scanpopup_show(curWord);
        //ui->defineation->showDefinition( curWord, 0, 0 );
    }
}
