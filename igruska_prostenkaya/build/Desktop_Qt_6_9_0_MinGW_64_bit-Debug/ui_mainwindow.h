/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGroupBox *groupBox;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QPushButton *buyButton;
    QGroupBox *groupBox_2;
    QGraphicsView *graphicsView;
    QLabel *label;
    QLabel *money;
    QLabel *profit;
    QGroupBox *groupBox_4;
    QLabel *label_16;
    QLabel *label_17;
    QLabel *label_18;
    QLabel *label_19;
    QLabel *label_20;
    QPushButton *pushButton_2;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1118, 634);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(10, 90, 341, 401));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(10, 50, 41, 20));
        QFont font;
        font.setPointSize(11);
        label_5->setFont(font);
        label_6 = new QLabel(groupBox);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(10, 100, 81, 20));
        label_6->setFont(font);
        label_7 = new QLabel(groupBox);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(10, 150, 51, 20));
        label_7->setFont(font);
        label_8 = new QLabel(groupBox);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(10, 200, 51, 20));
        label_8->setFont(font);
        label_9 = new QLabel(groupBox);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(10, 250, 61, 20));
        label_9->setFont(font);
        buyButton = new QPushButton(groupBox);
        buyButton->setObjectName("buyButton");
        buyButton->setGeometry(QRect(200, 360, 80, 24));
        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(350, 90, 411, 401));
        graphicsView = new QGraphicsView(groupBox_2);
        graphicsView->setObjectName("graphicsView");
        graphicsView->setGeometry(QRect(0, 30, 411, 361));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(300, 20, 411, 51));
        QFont font1;
        font1.setPointSize(20);
        font1.setBold(true);
        label->setFont(font1);
        label->setAlignment(Qt::AlignmentFlag::AlignCenter);
        money = new QLabel(centralwidget);
        money->setObjectName("money");
        money->setGeometry(QRect(40, 20, 361, 31));
        QFont font2;
        font2.setPointSize(13);
        font2.setBold(true);
        money->setFont(font2);
        profit = new QLabel(centralwidget);
        profit->setObjectName("profit");
        profit->setGeometry(QRect(770, 20, 161, 31));
        profit->setFont(font2);
        groupBox_4 = new QGroupBox(centralwidget);
        groupBox_4->setObjectName("groupBox_4");
        groupBox_4->setGeometry(QRect(760, 90, 341, 401));
        label_16 = new QLabel(groupBox_4);
        label_16->setObjectName("label_16");
        label_16->setGeometry(QRect(10, 50, 51, 20));
        label_16->setFont(font);
        label_17 = new QLabel(groupBox_4);
        label_17->setObjectName("label_17");
        label_17->setGeometry(QRect(10, 100, 81, 20));
        label_17->setFont(font);
        label_18 = new QLabel(groupBox_4);
        label_18->setObjectName("label_18");
        label_18->setGeometry(QRect(10, 150, 61, 20));
        label_18->setFont(font);
        label_19 = new QLabel(groupBox_4);
        label_19->setObjectName("label_19");
        label_19->setGeometry(QRect(10, 200, 51, 20));
        label_19->setFont(font);
        label_20 = new QLabel(groupBox_4);
        label_20->setObjectName("label_20");
        label_20->setGeometry(QRect(10, 250, 51, 20));
        label_20->setFont(font);
        pushButton_2 = new QPushButton(groupBox_4);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(200, 360, 80, 24));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1118, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\320\277\320\276\320\272\321\203\320\277\320\272\320\260 \320\260\320\272\321\202\320\270\320\262\320\260", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "pear", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "MacroHard", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Edison", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "\320\273\320\270\321\202\320\270\320\271 ", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "nikita", nullptr));
        buyButton->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\320\263\321\200\320\260\321\204\320\270\320\272", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\320\221\320\230\320\240\320\226\320\220", nullptr));
        money->setText(QCoreApplication::translate("MainWindow", "\320\264\320\265\320\275\320\265\320\263:", nullptr));
        profit->setText(QCoreApplication::translate("MainWindow", "\320\277\321\200\320\270\320\261\321\213\320\273\321\214:", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("MainWindow", "\320\277\321\200\320\276\320\264\320\260\320\266\320\260", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "pear", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "MacroHard", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "Edison", nullptr));
        label_19->setText(QCoreApplication::translate("MainWindow", "\320\273\320\270\321\202\320\270\320\271 ", nullptr));
        label_20->setText(QCoreApplication::translate("MainWindow", "nikita", nullptr));
        pushButton_2->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
