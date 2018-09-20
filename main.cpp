#include "mainwindow.h"
#include <QApplication>
#include "ftpdialog/ftpwindow.h"

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

//    return a.exec();

//    Q_INIT_RESOURCE(ftp);
#ifdef Q_OS_SYMBIAN
    // Change current directory from default private to c:\data
    // in order that user can access the downloaded content
    QDir::setCurrent( "c:\\data" );
#endif
    QApplication app(argc, argv);
    FtpWindow ftpWin;
#ifdef Q_OS_SYMBIAN
    // Make application better looking and more usable on small screen
    ftpWin.showMaximized();
#else
    ftpWin.show();
#endif
    return ftpWin.exec();
}
