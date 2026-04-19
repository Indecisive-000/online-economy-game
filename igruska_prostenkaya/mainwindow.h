#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QTimer>
#include <QLabel>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void connectToServer(const QString &host="127.0.0.1", quint16 port = 5555);
    void sendBuyCommand(const QVector<int> &amounts);
    void sendSellCommand(const QVector<int> &amounts);
    void requestStatus();


private slots:
    void on_AnyBuySlider_change(int index, int value);
    void on_AnySellSlider_change(int index, int value);
    void on_buyButton_clicked();
    void on_pushButton_2_clicked();
    void on_timerPrices_tick();

    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketReadyRead();
    void onSocketError(QAbstractSocket::SocketError error);

private:
    Ui::MainWindow *ui;
    void sendJson(const QJsonObject &obj);
    void parseMessage(const QByteArray &line);
    void updateUIFromServer(int money, const QVector<int> &prices, const QVector<int> &stocks);


    QSlider *buySliders[5];
    QSlider *sellSliders[5];
    QLabel *buyLabel [5];
    QLabel *sellLabel [5];
    QTimer *timerPrices;

    QTcpSocket *socket;
    QByteArray socketBuffer;
    bool serverMode = false;

    int money =10000;
    int stocks[5];
    int prices[5];

    static constexpr int STOCK_COUNT = 5;


};
#endif // MAINWINDOW_H
