#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QScrollBar>
#include <QStringListModel>
#include <QFileSystemModel>
#include <QAbstractItemModel>
#include <QFileIconProvider>
#include <QStandardItem>
//http://slproweb.com/products/Win32OpenSSL.html
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    currentDirectory=QDir::homePath()+"/Desktop";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_butOD_clicked()
{
    ui->listWidget->clear();
    SendMsg("Open Directory...");
    currentDirectory = QFileDialog::getExistingDirectory(this,
                                                         tr("Open Directory"),currentDirectory);
    SendMsg("Directory Path : "+currentDirectory);

    QStringList filters;
    filters <<"*RJ*";
    QDir myDir(currentDirectory);
    myDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    myDir.setNameFilters(filters);
    currentFileList=myDir.entryList();
    currentFileList = currentFileList.filter(QRegExp("(R|r)(J|j)\\d{6}"));


    foreach (QString  fileName, currentFileList)
    {
        QFileInfo fileInfo(currentDirectory,fileName) ;
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        QString name = fileInfo.fileName();
        item->setText(name);
        QFileIconProvider iconSource;
        QIcon icon = iconSource.icon(fileInfo);
        item->setIcon(icon);

    }

}

void MainWindow::on_butRename_clicked()
{
    QString path;
    if(!currentFileList.isEmpty()){
        for(int i=0;i<currentFileList.size();i++){
            QRegExp rx("(RJ\\d{6})");
            rx.setMinimal(true);
            rx.indexIn(currentFileList.at(i).toUpper(), 0);
            QString rjname= rx.cap(1);
            qDebug()<<rjname;
            if(rjname==""){
                continue;
            }

            path ="http://www.dlsite.com/maniax/work/=/product_id/"+rjname;
            SendMsg("Downloading Info..");
            SendMsg("Link : "+path);
            QUrl url(path);
            QNetworkAccessManager manager;
            QEventLoop loop;
            QNetworkReply *reply = manager.get(QNetworkRequest(url));
            QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();
            QString src(reply->readAll());


            if(!src.isEmpty()){

                QRegExp rx("<span itemprop=\"title\">.*<span itemprop=\"title\">.*<span itemprop=\"title\">(.*)</span></a>.*<span itemprop=\"brand\">(.*)<\/span><\/a>.*(\\d{2})年(\\d{2})月(\\d{2})日.*<tr><th>作品形式(.*)<tr><th>ファイル形式");
                rx.setMinimal(true);
                rx.indexIn(src, 0);
                //qDebug()<< rx.cap(1)<< rx.cap(2)<< rx.cap(3)<< rx.cap(4)<< rx.cap(5);

                if(rx.cap(1).isEmpty()||rx.cap(2).isEmpty()||rx.cap(5).isEmpty()){
                    SendMsg("Page Not Found!");

                    continue;
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

                QFileInfo rjfile(currentDirectory+currentFileList.at(i));
                QString oldname=currentFileList.at(i);

                SendMsg("File : ");
                SendMsg(oldname);
                SendMsg("ReName :");
                QString newname="["+rx.cap(2) + "]["+rx.cap(3)+ rx.cap(4)+ rx.cap(5)+"]["+rjname+"]"+rx.cap(1)+rjtype+"."+rjfile.suffix() ;
                newname.replace("?","？");
                newname.replace("~","～");
                newname.replace("*","＊");
                newname.replace("/","／");
                newname.replace("\\","＼");
                newname.replace(":","：");
                newname.replace("\"","＂");
                newname.replace("<","＜");
                newname.replace(">","＞");
                newname.replace("|","｜");

                SendMsg(newname);
                QDir myDir(currentDirectory);
                if(myDir.rename(oldname,newname)){
                    SendMsg("Rename.... <font size=6 color=\"blue\">success</font>");
                }else{
                    SendMsg("Rename.... <font size=6 color=\"red\">fail</font>");
                }

            }
        }
    }
}
void MainWindow::SendMsg(QString msg)
{

    ui->textEdit->insertHtml(msg);
    ui->textEdit->insertPlainText("\n");

    QScrollBar *sb = ui->textEdit->verticalScrollBar();
    sb->setValue(sb->maximum());

}

