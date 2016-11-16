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

#ifndef QTABLESPINBOXITEM_H
#define QTABLESPINBOXITEM_H

#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QSet>

class QTableLineEditItem : public QStyledItemDelegate
{
    Q_OBJECT

public:
    QTableLineEditItem(QObject *parent);

    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    //bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
    void closeIndexClicked(const QModelIndex &);
};

#endif // QTABLESPINBOXITEM_H
