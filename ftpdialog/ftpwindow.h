/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef __FTPWINDOW_H__
#define __FTPWINDOW_H__

#include <QDialog>
#include <QHash>
#include <QLabel>
#include <QFtp>
#include <QLineEdit>
#include <QPushButton>
#include <QHeaderView>
#include <QTreeWidget>
#include <QDialogButtonBox>
#include <QProgressDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFile>
#include <QNetworkSession>
#include <QFileDialog>
#include <QTextEdit>
#include <QVector>
#include <QSemaphore>
#include <QEventLoop>
#include <QTimer>
#include "wdir.h"

class QtWait:public QObject
{
    Q_OBJECT
private:
    int sem;
public:
    int val;
public:
    QtWait()
    {
        sem = 0;
    }
    void qt_waite(int t = 100)
    {
        while(sem == 0)
        {
            QEventLoop eventloop;

            QTimer::singleShot(t, &eventloop, SLOT(quit()));
            eventloop.exec();
        }
        sem = 0;
    }
    void send_sem(int v = 0)
    {
        sem = 1;
        val = v;
    }
};

class ZQTree :public QObject
{
    Q_OBJECT

public:
    ZQTree(QObject *parent = 0):QObject(parent)
    {
        ;
    }

    ~ZQTree()
    {
        url.clear();
    }
public:
    QVector<QUrlInfo> url;
public slots:
    void addtolist(const QUrlInfo &urlInfo);
};


class FtpWindow : public QDialog
{
    Q_OBJECT

public:
    FtpWindow(QWidget *parent = 0);
    QSize sizeHint() const;

private slots:
    void connectOrDisconnect();
    void downloadFile();

    void uploadFile();

    void cancelDownload();

    void ftpCommandFinished(int commandId, bool error);
    void addToList(const QUrlInfo &urlInfo);
    void processItem(QTreeWidgetItem *item, int column);
    void cdToParent();
    void updateDataTransferProgress(qint64 readBytes,
                                    qint64 totalBytes);
    void enableDownloadButton();
    void enableConnectButton();
    void enableUploadButton();
    void popMenu(const QPoint& point);
    void ftpdel(void);
    void ftpnw(void);


private:
    QLabel *ftpServerLabel;
    QLineEdit *ftpServerLineEdit;
    QLabel *statusLabel;
    QTreeWidget *fileList;
    QPushButton *cdToParentButton;
    QPushButton *connectButton;
    QPushButton *downloadButton;
    QPushButton *uploadButton;
    QPushButton *quitButton;
    QDialogButtonBox *buttonBox;
    QProgressDialog *progressDialog;

    QLabel       * ftpsite;

    WDir           * wfile;

    QLabel       * wsite;
    QTreeView *   wtreeView;

    QFileSystemModel * model;

//![1]
    QHash<QString, bool> isDirectory;
    QString currentPath;
    QFtp *ftp;
    QFile *file;

    QNetworkSession *networkSession;
    QTextEdit *textEdit;
    QtWait       ftpsem;
    int       listmark;

//![1]
public:
    void Open_File();
    void zdownloadFile(QString savefilePath, QString filePath, QString fileName, int type);
    void zuploadFile(QString localfilePath,  QString ftpfilePath, QString fileName, int type);
    void zrmFile(QString filePath, QString fileName, int type);
    QString get_ftp_file_path(void);
    QString get_local_file_path(void);
    void ListLocalFile(QString path);

    void re_list(void);
};

#endif
