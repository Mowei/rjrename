#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFileSystemModel>
#include <QListWidgetItem>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void SendMsg(QString msg);
    QString GetRJname(QString name);
    QString NameCheck(QString name);
    QString DownloadInfo(QString path);

private slots:
    void on_butOD_clicked();
    void on_butRename_clicked();

    //void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;

public:
    QString currentDirectory;
    QStringList currentFileList;

};

#endif // MAINWINDOW_H
