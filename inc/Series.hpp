#ifndef __SERIES_HPP__
#define __SERIES_HPP__

#include <tr1/memory>

#include <QSqlTableModel>

#include "Data.hpp"
#include "Database.hpp"
#include "Exceptions.hpp"
#include "IDClasses.hpp"
#include "DirectoryList.hpp"


class Series : public QObject
{
  Q_OBJECT

Q_PROPERTY(int activeViewSession READ getActiveViewSession NOTIFY activeViewSessionChanged)

signals:
    void activeViewSessionChanged();

public slots:
    void OnViewSessionSelected(int index);

public:
  typedef std::shared_ptr<Series> Ptr;

public:
  Series(const QString& name);

  //
  void ViewEpisode(Episode::Index index, ViewSessionID sessionID);
  void ViewNextEpisode(ViewSessionID sessionID);
  void MarkNextEpisodeViewed(ViewSessionID sessionID);

  // getters - setters
  QString  GetName() const { return name_; }
  void     SetName(const QString& name);
  SeriesID GetID() const { return id_; }
  int      getActiveViewSession() const { return activeViewSession_; }

  //
  ViewSessionIndex GetViewSessionIndex(const ViewSessionID& id);
  void           SelectViewSession(ViewSessionID sessionID);
  void           AddViewSession();
  void           RemoveViewSession(int row);
  //void           AddDirectory(const QString& dir);
  Episode::Index GetLastViewedEpisode();
  QStringList    GetEpisodeNumberList();
  int            GetCurrentHistoryCount() const { return history_.rowCount(); }

  ViewSessionID  GetSessionID(int index);

  QSqlTableModel& getSessions(){ return sessionList_; }
  QSqlTableModel& getHistory() { return history_; }

  //
  void OpenSettingsDialog();
  void RefreshEpisodes();

  //
  void RegisterEpisodeView(QAbstractItemView* view);
  void RegisterSessionsView(QAbstractItemView* view);
  void RegisterHistoryView(QAbstractItemView* view);
  //void RegisterDirectoriesView(QAbstractItemView* view);

private:
  void SelectViewSession(int index);
  void MarkEpisodeViewed(Episode::Index index, const ViewSessionID& sessionID);

private slots:
  void OnViewSessionSelected(const ViewSessionID& sessionID);

private:
  void SetupTables();
  void LoadLastViewSession();

private:
  SeriesID id_;
  QString  name_;
  int      activeViewSession_ = -1;

  //QSqlTableModel directoryList_;
  DirectoryList  directoryList_;
  QSqlTableModel sessionList_;
  QSqlTableModel history_;  

  EpisodeList::Ptr episodes_;
};




#endif // __SERIES_HPP__
