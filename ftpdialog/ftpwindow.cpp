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

#include <QtGui>
#include <QtNetwork>
#include <QAction>
#include <QMenu>
#include "ftpwindow.h"

void ZQTree::addtolist(const QUrlInfo &urlInfo)
{
    url.append(urlInfo);
}

FtpWindow::FtpWindow(QWidget *parent)
    : QDialog(parent), ftp(0), networkSession(0)
{
    ftpServerLabel = new QLabel(tr("Ftp &server:"));
    ftpServerLineEdit = new QLineEdit("ftp://root:root@192.168.1.112");
    ftpServerLabel->setBuddy(ftpServerLineEdit);

    statusLabel = new QLabel(tr("Please enter the name of an FTP server."));
#ifdef Q_OS_SYMBIAN
    // Use word wrapping to fit the text on screen
    statusLabel->setWordWrap( true );
#endif

    ftpsite = new QLabel(tr("远程站点:"));
    fileList = new QTreeWidget;
    fileList->setEnabled(false);
    fileList->setRootIsDecorated(false);
    fileList->setHeaderLabels(QStringList() << tr("Name") << tr("Size") << tr("Owner") << tr("Group") << tr("Time"));
    fileList->header()->setStretchLastSection(false);

    wsite = new QLabel(tr("本地站点:"));

    model = new QFileSystemModel;
    model->setReadOnly(false);
    model->setRootPath(QDir::currentPath());

    wtreeView = new QTreeView;
    wtreeView->setModel(model);

    wtreeView->header()->setStretchLastSection(true);
    wtreeView->header()->setSortIndicator(0, Qt::AscendingOrder);
    wtreeView->header()->setSortIndicatorShown(true);
//    treeView->header()->setClickable(true);

    QModelIndex index = model->index(QDir::currentPath());
    wtreeView->expand(index);
    wtreeView->scrollTo(index);
    wtreeView->resizeColumnToContents(0);


    connectButton = new QPushButton(tr("Connect"));
    connectButton->setDefault(true);

    cdToParentButton = new QPushButton;
    cdToParentButton->setIcon(QPixmap(":/images/cdtoparent.png"));
    cdToParentButton->setEnabled(false);

    downloadButton = new QPushButton(tr("Download"));
    downloadButton->setEnabled(false);
//    downloadButton->setEnabled(true);

    uploadButton = new QPushButton(tr("Upload"));
    uploadButton->setEnabled(false);
//    downloadButton->setEnabled(true);

    quitButton = new QPushButton(tr("Quit"));

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(downloadButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(uploadButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    progressDialog = new QProgressDialog(this);

    fileList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(fileList,SIGNAL(customContextMenuRequested(const QPoint&)),
            this,SLOT(popMenu(const QPoint&)));//检测鼠标右键

    connect(fileList, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
            this, SLOT(processItem(QTreeWidgetItem*,int)));
//    connect(fileList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
//            this, SLOT(enableDownloadButton()));
    connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
    connect(connectButton, SIGNAL(clicked()), this, SLOT(connectOrDisconnect()));
    connect(cdToParentButton, SIGNAL(clicked()), this, SLOT(cdToParent()));
    connect(downloadButton, SIGNAL(clicked()), this, SLOT(downloadFile()));
    connect(uploadButton, SIGNAL(clicked()), this, SLOT(uploadFile()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));



    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(ftpServerLabel);
    topLayout->addWidget(ftpServerLineEdit);
#ifndef Q_OS_SYMBIAN
    topLayout->addWidget(cdToParentButton);
    topLayout->addWidget(connectButton);
#else
    // Make app better lookin on small screen
    QHBoxLayout *topLayout2 = new QHBoxLayout;
    topLayout2->addWidget(cdToParentButton);
    topLayout2->addWidget(connectButton);
#endif

     QVBoxLayout *ftpsiteLayout = new QVBoxLayout;
     ftpsiteLayout->addWidget(ftpsite);
     ftpsiteLayout->addWidget(fileList);

     QVBoxLayout *wsiteLayout = new QVBoxLayout;
     wsiteLayout->addWidget(wsite);
     wsiteLayout->addWidget(wtreeView);

    QHBoxLayout *dirLayout = new QHBoxLayout;
    dirLayout->addLayout(wsiteLayout);
    dirLayout->addLayout(ftpsiteLayout);


    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);



#ifdef Q_OS_SYMBIAN
    // Make app better lookin on small screen
    mainLayout->addLayout(topLayout2);
#endif

    mainLayout->addLayout(dirLayout);
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, SIGNAL(opened()), this, SLOT(enableConnectButton()));

        connectButton->setEnabled(false);
        statusLabel->setText(tr("Opening network session."));
        networkSession->open();
    }

    setWindowTitle(tr("FTP"));

    listmark = 0;

}

