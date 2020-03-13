
#include <algorithm>

#include "Data.hpp"
#include "Database.hpp"
#include "Exceptions.hpp"



QStringList
EpisodeList::GetIndexList() const
{
  QStringList list;
  for (auto it = episodes_.cbegin(); it != episodes_.cend(); ++it){
  	list << QString::number(it->GetIndex());

  }
  return list;
}

const Episode*
EpisodeList::GetEpisode(Episode::Index index) const
{
  for (auto it = episodes_.begin(); it != episodes_.end(); ++it)
    if (it->GetIndex() == index)
      return &(*it);

  throw Exception("Episode with index " + QString::number(index) + " not found");
}

Episode*
EpisodeList::GetEpisode(Episode::Index index)
{
  return const_cast<Episode*>(static_cast<const EpisodeList*>(this)->GetEpisode(index));
}

Episode::Index
EpisodeList::GetLastEpisodeIndex() const
{
  if (episodes_.size() == 0)
    throw Exception("No episodes found");

  return episodes_.back().GetIndex();
}

bool
EpisodeList::HasEpisode(Episode::Index index) const
{
  for (auto it = episodes_.cbegin(); it != episodes_.cend(); ++it)
    if (it->GetIndex() == index)
      return true;

  return false;
}

void
EpisodeList::SortByIndices()
{
  std::sort(episodes_.begin(), episodes_.end(), Episode::CompareIndices());
  //reset();
}

void
EpisodeList::Populate(const DirectoryList& dirs, bool recursive)
{
  beginResetModel();
  
  episodes_.clear();

  for (auto it = dirs.cbegin(); it != dirs.cend(); ++it){
  	PopulateEpisodes(*it, recursive);
  }

  // sort
  SortByIndices();

  // notify views
  endResetModel();
}

void
EpisodeList::PopulateEpisodes(const QString& directory, bool recursive)
{
  QDir dir(directory);
  QStringList files = dir.entryList(QDir::Files | QDir::Readable);

  QRegExp rx("(\\d{1,6})");

  for (auto it = files.constBegin(); it != files.constEnd(); ++it){
  	auto index = rx.indexIn(*it);
  	if(index >= 0 && rx.captureCount() == 1){
  		// found 1 and only one match in the filename
  		Episode ep(rx.cap(0).toInt(), *it, directory);
  		episodes_.push_back( ep );
  	}
  }

  //episodes.SortByColumn<int>(0);

  if (recursive){
  	QStringList dirs = dir.entryList(QDir::Dirs | QDir::Readable | QDir::NoDot | QDir::NoDotDot);
  	for (auto it = dirs.constBegin(); it != dirs.constEnd(); ++it){
  		// filePath: convert dirname to absolute path
  		PopulateEpisodes(dir.filePath(*it), recursive);
  	}
  }
}

int      
EpisodeList::rowCount ( const QModelIndex& parent ) const
{
  if (parent.isValid())
    return 0;

  return episodes_.size();
}

int      
EpisodeList::columnCount ( const QModelIndex& parent ) const
{
  if (parent.isValid())
    return 0;

  return 3;
}

QVariant 
EpisodeList::data ( const QModelIndex& index, int role ) const
{
  if (role == Qt::DisplayRole){
    switch (index.column()){
    case 0:
      return episodes_[index.row()].GetIndex();
    case 1:
      return episodes_[index.row()].GetFilename();
    case 2:
      return episodes_[index.row()].GetPath();
    default:
      return QVariant();
    };
  } else 
    return QVariant();
}

QVariant 
EpisodeList::headerData ( int section, Qt::Orientation orientation
		      , int role ) const
{
  if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
    return QVariant();

  switch(section){
  case 0:
    return "Episode";
  case 1:
    return "Filename";
  case 2:
    return "Path";
  default:
    return QVariant();
  }
}
