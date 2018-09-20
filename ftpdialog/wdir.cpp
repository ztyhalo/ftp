#include "wdir.h"
#include <QDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QDir>
#include <QHeaderView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QMessageBox>

WDir::WDir(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("QFileSystemModel");
    model = new QFileSystemModel;
    model->setReadOnly(false);
    model->setRootPath(QDir::currentPath());

    treeView = new QTreeView;
    treeView->setModel(model);

    treeView->header()->setStretchLastSection(true);
    treeView->header()->setSortIndicator(0, Qt::AscendingOrder);
    treeView->header()->setSortIndicatorShown(true);
//    treeView->header()->setClickable(true);

    QModelIndex index = model->index(QDir::currentPath());
    treeView->expand(index);
    treeView->scrollTo(index);
    treeView->resizeColumnToContents(0);

//    QPushButton *createButton = new QPushButton(tr("Create Dir"));
//    QPushButton *removeButton = new QPushButton(tr("Remove Dir"));
//    connect(createButton, SIGNAL(clicked()), this, SLOT(createDirectory()));
//    connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));

//    QHBoxLayout *hLayout = new QHBoxLayout;
//    hLayout->addWidget(createButton);
//    hLayout->addWidget(removeButton);

//    QVBoxLayout *vLayout = new QVBoxLayout;
//    vLayout->addWidget(treeView);
//    vLayout->addLayout(hLayout);

//    setLayout(vLayout);
}

//void Dialog::createDirectory()
//{
//    QModelIndex index = treeView->currentIndex();
//    if( !index.isValid() )
//        return;
//    QString dirName = QInputDialog::getText(this, tr("create Dir"), tr("Dir name"));
//    if( !dirName.isEmpty() )
//    {
//        if( !model->mkdir(index, dirName).isValid() )
//            QMessageBox::information(this, tr("Create Dir"), tr("Failed to create Dir"));

//    }
//}

//void Dialog::remove()
//{
//    QModelIndex index = treeView->currentIndex();
//    if( !index.isValid() )
//        return;
//    bool ok;
//    if( model->fileInfo(index).isDir() )
//        ok = model->rmdir(index);
//    else
//        ok = model->remove(index);

//    if(!ok)
//        QMessageBox::information(this, tr("Remove"), tr("Failed to remove Dir").arg(model->fileName(index)));
//}
