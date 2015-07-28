#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>

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
    msg +="";
    msg +="Open Directory...\n" ;
    ui->plainTextEdit->setPlainText(msg);
    ui->listWidget->clear();
    currentDirectory = QFileDialog::getExistingDirectory(this,
                                                         tr("Open Directory"),QDir::homePath()+"/Desktop");
    QDir myDir(currentDirectory);
    myDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    currentFileList=myDir.entryList();

    ui->listWidget->addItems(currentFileList);
    ui->listWidget->currentIndex();

    msg +="Show FileList\n" ;
    ui->plainTextEdit->setPlainText(msg);
}

void MainWindow::on_butRename_clicked()
{
    msg +="ReName..\n";
    ui->plainTextEdit->setPlainText(msg);

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

            path ="http://www.dlsite.com/maniax/work/=/product_id/"+rjname;
            qDebug()<<path;

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
                qDebug()<< rx.cap(1)<< rx.cap(2)<< rx.cap(3)<< rx.cap(4)<< rx.cap(5);

                if(rx.cap(1).isEmpty()||rx.cap(2).isEmpty()||rx.cap(5).isEmpty()){
                    msg+="Page Not Found!\n";
                    ui->plainTextEdit->setPlainText(msg);
                    continue;
                }

                QRegExp type("title=\"(.*)\">");
                type.setMinimal(true);
                int pos = 0;
                QString rjtype;
                while((pos = type.indexIn(rx.cap(6), pos)) != -1) {
                    qDebug()<<type.cap(1);
                    pos += type.matchedLength();
                    rjtype +="("+type.cap(1)+")";
                }

                QFileInfo rjfile(currentDirectory+currentFileList.at(i));
                QString oldname=currentFileList.at(i);
                msg += "File : "+ oldname +"\n";

                msg += "rename to ";
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

                msg +=newname+"\n";
                ui->plainTextEdit->setPlainText(msg);

                QDir myDir(currentDirectory);
                myDir.rename(oldname,newname);

            }
        }
    }
}
