#include "Application.hpp"

#include "Exceptions.hpp"

#include <QtGui>
#include <QMessageBox>


Application::Application(int &c, char **v)
    : QApplication(c, v)
{
}

bool Application::notify(QObject *rec, QEvent *ev)
{
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
