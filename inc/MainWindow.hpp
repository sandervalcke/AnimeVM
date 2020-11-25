#pragma once

#include <QtGui>
#include <map>

#include "Data.hpp"
#include "Database.hpp"
#include "Exceptions.hpp"
#include "Series.hpp"


class MainWindow : public QWidget
{
Q_OBJECT

Q_PROPERTY(QObject* series READ getSeries NOTIFY seriesChanged)
Q_PROPERTY(QObject* viewSessionModel READ getViewSessions NOTIFY viewSessionsChanged)
Q_PROPERTY(QObject* history READ getHistory NOTIFY historyChanged)
Q_PROPERTY(int      currentRow READ getCurrentRow NOTIFY seriesChanged)

signals:
    void viewSessionsChanged();
    void historyChanged();
    void seriesChanged();

public:
  MainWindow();

  int         GetValidSeriesRow();
  QString     GetSelectedSeriesName() const;
  QString     GetSelectedSeriesName(const QModelIndex& index) const;
  int         getCurrentRow() const;

  QObject*    getSeries(){ return activeSeries_; }
  QObject*    getViewSessions();
  QObject*    getHistory();

  QSqlTableModel& getSeriesModel(){ return seriesList_; }

  //int        GetSessionRow() { return GetSessionIndex().row(); }
  //ViewSessionID GetSessionID() { return ViewSessionID(sessionList_.record(GetSessionRow()).value("id").toInt()); }
  //QModelIndex GetSessionIndex() { return ui_.tableView_sessions->currentIndex(); }
  
  //Episode::Index GetLastViewedEpisode() const;

public slots:
  void OnSeriesSelected(const QModelIndex& index);

  void addSeries();
  void RemoveSeries();
  void addViewSession();
  void RemoveViewSession();
  void ViewEpisode();
  void ViewNextEpisode();
  void MarkNextEpisodeViewed();

  void ShowSeriesSettings();
  void RefreshEpisodesList();


private:
  //EpisodeList::DirectoryList GetDirectoryList() const;
        Series* GetActiveSeries();
  const Series* GetActiveSeries() const;
  bool CheckSingleSelectedSeries();
  bool CheckSingleViewSessionSelected();

  void WarnSelectViewSession();

private:
  std::map<QString, Series::Ptr> seriesMap_;

  int     activeSeriesIndex_ = -1;
  Series* activeSeries_ = {};

  QSqlTableModel seriesList_;
};
