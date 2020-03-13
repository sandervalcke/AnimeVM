#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__

#include <QtSql>
#include <sstream>

#include "Data.hpp"
#include "IDClasses.hpp"


class Query
{
public:
  Query() : query_("") {}

  template<typename T>
  Query& operator<<(const T& rhs)
  {
    std::ostringstream oss;
    oss << rhs;
    query_ += oss.str();
    return *this;
  }

  void      Clear(){ query_ = ""; }
  QSqlQuery Exec();
  static QSqlQuery Exec(std::string query);

private:
  std::string query_;
};

template<> Query& Query::operator<< <QString>(const QString& rhs);


class Database
{
public:
  Database() : dbIsSetup_(false) {}

  void SetupDB();

  //
  static SeriesID    GetSeriesID(const QString& name);
  static QString     GetDateTime(){ return QDateTime::currentDateTime().toString(); }
  static ViewSessionID GetLastViewSessionID(const SeriesID& id);

private:
  QSqlDatabase db;
  bool         dbIsSetup_;
};


QSqlField createIntField(const QString& name, int value);
QSqlField createStringField(const QString& name, const QString& value);


#endif // __DATABASE_HPP__
