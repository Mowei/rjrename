#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QScrollBar>
#include <QFileIconProvider>
#include <QMenu>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Get Directory Path
    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("config");
    currentDirectory=setting.value("CurrentDirectory").toString();
    if(currentDirectory.isEmpty()){
        currentDirectory=QDir::homePath();
    }else{
        ListReload();
    }

    ui->progressBar->setValue(0);

    //listWidget Menu
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
            SLOT(showContextMenuForWidget(const QPoint &)));

    contextMenu= new QMenu(tr("功能選單"), this);
    QAction *openfile =new QAction(tr("打開檔案"), this);
    QAction *openfolder =new QAction(tr("打開檔案資料夾"), this);
    QAction *showimage =new QAction(tr("預覽圖片"), this);
    QAction *dlpage =new QAction(tr("打開DLsite連結"), this);
    QAction *renameactF =new QAction(tr("格式化重新命名"), this);
    QAction *renameactR =new QAction(tr("RJ編號重新命名"), this);

    contextMenu->addAction(openfile);
    contextMenu->addAction(openfolder);
    contextMenu->addAction(showimage);
    contextMenu->addAction(dlpage);
    contextMenu->addSeparator();
    contextMenu->addAction(renameactF);
    contextMenu->addAction(renameactR);

    //Menu Event
    connect(openfile, SIGNAL(triggered()), this, SLOT(MenuFileOpen()));
    connect(openfolder, SIGNAL(triggered()), this, SLOT(MenuFolderOpen()));
    connect(showimage, SIGNAL(triggered()), this, SLOT(MenuShowImage()));
    connect(dlpage, SIGNAL(triggered()), this, SLOT(MenuDLPage()));
    connect(renameactF, SIGNAL(triggered()), this, SLOT(MenuReName()));
    connect(renameactR, SIGNAL(triggered()), this, SLOT(MenuRJReName()));

    //Image Menu
    ui->label->setContextMenuPolicy(Qt::CustomContextMenu);
    contextMenuLabel= new QMenu(tr("功能選單"), this);
    QAction *saveimage =new QAction(tr("儲存圖片"), this);
    QAction *savedirimage =new QAction(tr("建立資料夾並儲存圖片"), this);
    QAction *saveformatdirimage =new QAction(tr("建立格式化命名資料夾並儲存圖片"), this);

    contextMenuLabel->addAction(saveimage);
    contextMenuLabel->addAction(savedirimage);
    contextMenuLabel->addAction(saveformatdirimage);

    //RJ Tool
    rjTool = new RJUtility();

    //Save Event
    connect(saveimage, SIGNAL(triggered()), this, SLOT(MenuSaveImage()));
    connect(savedirimage, SIGNAL(triggered()), this, SLOT(MenuSaveRJDirImage()));
    connect(saveformatdirimage, SIGNAL(triggered()), this, SLOT(MenuSaveFormatDirImage()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_butOD_clicked()
{
    dlsiteimage.clear();
    SendMsg("打開資料夾...");
    currentDirectory = QFileDialog::getExistingDirectory(this,
                                                         tr("Open Directory"),currentDirectory);
    SendMsg("資料夾路徑 : "+currentDirectory);

    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("config");
    setting.setValue("CurrentDirectory",QVariant(currentDirectory));

    ListReload();
}

void MainWindow::on_butRename_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("接續動作將會對檔案重新命名!");
    msgBox.setInformativeText("你確定要重新命名這些檔案?");
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
    this->setWindowTitle(currentDirectory);
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

    QString dt = "LogFile";
    dt += QDateTime::currentDateTime().toString("yyyyMMdd");
    dt += ".log";
    QFile outFile(dt);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream textStream(&outFile);
    textStream.setCodec("UTF-8");
    textStream << msg ;
    textStream << "\r\n";
    outFile.close();
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString filename =item->text();
    DownloadImage(filename);

    QString rjname= rjTool->GetRJname(filename);
    QString path =rjTool->GetUrlByRJNumber(rjname);
    ui->WebWidget->load(QUrl(path));
    ui->WebWidget->show();
}
void MainWindow::DownloadImage(QString filename)
{
    dlsiteimage.clear();
    SendMsg("Loading Image...");

    QString rjname= rjTool->GetRJname(filename);
    QString path =rjTool->GetUrlByRJNumber(rjname);
    SendMsg("File : "+filename);

    QString src =rjTool->DownloadInfo(path);
    dlsiteimage = rjTool->GetImageUrls(src);

    if(dlsiteimage.isEmpty()){
        SendMsg("<font size=6 color=\"red\">Not Found!</font>");
        dlsiteimage.clear();
    }else{
        //SendMsg(newname);
        SendMsg("Image link : "+dlsiteimage.at(0));

        QByteArray jpegData = rjTool->DownloadInfo(dlsiteimage.at(0));
        QPixmap pixmap;
        pixmap.loadFromData(jpegData);
        ui->label->setPixmap(pixmap);
        ui->label_2->setText(rjname);
        SendMsg("Image OK");
    }

}
bool MainWindow::RJReName(QString filename)
{
    QString rjname= rjTool->GetRJname(filename);
    QString path =rjTool->GetUrlByRJNumber(rjname);
    SendMsg("Downloading Info..");
    SendMsg("Link : "+path);
    QString src =rjTool->DownloadInfo(path);
    if(!src.isEmpty()){
        QString nameformat=rjTool->GetFormatName(src);
        if(nameformat.isEmpty()){
            SendMsg("<font size=6 color=\"red\">Page Not Found!</font>");
            return false;
        }
        QFileInfo rjfile(currentDirectory,filename);
        QString oldname=filename;
        SendMsg("File : ");
        SendMsg(oldname);
        SendMsg("ReName :");
        QString newname=rjTool->NameCheck(nameformat+"."+ rjfile.completeSuffix());
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
bool MainWindow::RJMoveFile(QString dirsrc,QString dirtarget,QString filename)
{
    QFile myfile(dirsrc+"/"+filename);
    myfile.rename(dirtarget+"/"+filename);
    ListReload();
    return true;
}

bool MainWindow::CreateFolder(QString dirpath,QString foldername){
    SendMsg("Create Folder...");
    QDir createfolder(dirpath);
    if(!createfolder.mkdir(foldername)){
        SendMsg("<font size=6 color=\"red\">Create Fail!</font>");
        return false;
    }
    SendMsg("Create Success!");
    SendMsg(currentDirectory+"/"+foldername);
    return true;
}
bool MainWindow::DownloadSaveImage(QString dir,QString imagesrc)
{
    SendMsg("Save Image...");
    QFileInfo fileInfo(imagesrc);
    QByteArray data = rjTool->DownloadInfo(imagesrc);
    QFile file(dir+"/"+fileInfo.fileName());
    file.open(QIODevice::WriteOnly);
    SendMsg("Image :"+fileInfo.fileName());
    int flag =file.write(data);
    file.close();
    if(flag==-1){
        SendMsg("Save Error!");
        return false;
    }
    SendMsg("Save!");
    return true;
}

void MainWindow::showContextMenuForWidget(const QPoint &pos)
{
    QModelIndex t = ui->listWidget->indexAt(pos);
    if(ui->listWidget->count()>0 && t.row()!=-1){
        ui->listWidget->item(t.row())->setSelected(true);	//right click will select the item
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
    QDesktopServices::openUrl(QUrl::fromLocalFile(currentDirectory));
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
    QString rjname= rjTool->GetRJname(filename);
    QString path =rjTool->GetUrlByRJNumber(rjname);
    QDesktopServices::openUrl(QUrl(path));
    SendMsg("Show DLpage!");
}
void MainWindow::MenuReName()
{
    QMessageBox msgBox;
    msgBox.setText("接續動作將會對檔案重新命名!");
    msgBox.setInformativeText("你確定要重新命名這些檔案?");
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
    msgBox.setText("接續動作將會對檔案重新命名!");
    msgBox.setInformativeText("你確定要重新命名這些檔案?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
        QString filename =ui->listWidget->selectedItems().at(0)->text();
        QFileInfo rjfile(currentDirectory,filename);
        QString rjname= rjTool->GetRJname(filename);
        SendMsg("File : ");
        SendMsg(filename);
        SendMsg("ReName :");
        QString newname=rjname+"."+ rjfile.completeSuffix();
        newname =rjTool->NameCheck(newname);
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
        ui->progressBar->setRange(0,dlsiteimage.count());
        int value=0;
        for(int i=0;i<dlsiteimage.count();i++){
            DownloadSaveImage(currentDirectory,dlsiteimage.at(i));
            value++;
            ui->progressBar->setValue(value);
        }
    }
}
void MainWindow::MenuSaveRJDirImage()
{
    if(!dlsiteimage.isEmpty()){
        QString foldername=ui->label_2->text();
        if(!CreateFolder(currentDirectory, foldername))
        {
            return;
        }
        ui->progressBar->setRange(0,dlsiteimage.count());
        int value=0;
        for(int i=0;i<dlsiteimage.count();i++){
            DownloadSaveImage(currentDirectory+"/"+foldername,dlsiteimage.at(i));
            value++;
            ui->progressBar->setValue(value);
        }
    }
}
void MainWindow::MenuSaveFormatDirImage()
{
    if(!dlsiteimage.isEmpty()){
        QString rjname=ui->label_2->text();
        QString path =rjTool->GetUrlByRJNumber(rjname);
        QString src=rjTool->DownloadInfo(path);
        QString nameformat =rjTool->GetFormatName(src);
        if(nameformat.isEmpty())
        {
            return;
        }
        QString foldername=nameformat;

        if(!CreateFolder(currentDirectory, foldername))
        {
            return;
        }
        ui->progressBar->setRange(0,dlsiteimage.count());
        int value=0;
        for(int i=0;i<dlsiteimage.count();i++){
            DownloadSaveImage(currentDirectory+"/"+foldername,dlsiteimage.at(i));
            value++;
            ui->progressBar->setValue(value);
        }
    }
}

void MainWindow::on_butCFMF_clicked()
{
    ReFormatName(false,false);
}

void MainWindow::on_butCFMFDL_clicked()
{
    ReFormatName(true,false);
}

void MainWindow::on_butCFMF_FN_clicked()
{
    ReFormatName(false,true);
}

void MainWindow::on_butCFMFDL_FN_clicked()
{
    ReFormatName(true,true);
}
void MainWindow::ReFormatName(bool downloadImgs,bool createFormatFolder){
    QMessageBox msgBox;
    msgBox.setText("接續動作將會移動檔案!");
    msgBox.setInformativeText("你確定要移動這些檔案?");
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
                //原檔案名稱
                QString filename=filelist.at(i);
                QString rjname =rjTool->GetRJname(filename);
                QString foldername=rjname;
                if(createFormatFolder){
                    QString path =rjTool->GetUrlByRJNumber(rjname);
                    QString src = rjTool->DownloadInfo(path);
                    QString nameformat = rjTool->GetFormatName(src);
                    if(nameformat.isEmpty())
                    {
                        value++;
                        ui->progressBar->setValue(value);
                        continue;
                    }
                    foldername=nameformat;
                }
                CreateFolder(currentDirectory, foldername);
                RJMoveFile(currentDirectory,currentDirectory+"/"+foldername,filename);
                if(downloadImgs){
                    DownloadImage(filename);
                    for(int i=0;i<dlsiteimage.count();i++){
                        DownloadSaveImage(currentDirectory+"/"+foldername,dlsiteimage.at(i));
                    }
                }
                value++;
                ui->progressBar->setValue(value);
            }
        }
        break;
    }
}
