#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_butOD_clicked();
    void on_butRename_clicked();



private:
    Ui::MainWindow *ui;

public:
    QString currentDirectory;
    QStringList currentFileList;
    QString msg;

};

#endif // MAINWINDOW_H