QSize FtpWindow::sizeHint() const
{
    return QSize(1000, 600);
}

//![0]
void FtpWindow::connectOrDisconnect()
{
    if (ftp) {
        ftp->abort();
        ftp->deleteLater();
        ftp = 0;
//![0]
        fileList->setEnabled(false);
        cdToParentButton->setEnabled(false);
        downloadButton->setEnabled(false);
        connectButton->setEnabled(true);
        connectButton->setText(tr("Connect"));
#ifndef QT_NO_CURSOR
        setCursor(Qt::ArrowCursor);
#endif
        statusLabel->setText(tr("Please enter the name of an FTP server."));
        return;
    }

#ifndef QT_NO_CURSOR
    setCursor(Qt::WaitCursor);
#endif

    ftp = new QFtp(this);
    connect(ftp, SIGNAL(commandFinished(int,bool)),
            this, SLOT(ftpCommandFinished(int,bool)));
    connect(ftp, SIGNAL(listInfo(QUrlInfo)),
            this, SLOT(addToList(QUrlInfo)));
    connect(ftp, SIGNAL(dataTransferProgress(qint64,qint64)),
            this, SLOT(updateDataTransferProgress(qint64,qint64)));

    fileList->clear();
    currentPath.clear();
    isDirectory.clear();
    currentPath = "/opt";

    QUrl url(ftpServerLineEdit->text());
    if (!url.isValid() || url.scheme().toLower() != QLatin1String("ftp")) {
        ftp->connectToHost(ftpServerLineEdit->text(), 21);
        ftp->login();
    } else {
        ftp->connectToHost(url.host(), url.port(21));

        if (!url.userName().isEmpty())
            ftp->login(QUrl::fromPercentEncoding(url.userName().toLatin1()), url.password());
        else
            ftp->login();

        ftp->setTransferMode(QFtp::Active);
        if (!url.path().isEmpty())
        {
            ftp->cd(url.path());
            ftpsem.qt_waite(10);
        }
    }

    fileList->setEnabled(true);
    connectButton->setEnabled(false);
    connectButton->setText(tr("Disconnect"));
    statusLabel->setText(tr("Connecting to FTP server %1...")
                         .arg(ftpServerLineEdit->text()));

}

QString  FtpWindow::get_ftp_file_path(void)
{
    QString fileName = fileList->currentItem()->text(0);
    if(isDirectory.value(fileName))
    {
        return currentPath+"/"+fileName;
    }
    return currentPath;

}

QString FtpWindow::get_local_file_path(void)
{
    QModelIndex index = wtreeView->currentIndex();
    if( !index.isValid() )
    {
       qDebug("error index!");
        return ".";
    }

    QFileInfo fileif = model->fileInfo(index);
    if(fileif.isDir())
    {
        return fileif.filePath();
    }
    else
    {
        return (fileif.path());
    }

}

void FtpWindow::re_list(void)
{
    fileList->clear();
    isDirectory.clear();
    ftp->list();
}


