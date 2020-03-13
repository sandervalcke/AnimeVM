
#include "DirectoryList.hpp"
#include "Database.hpp"
#include "Exceptions.hpp"


DirectoryList::DirectoryList(const QString& seriesName, QObject* parent)
  : QAbstractListModel(parent)
  , seriesID_()
  , directories_()
{
  directories_.setTable("directories");

  seriesID_ = Database::GetSeriesID(seriesName);
  SetSeries(seriesID_);
  
  connect(&directories_, SIGNAL(modelReset()), this, SIGNAL(modelReset()));
  connect(&directories_, SIGNAL(rowsInserted(const QModelIndex&, int, int))
	  , this, SIGNAL(rowsInserted(const QModelIndex&, int, int)));
  connect(&directories_, SIGNAL(rowsRemoved(const QModelIndex&, int, int))
	  , this, SIGNAL(rowsRemoved(const QModelIndex&, int, int)));
}

void
DirectoryList::SetSeries(const SeriesID& seriesID)
{
  seriesID_ = seriesID;
  directories_.setFilter(QString("seriesID='") + seriesID_.ToString()  + "'");
  directories_.select();  
}

int
DirectoryList::rowCount(const QModelIndex& parent) const
{
  return directories_.rowCount();
}

QVariant
DirectoryList::data(const QModelIndex& index, int role) const
{
  return directories_.data(directories_.index(index.row(), 2), role);
}

void
DirectoryList::AddDirectory(const Directory& directory)
{
  QSqlRecord record;
  record.append( createIntField("seriesID", seriesID_.ToInt()) );
  record.append( createStringField("name",  directory));

  // -1 : insert at the end
  if (! directories_.insertRecord(-1, record))
    throw Exception("Error adding new directory\n\n"
   		    + directories_.lastError().databaseText());  

  // if (!directories_.submitAll())
  //   throw Exception("Error adding new directory\n\n"
  // 		    + directories_.lastError().databaseText());  
}

void
DirectoryList::RemoveDirectory(const QModelIndex& index)
{
  directories_.removeRows(index.row(), 1, QModelIndex());
  directories_.submitAll();
}

void
DirectoryList::GetDirectoriesVector(std::vector<Directory>& list)
{
  list.clear();
  for (int i = 0, end = rowCount(); i < end; ++i)
    list.push_back(data(index(i,0)).toString());
}
