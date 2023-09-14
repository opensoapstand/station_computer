/********************************************************************************
** Form generated from reading UI file 'page_qr_payment.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_QR_PAYMENT_H
#define UI_PAGE_QR_PAYMENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_page_qr_payment
{
public:
    QPushButton *pushButton_previous_page;
    QLabel *label_product_price;
    QLabel *label_product_information;
    QLabel *label_qrCode;
    QLabel *label_refresh_page;
    QPushButton *pushButton_refresh;
    QLabel *label_processing;
    QLabel *label_steps;
    QLabel *label_title;
    QLabel *label_scan;

    void setupUi(QWidget *page_qr_payment)
    {
        if (page_qr_payment->objectName().isEmpty())
            page_qr_payment->setObjectName(QStringLiteral("page_qr_payment"));
        page_qr_payment->resize(1080, 1920);
        page_qr_payment->setStyleSheet(QStringLiteral(""));
        pushButton_previous_page = new QPushButton(page_qr_payment);
        pushButton_previous_page->setObjectName(QStringLiteral("pushButton_previous_page"));
        pushButton_previous_page->setGeometry(QRect(70, 40, 401, 41));
        label_product_price = new QLabel(page_qr_payment);
        label_product_price->setObjectName(QStringLiteral("label_product_price"));
        label_product_price->setGeometry(QRect(530, 1470, 451, 41));
        QFont font;
        font.setFamily(QStringLiteral("Montserrat"));
        label_product_price->setFont(font);
        label_product_price->setStyleSheet(QLatin1String("font-family: Montserrat;\n"
"font-style: light;\n"
"font-size: 24px;\n"
"color: #58595B;"));
        label_product_price->setTextFormat(Qt::RichText);
        label_product_price->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_product_information = new QLabel(page_qr_payment);
        label_product_information->setObjectName(QStringLiteral("label_product_information"));
        label_product_information->setGeometry(QRect(100, 1470, 521, 81));
        label_product_information->setStyleSheet(QLatin1String("font-family: Montserrat;\n"
"font-style: light;\n"
"font-size: 24px;\n"
"color: #58595B;"));
        label_qrCode = new QLabel(page_qr_payment);
        label_qrCode->setObjectName(QStringLiteral("label_qrCode"));
        label_qrCode->setGeometry(QRect(360, 530, 360, 360));
        label_qrCode->setAutoFillBackground(false);
        label_qrCode->setStyleSheet(QStringLiteral("background-color: white;"));
        label_refresh_page = new QLabel(page_qr_payment);
        label_refresh_page->setObjectName(QStringLiteral("label_refresh_page"));
        label_refresh_page->setGeometry(QRect(150, 1340, 861, 421));
        QFont font1;
        font1.setFamily(QStringLiteral("Montserrat"));
        font1.setBold(true);
        font1.setWeight(75);
        label_refresh_page->setFont(font1);
        label_refresh_page->setStyleSheet(QLatin1String("color: #FF0000;\n"
"font-family: Montserrat;\n"
"font-style: light;\n"
"font-weight: bold;\n"
"font-size: 40px;\n"
"line-height: 29px;"));
        label_refresh_page->setAlignment(Qt::AlignCenter);
        label_refresh_page->setWordWrap(true);
        pushButton_refresh = new QPushButton(page_qr_payment);
        pushButton_refresh->setObjectName(QStringLiteral("pushButton_refresh"));
        pushButton_refresh->setGeometry(QRect(0, 0, 1081, 1931));
        pushButton_refresh->setStyleSheet(QStringLiteral(""));
        label_processing = new QLabel(page_qr_payment);
        label_processing->setObjectName(QStringLiteral("label_processing"));
        label_processing->setGeometry(QRect(0, 690, 1080, 251));
        label_processing->setAutoFillBackground(false);
        label_processing->setAlignment(Qt::AlignCenter);
        label_processing->setWordWrap(false);
        label_steps = new QLabel(page_qr_payment);
        label_steps->setObjectName(QStringLiteral("label_steps"));
        label_steps->setGeometry(QRect(100, 940, 980, 501));
        label_title = new QLabel(page_qr_payment);
        label_title->setObjectName(QStringLiteral("label_title"));
        label_title->setGeometry(QRect(0, 170, 1080, 81));
        label_title->setAlignment(Qt::AlignCenter);
        label_scan = new QLabel(page_qr_payment);
        label_scan->setObjectName(QStringLiteral("label_scan"));
        label_scan->setGeometry(QRect(0, 390, 1080, 111));
        label_scan->setAlignment(Qt::AlignCenter);
        pushButton_refresh->raise();
        pushButton_previous_page->raise();
        label_product_price->raise();
        label_product_information->raise();
        label_qrCode->raise();
        label_refresh_page->raise();
        label_steps->raise();
        label_title->raise();
        label_scan->raise();
        label_processing->raise();

        retranslateUi(page_qr_payment);

        QMetaObject::connectSlotsByName(page_qr_payment);
    } // setupUi

    void retranslateUi(QWidget *page_qr_payment)
    {
        page_qr_payment->setWindowTitle(QApplication::translate("page_qr_payment", "Form", Q_NULLPTR));
        pushButton_previous_page->setText(QString());
        label_product_price->setText(QApplication::translate("page_qr_payment", "label_product_price", Q_NULLPTR));
        label_product_information->setText(QApplication::translate("page_qr_payment", "order summary", Q_NULLPTR));
        label_qrCode->setText(QString());
        label_refresh_page->setText(QApplication::translate("page_qr_payment", "timeouttext", Q_NULLPTR));
        pushButton_refresh->setText(QString());
        label_processing->setText(QApplication::translate("page_qr_payment", "processing label", Q_NULLPTR));
        label_steps->setText(QApplication::translate("page_qr_payment", "steps", Q_NULLPTR));
        label_title->setText(QApplication::translate("page_qr_payment", "title", Q_NULLPTR));
        label_scan->setText(QApplication::translate("page_qr_payment", "scan label", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class page_qr_payment: public Ui_page_qr_payment {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_QR_PAYMENT_H