void FtpWindow::zrmFile(QString filePath, QString fileName, int type)
{

   if(type)                                 //目录
   {
       qDebug("Is dir!");

       listmark = 1;

       ZQTree * midtree = new ZQTree();

       connect(ftp, SIGNAL(listInfo(QUrlInfo)),
               midtree, SLOT(addtolist(QUrlInfo)));
       qDebug() << fileName;

       filePath = filePath +"/"+fileName;

       qDebug() << "linux path " << filePath;
       ftp->cd(filePath);
       ftpsem.qt_waite(10);
       ftp->list();
       ftpsem.qt_waite(100);

       qDebug() << "list over!!!!!!!!!!";

       disconnect(ftp, 0, midtree, 0);

       QVector<QUrlInfo>::iterator iter;

       for(iter = midtree->url.begin(); iter != midtree->url.end(); iter++)
       {
           if(!iter->isDir())
           {
               zrmFile(filePath,  iter->name(), 0);
           }
       }
       for(iter = midtree->url.begin(); iter != midtree->url.end(); iter++)
       {
           if(iter->isDir())
           {
               zrmFile(filePath,  iter->name(), 1);
           }
       }
       ftp->rmdir(filePath);
       ftpsem.qt_waite(100);
   }
   else
   {
       QString na = filePath +"/" +fileName;
       qDebug() << na;

        ftp->remove(na);

        progressDialog->setLabelText(tr("delete file %1...").arg(fileName));
        downloadButton->setEnabled(false);
        progressDialog->exec();
        ftpsem.qt_waite(100);
   }
}

void FtpWindow::ftpdel(void)
{
    QString fileName = fileList->currentItem()->text(0);

    zrmFile(currentPath, fileName, isDirectory.value(fileName));
    listmark = 0;
    ftp->cd(currentPath);
    ftpsem.qt_waite(10);
    re_list();
}

void FtpWindow::ftpnw(void)
{
    QString fileName = fileList->currentItem()->text(0);

}

void FtpWindow::popMenu(const QPoint& point)
{

    if(fileList==NULL)return;                            //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    QTreeWidgetItem* curItem = fileList->currentItem();  //获取当前被点击的节点

    QString wellName = curItem->text(0);
    qDebug() << wellName;
    if(!wellName.isEmpty())
    {
        QAction del(tr("删除"),this);
        QAction nw(tr("新建"),   this);
        QAction ren(tr("重命名"),this);
        //在界面上删除该item
        connect(&del, SIGNAL(triggered()), this, SLOT(ftpdel()));
        connect(&nw,  SIGNAL(triggered()), this, SLOT(ftpnw()));
//        connect(&ren, SIGNAL(triggered()), this, SLOT(ftpren()));

        QPoint pos;
        QMenu menu(this->fileList);
        menu.addAction(&del);
        menu.addAction(&nw);
        menu.addAction(&ren);
        menu.exec(QCursor::pos());  //在当前鼠标位置显示

    }
}

void FtpWindow::downloadFile()
{

    QString fileName = fileList->currentItem()->text(0);

    QString localdir = get_local_file_path();
    qDebug() << localdir;

    downloadButton->setEnabled(false);
    uploadButton->setEnabled(false);
    zdownloadFile(localdir, currentPath,  fileName, isDirectory.value(fileName));

    listmark = 0;
    ftp->cd(currentPath);
    ftpsem.qt_waite(10);
    downloadButton->setEnabled(true);
    uploadButton->setEnabled(true);

}

