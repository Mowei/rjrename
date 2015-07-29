#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFileSystemModel>
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

private slots:
    void on_butOD_clicked();
    void on_butRename_clicked();


private:
    Ui::MainWindow *ui;

public:
    QFileSystemModel *model;
    QString currentDirectory;
    QStringList currentFileList;

};

#endif // MAINWINDOW_H
