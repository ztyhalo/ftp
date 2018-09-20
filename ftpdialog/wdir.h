#ifndef __WDIR_H__
#define __WDIR_H__


#include <QDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QDir>

class WDir : public QDialog
{
    Q_OBJECT

public:
    WDir(QWidget *parent = 0);

//private slots:
//    void createDirectory();
//    void remove();

private:
    QFileSystemModel *model;
    QTreeView *treeView;

};

#endif // DIALOG_H
