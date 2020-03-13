
#include <QtGui/QtGui>
#include <QtCore>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "CMakeVars.hpp"
#include "MainWindow.hpp"
#include "Application.hpp"

#include <iostream>

int
main(int argc, char* argv[])
{
  Application app(argc, argv);

  // construct after constructing the app: allows db plugings to be loaded
  Database db;
  db.SetupDB();

  MainWindow mainWindow;
//  mainWindow.show();

  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("cpp_series", &mainWindow.getSeriesModel());
  engine.rootContext()->setContextProperty("cpp_model", &mainWindow);

  engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
  if (engine.rootObjects().isEmpty()){
      QList<QQmlError> warnings;
      engine.warnings(warnings);

      std::wcout << warnings.size() << " warnings" << std::endl;
      for (const auto& warning: warnings){
          std::wcout << warning.description().toStdWString() << std::endl;
      }

      return -1;
  }

  app.exec();

  return 0;
}
