#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QHostAddress>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket (new QTcpSocket(this))
    , timerPrices(nullptr)
{
    ui->setupUi(this);

    connect(socket, &QTcpSocket::connected, this, &MainWindow::onSocketConnected);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::onSocketDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onSocketReadyRead);
    connect(socket, &QAbstractSocket::errorOccurred, this, &MainWindow::onSocketError);

    socket->connectToHost(QHostAddress::LocalHost, 5555);

    for (int i = 0; i<5; i++){
        stocks[i] = 0;
        prices[i] = 10 + 10*i;



        buySliders[i] = new QSlider(Qt::Horizontal, this);
        buySliders[i]->setParent(ui->groupBox);
        buySliders[i]->resize(160, 16);
        buySliders[i]->move(100, 50+50 * i);
        buySliders[i]->setRange(0,100);
        buySliders[i]->show();

        connect(buySliders[i], &QSlider::valueChanged, [this,i](int value)
                {on_AnyBuySlider_change(i, value);});

        sellSliders[i] = new QSlider(Qt::Horizontal, this);
        sellSliders[i]->setParent(ui->groupBox_4);
        sellSliders[i]->resize(160, 16);
        sellSliders[i]->move(100, 50+50 * i);
        sellSliders[i]->setRange(0,100);
        sellSliders[i]->show();
        sellSliders[i]->setMinimum(0);
        sellSliders[i]->setMaximum(0);


        connect(sellSliders[i], &QSlider::valueChanged,
                [this, i](int value) {on_AnySellSlider_change(i, value);});

        buyLabel[i] = new QLabel();
        buyLabel[i]->setParent(ui->groupBox);
        buyLabel[i]->resize(49, 16);
        buyLabel[i]->move(270, 50+50 * i);
        buyLabel[i]->setText("0$");
        buyLabel[i]->show();

        sellLabel[i] = new QLabel();
        sellLabel[i]->setParent(ui->groupBox_4);
        sellLabel[i]->resize(49, 16);
        sellLabel[i]->move(270, 50+50 * i);
        sellLabel[i]->setText("0$");
        sellLabel[i]->show();



    }
    ui->money->setText("Money:"+ QString::number(money)+"$");

    timerPrices = new QTimer(this);
    timerPrices->setInterval(1000);
    connect(timerPrices, &QTimer::timeout, this, &MainWindow::on_timerPrices_tick);
    timerPrices->start();
}

MainWindow::~MainWindow()
{
    if (socket->isOpen()){
        socket->disconnectFromHost();
        socket->waitForDisconnected(1000);
    }
    delete ui;
}

void MainWindow::sendJson(const QJsonObject &obj){
    if (socket->state() != QTcpSocket::ConnectedState){
        QMessageBox::warning(this, "no connection", "failed to send a command");
        return;

    }
    QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
    socket->write(data);
}

void MainWindow::requestStatus(){
    QJsonObject cmd;
    cmd["type"] = "getStatus";
    sendJson(cmd);
}

void MainWindow::parseMessage(const QByteArray &line){
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(line, &err);
    if (err.error != QJsonParseError::NoError){
        qDebug() << "JSON parse error: " << err.errorString();
        return;
    }
    QJsonObject obj = doc.object();
    QString type = obj["type"].toString();
    if (type == "welcome"){
        qDebug() << "Welcome: " << obj["message"].toString();
        requestStatus();
    }
    else if (type == "statusResponse") {
        money = obj["money"].toInt();
        QJsonArray pricesArr = obj["price"].toArray();
        QJsonArray stocksArr = obj["stocks"].toArray();

        for (int i = 0; i < STOCK_COUNT && i < pricesArr.size(); i++){
            prices[i] = pricesArr[i].toInt();
            stocks[i] = stocksArr[i].toInt();
            sellSliders[i]->setMaximum(stocks[i]);
        }

    }
    else if (type == "priceUpdate"){
        QJsonArray pricesArr = obj["prices"].toArray();
        for (int i = 0; i < STOCK_COUNT && i < pricesArr.size(); i++){
            prices[i] = pricesArr[i].toInt();

        }
        for (int i =0; i < STOCK_COUNT; i++){
            buyLabel[i] -> setText(QString::number(prices[i] * buySliders[i]->value()) + "$");
            sellLabel[i] -> setText(QString::number(prices[i] * sellSliders[i]->value()) + "$");
        }
    }
    else if (type == "buyResult"){
        if (obj["success"].toBool()){
            money = obj["money"].toInt();
            ui->money->setText("Money: " + QString::number(money) + "$");
            requestStatus();
        }
        else{
            QMessageBox::warning(this, "buying error", obj["error"].toString());
        }

    }
    else if (type == "sellResult"){
        if (obj["success"].toBool()){
            money = obj["money"].toInt();
            ui->money->setText("Money: " + QString::number(money) + "$");
            requestStatus();
        }
        else{
            QMessageBox::warning(this, "selling error", obj["error"].toString());
        }
    }
    else if (type == "error"){
        QMessageBox::critical(this, "server error", obj["message"].toString());
    }
}

