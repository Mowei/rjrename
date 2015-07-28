#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QScrollBar>
//http://slproweb.com/products/Win32OpenSSL.html
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_butOD_clicked()
{
    msg ="";
    SendMsg("Open Directory...");

    ui->listWidget->clear();
    currentDirectory = QFileDialog::getExistingDirectory(this,
                                                         tr("Open Directory"),QDir::homePath()+"/Desktop");
    QDir myDir(currentDirectory);
    myDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QStringList filters;
    filters <<"*RJ*"<<"*rj*";
    myDir.setNameFilters(filters);
    currentFileList=myDir.entryList();
    ui->listWidget->addItems(currentFileList);
    ui->listWidget->currentIndex();

    SendMsg("Show FileList");

}

void MainWindow::on_butRename_clicked()
{


    QString path;
    if(!currentFileList.isEmpty()){
        for(int i=0;i<currentFileList.size();i++){
            QRegExp rx("(RJ\\d{6})");
            rx.setMinimal(true);
            rx.indexIn(currentFileList.at(i), 0);
            QString rjname= rx.cap(1).toUpper();

            if(rjname==""){
                continue;
            }
            SendMsg("Downloading Info..");
            path ="http://www.dlsite.com/maniax/work/=/product_id/"+rjname;
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

                SendMsg("ReName...");
                QFileInfo rjfile(currentDirectory+currentFileList.at(i));
                QString oldname=currentFileList.at(i);

                SendMsg("File : ");
                SendMsg(oldname);
                SendMsg("rename to ");
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
                    SendMsg("rename success");
                }else{
                    SendMsg("rename fail");
                }

            }
        }
    }
}
void MainWindow::SendMsg(QString msg)
{
    this->msg += msg +"\n";
    ui->plainTextEdit->setPlainText(this->msg);
    QScrollBar *sb = ui->plainTextEdit->verticalScrollBar();
    sb->setValue(sb->maximum());
}
