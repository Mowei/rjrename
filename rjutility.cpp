#include "rjutility.h"
#include <QWidget>
#include <QSettings>
#include <QtWebKit>
#include <QtWebKitWidgets>

RJUtility::RJUtility(QWidget *parent)
    : QWidget(parent)
{

    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("RJNameConfig");
    UrlBase=setting.value("DLsiteUrlBase").toString();
    FormatNameTemplate=setting.value("FormatNameTemplate").toString();
    RJNumber_URL_Selector=setting.value("RJNumber_URL_Selector").toString();
    maker_name_Selector = setting.value("maker_name_Selector").toString();
    work_name_Selector = setting.value("work_name_Selector").toString();
    work_outline_Selector = setting.value("work_outline_Selector").toString();
    img_body_Selector = setting.value("img_body_Selector").toString();
    img_attribute = setting.value("img_Attribute").toString();
}

QString RJUtility::NameCheck(QString newname)
{
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
    return newname;
}

QString RJUtility::GetRJname(QString filename)
{
    QRegExp rx("(RJ\\d{6})");
    rx.setMinimal(true);
    rx.indexIn(filename.toUpper(), 0);
    return rx.cap(1);
}

QString RJUtility::GetFormatName(QString src)
{
    QString name = FormatNameTemplate;

    QWebView *webView =new QWebView();
    webView->setHtml(src);
    QWebElement document = webView->page()->mainFrame()->documentElement();
    //QWebElementCollection elements = document.findAll(work_name_Selector);

    //RJ號
    QString RJNumber = GetRJname(document.findFirst(RJNumber_URL_Selector).attribute("href"));
    //class = maker_name 社團
    QString maker_name = document.findAll(maker_name_Selector).first().toPlainText();
    //id = work_name 名稱
    QString work_name = document.findAll(work_name_Selector).first().toPlainText();
    //id = work_outline 描述
    QWebElement work_outline = document.findAll(work_outline_Selector).first();

    QList<QWebElement>  fields  = work_outline.findAll("tbody tr th").toList();
    QList<QWebElement>  values = work_outline.findAll("tbody tr td").toList();

    int findSaleDateRow =-1;
    int findRJTypesRow =-1;
    for(int i =0; i<fields.length();i++){
        if(fields.at(i).toPlainText()=="販売日"){
            findSaleDateRow = i;
        }
        if(fields.at(i).toPlainText()=="作品形式"){
            findRJTypesRow = i;
        }
    }
    if(findSaleDateRow==-1 || findRJTypesRow==-1){
        return "";
    }

    //販售日
    QString saleDate =values.at(findSaleDateRow).toPlainText()
            .replace("年","")
            .replace("月","")
            .replace("日","")
            .right(6);

    //類型
    QString rjtype;
    QWebElementCollection rjtypes = values.at(findRJTypesRow).findAll("a");
    foreach (QWebElement item , rjtypes){
        rjtype +="("+item.toPlainText() +")";
    }

    name =name.replace("{maker_name}",maker_name)
            .replace("{saleDate}",saleDate)
            .replace("{RJNumber}",RJNumber)
            .replace("{work_name}",work_name)
            .replace("{rjtype}",rjtype);

    return name;
}
QString RJUtility::GetUrlByRJNumber(QString rjNumber)
{
    QString base = UrlBase;
    return base.replace("{RJNumber}",rjNumber);
}
QByteArray RJUtility::DownloadInfo(QString path)
{
    QUrl url(path);
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply = manager.get(QNetworkRequest(url));
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    QByteArray data = reply->readAll();
    return data;
}

QStringList RJUtility::GetImageUrls(QString src)
{
    QStringList Urls;
    if(!src.isEmpty()){
        QWebView *webView =new QWebView();
        webView->setHtml(src);
        QWebElement document = webView->page()->mainFrame()->documentElement();

        QList<QWebElement> img_body = document.findAll(img_body_Selector).toList();

        for(int i =0; i<img_body.length();i++){
            Urls<<"http:"+img_body.at(i).attribute(img_attribute);
        }
    }
    return Urls;
}
