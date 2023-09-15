/********************************************************************************
** Form generated from reading UI file 'page_idle.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_IDLE_H
#define UI_PAGE_IDLE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_page_idle
{
public:
    QPushButton *pushButton_to_select_product_page;
    QLabel *savedBottles_label;
    QLabel *label_customer_logo;
    QLabel *label_welcome_message;
    QPushButton *pushButton_test;
    QLabel *label_manufacturer_logo;
    QLabel *label_printer_status;
    QWidget *video_player;
    QWidget *media_player;
    QLabel *label_temperature_status;
    QLabel *label_show_temperature;

    void setupUi(QWidget *page_idle)
    {
        if (page_idle->objectName().isEmpty())
            page_idle->setObjectName(QStringLiteral("page_idle"));
        page_idle->resize(1080, 1920);
        pushButton_to_select_product_page = new QPushButton(page_idle);
        pushButton_to_select_product_page->setObjectName(QStringLiteral("pushButton_to_select_product_page"));
        pushButton_to_select_product_page->setGeometry(QRect(0, 0, 1081, 1921));
        pushButton_to_select_product_page->setStyleSheet(QStringLiteral(""));
        savedBottles_label = new QLabel(page_idle);
        savedBottles_label->setObjectName(QStringLiteral("savedBottles_label"));
        savedBottles_label->setGeometry(QRect(0, 1690, 1080, 141));
        QFont font;
        savedBottles_label->setFont(font);
        savedBottles_label->setStyleSheet(QLatin1String("color: white;\n"
"text-transform: lowercase;"));
        savedBottles_label->setAlignment(Qt::AlignCenter);
        label_customer_logo = new QLabel(page_idle);
        label_customer_logo->setObjectName(QStringLiteral("label_customer_logo"));
        label_customer_logo->setGeometry(QRect(370, 1580, 351, 211));
        label_welcome_message = new QLabel(page_idle);
        label_welcome_message->setObjectName(QStringLiteral("label_welcome_message"));
        label_welcome_message->setGeometry(QRect(40, 600, 981, 481));
        pushButton_test = new QPushButton(page_idle);
        pushButton_test->setObjectName(QStringLiteral("pushButton_test"));
        pushButton_test->setGeometry(QRect(0, 30, 121, 91));
        label_manufacturer_logo = new QLabel(page_idle);
        label_manufacturer_logo->setObjectName(QStringLiteral("label_manufacturer_logo"));
        label_manufacturer_logo->setGeometry(QRect(300, 250, 471, 331));
        label_printer_status = new QLabel(page_idle);
        label_printer_status->setObjectName(QStringLiteral("label_printer_status"));
        label_printer_status->setGeometry(QRect(50, 1050, 980, 120));
        video_player = new QWidget(page_idle);
        video_player->setObjectName(QStringLiteral("video_player"));
        video_player->setGeometry(QRect(10, -10, 1080, 1920));
        media_player = new QWidget(page_idle);
        media_player->setObjectName(QStringLiteral("media_player"));
        media_player->setGeometry(QRect(200, 570, 331, 221));
        label_temperature_status = new QLabel(page_idle);
        label_temperature_status->setObjectName(QStringLiteral("label_temperature_status"));
        label_temperature_status->setGeometry(QRect(49, 1000, 981, 381));
        label_show_temperature = new QLabel(page_idle);
        label_show_temperature->setObjectName(QStringLiteral("label_show_temperature"));
        label_show_temperature->setGeometry(QRect(10, 1840, 581, 71));
        pushButton_to_select_product_page->raise();
        savedBottles_label->raise();
        label_customer_logo->raise();
        label_welcome_message->raise();
        pushButton_test->raise();
        label_manufacturer_logo->raise();
        media_player->raise();
        video_player->raise();
        label_printer_status->raise();
        label_temperature_status->raise();
        label_show_temperature->raise();

        retranslateUi(page_idle);

        QMetaObject::connectSlotsByName(page_idle);
    } // setupUi

    void retranslateUi(QWidget *page_idle)
    {
        page_idle->setWindowTitle(QApplication::translate("page_idle", "Form", Q_NULLPTR));
        pushButton_to_select_product_page->setText(QString());
        savedBottles_label->setText(QString());
        label_customer_logo->setText(QString());
        label_welcome_message->setText(QApplication::translate("page_idle", "TextLabel", Q_NULLPTR));
        pushButton_test->setText(QString());
        label_manufacturer_logo->setText(QString());
        label_printer_status->setText(QString());
        label_temperature_status->setText(QString());
        label_show_temperature->setText(QApplication::translate("page_idle", "Label show temperature ", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class page_idle: public Ui_page_idle {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_IDLE_H
