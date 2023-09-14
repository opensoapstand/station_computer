/********************************************************************************
** Form generated from reading UI file 'page_tap_payment.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_TAP_PAYMENT_H
#define UI_PAGE_TAP_PAYMENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_page_tap_payment
{
public:
    QPushButton *pushButton_previous_page;
    QPushButton *pushButton_to_idle;
    QLabel *order_total_amount;
    QLabel *order_drink_amount;
    QPushButton *pushButton_payment_bypass;
    QLabel *productLabel;
    QLabel *label_title;
    QLabel *animated_Label;
    QLabel *preauthLabel;

    void setupUi(QWidget *page_tap_payment)
    {
        if (page_tap_payment->objectName().isEmpty())
            page_tap_payment->setObjectName(QStringLiteral("page_tap_payment"));
        page_tap_payment->resize(1080, 1920);
        page_tap_payment->setStyleSheet(QStringLiteral(""));
        pushButton_previous_page = new QPushButton(page_tap_payment);
        pushButton_previous_page->setObjectName(QStringLiteral("pushButton_previous_page"));
        pushButton_previous_page->setGeometry(QRect(70, 40, 401, 41));
        pushButton_to_idle = new QPushButton(page_tap_payment);
        pushButton_to_idle->setObjectName(QStringLiteral("pushButton_to_idle"));
        pushButton_to_idle->setEnabled(true);
        pushButton_to_idle->setGeometry(QRect(460, 1760, 161, 141));
        order_total_amount = new QLabel(page_tap_payment);
        order_total_amount->setObjectName(QStringLiteral("order_total_amount"));
        order_total_amount->setGeometry(QRect(800, 1105, 250, 41));
        QFont font;
        font.setFamily(QStringLiteral("Montserrat"));
        font.setBold(true);
        font.setWeight(75);
        order_total_amount->setFont(font);
        order_total_amount->setStyleSheet(QLatin1String("font-family: Montserrat;\n"
"background-image: url(background.png);\n"
"font-style: light;\n"
"font-weight: bold;\n"
"font-size: 36px;\n"
"line-height: 44px;\n"
"\n"
"color: #58595B;"));
        order_total_amount->setTextFormat(Qt::RichText);
        order_drink_amount = new QLabel(page_tap_payment);
        order_drink_amount->setObjectName(QStringLiteral("order_drink_amount"));
        order_drink_amount->setGeometry(QRect(750, 1470, 221, 41));
        QFont font1;
        font1.setFamily(QStringLiteral("Montserrat"));
        order_drink_amount->setFont(font1);
        order_drink_amount->setStyleSheet(QLatin1String("font-family: Montserrat;\n"
"font-style: light;\n"
"font-size: 24px;\n"
"color: #58595B;"));
        order_drink_amount->setTextFormat(Qt::RichText);
        order_drink_amount->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pushButton_payment_bypass = new QPushButton(page_tap_payment);
        pushButton_payment_bypass->setObjectName(QStringLiteral("pushButton_payment_bypass"));
        pushButton_payment_bypass->setEnabled(false);
        pushButton_payment_bypass->setGeometry(QRect(880, 1730, 201, 191));
        productLabel = new QLabel(page_tap_payment);
        productLabel->setObjectName(QStringLiteral("productLabel"));
        productLabel->setGeometry(QRect(100, 1470, 521, 81));
        productLabel->setStyleSheet(QLatin1String("font-family: Montserrat;\n"
"font-style: light;\n"
"font-size: 24px;\n"
"color: #58595B;"));
        label_title = new QLabel(page_tap_payment);
        label_title->setObjectName(QStringLiteral("label_title"));
        label_title->setGeometry(QRect(190, 170, 700, 81));
        label_title->setAlignment(Qt::AlignCenter);
        animated_Label = new QLabel(page_tap_payment);
        animated_Label->setObjectName(QStringLiteral("animated_Label"));
        animated_Label->setGeometry(QRect(221, 327, 639, 639));
        preauthLabel = new QLabel(page_tap_payment);
        preauthLabel->setObjectName(QStringLiteral("preauthLabel"));
        preauthLabel->setGeometry(QRect(180, 1090, 581, 81));
        preauthLabel->setStyleSheet(QLatin1String("font-family: Montserrat;\n"
"font-style: light;\n"
"font-size: 24px;\n"
"color: #58595B;"));
        preauthLabel->setWordWrap(true);

        retranslateUi(page_tap_payment);

        QMetaObject::connectSlotsByName(page_tap_payment);
    } // setupUi

    void retranslateUi(QWidget *page_tap_payment)
    {
        page_tap_payment->setWindowTitle(QApplication::translate("page_tap_payment", "Form", Q_NULLPTR));
        pushButton_previous_page->setText(QApplication::translate("page_tap_payment", "back", Q_NULLPTR));
        pushButton_to_idle->setText(QString());
        order_total_amount->setText(QString());
        order_drink_amount->setText(QApplication::translate("page_tap_payment", "order_drink_amount", Q_NULLPTR));
        pushButton_payment_bypass->setText(QString());
        productLabel->setText(QApplication::translate("page_tap_payment", "order summary", Q_NULLPTR));
        label_title->setText(QApplication::translate("page_tap_payment", "title", Q_NULLPTR));
        animated_Label->setText(QString());
        preauthLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class page_tap_payment: public Ui_page_tap_payment {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_TAP_PAYMENT_H
