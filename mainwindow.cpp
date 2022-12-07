#include "mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QStringList>
#include <QClipboard>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_view(new QWebEngineView(this))
{
    this->setWindowTitle("maps - API");
    m_view->setMouseTracking(true);
    QWebEnginePage *page = m_view->page();
    connect(page, &QWebEnginePage::featurePermissionRequested,
            [ page](const QUrl &securityOrigin, QWebEnginePage::Feature feature) {
        if (feature != QWebEnginePage::Geolocation)
            return;
        page->setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionGrantedByUser);
        page->setZoomFactor(0.25);
    });
    networkAccessManager = new QNetworkAccessManager(this);
    networkReply = nullptr;
    retriesCounter = 0;
    apiKey = "7b4bfcab334a20111eeb3f9eefcff206";
    statusbar = new QStatusBar(parent);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    statusbar->showMessage(QString("copy a coordinate and then click on this area"));
    this->setStatusBar(statusbar);
    setCentralWidget(m_view);
    setFixedSize(801,601);
    page->load(QUrl(QStringLiteral("https://maps.google.com")));
}


void MainWindow::clearValues()
{
    receivedData.clear();
}


MainWindow::~MainWindow()
{
}


void MainWindow::readData()
{
    receivedData.append(networkReply->readAll());
}

void MainWindow::finishReading()
{
    if(networkReply->error() != QNetworkReply::NoError){
        qDebug() << "Error : " << networkReply->errorString();
        statusbar->showMessage(QString("Api error: %1").arg(networkReply->errorString()));
    }else{
        //CONVERT THE DATA FROM A JSON DOC TO A JSON OBJECT
        QJsonParseError jsonError;
        QJsonObject apiJsonInfo = QJsonDocument::fromJson(receivedData, &jsonError).object();
        int tamanoData = receivedData.size();
        qDebug() << QString(receivedData);
        receivedData.clear();
        qDebug() << apiJsonInfo.size() << ".  data=>" << tamanoData;
        if( jsonError.error != QJsonParseError::NoError )
        {
             qDebug() << QString("Json error: %1").arg(jsonError.errorString());
             if(retriesCounter++ <=10)
             {
                requestInfoFromApi();
             }
             else
             {
                retriesCounter = 0;
                statusbar->showMessage("api doesn't respond");
             }
             return;
        }
        if (apiJsonInfo.isEmpty()) {
            qDebug() << "JSON object is empty.";
            if(retriesCounter++ <=10)
            {
               requestInfoFromApi();
            }
            else
            {
               retriesCounter = 0;
            }
            return;
        }
        qDebug() << "JSON OK";
        retriesCounter = 0;
        QJsonArray jsonArrayData = apiJsonInfo.value("data").toArray();
        QString  qStrInfo = QString("[REGION: no info]  [COUNTRY: no info]  [COUNTRY_CODE: no info]  [CONTINENT no info]");
        foreach (const QJsonValue & value, jsonArrayData)
        {
            QJsonObject objJson = value.toObject();
            if (objJson.value("region").toString().isEmpty()==false &&
                objJson.value("country").toString().isEmpty()==false    )
            {
                qDebug()  << "objJson.value(\"region\")=>"  << objJson.value("region").toString();
                qDebug()  << "objJson.value(\"country\")=>"  << objJson.value("country").toString();
                qDebug()  << "objJson.value(\"country_code\")=>"  << objJson.value("country_code").toString();
                qDebug()  << "objJson.value(\"continent\")=>"  << objJson.value("continent").toString();
                qStrInfo = QString("[REGION: %1]  [COUNTRY: %2]  [COUNTRY_CODE: %3]  [CONTINENT: %4]").arg(
                            objJson.value("region").toString(),
                            objJson.value("country").toString(),
                            objJson.value("country_code").toString(),
                            objJson.value("continent").toString()  );
                break;
            }
        }
        qDebug()  << "qStrInfo =>"  << qStrInfo;
        statusbar->showMessage(qStrInfo);
    }
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    bool isLat = false, isLon = false;
    double latitude = 0, longitude = 0;
    QString currentCoordinate = QApplication::clipboard()->text().remove(" ");
    QStringList list = currentCoordinate.split(',');
    if (list.size()!=2)
    {
        statusbar->showMessage("There isn't a coordinate on clipboard");
        return;
    }
    latitude = list.at(0).toDouble(&isLat);
    longitude = list.at(1).toDouble(&isLon);
    qDebug() << "latitude : " << latitude;
    qDebug() << "longitude: " << longitude;
    qDebug() << "isLat = " << isLat;
    qDebug() << "isLon = " << isLon;
    if ( ( (isLat==false  || isLon==false)  ||
           (latitude>90   || latitude <-90) ||
           (longitude>180 || longitude<-180)  )== true  )
    {
        statusbar->showMessage("There isn't a valid coordinate on clipboard");
        return;
    }
    coordinate = currentCoordinate;
    statusbar->showMessage("looking for information...");
    requestInfoFromApi();
    if (event->button()== Qt::LeftButton)
    {
    }
    else if (event->button()== Qt::RightButton)
    {
    }
}


void MainWindow::requestInfoFromApi()
{
    QString urlApi =QString("http://api.positionstack.com/v1/reverse?access_key=%1&query=%2").arg(apiKey,coordinate.remove(" "));
    qDebug() << coordinate.remove(" ");
    qDebug() << "urlApi=> " << urlApi;
    QNetworkRequest request{QUrl(urlApi)};
    networkReply = networkAccessManager->get(request);
    connect(networkReply,&QNetworkReply::readyRead,this,&MainWindow::readData);
    connect(networkReply,&QNetworkReply::finished,this,&MainWindow::finishReading);
}

