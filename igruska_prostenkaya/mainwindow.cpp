#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>
#include <QFont>
#include <climits>
#include <cstdlib>

static const QColor STOCK_COLORS[5] = {
    Qt::red, Qt::blue, Qt::green, QColor(255, 140, 0), Qt::magenta
};
static const char *STOCK_NAMES[5] = { "pear", "MacroHard", "Edison", "litiy", "nikita" };

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    priceHistory.resize(STOCK_COUNT);

    for (int i = 0; i < STOCK_COUNT; i++) {
        stocks[i] = 0;
        prices[i] = 10 + 10 * i;
        priceHistory[i].append(prices[i]);

        buySliders[i] = new QSlider(Qt::Horizontal, ui->groupBox);
        buySliders[i]->resize(160, 16);
        buySliders[i]->move(100, 50 + 50 * i);
        buySliders[i]->setRange(0, 100);
        buySliders[i]->show();
        connect(buySliders[i], &QSlider::valueChanged, [this, i](int v) {
            on_AnyBuySlider_change(i, v);
        });

        sellSliders[i] = new QSlider(Qt::Horizontal, ui->groupBox_4);
        sellSliders[i]->resize(160, 16);
        sellSliders[i]->move(100, 50 + 50 * i);
        sellSliders[i]->setRange(0, 0);
        sellSliders[i]->show();
        connect(sellSliders[i], &QSlider::valueChanged, [this, i](int v) {
            on_AnySellSlider_change(i, v);
        });

        buyLabel[i] = new QLabel("0$", ui->groupBox);
        buyLabel[i]->resize(49, 16);
        buyLabel[i]->move(270, 50 + 50 * i);
        buyLabel[i]->show();

        sellLabel[i] = new QLabel("0$", ui->groupBox_4);
        sellLabel[i]->resize(49, 16);
        sellLabel[i]->move(270, 50 + 50 * i);
        sellLabel[i]->show();
    }

    ui->money->setText("Money: " + QString::number(money) + "$");
    updateProfit();

    graphScene = new QGraphicsScene(this);
    ui->graphicsView->setScene(graphScene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setBackgroundBrush(QColor(20, 20, 30));

    timerPrices = new QTimer(this);
    timerPrices->setInterval(10000);
    connect(timerPrices, &QTimer::timeout, this, &MainWindow::on_timerPrices_tick);
    timerPrices->start();

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &MainWindow::onSocketConnected);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onSocketReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::onSocketDisconnected);
    socket->connectToHost("127.0.0.1", 5555);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_AnyBuySlider_change(int index, int value)
{
    buyLabel[index]->setText(QString::number(prices[index] * value) + "$");
}

void MainWindow::on_AnySellSlider_change(int index, int value)
{
    sellLabel[index]->setText(QString::number(prices[index] * value) + "$");
}

void MainWindow::on_timerPrices_tick()
{
    for (int i = 0; i < STOCK_COUNT; i++) {
        if (rand() % 2 == 1)
            prices[i] += 1 + rand() % 10;
        else {
            prices[i] -= 1 + rand() % 10;
            if (prices[i] < 1) prices[i] = 1;
        }
        buyLabel[i]->setText(QString::number(prices[i] * buySliders[i]->value()) + "$");
        sellLabel[i]->setText(QString::number(prices[i] * sellSliders[i]->value()) + "$");
        priceHistory[i].append(prices[i]);
        if (priceHistory[i].size() > HISTORY_SIZE)
            priceHistory[i].removeFirst();
    }
    updateGraph();
    updateProfit();
}

void MainWindow::onSocketConnected()
{
    connectedToServer = true;
    timerPrices->stop();
    QJsonObject cmd;
    cmd["type"] = "getStatus";
    sendToServer(cmd);
}

void MainWindow::onSocketReadyRead()
{
    socketBuffer.append(socket->readAll());
    while (socketBuffer.contains('\n')) {
        int idx = socketBuffer.indexOf('\n');
        QByteArray line = socketBuffer.left(idx);
        socketBuffer = socketBuffer.mid(idx + 1);
        if (line.isEmpty()) continue;
        QJsonDocument doc = QJsonDocument::fromJson(line);
        if (!doc.isNull())
            processServerMessage(doc.object());
    }
}

void MainWindow::onSocketDisconnected()
{
    connectedToServer = false;
    timerPrices->start();
}

void MainWindow::sendToServer(const QJsonObject &cmd)
{
    socket->write(QJsonDocument(cmd).toJson(QJsonDocument::Compact) + "\n");
}

