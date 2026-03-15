#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class StocksEngine : public QObject{
    Q_OBJECT
public:
    explicit StocksEngine(QObject *parent = nullptr);
    ~StocksEngine();
    bool buyStocks(const QVector<int>& amounts);
    bool sellStocks(const QVector<int>& amounts);

    QJsonDocument getStatus() const;

signals:
    void pricesUpdated();

private slots:
    void onTimerPricesTick();

private:
    int money;
    QVector<int> stocks;
    QVector<int> prices;
    QTimer *timerPrices;

    static const int STOCK_COUNT = 5;

};

#endif // MAINWINDOW_H
