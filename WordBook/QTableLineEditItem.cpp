/*
 * (C) Copyright 2013-2015 Gaoee. All rights reserved.
 *
 * @brief
 * @file
 * @version 1.0
 * @author  Leon
 * @note
 *      -Create by Leon<hityixiaoyang@gmail.com> on 2016-2-25
 */

#include "QTableLineEditItem.h"
#include <QTextOption>
#include <QEvent>

QTableLineEditItem::QTableLineEditItem(QObject *parent):
    QStyledItemDelegate(parent)
{
}

QWidget *QTableLineEditItem::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //if(index.isValid() && cols.contains(index.column()))
    if(true)
    {
        QLineEdit* editor = new QLineEdit(parent);
        return editor;
    }else{
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void QTableLineEditItem::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    //if(index.isValid() && cols.contains(index.column()))
    if(true)
    {
        QString str = index.model()->data(index, Qt::EditRole).toString();
        QLineEdit *meditor = static_cast<QLineEdit*>(editor);
        meditor->setText(str);
    }else{
        return QStyledItemDelegate::setEditorData(editor, index);
    }
}

void QTableLineEditItem::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    //if(index.isValid() && cols.contains(index.column()))
    if(true)
    {
        QLineEdit *meditor = static_cast<QLineEdit*>(editor);
        model->setData(index, meditor->text(), Qt::EditRole);
    }else{
        return QStyledItemDelegate::setModelData(editor,model,index);
    }
}

void QTableLineEditItem::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //if(index.isValid() && cols.contains(index.column()))
    if(true)
    {
        editor->setGeometry(option.rect);
    }else{
        return QStyledItemDelegate::updateEditorGeometry(editor,option,index);
    }
}
