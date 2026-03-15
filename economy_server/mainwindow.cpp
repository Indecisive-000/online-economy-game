#include "mainwindow.h"
#include <QTime>
#include <QtGlobal>

StocksEngine::StocksEngine(QObject *parent) : QObject(parent){
    money = 10000;
    stocks.fill(0, STOCK_COUNT);
    for (int i=0; i<STOCK_COUNT; i++){
        prices.append(10+10*i);

    }
    timerPrices = new QTimer(this);
    timerPrices->setInterval(10000);
    connect(timerPrices, &QTimer::timeout, this, &StocksEngine::onTimerPricesTick);
    timerPrices->start();

    srand(QTime::currentTime().msec());

}

StocksEngine::~StocksEngine(){
    delete timerPrices;

}

void StocksEngine::onTimerPricesTick(){
    for (int i=0; i<5; i++){
        if (rand()%2==1){
            prices[i] += 1 + rand() % 10;
        }
        else{
            prices[i] -= 1 + rand() % 10;
            if (prices[i]< 1) {
                prices[i]=1;
            }
        }
    }
    emit pricesUpdated();
}
bool StocksEngine::buyStocks(const QVector<int> &amounts){
    if (amounts.size() != STOCK_COUNT) return false;

    int cost =0;
    for (int i = 0; i<STOCK_COUNT; i++){
        cost += prices[i] * amounts[i];

    }
    if (cost<=money){
        money-=cost;
        for (int i=0; i<STOCK_COUNT; i++){
            stocks[i] += amounts[i];
        }
        return true;
    }
    return false;

}

bool StocksEngine::sellStocks(const QVector<int> &amounts)
{
    if (amounts.size() != STOCK_COUNT) return false;

    for (int i = 0; i< STOCK_COUNT; i++){
        if (amounts[i] > stocks[i]) return false;
    }

    int income = 0;
    for (int i = 0; i< STOCK_COUNT; i++){
        income += prices[i] * amounts[i];
        stocks[i] -= amounts[i];
    }

    money+=income;
    return true;
}

QJsonDocument StocksEngine::getStatus() const{
    QJsonObject root;
    root["money"] = money;
    QJsonArray pricesArr, stocksArr;
    for (int i =0; i < STOCK_COUNT; i++){
        pricesArr.append(prices[i]);
        stocksArr.append(stocks[i]);

    }
    root["prices"] = pricesArr;
    root["stocks"] = stocksArr;

    return QJsonDocument(root);

}


