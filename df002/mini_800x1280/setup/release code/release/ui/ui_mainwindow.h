/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QPushButton *option1Button;
    QPushButton *option3Button;
    QLabel *option1Label;
    QLabel *option3Label;
    QLabel *option3SoldOutLabel;
    QLabel *option1SoldOutLabel;
    QPushButton *surveyButton;
    QLabel *option3ALabel;
    QLabel *option1ALabel;
    QLabel *logoLabel;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->setEnabled(true);
        MainWindow->resize(800, 1280);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setAutoFillBackground(false);
        MainWindow->setStyleSheet(QString::fromUtf8(""));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        option1Button = new QPushButton(centralWidget);
        option1Button->setObjectName(QString::fromUtf8("option1Button"));
        option1Button->setGeometry(QRect(25, 250, 400, 400));
        option1Button->setText(QString::fromUtf8(""));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/prefix/ASSETS/coldbrewJJ.png"), QSize(), QIcon::Normal, QIcon::Off);
        option1Button->setIcon(icon);
        option1Button->setIconSize(QSize(400, 400));
        option3Button = new QPushButton(centralWidget);
        option3Button->setObjectName(QString::fromUtf8("option3Button"));
        option3Button->setGeometry(QRect(25, 750, 400, 400));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/new/prefix/ASSETS/elderflower.png"), QSize(), QIcon::Normal, QIcon::Off);
        option3Button->setIcon(icon1);
        option3Button->setIconSize(QSize(400, 400));
        option1Label = new QLabel(centralWidget);
        option1Label->setObjectName(QString::fromUtf8("option1Label"));
        option1Label->setGeometry(QRect(450, 250, 300, 350));
        option1Label->setStyleSheet(QString::fromUtf8("font: 75pt \"Ubuntu\";\n"
"background-color: rgba(0, 0, 0,0);"));
        option3Label = new QLabel(centralWidget);
        option3Label->setObjectName(QString::fromUtf8("option3Label"));
        option3Label->setGeometry(QRect(450, 750, 300, 350));
        option3Label->setStyleSheet(QString::fromUtf8("font: 75pt \"Ubuntu\";\n"
""));
        option3SoldOutLabel = new QLabel(centralWidget);
        option3SoldOutLabel->setObjectName(QString::fromUtf8("option3SoldOutLabel"));
        option3SoldOutLabel->setGeometry(QRect(25, 750, 400, 400));
        option1SoldOutLabel = new QLabel(centralWidget);
        option1SoldOutLabel->setObjectName(QString::fromUtf8("option1SoldOutLabel"));
        option1SoldOutLabel->setGeometry(QRect(25, 250, 400, 400));
        surveyButton = new QPushButton(centralWidget);
        surveyButton->setObjectName(QString::fromUtf8("surveyButton"));
        surveyButton->setGeometry(QRect(125, 1150, 550, 100));
        surveyButton->setIcon(icon1);
        surveyButton->setIconSize(QSize(400, 400));
        option3ALabel = new QLabel(centralWidget);
        option3ALabel->setObjectName(QString::fromUtf8("option3ALabel"));
        option3ALabel->setGeometry(QRect(450, 1000, 300, 50));
        option3ALabel->setStyleSheet(QString::fromUtf8("font: 75pt \"Ubuntu\";\n"
""));
        option1ALabel = new QLabel(centralWidget);
        option1ALabel->setObjectName(QString::fromUtf8("option1ALabel"));
        option1ALabel->setGeometry(QRect(450, 500, 300, 50));
        option1ALabel->setStyleSheet(QString::fromUtf8("font: 75pt \"Ubuntu\";\n"
""));
        logoLabel = new QLabel(centralWidget);
        logoLabel->setObjectName(QString::fromUtf8("logoLabel"));
        logoLabel->setGeometry(QRect(50, 30, 700, 175));
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        option1Label->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:72pt; color:#000000;\">Label</span></p></body></html>", nullptr));
        option3Label->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:72pt; color:#000000;\">Label</span></p></body></html>", nullptr));
        option3SoldOutLabel->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p align=\"center\"><span style=\" font-size:84pt; font-weight:600; color:#444444; vertical-align:super;\">Sold Out</span></p></body></html>", nullptr));
        option1SoldOutLabel->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p align=\"center\"><span style=\" font-size:84pt; font-weight:600; color:#444444; vertical-align:super;\">Sold Out</span></p></body></html>", nullptr));
        option3ALabel->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:72pt; color:#000000;\">Label</span></p></body></html>", nullptr));
        option1ALabel->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:72pt; color:#000000;\">Label</span></p></body></html>", nullptr));
        logoLabel->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
