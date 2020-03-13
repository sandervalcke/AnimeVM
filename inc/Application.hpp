#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include <QtGui>
#include <QMessageBox>
#include <QApplication>

#include "Exceptions.hpp"

/*!
 * Own application to get nice exception reports
 */
class Application: public QApplication {
   Q_OBJECT;

public:
   Application(int &c, char **v): QApplication(c, v) {}
   bool notify(QObject *rec, QEvent *ev) {
     try {
       return QApplication::notify(rec, ev);
     }
     catch (Exception& e) {
       QMessageBox::critical(0, qApp->tr("Application Error"),
			     QString("The application has encountered an unexpected error:\n\n")
			     + e.what().c_str()
			     );
     }
     catch (...) {
       QMessageBox::critical(0, qApp->tr("Application Error"),
			     QString("The application has encountered an unexpected error")
			     );
     }
     return false;
   }
};


#endif // __APPLICATION_HPP__