void MainWindow::updateUIFromServer(int money, const QVector<int> &prices, const QVector<int> &stocks){
    ui->money->setText("money: "+QString::number(money)+"$");
    for (int i=0; i<STOCK_COUNT; i++){
        sellSliders[i]->setMaximum(stocks[i]);
        sellSliders[i]->setValue(0);
        buyLabel[i]->setText(QString::number(prices[i]*buySliders[i]->value())+"$");
        sellLabel[i]->setText(QString::number(prices[i]*sellSliders[i]->value())+"$");
    }

}

void MainWindow::onSocketConnected(){

}

void MainWindow::onSocketDisconnected(){
    for (int i = 0; i<STOCK_COUNT; i++){
        buySliders[i]->setEnabled(false);
        sellSliders[i]->setEnabled(false);
    }
    ui->buyButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
}

void MainWindow::onSocketReadyRead(){
    socketBuffer.append(socket->readAll());
    while(socketBuffer.contains('\n')){
        int pos = socketBuffer .indexOf('\n');
        QByteArray line = socketBuffer.left(pos).trimmed();
        socketBuffer = socketBuffer.mid(pos + 1);

        if (line.isEmpty()) continue;
        parseMessage(line);
    }
}

void MainWindow::onSocketError(QAbstractSocket::SocketError error){
    Q_UNUSED(error);
    qDebug() << "Socket error: " << socket->errorString();

}


void MainWindow::on_AnyBuySlider_change(int index, int value){
    buyLabel[index]->setText(QString::number(prices[index]*value)+"$");
}

void MainWindow::on_AnySellSlider_change(int index, int value){
    sellLabel[index]->setText(QString::number(prices[index]*value)+"$");
}

void MainWindow::on_timerPrices_tick(){
    for (int i=0; i<STOCK_COUNT; i++){
        buyLabel[i]->setText(QString::number(prices[i]*buySliders[i]->value())+"$");
        sellLabel[i]->setText(QString::number(prices[i]*sellSliders[i]->value())+"$");
    }
}


void MainWindow::on_buyButton_clicked()
{
    QVector<int> amounts(STOCK_COUNT);
    for (int i = 0; i < STOCK_COUNT; i++){
        amounts[i] = buySliders[i]->value();

    }
    QJsonObject cmd;
    cmd["type"] = "buy";
    QJsonArray arr;
    for (int v : amounts) arr.append(v);
    cmd["amounts"] = arr;
    sendJson(cmd);
    for (int i = 0; i < STOCK_COUNT; i++){
        buySliders[i]->setValue(0);
        buyLabel[i]->setText("0$");
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    QVector<int> amounts(STOCK_COUNT);
    for (int i = 0; i < STOCK_COUNT; i++){
        amounts[i] = sellSliders[i]->value();

    }
    QJsonObject cmd;
    cmd["type"] = "sell";
    QJsonArray arr;
    for (int v : amounts) arr.append(v);
    cmd["amounts"] = arr;
    sendJson(cmd);
    for (int i = 0; i < STOCK_COUNT; i++){
        sellSliders[i]->setValue(0);
        sellLabel[i]->setText("0$");
    }




}

