#-------------------------------------------------
#
# Project created by QtCreator 2018-09-13T14:59:45
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ftp
TEMPLATE = app



linux-g++{
          CONFIG(debug, debug|release) {
               QBUILD = $$PWD/bin/debug/x86
          }
          CONFIG(release, debug|release) {
                QBUILD =$$PWD/bin/release/x86
               DEFINES += QT_NO_DEBUG_OUTPUT
           }
           #库使�?
           DEFINES += I386
           DEFINES+=SUPPORT_SYSCOMM
           #应用程序使用
           DEFINES += PC_PLATFORM
           DEFINES+=FTPCONNECTION
}

linux-arm-gnueabi-g++{
           CONFIG(debug, debug|release) {
               QBUILD =  $$PWD/bin/debug/arm
           }
           CONFIG(release, debug|release) {
                QBUILD = $$PWD/bin/release/arm
                DEFINES += QT_NO_DEBUG_OUTPUT
            }
           #库使�?
           DEFINES += ARM_PLATFORM
          # DEFINES += TASHIZHUJI
           DEFINES += ARM
           DEFINES+=SUPPORT_SYSCOMM
           #应用程序使用
           #DEFINES+=FTPCONNECTION
}
win32-g++{
           CONFIG(debug, debug|release) {
                QBUILD =  $$PWD/bin/debug/win
                LIBS  += C:/Qt/Qt5.4.1/5.4/mingw491_32/bin/Qt5Ftpd.dll
            }
           CONFIG(release, debug|release) {
                QBUILD =  $$PWD/bin/release/win
                DEFINES += QT_NO_DEBUG_OUTPUT
                LIBS  += C:/Qt/Qt5.4.1/5.4/mingw491_32/bin/Qt5Ftp.dll
            }
           #库使�?
            DEFINES += I386
            DEFINES += SUPPORT_SYSCOMM
           #应用程序使用
            DEFINES += PC_PLATFORM
            DEFINES += OS_WINDOWS
            DEFINES += FTPCONNECTION
}

linux-g++-64{
           CONFIG(debug, debug|release) {
               QBUILD = $$PWD/bin/debug/x64
           }
           CONFIG(release, debug|release) {
                QBUILD = $$PWD/bin/release/arm
                DEFINES += QT_NO_DEBUG_OUTPUT
           }
          #库使�?
           DEFINES += I386
           DEFINES+=SUPPORT_SYSCOMM
          #应用程序使用
           DEFINES += PC_PLATFORM
           DEFINES+=FTPCONNECTION
}

SOURCES += main.cpp\
        mainwindow.cpp \
    ftpdialog/ftpwindow.cpp \
    ftpdialog/wdir.cpp

HEADERS  += mainwindow.h \
    ftpdialog/ftpwindow.h \
    ftpdialog/wdir.h

FORMS    += mainwindow.ui

RESOURCES += \
    image.qrc
