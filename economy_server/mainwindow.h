#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSqlDatabase>
#include <QSqlQuery>


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

    void onNewConnection();

    void onClientReadyRead();

    void onClientDisconnected();

private:
    int money;
    QVector<int> stocks;
    QVector<int> prices;
    QTimer *timerPrices;

    static const int STOCK_COUNT = 5;

    QTcpServer *tcpServer;
    QMap<QTcpSocket*, QByteArray> clientBuffers;
    QSqlDatabase db;

    void processCommand(QTcpSocket *socket, const QJsonObject &cmd);
    void sendError(QTcpSocket *socket, const QString &msg);
    void broadcastStatus();

    void initDb();
    void loadState();
    void savePlayerState();
    void savePriceHistory();

};

#endif // MAINWINDOW_H
