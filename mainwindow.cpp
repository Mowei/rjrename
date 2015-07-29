#include "mainwindow.h"
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
//http://slproweb.com/products/Win32OpenSSL.html
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    currentDirectory=QDir::homePath()+"/Desktop";
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
            SLOT(showContextMenuForWidget(const QPoint &)));


    contextMenu= new QMenu(tr("Context menu"), this);
    QAction *openfile =new QAction(tr("Open file"), this);
    QAction *showimage =new QAction(tr("Show image"), this);
    contextMenu->addAction(openfile);
    contextMenu->addAction(showimage);
    connect(openfile, SIGNAL(triggered()), this, SLOT(MenuFileOpen()));
    connect(showimage, SIGNAL(triggered()), this, SLOT(MenuShowImage()));
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
    this->setWindowTitle(currentDirectory);


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

void MainWindow::on_butRename_clicked()
{
    QString path;
    if(!currentFileList.isEmpty()){
        for(int i=0;i<currentFileList.size();i++){

            QString rjname= GetRJname(currentFileList.at(i));
            qDebug()<<rjname;
            if(rjname==""){
                continue;
            }
            path ="http://www.dlsite.com/maniax/work/=/product_id/"+rjname;
            SendMsg("Downloading Info..");
            SendMsg("Link : "+path);
            QString src =DownloadInfo(path);

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
                QString newname="["+rx.cap(2) + "]["+rx.cap(3)+ rx.cap(4)+ rx.cap(5)+"]["+rjname+"]"+rx.cap(1)+rjtype+"."+rjfile.completeSuffix() ;
                newname =NameCheck(newname);
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
QString MainWindow::NameCheck(QString name)
{
    name.replace("?","？");
    name.replace("~","～");
    name.replace("*","＊");
    name.replace("/","／");
    name.replace("\\","＼");
    name.replace(":","：");
    name.replace("\"","＂");
    name.replace("<","＜");
    name.replace(">","＞");
    name.replace("|","｜");
    return name;
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
    QString name =item->text();
    DownloadImage(name);
}
void MainWindow::DownloadImage(QString name)
{
    SendMsg("Loading Image...");
    QString rjname= GetRJname(name);
    QString path ="http://www.dlsite.com/maniax/work/=/product_id/"+rjname;
    QString src =DownloadInfo(path);

    SendMsg("File : "+name);

    if(!src.isEmpty()){
        QRegExp namerx("<span itemprop=\"title\">.*<span itemprop=\"title\">.*<span itemprop=\"title\">(.*)</span></a>.*<span itemprop=\"brand\">(.*)<\/span><\/a>.*(\\d{2})年(\\d{2})月(\\d{2})日.*<tr><th>作品形式(.*)<tr><th>ファイル形式");
        namerx.setMinimal(true);
        namerx.indexIn(src, 0);

        QString newname="["+namerx.cap(2) + "]["+namerx.cap(3)+ namerx.cap(4)+ namerx.cap(5)+"]["+rjname+"]"+namerx.cap(1);
        newname =NameCheck(newname);


        QRegExp rx("background-image: url\\((.*_main\.jpg)");
        rx.setMinimal(true);
        rx.indexIn(src, 0);
        //qDebug()<< rx.cap(1);

        if(rx.cap(1).isEmpty()){
            SendMsg("Not Found!");
        }else{
            SendMsg(newname);
            SendMsg("Image link : "+rx.cap(1));
            QNetworkAccessManager manager;
            QEventLoop loop;
            QNetworkReply *reply = manager.get(QNetworkRequest("http:"+rx.cap(1)));
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

void MainWindow::showContextMenuForWidget(const QPoint &pos)
{
    QModelIndex t = ui->listWidget->indexAt(pos);
    if(ui->listWidget->count()>0){
        ui->listWidget->item(t.row())->setSelected(true);	// even a right click will select the item
        contextMenu->exec(mapToGlobal(pos));
    }
}
void MainWindow::MenuFileOpen()
{
    QString name =ui->listWidget->selectedItems().at(0)->text();
    QFileInfo fileInfo(currentDirectory,name) ;
    QDesktopServices::openUrl ( QUrl::fromLocalFile(fileInfo.absoluteFilePath()) );
    SendMsg("Open file!");
}
void MainWindow::MenuShowImage(){
    QString name =ui->listWidget->selectedItems().at(0)->text();
    DownloadImage(name);
}
