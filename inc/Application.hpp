#pragma once

#include <QApplication>


/*!
 * Own application to get nice exception reports
 */
class Application: public QApplication
{
public:
   Application(int &c, char **v);
   bool notify(QObject *rec, QEvent *ev);
};
