#ifndef __DIRECTORYLIST_HPP__
#define __DIRECTORYLIST_HPP__

#include <QAbstractListModel>
#include <QSqlTableModel>
#include <vector>

#include "IDClasses.hpp"


class DirectoryList : public QAbstractListModel
{
Q_OBJECT

public:
  typedef QString Directory;

public:
  DirectoryList(const QString& seriesName, QObject* parent = 0);
  
  void SetSeries(const SeriesID& seriesID);
  void GetDirectoriesVector(std::vector<Directory>& list);

  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

public slots:
  void AddDirectory(const Directory& directory);
  void RemoveDirectory(const QModelIndex& index);

private:
  SeriesID       seriesID_;
  QSqlTableModel directories_;
};




#endif // __DIRECTORYLIST_HPP__
