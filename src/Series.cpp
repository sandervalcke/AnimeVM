
#include <QInputDialog>

#include "Series.hpp"
#include "Database.hpp"
#include "SeriesSettings.hpp"


Series::Series(const QString& name)
  : directoryList_(name, this)
  , episodes_(new EpisodeList())
{
  SetupTables();
  // set a name that should never occur :p
  //SetName("___UNNAMED___12345678987654321___ooooo___aabbccddeeff");
  SetName(name);

  RefreshEpisodes();
}

void
Series::SetupTables()
{
  // directoryList_.setTable("directories");
  // directoryList_.setEditStrategy(QSqlTableModel::OnManualSubmit);
  // directoryList_.select();

  sessionList_.setTable("viewsessions");
  sessionList_.setEditStrategy(QSqlTableModel::OnManualSubmit);
  sessionList_.select();

  history_.setTable("views");
  history_.setEditStrategy(QSqlTableModel::OnManualSubmit);
  history_.select();

  // set table names
  history_.setHeaderData(2, Qt::Horizontal, tr("Episode"));
  history_.setHeaderData(3, Qt::Horizontal, tr("Date"));

}

void
Series::SetName(const QString& name)
{
  name_ = name;
  id_   = Database::GetSeriesID(name_);
  qDebug() << "name: " << name;
  qDebug() << QString("seriesID='") + GetID().ToString()  + "'";
  sessionList_.setFilter(QString("seriesID='") + GetID().ToString()  + "'");
  //directoryList_.setFilter(QString("seriesID='") + GetID().ToString()  + "'");
  LoadLastViewSession();
}

void
Series::LoadLastViewSession()
{
  ViewSessionID session = Database::GetLastViewSessionID(GetID());
  SelectViewSession(session);
}

void
Series::OnViewSessionSelected(int index)
{
    activeViewSession_ = index;
    OnViewSessionSelected( GetSessionID(index) );
    activeViewSessionChanged();
}

void
Series::OnViewSessionSelected(const ViewSessionID& sessionID)
{
  // invalid id -> filter should make the history empty
  qDebug() << QString("sessionID='") + sessionID.ToString()  + "'";
  history_.setFilter(QString("sessionID='") + sessionID.ToString()  + "'");

  // store this session
  Query query;
  query << "update settings set lastSessionID='" << sessionID.ToString() << "'"
      << " where seriesID='" << GetID().ToString() << "'";
  query.Exec();
}

ViewSessionIndex
Series::GetViewSessionIndex(const ViewSessionID& id)
{
  for (size_t i = 0, end = sessionList_.rowCount(); i < end; ++i){
    if (ViewSessionID(sessionList_.record(i).value("id").toInt()) == id)
      return i+1;
  }

  throw Exception("Did not find index for view session");
}

void
Series::SelectViewSession(int index)
{
  OnViewSessionSelected(index);
}

void
Series::SelectViewSession(ViewSessionID sessionID)
{
  OnViewSessionSelected(sessionID);
}

void
Series::AddViewSession()
{
  // id: auto-set (primary key, auto-incremented)
  QSqlRecord record;
  record.append( createIntField("seriesID", id_.ToInt()) );
  record.append( createStringField("date",  Database::GetDateTime()) );

  // -1 : insert at the end
  sessionList_.insertRecord(-1, record);

  if (!sessionList_.submitAll())
    throw Exception("Error adding new view session\n\n"
        + sessionList_.lastError().databaseText());

  // select the new view session
  SelectViewSession( sessionList_.index(sessionList_.rowCount()-1, 0).row() );
}

Episode::Index
Series::GetLastViewedEpisode()
{
  if (history_.rowCount() == 0)
    return -1;
  return history_.record(history_.rowCount()-1).value("episodeID").toInt();
}

