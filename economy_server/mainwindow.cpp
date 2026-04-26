#include "mainwindow.h"
#include <QTime>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QHostAddress>
#include <QSqlError>
#include <QDateTime>


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

    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, 5555)){
        qDebug()<<"не удалось запустить tcp server:" << tcpServer->errorString();

    }
    else{
        qDebug()<<"TCP сервер запущен на порту 5555";

    }
    connect(tcpServer, &QTcpServer::newConnection, this, &StocksEngine::onNewConnection);

    srand(QTime::currentTime().msec());

    initDb();
}

StocksEngine::~StocksEngine(){
    delete timerPrices;
    tcpServer->close();

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
    savePriceHistory();
    broadcastStatus();
}

// network logic

void StocksEngine::onNewConnection(){
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &StocksEngine::onClientReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &StocksEngine::onClientDisconnected);
    clientBuffers[clientSocket] = QByteArray();

    qDebug() << "new client" << clientSocket->peerAddress();

    QJsonObject welcome;
    welcome["type"] = "welcome";
    welcome["message"] = "Connected StcoksEngine";
    clientSocket->write(QJsonDocument(welcome).toJson(QJsonDocument::Compact));
    clientSocket->write("\n");

}

void StocksEngine::onClientReadyRead(){
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    clientBuffers[socket].append(socket->readAll());

    QByteArray &buffer = clientBuffers[socket];
    while (buffer.contains("\n")){
        int indexOfNewLine = buffer.indexOf("\n");
        QByteArray message = buffer.left(indexOfNewLine);
        buffer = buffer.mid(indexOfNewLine +1);
        if (message.isEmpty()) continue;
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(message, &parseError);

        if (parseError.error != QJsonParseError::NoError){
            sendError(socket, "Invalid Json");
            continue;
        }
        processCommand(socket, doc.object());
    }
}

void StocksEngine::onClientDisconnected(){
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket){
        clientBuffers.remove(socket);
        qDebug() << "client disconected" << socket->peerAddress();
        socket->deleteLater();
    }
}

void StocksEngine::processCommand(QTcpSocket *socket, const QJsonObject &cmd){
    QString type = cmd["type"].toString();
    QJsonObject response;

    if (type == "getStatus"){
        QJsonDocument statusDoc = getStatus();
        response = statusDoc.object();
        response["type"] = "statusResponse";

    }
    else if (type == "sell"){
        QJsonArray amountArr = cmd["amounts"].toArray();
        QVector<int> amounts;
        for (const auto &val : amountArr){
            amounts.append(val.toInt());
        }
        if (sellStocks(amounts)){
            response["type"] = "sellResult";
            response["success"] = true;
            response["money"] = money;
            savePlayerState();
        }
        else{
            response["type"] = "sellResult";
            response["success"] = false;
            response["error"] = "Not Enough Stocks";
        }
    }
    else if (type == "buy"){
        QJsonArray amountArr = cmd["amounts"].toArray();
        QVector<int> amounts;
        for (const auto &val : amountArr){
            amounts.append(val.toInt());
        }
        if (buyStocks(amounts)){
            response["type"] = "buyResult";
            response["success"] = true;
            response["money"] = money;
            savePlayerState();
        }
        else{
            response["type"] = "buyResult";
            response["success"] = false;
            response["error"] = "Not money";
        }
    }
    else{
        sendError(socket, "Unknown command" + type);
        return;
    }
    socket -> write (QJsonDocument(response).toJson(QJsonDocument::Compact) +"\n");

}
void StocksEngine::broadcastStatus(){
    QJsonObject update;
    update["type"] = "priceUpdated";

    QJsonArray priceArray;
    for (int p:prices){
        priceArray.append(p);
    }
    update["prices"] = priceArray;

    QByteArray data= QJsonDocument(update).toJson(QJsonDocument::Compact) + "\n";

    for (QTcpSocket *client : clientBuffers.keys()){
        if (client && client->state() == QTcpSocket::ConnectedState){
            client->write(data);
        }
    }
}
void StocksEngine::sendError(QTcpSocket *socket, const QString &msg){
    QJsonObject error;
    error["type"] = "error";
    error["message"] = msg;
    socket->write(QJsonDocument(error).toJson(QJsonDocument::Compact) + "\n");

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

void StocksEngine::initDb()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("economy.db");
    if (!db.open()) {
        qDebug() << "DB open failed:" << db.lastError().text();
        return;
    }

    QSqlQuery q;
    q.exec("CREATE TABLE IF NOT EXISTS player_state ("
           "id INTEGER PRIMARY KEY, money INTEGER, stocks TEXT, prices TEXT)");
    q.exec("CREATE TABLE IF NOT EXISTS price_history ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
           "timestamp INTEGER, stock_index INTEGER, price INTEGER)");

    loadState();
}

void StocksEngine::loadState()
{
    QSqlQuery q("SELECT money, stocks, prices FROM player_state WHERE id=1");
    if (!q.next()) return;

    money = q.value(0).toInt();

    QJsonDocument stocksDoc = QJsonDocument::fromJson(q.value(1).toString().toUtf8());
    QJsonArray stocksArr = stocksDoc.array();
    for (int i = 0; i < STOCK_COUNT && i < stocksArr.size(); i++)
        stocks[i] = stocksArr[i].toInt();

    QJsonDocument pricesDoc = QJsonDocument::fromJson(q.value(2).toString().toUtf8());
    QJsonArray pricesArr = pricesDoc.array();
    for (int i = 0; i < STOCK_COUNT && i < pricesArr.size(); i++)
        prices[i] = pricesArr[i].toInt();

    qDebug() << "State loaded: money=" << money;
}

void StocksEngine::savePlayerState()
{
    QJsonArray stocksArr, pricesArr;
    for (int i = 0; i < STOCK_COUNT; i++) {
        stocksArr.append(stocks[i]);
        pricesArr.append(prices[i]);
    }
    QSqlQuery q;
    q.prepare("INSERT OR REPLACE INTO player_state (id, money, stocks, prices) "
              "VALUES (1, ?, ?, ?)");
    q.addBindValue(money);
    q.addBindValue(QString(QJsonDocument(stocksArr).toJson(QJsonDocument::Compact)));
    q.addBindValue(QString(QJsonDocument(pricesArr).toJson(QJsonDocument::Compact)));
    q.exec();
}

void StocksEngine::savePriceHistory()
{
    qint64 ts = QDateTime::currentSecsSinceEpoch();
    QSqlQuery q;
    for (int i = 0; i < STOCK_COUNT; i++) {
        q.prepare("INSERT INTO price_history (timestamp, stock_index, price) VALUES (?, ?, ?)");
        q.addBindValue(ts);
        q.addBindValue(i);
        q.addBindValue(prices[i]);
        q.exec();
    }
}