void FtpWindow::zdownloadFile(QString savefilePath, QString filePath, QString fileName, int type)
{

   if(type)                                 //目录
   {
       qDebug("Is dir!");
       QDir dir;
       dir.cd(savefilePath);

       qDebug() << "window path " << savefilePath;

       if(!dir.exists(fileName))//判断需要创建的文件夹是否存在
       {
           dir.mkdir(fileName); //创建文件夹

       }
       listmark = 1;

       ZQTree * midtree = new ZQTree();

       connect(ftp, SIGNAL(listInfo(QUrlInfo)),
               midtree, SLOT(addtolist(QUrlInfo)));
       qDebug() << fileName;

       filePath = filePath +"/"+fileName;
       qDebug() << "linux path " << filePath;
       ftp->cd(filePath);
       ftpsem.qt_waite(10);
       ftp->list();

       savefilePath = savefilePath + "/" +fileName;

       ftpsem.qt_waite(100);

       qDebug() << "list over!!!!!!!!!!";

       disconnect(ftp, 0, midtree, 0);

       QVector<QUrlInfo>::iterator iter;

       for(iter = midtree->url.begin(); iter != midtree->url.end(); iter++)
       {
           if(!iter->isDir())
           {
               zdownloadFile(savefilePath, filePath,  iter->name(), 0);
           }
       }
       for(iter = midtree->url.begin(); iter != midtree->url.end(); iter++)
       {
           if(iter->isDir())
           {
               zdownloadFile(savefilePath, filePath,  iter->name(), 1);
           }
       }
   }
   else
   {
       QString na = savefilePath +"/" +fileName;
       qDebug() << na;
       file = new QFile(na);
       if (!file->open(QIODevice::WriteOnly)) {
           QMessageBox::information(this, tr("FTP"),
                                    tr("Unable to save the file %1: %2.")
                                    .arg(fileName).arg(file->errorString()));
           delete file;
           return;
       }


       ftp->get(fileName, file);

       progressDialog->setLabelText(tr("Downloading %1...").arg(fileName));

       progressDialog->exec();

       ftpsem.qt_waite(100);
   }
}


void FtpWindow::ListLocalFile(QString path)
{

    //判断路径是否存在
    QDir dir(path);
    if(!dir.exists())
    {
        return;
    }


    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList flist =dir.entryInfoList();


    int file_count =flist.count();
    if(file_count <=0)
    {
        return;
    }
    for(int i = 0; i < file_count; i++)
    {
        QFileInfo file_info = flist.at(i);
        qDebug() <<file_info.filePath();
    }

    dir.setFilter(QDir::Dirs | QDir::NoSymLinks);
    QFileInfoList list =dir.entryInfoList();

    file_count =list.count();
    if(file_count <=0)
    {
        return;
    }

     for(int i=0; i < file_count; i++)
     {
        QFileInfo file_info = list.at(i);


        if(file_info.fileName() != "."&& file_info.fileName() != "..")
        {
            qDebug() <<file_info.filePath();
        }
     }
}
void FtpWindow::zuploadFile(QString localfilePath,  QString ftpfilePath, QString fileName, int type)
{
    if(type)               //目录
    {
       QDir dir(localfilePath);
       if(!dir.exists())
       {
           return;
       }

           ftp->cd(ftpfilePath);
           ftpsem.qt_waite(10);
           ftp->mkdir(fileName);
           ftpsem.qt_waite(100);

           dir.setFilter(QDir::Files | QDir::NoSymLinks);
           QFileInfoList list =dir.entryInfoList();


           int file_count =list.count();
           if(file_count <=0)
           {
               return;
           }
           for(int i = 0; i < file_count; i++)
           {
               QFileInfo file_info = list.at(i);
               zuploadFile(file_info.filePath(),  ftpfilePath+"/"+fileName,  file_info.fileName(), 0);
           }

           dir.setFilter(QDir::Dirs | QDir::NoSymLinks);
           QFileInfoList dlist =dir.entryInfoList();

           file_count =dlist.count();
           if(file_count <=0)
           {
               return;
           }

            for(int i=0; i < file_count; i++)
            {
               QFileInfo file_info = dlist.at(i);

               if(file_info.fileName() != "."&& file_info.fileName() != "..")
               {
                   zuploadFile(file_info.filePath(),  ftpfilePath+"/"+fileName,  file_info.fileName(), 1);
               }
            }
    }
    else
    {
        qDebug("upload is file~!");

        file = new QFile(localfilePath);
        if (!file->open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("FTP"),
                                     tr("Unable to save the file %1: %2.")
                                     .arg(fileName).arg(file->errorString()));
            delete file;
            return;
        }

        QString na = ftpfilePath +"/" +fileName;
        ftp->put(file, na);

        progressDialog->setLabelText(tr("Uploading %1...").arg(fileName));

        progressDialog->exec();
        ftpsem.qt_waite(100);
    }
}

