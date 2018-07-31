#ifndef RJUTILITY_H
#define RJUTILITY_H

#include <QWidget>
#include <QString>
#include <QSettings>
#include <QWebView>

class RJUtility: public QWidget
{
    Q_OBJECT

public:
    RJUtility(QWidget *parent = 0);
public:
    QString NameCheck(QString newname);
    QString GetRJname(QString filename);
    QString GetFormatName(QString pagedata);
    QStringList GetImageUrls(QString pagedata);
    QByteArray DownloadInfo(QString path);


public:
    QString UrlBase ;
    QString FormatNameTemplate;
    QString RJNumber_URL_Selector;
    QString maker_name_Selector;
    QString work_name_Selector;
    QString work_outline_Selector;
    QString img_body_Selector;
    QString img_attribute;

};

#endif // RJUTILITY_H