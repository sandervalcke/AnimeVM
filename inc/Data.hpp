#ifndef __DATA_HPP__
#define __DATA_HPP__

#include <QtCore>

#include <vector>
#include <string>
#include <map>
#include <tr1/memory>

//#include "Model.hpp"

typedef int ViewSessionIndex;




class Episode
{
public:
  struct CompareIndices
  {
    bool operator()(const Episode ep1, const Episode& ep2) const
    { return ep1.GetIndex() < ep2.GetIndex(); }
  };

public:
  typedef int Index;

public:
  Episode(Index iindex, const QString& ifilename, const QString& ipath)
    : myIndex_(iindex), filename_(ifilename), path_(ipath) {}

  Index   GetIndex() const { return myIndex_; }
  QString GetFullName() const { return QFileInfo(path_ + "/" + filename_).absoluteFilePath(); }
  QString GetFilename() const { return filename_; }
  QString GetPath() const { return path_; }

  // implementing pure virtuals
//  int      rowCount ( const QModelIndex& parent = QModelIndex() ) const;
//  int      columnCount ( const QModelIndex& parent = QModelIndex() ) const;
//  QVariant data ( const QModelIndex& index, int role = Qt::DisplayRole ) const;
//  QVariant headerData ( int section, Qt::Orientation orientation
//			, int role = Qt::DisplayRole ) const;

private:
  Index   myIndex_;
  QString filename_;
  QString path_;
};

class EpisodeList : public QAbstractTableModel
{
public:
  typedef std::vector<Episode> List;
  typedef std::vector<QString> DirectoryList;
  typedef std::shared_ptr<EpisodeList> Ptr;

public:
  EpisodeList() {}

  int      rowCount ( const QModelIndex& parent = QModelIndex() ) const;
  int      columnCount ( const QModelIndex& parent = QModelIndex() ) const;
  QVariant data ( const QModelIndex& index, int role = Qt::DisplayRole ) const;
  QVariant headerData ( int section, Qt::Orientation orientation
			, int role = Qt::DisplayRole ) const;

  Episode*       GetEpisode(Episode::Index index);
  const Episode* GetEpisode(Episode::Index index) const;
  void           Populate(const DirectoryList& dirs, bool recursive=true);
  QStringList    GetIndexList() const;
  bool           HasEpisode(Episode::Index index) const;
  Episode::Index GetLastEpisodeIndex() const;
  void           SortByIndices();

private:
  void PopulateEpisodes(const QString& directory, bool recursive);

private:
  List    episodes_;
};


#endif // __DATA_HPP__
