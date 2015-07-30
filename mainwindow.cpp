﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QScrollBar>
#include <QFileIconProvider>
#include <QStandardItem>
#include <QImageReader>
#include <QMenu>
#include <QDesktopServices>
#include <QMessageBox>
//http://slproweb.com/products/Win32OpenSSL.html
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    currentDirectory=QDir::homePath()+"/Desktop";
    ui->progressBar->setValue(0);

    //listWidget menu
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
            SLOT(showContextMenuForWidget(const QPoint &)));

    contextMenu= new QMenu(tr("Context menu"), this);
    QAction *openfile =new QAction(tr("Open The File"), this);
    QAction *openfolder =new QAction(tr("Open Folder"), this);
    QAction *showimage =new QAction(tr("Show Image"), this);
    QAction *dlpage =new QAction(tr("Open DLsite Link"), this);
    QAction *renameactF =new QAction(tr("Format ReName"), this);
    QAction *renameactR =new QAction(tr("RJNamber ReName"), this);

    contextMenu->addAction(openfile);
    contextMenu->addAction(openfolder);
    contextMenu->addAction(showimage);
    contextMenu->addAction(dlpage);
    contextMenu->addSeparator();
    contextMenu->addAction(renameactF);
    contextMenu->addAction(renameactR);

    connect(openfile, SIGNAL(triggered()), this, SLOT(MenuFileOpen()));
    connect(openfolder, SIGNAL(triggered()), this, SLOT(MenuFolderOpen()));
    connect(showimage, SIGNAL(triggered()), this, SLOT(MenuShowImage()));
    connect(dlpage, SIGNAL(triggered()), this, SLOT(MenuDLPage()));
    connect(renameactF, SIGNAL(triggered()), this, SLOT(MenuReName()));
    connect(renameactR, SIGNAL(triggered()), this, SLOT(MenuRJReName()));

    //image menu
    ui->label->setContextMenuPolicy(Qt::CustomContextMenu);
    contextMenuLabel= new QMenu(tr("Context menu"), this);
    QAction *saveimage =new QAction(tr("Save Images"), this);
    QAction *savedirimage =new QAction(tr("Create Folder and Save Images"), this);

    contextMenuLabel->addAction(saveimage);
    contextMenuLabel->addAction(savedirimage);

    connect(savedirimage, SIGNAL(triggered()), this, SLOT(MenuSaveImage()));
    connect(savedirimage, SIGNAL(triggered()), this, SLOT(MenuSaveDirImage()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_butOD_clicked()
{
    dlsiteimage.clear();
    SendMsg("Open Directory...");
    currentDirectory = QFileDialog::getExistingDirectory(this,
                                                         tr("Open Directory"),currentDirectory);
    SendMsg("Directory Path : "+currentDirectory);
    this->setWindowTitle(currentDirectory);
    ListReload();
}

void MainWindow::on_butRename_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Files will be Renamed.");
    msgBox.setInformativeText("Do you want to Renamed your Files?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
        QStringList filelist =currentFileList;
        ui->progressBar->setRange(0,filelist.count());
        int value=0;
        if(!filelist.isEmpty()){
            for(int i=0;i<filelist.count();i++){
                RJReName(filelist.at(i));
                value++;
                ui->progressBar->setValue(value);
            }
        }
        break;
    }
}
void MainWindow::ListReload()
{
    ui->listWidget->clear();
    QStringList filters;
    filters <<"*RJ*";
    QDir myDir(currentDirectory);
    myDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    myDir.setNameFilters(filters);
    currentFileList=myDir.entryList();
    currentFileList = currentFileList.filter(QRegExp("(R|r)(J|j)\\d{6}"));

    for(int i=0;i<currentFileList.size();i++)
    {
        QFileInfo fileInfo(currentDirectory,currentFileList.at(i)) ;
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        QString name = fileInfo.fileName();
        item->setText(name);
        QFileIconProvider iconSource;
        QIcon icon = iconSource.icon(fileInfo);
        item->setIcon(icon);
    }
}

void MainWindow::SendMsg(QString msg)
{
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->insertHtml(msg);
    ui->textEdit->insertPlainText("\n");
    QScrollBar *sb = ui->textEdit->verticalScrollBar();
    sb->setValue(sb->maximum());
}
QString MainWindow::GetRJname(QString name)
{
    QRegExp rx("(RJ\\d{6})");
    rx.setMinimal(true);
    rx.indexIn(name.toUpper(), 0);
    return rx.cap(1);
}
QString MainWindow::NameCheck(QString rjname)
{
    rjname.replace("?","？");
    rjname.replace("~","～");
    rjname.replace("*","＊");
    rjname.replace("/","／");
    rjname.replace("\\","＼");
    rjname.replace(":","：");
    rjname.replace("\"","＂");
    rjname.replace("<","＜");
    rjname.replace(">","＞");
    rjname.replace("|","｜");
    return rjname;
}
QString MainWindow::DownloadInfo(QString path)
{
    QUrl url(path);
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply = manager.get(QNetworkRequest(url));
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    QString src(reply->readAll());
    return src;
}
void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString filename =item->text();
    DownloadImage(filename);
}
void MainWindow::DownloadImage(QString filename)
{
    dlsiteimage.clear();
    SendMsg("Loading Image...");
    QString rjname= GetRJname(filename);
    QString path ="http://www.dlsite.com/maniax/work/=/product_id/"+rjname;
    QString src =DownloadInfo(path);
    SendMsg("File : "+filename);

    if(!src.isEmpty()){
        QRegExp namerx("<span itemprop=\"title\">.*<span itemprop=\"title\">.*<span itemprop=\"title\">(.*)</span></a>.*<span itemprop=\"brand\">(.*)<\/span><\/a>.*(\\d{2})年(\\d{2})月(\\d{2})日.*<tr><th>作品形式(.*)<tr><th>ファイル形式");
        namerx.setMinimal(true);
        namerx.indexIn(src, 0);

        QString newname="["+namerx.cap(2) + "]["+namerx.cap(3)+ namerx.cap(4)+ namerx.cap(5)+"]["+rjname+"]"+namerx.cap(1);
        newname =NameCheck(newname);

        QRegExp rx("background-image: url\\((.*_main\.jpg)");
        rx.setMinimal(true);
        rx.indexIn(src, 0);

        dlsiteimage<<"http:"+rx.cap(1);
        QRegExp samplerx("sample_images.*href=\"(.*\.jpg)");
        samplerx.setMinimal(true);
        int pos = 0;
        while((pos = samplerx.indexIn(src, pos)) != -1) {
            pos += samplerx.matchedLength();
            dlsiteimage<<"http:"+samplerx.cap(1);
        }

        if(rx.cap(1).isEmpty()){
            SendMsg("<font size=6 color=\"red\">Not Found!</font>");
        }else{
            SendMsg(newname);
            SendMsg("Image link : "+rx.cap(1));
            QNetworkAccessManager manager;
            QEventLoop loop;
            QNetworkReply *reply = manager.get(QNetworkRequest(dlsiteimage.at(0)));
            QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();
            QByteArray jpegData = reply->readAll();
            QPixmap pixmap;
            pixmap.loadFromData(jpegData);
            ui->label->setPixmap(pixmap);
            ui->label_2->setText(rjname);
            SendMsg("Image OK");
        }
    }
}
bool MainWindow::RJReName(QString filename)
{
    QString rjname= GetRJname(filename);
    QString path ="http://www.dlsite.com/maniax/work/=/product_id/"+rjname;
    SendMsg("Downloading Info..");
    SendMsg("Link : "+path);
    QString src =DownloadInfo(path);

    if(!src.isEmpty()){
        QRegExp rx("<span itemprop=\"title\">.*<span itemprop=\"title\">.*<span itemprop=\"title\">(.*)</span></a>.*<span itemprop=\"brand\">(.*)<\/span><\/a>.*(\\d{2})年(\\d{2})月(\\d{2})日.*<tr><th>作品形式(.*)<tr><th>ファイル形式");
        rx.setMinimal(true);
        rx.indexIn(src, 0);

        if(rx.cap(1).isEmpty()||rx.cap(2).isEmpty()||rx.cap(5).isEmpty()){
            SendMsg("<font size=6 color=\"red\">Page Not Found!</font>");
            return false;
        }

        QRegExp type("title=\"(.*)\">");
        type.setMinimal(true);
        int pos = 0;
        QString rjtype;
        while((pos = type.indexIn(rx.cap(6), pos)) != -1) {
            //qDebug()<<type.cap(1);
            pos += type.matchedLength();
            rjtype +="("+type.cap(1)+")";
        }

        QFileInfo rjfile(currentDirectory,filename);
        QString oldname=filename;
        SendMsg("File : ");
        SendMsg(oldname);
        SendMsg("ReName :");
        QString newname="["+rx.cap(2) + "]["+rx.cap(3)+ rx.cap(4)+ rx.cap(5)+"]["+rjname+"]"+rx.cap(1)+rjtype+"."+ rjfile.completeSuffix();
        newname =NameCheck(newname);
        SendMsg(newname);
        QDir myDir(currentDirectory);
        if(myDir.rename(oldname,newname)){
            SendMsg("Rename.... <font size=6 color=\"blue\">success</font>");
        }else{
            SendMsg("Rename.... <font size=6 color=\"red\">fail</font>");
        }
    }
    ListReload();
    return true;
}

