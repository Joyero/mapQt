#pragma once

#include <QMainWindow>
#include <QWebEngineView>
#include <QtWidgets/QApplication>

#include <QtQuickWidgets/QQuickWidget>
#include <QtWidgets/QStatusBar>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void clearValues();
    ~MainWindow();

private slots:
    void readData();
    void finishReading();

private:
    int retriesCounter;
    QString apiKey;
    QString coordinate;
    QWebEngineView *m_view;
    QStatusBar *statusbar;
    QNetworkAccessManager *networkAccessManager;
    QNetworkReply *networkReply;
    QByteArray receivedData;

    void mousePressEvent(QMouseEvent *event);
    void requestInfoFromApi();
};

