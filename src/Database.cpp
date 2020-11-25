
#include <QtGui>
#include <QDebug>

#include "Database.hpp"
#include "Exceptions.hpp"
#include "CMakeVars.hpp"


// specialization for QString
template<>
Query& Query::operator<< <QString>(const QString& rhs)
{
  std::ostringstream oss;
  oss << rhs.toStdString();
  query_ += oss.str();
  return *this;
}

QSqlQuery
Query::Exec(std::string query)
{
  Query myQuery;
  myQuery << query;
  return myQuery.Exec();
}

QSqlQuery
Query::Exec()
{
  qDebug() << query_.c_str();

  QSqlQuery sqlquery;

  sqlquery.exec(query_.c_str());
  if (sqlquery.lastError().type() != QSqlError::NoError){
      qCritical() << qApp->tr("SQL Error\n")
                  << "Query:\n"
                     + sqlquery.lastQuery()
                     + "\n\n"
                       "Error:\n"
                     + sqlquery.lastError().databaseText();
  }

  return sqlquery;
}

SeriesID
Database::GetSeriesID(const QString& name)
{
  Query myQuery;
  myQuery << "select id from series where name='" << name << "'";
  QSqlQuery query = myQuery.Exec();
  if (! query.next()){
    // return an invalid index
    return SeriesID();
  }

  return SeriesID(query.value(0).toInt());
}

ViewSessionID
Database::GetLastViewSessionID(const SeriesID& id)
{
  Query query;
  query << "select lastSessionID from settings where seriesID='" << id.ToInt() << "'";
  QSqlQuery res = query.Exec();
  if (! res.next()){
    // create settings entry
    query.Clear();
    query << "insert into settings (id, seriesID) values (null, '"
        << id.ToInt() << "')";
    query.Exec();

    // return invalid id
    return ViewSessionID();
  } else {
    return ViewSessionID( res.value(0).toInt() );
  }
}

void
Database::SetupDB()
{
  if (dbIsSetup_)
    return;

  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName( QString("db/ViewManager.db") );

  if (! db.open()){
    qCritical()
            << qApp->tr("Cannot open database db/ViewManager.db\n")
            << qApp->tr("Unable to establish a database connection.\n"
				 "This program needs SQLite support."
				 " Please read the Qt SQL driver documentation "
				 "for information how to build it.\n\n"
                 "Click Cancel to exit.");
    exit(-1);
  }
  else
    qDebug() << "Successfully created database connection";
          
  QStringList tables = db.tables();
  Query myQuery;
  
  if (! tables.contains("series")){
    myQuery.Clear();
    myQuery << "create table series "
		    "(id integer primary key autoincrement, "
		    "name varchar(40),"
        "test bool default true"
                    ")";
    myQuery.Exec();
  }

  if (! tables.contains("directories") ){
        myQuery.Clear();
	myQuery << ("create table directories "
		    "(id integer primary key autoincrement, "
		    "seriesID int, "
		    "name varchar(120)"
		    ")"
		    );
	myQuery.Exec();
  }

  if (! tables.contains("viewsessions") ){
        myQuery.Clear();
	myQuery << ("create table viewsessions "
		    "(id integer primary key autoincrement, "
		    "seriesID int, "
		    "date varchar(40)"
		    ")"
		    );
	myQuery.Exec();
  }

  if (! tables.contains("views") ){
        myQuery.Clear();
	myQuery << ("create table views "
		    "(id integer primary key autoincrement, "
		    "sessionID int, "
		    "episodeID int, "
		    "date varchar(40)"
		    ")"
		    );
	myQuery.Exec();
  }

  if (! tables.contains("settings") ){
    Query::Exec("create table settings "
		    "(id integer primary key autoincrement, "
		    "seriesID  int, "
		    "lastSessionID int default 0"
		    ")"
		    );
  }

  dbIsSetup_ = true;
}


QSqlField createIntField(const QString& name, int value)
{
  QSqlField field(name, QVariant::Int);
  field.setValue(value);
  return field;
}

QSqlField createStringField(const QString& name, const QString& value)
{
  QSqlField field(name, QVariant::String);
  field.setValue(value);
  return field;
}