void MainWindow::showContextMenuForWidget(const QPoint &pos)
{
    QModelIndex t = ui->listWidget->indexAt(pos);
    if(ui->listWidget->count()>0 && t.row()!=-1){
        ui->listWidget->item(t.row())->setSelected(true);	// even a right click will select the item
        contextMenu->exec(mapToGlobal(pos));
    }

}
void MainWindow::MenuFileOpen()
{
    QString filename =ui->listWidget->selectedItems().at(0)->text();
    QFileInfo fileInfo(currentDirectory,filename) ;
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    SendMsg("Open The File!");
}
void MainWindow::MenuFolderOpen()
{
    QDesktopServices::openUrl(QUrl(currentDirectory));
    SendMsg("Open Folder!");
}

void MainWindow::MenuShowImage()
{
    QString filename =ui->listWidget->selectedItems().at(0)->text();
    DownloadImage(filename);
}
void MainWindow::MenuDLPage()
{
    QString filename =ui->listWidget->selectedItems().at(0)->text();
    QString rjname= GetRJname(filename);
    QDesktopServices::openUrl(QUrl("http://www.dlsite.com/maniax/work/=/product_id/"+rjname));
    SendMsg("Show DLpage!");
}
void MainWindow::MenuReName()
{
    QMessageBox msgBox;
    msgBox.setText("File will be Renamed.");
    msgBox.setInformativeText("Do you want to Renamed your File?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
        QString filename =ui->listWidget->selectedItems().at(0)->text();
        RJReName(filename);
        break;
    }
}
void MainWindow::MenuRJReName()
{
    QMessageBox msgBox;
    msgBox.setText("File will be Renamed.");
    msgBox.setInformativeText("Do you want to Renamed your File?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
        QString filename =ui->listWidget->selectedItems().at(0)->text();
        QFileInfo rjfile(currentDirectory,filename);
        QString rjname= GetRJname(filename);
        SendMsg("File : ");
        SendMsg(filename);
        SendMsg("ReName :");
        QString newname=rjname+"."+ rjfile.completeSuffix();
        newname =NameCheck(newname);
        SendMsg(newname);
        QDir myDir(currentDirectory);
        if(myDir.rename(filename,newname)){
            SendMsg("Rename.... <font size=6 color=\"blue\">success</font>");
        }else{
            SendMsg("Rename.... <font size=6 color=\"red\">fail</font>");
        }
        ListReload();
        break;
    }

}
void MainWindow::on_label_customContextMenuRequested(const QPoint &pos)
{
    if(!dlsiteimage.isEmpty()){
        QPoint labelpos= ui->label->mapToGlobal(pos);
        contextMenuLabel->exec(labelpos);
    }
}
void MainWindow::MenuSaveImage()
{
    if(!dlsiteimage.isEmpty()){
        SendMsg("Save Image...");
        ui->progressBar->setRange(0,dlsiteimage.count());
        int value=0;
        for(int i=0;i<dlsiteimage.count();i++){
            QFileInfo fileInfo(dlsiteimage.at(i));
            QString url=fileInfo.absoluteFilePath();
            qDebug()<<url;

            QNetworkAccessManager manager;
            QEventLoop loop;
            QNetworkReply *reply = manager.get(QNetworkRequest(dlsiteimage.at(i)));
            QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();
            QByteArray data = reply->readAll();
            QFile file(currentDirectory+"/"+fileInfo.fileName());
            file.open(QIODevice::WriteOnly);
            SendMsg("Image :"+fileInfo.fileName());
            if(file.write(data)==-1){
                SendMsg("<font size=6 color=\"red\">Save Error!</font>");
            }
            file.close();
            SendMsg("Save!");
            value++;
            ui->progressBar->setValue(value);
        }
    }
}
void MainWindow::MenuSaveDirImage()
{
    if(!dlsiteimage.isEmpty()){
        QString foldername=ui->label_2->text();
        SendMsg("Create Folder...");
        QDir createfolder(currentDirectory);
        if(!createfolder.mkdir(foldername)){
            SendMsg("<font size=6 color=\"red\">Create Fail!</font>");
        }
        SendMsg("Create Success!");

        SendMsg("Save Image...");
        ui->progressBar->setRange(0,dlsiteimage.count());
        int value=0;
        for(int i=0;i<dlsiteimage.count();i++){
            QFileInfo fileInfo(dlsiteimage.at(i));
            QString url=fileInfo.absoluteFilePath();
            qDebug()<<url;

            QNetworkAccessManager manager;
            QEventLoop loop;
            QNetworkReply *reply = manager.get(QNetworkRequest(dlsiteimage.at(i)));
            QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();
            QByteArray data = reply->readAll();
            QFile file(currentDirectory+"/"+foldername+"/"+fileInfo.fileName());
            file.open(QIODevice::WriteOnly);
            SendMsg("Image :"+fileInfo.fileName());
            if(file.write(data)==-1){
                SendMsg("Save Error!");
            }
            file.close();
            SendMsg("Save!");
            value++;
            ui->progressBar->setValue(value);
        }
    }
}
