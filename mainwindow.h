#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QMenu>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void ListReload();
    void SendMsg(QString msg);
    QString GetRJname(QString filename);
    QString NameCheck(QString newname);
    QString DownloadInfo(QString path);
    QStringList GetFormatNname(QString pagedata);
    void DownloadImage(QString filename);
    bool RJReName(QString filename);

private slots:
    void on_butOD_clicked();
    void on_butRename_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_label_customContextMenuRequested(const QPoint &pos);

    void showContextMenuForWidget(const QPoint &pos);
    void MenuFileOpen();
    void MenuFolderOpen();
    void MenuShowImage();
    void MenuDLPage();
    void MenuReName();
    void MenuRJReName();

    void MenuSaveImage();
    void MenuSaveDirImage();
    void MenuSaveFormatDirImage();


private:
    Ui::MainWindow *ui;

public:
    QString currentDirectory;
    QStringList currentFileList;
    QMenu *contextMenu;
    QMenu *contextMenuLabel;
    QStringList dlsiteimage;

};

#endif // MAINWINDOW_H
