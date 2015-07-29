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
    QString GetRJname(QString name);
    QString NameCheck(QString name);
    QString DownloadInfo(QString path);
    void DownloadImage(QString filename);
    bool RJReName(QString filename);

private slots:
    void on_butOD_clicked();
    void on_butRename_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void showContextMenuForWidget(const QPoint &pos);
    void MenuFileOpen();
    void MenuShowImage();
    void MenuDLPage();
    void MenuReName();
    void MenuRJReName();

private:
    Ui::MainWindow *ui;

public:
    QString currentDirectory;
    QStringList currentFileList;
    QMenu *contextMenu;

};

#endif // MAINWINDOW_H
