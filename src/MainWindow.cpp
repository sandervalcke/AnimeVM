
#include <iostream>
#include <cstdlib>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>

#include "MainWindow.hpp"

// testing
#include "CheckBoxDelegate.hpp"
#include "SeriesSettings.hpp"

using namespace std;


MainWindow::MainWindow()
  : seriesMap_()
  , seriesList_()
{

  seriesList_.setTable("series");
  seriesList_.select();

  // default selection
  //SelectSeries(0);
}

void
MainWindow::OnSeriesSelected(const QModelIndex& index)
{
    QString name = GetSelectedSeriesName(index);
    auto it = seriesMap_.find(name);

    if (it == seriesMap_.end())
        seriesMap_[name] = Series::Ptr(new Series(name));

    activeSeriesIndex_ = index.row();
    activeSeries_ = seriesMap_[name].get();

    // select last view session (if applicable)
    ViewSessionID lastSessionID = Database::GetLastViewSessionID(activeSeries_->GetID());
    if (lastSessionID.IsValid()){
        activeSeries_->SelectViewSession(lastSessionID);
    }

    emit seriesChanged();
    emit viewSessionsChanged();
    emit historyChanged();
}

//EpisodeList::DirectoryList
//MainWindow::GetDirectoryList() const
//{
//  EpisodeList::DirectoryList list;
//  const int nrows = directoryList_.rowCount();
//  for (int i = 0; i < nrows; ++i)
//    list.push_back(directoryList_.record(i).value("name").toString());
//  return list;
//}

void
MainWindow::addSeries()
{
  QString name = QInputDialog::getText(
    this, tr("Series name"),
    tr("Series name:"));

  if (name.isEmpty())
    return;

  QSqlRecord record;
  record.append( createStringField("name", name));

  // -1 : insert at the end
  if (! seriesList_.insertRecord(-1, record))
    throw Exception("Error adding new series\n\n"
		    + seriesList_.lastError().databaseText());  

  seriesList_.submitAll();

  qWarning() << "Still need to enter into settings, read the seriesID we just wrote";
  // create settings entry
//  Query query;
//  query << "insert into settings (id, seriesID) values (null, '"
//	<< GetSeriesID() << "')";
//  query.Exec();
}

bool
MainWindow::CheckSingleSelectedSeries()
{
  int row = getCurrentRow();
  if (row < 0){
    QMessageBox::information(this, tr("Notification"), tr("Please select a series."));
  }
  return row >= 0;
}

/**
   \return a valid series row. Displays a message box if no valid series is selected, returns
   -1 then.
*/
int
MainWindow::GetValidSeriesRow()
{
  if (CheckSingleSelectedSeries())
    return getCurrentRow();
  else
      return -1;
}

QString MainWindow::GetSelectedSeriesName() const
{
    return seriesList_.record(getCurrentRow()).value("name").toString();
}

QString MainWindow::GetSelectedSeriesName(const QModelIndex &index) const
{
    return seriesList_.record(index.row()).value("name").toString();
}

QObject *MainWindow::getViewSessions()
{
    if (activeSeries_){
        return &activeSeries_->getSessions();
    } else {
        return {};
    }
}

QObject *MainWindow::getHistory()
{
    if (activeSeries_){
        return &activeSeries_->getHistory();
    } else {
        return {};
    }
}

void
MainWindow::RemoveSeries()
{
  int row = GetValidSeriesRow();
  if (row < 0) return;  

  QString name = GetSelectedSeriesName();
  
  QMessageBox msgBox;
  msgBox.setText(tr("Removing Series %0").arg(name));
  msgBox.setInformativeText(tr("Do you really want to remove the series '%0'? This action is irreversible!").arg(name));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  int ret = msgBox.exec();

  if (ret == QMessageBox::Yes){
    seriesList_.removeRows(row, 1, QModelIndex());

    // returns false if removing when having added in the same session (for some reason...)
    seriesList_.submitAll();
  }
}

Series*
MainWindow::GetActiveSeries()
{
  return const_cast<Series*>(static_cast<const MainWindow*>(this)->GetActiveSeries());
}

const Series*
MainWindow::GetActiveSeries() const
{
    if (! activeSeries_){
        throw Exception(tr("No active series found?"));
    }

    return activeSeries_;
}

void
MainWindow::addViewSession()
{  
  if (! CheckSingleSelectedSeries())
    return;

  GetActiveSeries()->AddViewSession();
}

void
MainWindow::RemoveViewSession()
{
  if (! CheckSingleSelectedSeries())
    return;
  
  Series* series = GetActiveSeries();
  if (! series)
    throw Exception("Error finding series");

  if (! CheckSingleViewSessionSelected())
    return;
  
  QMessageBox msgBox;
  msgBox.setText(tr("Removing View Session"));
  msgBox.setInformativeText(tr("Do you really want to remove the selected view session (%0 entries)? This action is irreversible!").arg(series->GetCurrentHistoryCount()));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  int ret = msgBox.exec();
}

//Episode::Index
//MainWindow::GetLastViewedEpisode() const
//{
//  if (history_.rowCount() == 0)
//    return -1;
//
//  return history_.record(history_.rowCount()-1).value("episodeID").toInt();
//}

void
MainWindow::ViewEpisode()
{
  Series* series = GetActiveSeries();

  bool ok = true;
  QString numberString = QInputDialog::getItem(
					       this, tr("Episode number"),
					       tr("Episode number:"),
					       series->GetEpisodeNumberList()
					       , 0, true, &ok
					       );
  if (! ok)
    return;
  int number = numberString.toInt();

  if (! CheckSingleViewSessionSelected())
    return;

//  series->ViewEpisode(number,
//      series->GetSessionID(ui_.tableView_sessions->currentIndex()) );
}

void
MainWindow::MarkNextEpisodeViewed()
{
  if (! CheckSingleViewSessionSelected())
    return;

  Series* series = GetActiveSeries();
  series->MarkNextEpisodeViewed(
      series->GetSessionID( series->getActiveViewSession() ));
}

bool
MainWindow::CheckSingleViewSessionSelected()
{
  if (activeSeries_&& activeSeries_->getActiveViewSession() < 0){
      WarnSelectViewSession();
      return false;
  }

  return true;
}

void
MainWindow::WarnSelectViewSession()
{
  QMessageBox::information(this, tr("Notification"), tr("Please select a view session."));
}

void
MainWindow::ViewNextEpisode()
{
  if (! CheckSingleViewSessionSelected())
    return;

  Series* series = GetActiveSeries();
  series->ViewNextEpisode(
      series->GetSessionID(series->getActiveViewSession()) );
}

void
MainWindow::ShowSeriesSettings()
{
  if (! CheckSingleSelectedSeries())
    return;

  GetActiveSeries()->OpenSettingsDialog();
}

void
MainWindow::RefreshEpisodesList()
{
  GetActiveSeries()->RefreshEpisodes();
}

int MainWindow::getCurrentRow() const
{
    return activeSeriesIndex_;
}
