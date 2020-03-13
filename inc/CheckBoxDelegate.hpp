#ifndef __CHECKBOXDELEGATE_HPP__
#define __CHECKBOXDELEGATE_HPP__

#include <QStyledItemDelegate>



class CheckBoxDelegate : public QStyledItemDelegate
{
public:
  CheckBoxDelegate(QObject *parent = 0);

  QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option
  	, const QModelIndex &index) const;
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model
  	, const QModelIndex &index) const;

  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


#endif // __CHECKBOXDELEGATE_HPP__