void
Series::ViewEpisode(Episode::Index index, ViewSessionID sessionID)
{
  if (index < 0)
    return;

  if (! sessionID.IsValid())
    throw Exception(tr("Invalid view session when viewing episode %0").arg(index));

  // sane number?
  Episode::Index last = GetLastViewedEpisode();
  if (index <= last)
    throw Exception("Cannot view episode number " + QString::number(index)
		    + tr(" in the current session (%0),\n because it has already been viewed.\n").arg(sessionID.ToInt())
		    + " Last episode index: "
		    + QString::number(last)
		    + "\n\n"
		    "To view this episode: either start a new session or start it manually.");
  
  MarkEpisodeViewed(index, sessionID);

  QString program = "vlc";
  QStringList arguments;
  arguments << episodes_->GetEpisode(index)->GetFullName();
  // arguments << "-style" << "motif";

  QProcess *myProcess = new QProcess(this);
  myProcess->start(program, arguments);
}

void
Series::MarkEpisodeViewed(Episode::Index index, const ViewSessionID& sessionID)
{
  QSqlRecord record;
  record.append( createIntField("sessionID", sessionID.ToInt()) );
  record.append( createIntField("episodeID", index) );
  record.append( createStringField("date",   Database::GetDateTime()) );

  // -1 : insert at the end
  history_.insertRecord(-1, record);

  if (!history_.submitAll())
    throw Exception("Error adding history entry\n\n"
        + history_.lastError().databaseText());  
}

void
Series::ViewNextEpisode(ViewSessionID sessionID)
{
  if (! sessionID.IsValid())
    throw Exception("Invalid view session when viewing next episode");

  if (GetLastViewedEpisode() == -1)
    throw Exception(tr("Cannot view the next episode (session ID: '%0') as no episodes were viewed yet")
		    .arg(sessionID.ToInt()));

  ViewEpisode(GetLastViewedEpisode() + 1, sessionID);
}


void
Series::MarkNextEpisodeViewed(ViewSessionID sessionID)
{
  if (! sessionID.IsValid())
    throw Exception("Invalid view session when viewing next episode");

  if (GetLastViewedEpisode() == -1)
    throw Exception(tr("Cannot view the next episode (session ID: '%0') as no episodes were viewed yet")
		    .arg(sessionID.ToInt()));

  MarkEpisodeViewed(GetLastViewedEpisode() + 1, sessionID);
}

QStringList
Series::GetEpisodeNumberList()
{
  return episodes_->GetIndexList();
}

ViewSessionID
Series::GetSessionID(int index)
{
  if (index < 0)
    throw Exception(tr("Trying to get view session ID based on invalid index (row: %0)").arg(index));

  return ViewSessionID( sessionList_.record(index).value("id").toInt() );
}

void
Series::RegisterEpisodeView(QAbstractItemView* view)
{
  view->setModel(&*episodes_);
}

void
Series::RegisterSessionsView(QAbstractItemView* view)
{
  view->setModel(&sessionList_);
  QObject::connect(view, SIGNAL(clicked(const QModelIndex&))
      , this, SLOT(OnViewSessionSelected(const QModelIndex&)));
}

void
Series::RegisterHistoryView(QAbstractItemView* view)
{
  view->setModel(&history_);
}

// void
// Series::RegisterDirectoriesView(QAbstractItemView* view)
// {
//   view->setModel(&directoryList_);
// }

void
Series::OpenSettingsDialog()
{
  SeriesSettings dialog(&directoryList_);
  if (dialog.exec())
    return;
}

void
Series::RefreshEpisodes()
{
  if (! episodes_)
    throw Exception("Found an invalid episodes pointer");

  std::vector<DirectoryList::Directory> list;
  directoryList_.GetDirectoriesVector(list);
  episodes_->Populate(list, true);
}

void
Series::RemoveViewSession(int row)
{
  qDebug() << "removing: " << row;
  sessionList_.removeRows(row, 1, QModelIndex());
  sessionList_.submitAll();
}
