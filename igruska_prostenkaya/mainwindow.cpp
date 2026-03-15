#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

    timerPrices = new QTimer();
    timerPrices->setInterval(10000);
    connect(timerPrices, QTimer::timeout, this, &MainWindow::on_timerPrices_tick);
    timerPrices->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_AnyBuySlider_change(int index, int value){
    buyLabel[index]->setText(QString::number(prices[index]*value)+"$");
}

void MainWindow::on_AnySellSlider_change(int index, int value){
    sellLabel[index]->setText(QString::number(prices[index]*value)+"$");
}

void MainWindow::on_timerPrices_tick(){
    for (int i=0; i<5; i++){
        if (rand()%2==1){
            prices[i] += 1 + rand() % 10;
        }
        else{
            prices[i] -= 1 + rand() % 10;
        }

        buyLabel[i]->setText(QString::number(prices[i]*buySliders[i]->value())+"$");
        sellLabel[i]->setText(QString::number(prices[i]*sellSliders[i]->value())+"$");
    }
}


void MainWindow::on_buyButton_clicked()
{
    int cost = 0;
    for (int i=0; i<5; i++){
        cost += prices[i] * buySliders[i]->value();
    }
    if (cost <= money){
        money -= cost;
        for (int i = 0; i < 5; i++){
            stocks[i] += buySliders[i]->value();
            sellSliders[i]->setMaximum(stocks[i]);
        }
        ui->money->setText("Money:"+ QString::number(money)+"$");


    }
}


void MainWindow::on_pushButton_2_clicked()
{
    int cost = 0;
    for (int i=0; i<5; i++){
        cost += prices[i] * sellSliders[i]->value();
    }

    money += cost;
    for (int i = 0; i < 5; i++){
        stocks[i] -= sellSliders[i]->value();
        sellSliders[i]->setMaximum(stocks[i]);
    }
    ui->money->setText("Money:"+ QString::number(money)+"$");



}

