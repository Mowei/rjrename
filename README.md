RJ改名工具
=====

## 開發版本
QT版本: Qt 5.5.1 mingw492_32     
QT套件: webkit,webkitwidgets     

## 打包
upx > Enigma Virtual Box     

## config.ini 說明
#DLsite連結網址 {RJNumber}將會填入抓取到的RJNumber值     
DLsiteUrlBase="http://www.dlsite.com/maniax/work/=/product_id/{RJNumber}"     

#自定義重新命名檔名規則     
FormatNameTemplate=[{maker_name}][{saleDate}][{RJNumber}]{work_name}{rjtype}     

#從網頁取得RJNumber的Selector     
#此Selector會取得網頁URL重新辨識RJNumber     
RJNumber_URL_Selector=#work_name a     


#從網頁取得檔案描述群組的Selector     
#描述群組包含社團名稱,標題名稱,販售日,作品形式     
work_outline_Selector=#work_outline     
#從描述群組取得社團名稱的Selector     
maker_name_Selector=.maker_name     
#從描述群組取得標題名稱的Selector     
work_name_Selector=#work_name     


#從網頁取得流覽圖群組的Selector     
img_body_Selector=.product-slider-data div     
#從網頁取得流覽圖群組內的來源屬性     
img_attribute=data-src     


## 展示圖片
<a href="http://imgur.com/VKX2HeZ"><img src="http://i.imgur.com/VKX2HeZ.png" title="source: imgur.com" /></a>
<a href="http://imgur.com/Q7JqYBJ"><img src="http://i.imgur.com/Q7JqYBJ.png" title="source: imgur.com" /></a>
