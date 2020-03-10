/********************************************************************************
** Form generated from reading UI file 'payment.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAYMENT_H
#define UI_PAYMENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_payment
{
public:
    QLabel *productLabel;
    QLabel *imageLabel;
    QLabel *priceLabel;
    QLabel *totalLabel;
    QPushButton *goBackButton;
    QLabel *orLabel;
    QLabel *tapLabel;
    QLabel *processLabel;
    QPushButton *payButton;
    QLabel *declineLabel;
    QPushButton *priceVolume1Button;
    QPushButton *priceVolume2Button;

    void setupUi(QWidget *payment)
    {
        if (payment->objectName().isEmpty())
            payment->setObjectName(QString::fromUtf8("payment"));
        payment->resize(800, 1280);
        productLabel = new QLabel(payment);
        productLabel->setObjectName(QString::fromUtf8("productLabel"));
        productLabel->setGeometry(QRect(0, 25, 800, 100));
        imageLabel = new QLabel(payment);
        imageLabel->setObjectName(QString::fromUtf8("imageLabel"));
        imageLabel->setGeometry(QRect(200, 150, 400, 400));
        priceLabel = new QLabel(payment);
        priceLabel->setObjectName(QString::fromUtf8("priceLabel"));
        priceLabel->setGeometry(QRect(0, 775, 800, 100));
        totalLabel = new QLabel(payment);
        totalLabel->setObjectName(QString::fromUtf8("totalLabel"));
        totalLabel->setGeometry(QRect(0, 875, 800, 100));
        goBackButton = new QPushButton(payment);
        goBackButton->setObjectName(QString::fromUtf8("goBackButton"));
        goBackButton->setGeometry(QRect(100, 1050, 260, 100));
        orLabel = new QLabel(payment);
        orLabel->setObjectName(QString::fromUtf8("orLabel"));
        orLabel->setGeometry(QRect(375, 1075, 50, 50));
        tapLabel = new QLabel(payment);
        tapLabel->setObjectName(QString::fromUtf8("tapLabel"));
        tapLabel->setGeometry(QRect(440, 1050, 260, 100));
        processLabel = new QLabel(payment);
        processLabel->setObjectName(QString::fromUtf8("processLabel"));
        processLabel->setGeometry(QRect(0, 1000, 800, 150));
        payButton = new QPushButton(payment);
        payButton->setObjectName(QString::fromUtf8("payButton"));
        payButton->setGeometry(QRect(440, 1050, 260, 100));
        declineLabel = new QLabel(payment);
        declineLabel->setObjectName(QString::fromUtf8("declineLabel"));
        declineLabel->setGeometry(QRect(0, 875, 800, 100));
        priceVolume1Button = new QPushButton(payment);
        priceVolume1Button->setObjectName(QString::fromUtf8("priceVolume1Button"));
        priceVolume1Button->setGeometry(QRect(100, 600, 260, 160));
        priceVolume2Button = new QPushButton(payment);
        priceVolume2Button->setObjectName(QString::fromUtf8("priceVolume2Button"));
        priceVolume2Button->setGeometry(QRect(440, 600, 260, 160));

        retranslateUi(payment);

        QMetaObject::connectSlotsByName(payment);
    } // setupUi

    void retranslateUi(QWidget *payment)
    {
        payment->setWindowTitle(QCoreApplication::translate("payment", "Form", nullptr));
        productLabel->setText(QCoreApplication::translate("payment", "TextLabel", nullptr));
        imageLabel->setText(QCoreApplication::translate("payment", "TextLabel", nullptr));
        priceLabel->setText(QCoreApplication::translate("payment", "TextLabel", nullptr));
        totalLabel->setText(QCoreApplication::translate("payment", "TextLabel", nullptr));
        goBackButton->setText(QString());
        orLabel->setText(QCoreApplication::translate("payment", "TextLabel", nullptr));
        tapLabel->setText(QCoreApplication::translate("payment", "TextLabel", nullptr));
        processLabel->setText(QCoreApplication::translate("payment", "TextLabel", nullptr));
        payButton->setText(QString());
        declineLabel->setText(QCoreApplication::translate("payment", "TextLabel", nullptr));
        priceVolume1Button->setText(QCoreApplication::translate("payment", "PushButton", nullptr));
        priceVolume2Button->setText(QCoreApplication::translate("payment", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class payment: public Ui_payment {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAYMENT_H
