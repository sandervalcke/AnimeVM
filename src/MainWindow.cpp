
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
  : QMainWindow()
  , ui_()
  , seriesMap_()
  , seriesList_()
{
  ui_.setupUi(this);

  seriesList_.setTable("series");
  seriesList_.select();
  ui_.tableView_series->setModel(&seriesList_);

  ui_.toolButton_seriesSettings->setIcon( QIcon::fromTheme("emblem-system") );
  ui_.toolButton_refreshEpisodeList->setIcon( QIcon::fromTheme("view-refresh") );
  ui_.toolButton_addSeries->setIcon( QIcon::fromTheme("list-add") );
  ui_.toolButton_removeSeries->setIcon( QIcon::fromTheme("list-remove") );
  ui_.toolButton_addViewSession->setIcon( QIcon::fromTheme("list-add") );
  ui_.toolButton_removeViewSession->setIcon( QIcon::fromTheme("list-remove") );

  connect(ui_.tableView_series, SIGNAL(clicked(const QModelIndex&)),
	  this, SLOT(OnSeriesSelected(const QModelIndex&)) );

  connect(ui_.toolButton_refreshEpisodeList, SIGNAL(clicked()), 
	  this, SLOT(RefreshEpisodesList()));
  connect(ui_.toolButton_seriesSettings, SIGNAL(clicked()),
	  this, SLOT(ShowSeriesSettings()));
  connect(ui_.toolButton_addSeries, SIGNAL(clicked()),
	  this, SLOT(AddSeries()));
  connect(ui_.toolButton_removeSeries, SIGNAL(clicked()),
	  this, SLOT(RemoveSeries()));
  connect(ui_.toolButton_addViewSession, SIGNAL(clicked()),
	  this, SLOT(AddViewSession()));
  connect(ui_.toolButton_removeViewSession, SIGNAL(clicked()),
	  this, SLOT(RemoveViewSession()));

  // actions
  connect(ui_.actionAdd_Series, SIGNAL(triggered()),
	  this, SLOT(AddSeries()));
  connect(ui_.actionAdd_View_Session, SIGNAL(triggered()),
	  this, SLOT(AddViewSession()));
  connect(ui_.actionView_Episode, SIGNAL(triggered()),
	  this, SLOT(ViewEpisode()));
  connect(ui_.actionView_Next_Episode, SIGNAL(triggered()),
	  this, SLOT(ViewNextEpisode()));
  connect(ui_.action_Mark_Next_Episode_Viewed, SIGNAL(triggered()),
	  this, SLOT(MarkNextEpisodeViewed()));

  // set row heights
  const int rowHeight = 20;
  ui_.tableView_history->verticalHeader()->setDefaultSectionSize(rowHeight);
  ui_.tableView_episodes->verticalHeader()->setDefaultSectionSize(rowHeight);

  // Adjust the headers
  ui_.tableView_sessions->horizontalHeader()->hide();
  ui_.tableView_series->horizontalHeader()->hide();

  ui_.tableView_series->verticalHeader()->hide();
  ui_.tableView_sessions->verticalHeader()->hide();
  ui_.tableView_history->verticalHeader()->hide();
  ui_.tableView_episodes->verticalHeader()->hide();

  // hide some columns
  ui_.tableView_series->setColumnHidden(0, true);

  // prevent some tables from being edited
  ui_.tableView_sessions->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui_.tableView_episodes->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui_.tableView_series->setEditTriggers(QAbstractItemView::NoEditTriggers);

  //
  ui_.tableView_series->horizontalHeader()->setStretchLastSection(true);
  ui_.tableView_sessions->horizontalHeader()->setStretchLastSection(true);
  ui_.tableView_episodes->horizontalHeader()->setStretchLastSection(true);
  ui_.tableView_history->horizontalHeader()->setStretchLastSection(true);

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

    activeSeries_ = seriesMap_[name].get();

    activeSeries_->RegisterEpisodeView(ui_.tableView_episodes);
    activeSeries_->RegisterHistoryView(ui_.tableView_history);
    activeSeries_->RegisterSessionsView(ui_.tableView_sessions);

    // hide some columns
    ui_.tableView_sessions->setColumnHidden(0, true);
    ui_.tableView_sessions->setColumnHidden(1, true);
    ui_.tableView_history->setColumnHidden(0, true);
    ui_.tableView_history->setColumnHidden(1, true);

    // select last view session (if applicable)
    ViewSessionID lastSessionID = Database::GetLastViewSessionID(activeSeries_->GetID());
    if (lastSessionID.IsValid()){
        SelectViewSession(activeSeries_->GetViewSessionIndex(lastSessionID));
    }

    viewSessionsChanged();
    historyChanged();
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
MainWindow::AddSeries()
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

  // create settings entry
  Query query;
  query << "insert into settings (id, seriesID) values (null, '"
	<< GetSeriesID() << "')";
  query.Exec();
}

bool
MainWindow::CheckSingleSelectedSeries()
{
  int row = GetSeriesRow();
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
    return GetSeriesRow();
  else
      return -1;
}

QString MainWindow::GetSelectedSeriesName() const
{
    return seriesList_.record(GetSeriesRow()).value("name").toString();
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
MainWindow::AddViewSession()
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

  if (ret == QMessageBox::Yes){
    series->RemoveViewSession( ui_.tableView_sessions->currentIndex().row() );
  }
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

  series->ViewEpisode(number,
      series->GetSessionID(ui_.tableView_sessions->currentIndex()) );
}

void
MainWindow::MarkNextEpisodeViewed()
{
  if (! CheckSingleViewSessionSelected())
    return;

  Series* series = GetActiveSeries();
  series->MarkNextEpisodeViewed(
      series->GetSessionID(ui_.tableView_sessions->currentIndex()) );
}

bool
MainWindow::CheckSingleViewSessionSelected()
{
  if (! ui_.tableView_sessions->currentIndex().isValid()){
    WarnSelectViewSession();
    return false;
  }
  else
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
      series->GetSessionID(ui_.tableView_sessions->currentIndex()) );
}

void
MainWindow::SelectViewSession(ViewSessionIndex id)
{
  ui_.tableView_sessions->selectRow(id-1);
  //not needed for some reason
  //OnViewSessionSelected( GetSessionIndex() );
}

void
MainWindow::SelectSeries(SeriesID id)
{
  ui_.tableView_series->selectRow(id.ToInt());
  OnSeriesSelected( GetSeriesIndex() );
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