void FtpWindow::uploadFile()
{

    QModelIndex index = wtreeView->currentIndex();
    if( !index.isValid() )
    {
       qDebug("error index!");
        return;
    }

    downloadButton->setEnabled(false);
    uploadButton->setEnabled(false);

    QFileInfo fileif = model->fileInfo(index);
    zuploadFile(fileif.filePath(), get_ftp_file_path(), fileif.fileName(), fileif.isDir());
    ftp->cd(currentPath);
    ftpsem.qt_waite(10);

    re_list();
    downloadButton->setEnabled(true);
    uploadButton->setEnabled(true);

}


void FtpWindow::cancelDownload()
{
    ftp->abort();
}

void FtpWindow::ftpCommandFinished(int, bool error)
{
#ifndef QT_NO_CURSOR
    setCursor(Qt::ArrowCursor);
#endif

    if (ftp->currentCommand() == QFtp::ConnectToHost) {
        if (error) {
            QMessageBox::information(this, tr("FTP"),
                                     tr("Unable to connect to the FTP server "
                                        "at %1. Please check that the host "
                                        "name is correct.")
                                     .arg(ftpServerLineEdit->text()));
            connectOrDisconnect();
            return;
        }
        statusLabel->setText(tr("Logged onto %1.")
                             .arg(ftpServerLineEdit->text()));
        fileList->setFocus();
        downloadButton->setDefault(true);
        uploadButton->setDefault(true);
        connectButton->setEnabled(true);
        uploadButton->setEnabled(true);
        downloadButton->setEnabled(true);
        return;
    }
    if (ftp->currentCommand() == QFtp::Login)
        ftp->list();
    if (ftp->currentCommand() == QFtp::Get) {
        if (error) {
            statusLabel->setText(tr("Canceled download of %1.")
                                 .arg(file->fileName()));
            file->close();
            file->remove();
        } else {
            statusLabel->setText(tr("Downloaded %1 to current directory.")
                                 .arg(file->fileName()));
            file->close();
            qDebug() << "download over!";
        }
        delete file;
        enableDownloadButton();
        progressDialog->hide();
        if(listmark)
        {
            ftpsem.send_sem(error);
        }
    } else if (ftp->currentCommand() == QFtp::List) {
        if (isDirectory.isEmpty()) {
            fileList->addTopLevelItem(new QTreeWidgetItem(QStringList() << tr("<empty>")));
            fileList->setEnabled(false);
        }
        if(listmark)
        {
           ftpsem.send_sem(error);
        }
    }
    else if(ftp->currentCommand() == QFtp::Put)
    {

        if (error) {
            statusLabel->setText(tr("Canceled upload of %1.")
                                 .arg(file->fileName()));
            file->close();
            file->remove();
        } else {
            statusLabel->setText(tr("Uploaded %1 to current directory.")
                                 .arg(file->fileName()));
            file->close();
            qDebug() << "upload over!";
        }
        delete file;
        enableUploadButton();
        progressDialog->hide();
        ftpsem.send_sem(error);
    }
    else if(ftp->currentCommand() == QFtp::Cd)
    {
        if(error) {
            statusLabel->setText("Ftp cd fail!");
        }

        ftpsem.send_sem(error);

    }
    else if(ftp->currentCommand() == QFtp::Mkdir)
    {
        if(error) {
            statusLabel->setText("Ftp cd fail!");
        }

         ftpsem.send_sem(error);

    }
    else if(ftp->currentCommand() == QFtp::Rmdir)
    {
        if(error) {
            statusLabel->setText("Ftp rmdir fail!");
        }
         qDebug() << "rm dir over!";
         ftpsem.send_sem(error);
    }
    else if(ftp->currentCommand() == QFtp::Remove)
    {
        if(error) {
            statusLabel->setText("Ftp remove fail!");
        }
         qDebug() << "rm file over!";
         progressDialog->hide();
         ftpsem.send_sem(error);
    }
}