void MainWindow::processServerMessage(const QJsonObject &msg)
{
    QString type = msg["type"].toString();

    if (type == "welcome") {
        QJsonObject cmd;
        cmd["type"] = "getStatus";
        sendToServer(cmd);
    } else if (type == "statusResponse") {
        money = msg["money"].toInt();
        QJsonArray pricesArr = msg["prices"].toArray();
        QJsonArray stocksArr = msg["stocks"].toArray();
        for (int i = 0; i < STOCK_COUNT; i++) {
            prices[i] = pricesArr[i].toInt();
            stocks[i] = stocksArr[i].toInt();
            sellSliders[i]->setMaximum(stocks[i]);
            buyLabel[i]->setText(QString::number(prices[i] * buySliders[i]->value()) + "$");
            sellLabel[i]->setText(QString::number(prices[i] * sellSliders[i]->value()) + "$");
            if (priceHistory[i].isEmpty() || priceHistory[i].last() != prices[i])
                priceHistory[i].append(prices[i]);
            if (priceHistory[i].size() > HISTORY_SIZE)
                priceHistory[i].removeFirst();
        }
        ui->money->setText("Money: " + QString::number(money) + "$");
        updateGraph();
        updateProfit();
    } else if (type == "priceUpdated") {
        QJsonArray pricesArr = msg["prices"].toArray();
        for (int i = 0; i < STOCK_COUNT; i++) {
            prices[i] = pricesArr[i].toInt();
            buyLabel[i]->setText(QString::number(prices[i] * buySliders[i]->value()) + "$");
            sellLabel[i]->setText(QString::number(prices[i] * sellSliders[i]->value()) + "$");
            priceHistory[i].append(prices[i]);
            if (priceHistory[i].size() > HISTORY_SIZE)
                priceHistory[i].removeFirst();
        }
        updateGraph();
        updateProfit();
    } else if (type == "buyResult" || type == "sellResult") {
        if (msg["success"].toBool()) {
            money = msg["money"].toInt();
            ui->money->setText("Money: " + QString::number(money) + "$");
            QJsonObject cmd;
            cmd["type"] = "getStatus";
            sendToServer(cmd);
        }
    }
}

void MainWindow::updateGraph()
{
    graphScene->clear();
    double w = ui->graphicsView->width() - 4;
    double h = ui->graphicsView->height() - 30;
    graphScene->setSceneRect(0, 0, w, ui->graphicsView->height() - 4);

    int minPrice = INT_MAX, maxPrice = 0, maxLen = 0;
    for (int i = 0; i < STOCK_COUNT; i++) {
        maxLen = qMax(maxLen, priceHistory[i].size());
        for (int p : priceHistory[i]) {
            minPrice = qMin(minPrice, p);
            maxPrice = qMax(maxPrice, p);
        }
    }
    if (maxLen < 2) return;
    if (maxPrice == minPrice) { maxPrice++; minPrice--; }

    QPen gridPen(QColor(60, 60, 80));
    for (int g = 0; g <= 4; g++) {
        double y = g * h / 4;
        graphScene->addLine(0, y, w, y, gridPen);
        int priceAtLine = maxPrice - g * (maxPrice - minPrice) / 4;
        QGraphicsTextItem *txt = graphScene->addText(QString::number(priceAtLine));
        txt->setDefaultTextColor(QColor(150, 150, 150));
        txt->setFont(QFont("Arial", 7));
        txt->setPos(2, y);
    }

    for (int i = 0; i < STOCK_COUNT; i++) {
        const QVector<int> &hist = priceHistory[i];
        if (hist.size() < 2) continue;
        QPen pen(STOCK_COLORS[i]);
        pen.setWidth(2);
        for (int j = 1; j < hist.size(); j++) {
            double x1 = (double)(j - 1) / (maxLen - 1) * w;
            double x2 = (double)j / (maxLen - 1) * w;
            double y1 = h - (double)(hist[j-1] - minPrice) / (maxPrice - minPrice) * h;
            double y2 = h - (double)(hist[j] - minPrice) / (maxPrice - minPrice) * h;
            graphScene->addLine(x1, y1, x2, y2, pen);
        }
    }

    for (int i = 0; i < STOCK_COUNT; i++) {
        double lx = i * (w / STOCK_COUNT) + 5;
        double ly = h + 5;
        graphScene->addRect(lx, ly + 3, 10, 10, QPen(Qt::NoPen), QBrush(STOCK_COLORS[i]));
        QGraphicsTextItem *txt = graphScene->addText(STOCK_NAMES[i]);
        txt->setDefaultTextColor(STOCK_COLORS[i]);
        txt->setFont(QFont("Arial", 8));
        txt->setPos(lx + 13, ly);
    }
}

void MainWindow::updateProfit()
{
    int portfolioValue = 0;
    for (int i = 0; i < STOCK_COUNT; i++)
        portfolioValue += stocks[i] * prices[i];
    int profit = money + portfolioValue - initialMoney;
    QString sign = profit >= 0 ? "+" : "";
    ui->profit->setText("Profit: " + sign + QString::number(profit) + "$");
}

void MainWindow::on_buyButton_clicked()
{
    if (connectedToServer) {
        QJsonArray amounts;
        for (int i = 0; i < STOCK_COUNT; i++)
            amounts.append(buySliders[i]->value());
        QJsonObject cmd;
        cmd["type"] = "buy";
        cmd["amounts"] = amounts;
        sendToServer(cmd);
    } else {
        int cost = 0;
        for (int i = 0; i < STOCK_COUNT; i++)
            cost += prices[i] * buySliders[i]->value();
        if (cost <= money) {
            money -= cost;
            for (int i = 0; i < STOCK_COUNT; i++) {
                stocks[i] += buySliders[i]->value();
                sellSliders[i]->setMaximum(stocks[i]);
            }
            ui->money->setText("Money: " + QString::number(money) + "$");
            updateProfit();
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if (connectedToServer) {
        QJsonArray amounts;
        for (int i = 0; i < STOCK_COUNT; i++)
            amounts.append(sellSliders[i]->value());
        QJsonObject cmd;
        cmd["type"] = "sell";
        cmd["amounts"] = amounts;
        sendToServer(cmd);
    } else {
        int income = 0;
        for (int i = 0; i < STOCK_COUNT; i++)
            income += prices[i] * sellSliders[i]->value();
        money += income;
        for (int i = 0; i < STOCK_COUNT; i++) {
            stocks[i] -= sellSliders[i]->value();
            sellSliders[i]->setMaximum(stocks[i]);
        }
        ui->money->setText("Money: " + QString::number(money) + "$");
        updateProfit();
    }
}
