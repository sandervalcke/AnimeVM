
#include <cassert>

#include "SeriesSettings.hpp"
#include "Database.hpp"


SeriesSettings::SeriesSettings(DirectoryList* directoryList)
  : directoryList_(directoryList)
{
  ui_.setupUi(this);
  
  ui_.tableView_directories->setModel(directoryList_);
  //ui_.tableView_directories->resizeColumnsToContents();
  ui_.tableView_directories->horizontalHeader()->setStretchLastSection(true);

  ui_.toolButton_addDirectory->setIcon( QIcon::fromTheme("list-add") );
  ui_.toolButton_removeDirectory->setIcon( QIcon::fromTheme("list-remove") );

  ConnectSignalSlots();
}

void
SeriesSettings::ConnectSignalSlots()
{
  connect(ui_.toolButton_addDirectory, SIGNAL(clicked()), this, SLOT(AddDirectory()));
  connect(ui_.toolButton_removeDirectory, SIGNAL(clicked()), this, SLOT(RemoveDirectory()));
}

void
SeriesSettings::RemoveDirectory()
{
  assert(directoryList_);
  if (ui_.tableView_directories->currentIndex().row() < 0)
    return;
  directoryList_->RemoveDirectory(ui_.tableView_directories->currentIndex());
}

void 
SeriesSettings::AddDirectory()
{
  QString dir = QFileDialog::getExistingDirectory (this, "Select a top-level directory");
  
  if (dir.isEmpty())
    return;

  directoryList_->AddDirectory(dir);
}

// void
// SeriesSettings::SetName(const QString& name)
// {
//   series_   = name;
//   seriesID_ = Database::GetSeriesID(GetName());

//   directoryList_.SetSeries(seriesID_);

//   ui_.tableView_directories->setModel(&directoryList_);
//   ui_.tableView_directories->resizeColumnsToContents();
//   // ui_.tableView_directories->setColumnHidden(0, true);
//   // ui_.tableView_directories->setColumnHidden(1, true);

// //  ui_.label_name->setText(GetName());

// //  sessionList_.setFilter(QString("seriesID='") + QString::number(GetSeriesID())  + "'");
// //    directoryList_.setFilter(QString("seriesID='") + QString::number(GetSeriesID())  + "'");
// //
// //    if (! episodes_.get())
// //      throw Exception("Error: episodes is NULL");
// //
// //    Query query;
// //    query << "select lastSessionID from settings where seriesID='" << GetSeriesID() << "'";
// //    QSqlQuery res = query.Exec();
// //    if (! res.next()){
// //      // create settings entry
// //      query.Clear();
// //      query << "insert into settings (id, seriesID) values (null, '"
// //      << GetSeriesID() << "')";
// //      query.Exec();
// //
// //      // clear view list
// //      OnViewSessionSelected(QModelIndex());
// //    } else {
// //      SelectViewSession( res.value(0).toInt() );
// //    }
// //
// //    // populate episodes
// //    episodes_->Populate(GetDirectoryList(), true);
// //
// //    OnViewSessionSelected( GetSessionIndex() );
// //
// //    //sessionList_.SetTableData( data_.GetSeries(row)->GetSessions() );
// //    //episodeTable_.SetTableData( data_.GetSeries(row)->GetEpisodes() );
// //    //directoryList_.SetVectorData( data_.GetSeries(row)->GetDirectories() );
// //
// //    ui_.tableView_episodes->resizeColumnsToContents();
// //    ui_.tableView_sessions->resizeColumnsToContents();
// //    ui_.tableView_directories->resizeColumnsToContents();
// }