void FtpWindow::addToList(const QUrlInfo &urlInfo)
{
    if(listmark == 1) return;
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, urlInfo.name());
    item->setText(1, QString::number(urlInfo.size()));
    item->setText(2, urlInfo.owner());
    item->setText(3, urlInfo.group());
    item->setText(4, urlInfo.lastModified().toString("MMM dd yyyy"));

    QPixmap pixmap(urlInfo.isDir() ? ":/images/dir.png" : ":/images/file.png");
    item->setIcon(0, pixmap);

    isDirectory[urlInfo.name()] = urlInfo.isDir();
    fileList->addTopLevelItem(item);
    if (!fileList->currentItem()) {
        fileList->setCurrentItem(fileList->topLevelItem(0));
        fileList->setEnabled(true);
    }
}

void FtpWindow::processItem(QTreeWidgetItem *item, int /*column*/)
{
    QString name = item->text(0);
    if (isDirectory.value(name)) {
        fileList->clear();
        isDirectory.clear();
        currentPath += "/";
        currentPath += name;
        qDebug() << "val " << name;
        ftp->cd(name);
        ftpsem.qt_waite(10);
        ftp->list();
        cdToParentButton->setEnabled(true);
#ifndef QT_NO_CURSOR
        setCursor(Qt::WaitCursor);
#endif
        return;
    }
}

void FtpWindow::cdToParent()
{
#ifndef QT_NO_CURSOR
    setCursor(Qt::WaitCursor);
#endif
    fileList->clear();
    isDirectory.clear();
    currentPath = currentPath.left(currentPath.lastIndexOf('/'));
    if (currentPath.isEmpty()) {
        cdToParentButton->setEnabled(false);
        ftp->cd("/");
        ftpsem.qt_waite(10);
    } else {
        ftp->cd(currentPath);
        ftpsem.qt_waite(10);
    }
    ftp->list();
}

void FtpWindow::updateDataTransferProgress(qint64 readBytes,
                                           qint64 totalBytes)
{
    progressDialog->setMaximum(totalBytes);
    progressDialog->setValue(readBytes);
}

void FtpWindow::enableDownloadButton()
{
    QTreeWidgetItem *current = fileList->currentItem();
    if (current) {
        QString currentFile = current->text(0);
//        downloadButton->setEnabled(!isDirectory.value(currentFile));
         downloadButton->setEnabled(true);
    } else {
        downloadButton->setEnabled(false);
    }
}

void FtpWindow::enableUploadButton()
{
    uploadButton->setEnabled(true);
}


void FtpWindow::enableConnectButton()
{
    // Save the used configuration
    QNetworkConfiguration config = networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();

    connectButton->setEnabled(networkSession->isOpen());
    statusLabel->setText(tr("Please enter the name of an FTP server."));
}

void FtpWindow::Open_File()
{

    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,tr("Open File"),tr(""),tr("Text File (*.txt)"));
    if(fileName == "")                  //如果用户直接关闭了文件浏览对话框，那么文件名就为空值，直接返回
    {
        return;
    }
    else
    {
       QFile file(fileName);
       if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
       {
           QMessageBox::warning(this,tr("错误"),tr("打开文件失败"));
           return;
       }
       else
       {
           if(!file.isReadable())
           {
               QMessageBox::warning(this,tr("错误"),tr("该文件不可读"));
           }
           else
           {
               QTextStream textStream(&file);       //读取文件，使用QTextStream
               while(!textStream.atEnd())
               {
                   textEdit->setPlainText(textStream.readAll());
               }
               textEdit->show();
               file.close();
//               Flag_isOpen = 1;
//               Last_FileName = fileName;
           }
       }
    }
}

