#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QTimer>
#include <QLabel>
#include <QTcpSocket>
#include <QGraphicsScene>
#include <QVector>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_AnyBuySlider_change(int index, int value);
    void on_AnySellSlider_change(int index, int value);
    void on_buyButton_clicked();
    void on_pushButton_2_clicked();
    void on_timerPrices_tick();
    void onSocketConnected();
    void onSocketReadyRead();
    void onSocketDisconnected();

private:
    Ui::MainWindow *ui;
    QSlider *buySliders[5];
    QSlider *sellSliders[5];
    QLabel *buyLabel[5];
    QLabel *sellLabel[5];
    QTimer *timerPrices;
    QTcpSocket *socket;
    QByteArray socketBuffer;
    QGraphicsScene *graphScene;

    static const int STOCK_COUNT = 5;
    static const int HISTORY_SIZE = 50;

    int money = 10000;
    int initialMoney = 10000;
    int stocks[5] = {};
    int prices[5] = {};
    bool connectedToServer = false;
    QVector<QVector<int>> priceHistory;

    void sendToServer(const QJsonObject &cmd);
    void processServerMessage(const QJsonObject &msg);
    void updateGraph();
    void updateProfit();
};

#endif // MAINWINDOW_H
