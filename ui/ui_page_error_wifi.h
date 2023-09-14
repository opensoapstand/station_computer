/********************************************************************************
** Form generated from reading UI file 'page_error_wifi.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_ERROR_WIFI_H
#define UI_PAGE_ERROR_WIFI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_page_error_wifi
{
public:
    QPushButton *pushButton_mainPage;
    QLabel *label_error_message;
    QLabel *label_logo_drinkfill;
    QLabel *label_oops;
    QLabel *label_error_occured;
    QLabel *label_check_back;

    void setupUi(QWidget *page_error_wifi)
    {
        if (page_error_wifi->objectName().isEmpty())
            page_error_wifi->setObjectName(QStringLiteral("page_error_wifi"));
        page_error_wifi->resize(1080, 1920);
        page_error_wifi->setLayoutDirection(Qt::RightToLeft);
        page_error_wifi->setStyleSheet(QStringLiteral(""));
        pushButton_mainPage = new QPushButton(page_error_wifi);
        pushButton_mainPage->setObjectName(QStringLiteral("pushButton_mainPage"));
        pushButton_mainPage->setGeometry(QRect(0, 0, 1080, 1920));
        label_error_message = new QLabel(page_error_wifi);
        label_error_message->setObjectName(QStringLiteral("label_error_message"));
        label_error_message->setGeometry(QRect(250, 650, 580, 620));
        label_logo_drinkfill = new QLabel(page_error_wifi);
        label_logo_drinkfill->setObjectName(QStringLiteral("label_logo_drinkfill"));
        label_logo_drinkfill->setGeometry(QRect(440, 720, 200, 120));
        label_oops = new QLabel(page_error_wifi);
        label_oops->setObjectName(QStringLiteral("label_oops"));
        label_oops->setGeometry(QRect(440, 870, 200, 71));
        label_error_occured = new QLabel(page_error_wifi);
        label_error_occured->setObjectName(QStringLiteral("label_error_occured"));
        label_error_occured->setGeometry(QRect(250, 990, 580, 60));
        label_check_back = new QLabel(page_error_wifi);
        label_check_back->setObjectName(QStringLiteral("label_check_back"));
        label_check_back->setGeometry(QRect(250, 1050, 580, 60));

        retranslateUi(page_error_wifi);

        QMetaObject::connectSlotsByName(page_error_wifi);
    } // setupUi

    void retranslateUi(QWidget *page_error_wifi)
    {
        page_error_wifi->setWindowTitle(QApplication::translate("page_error_wifi", "Form", Q_NULLPTR));
        pushButton_mainPage->setText(QString());
        label_error_message->setText(QString());
        label_logo_drinkfill->setText(QApplication::translate("page_error_wifi", "TextLabel", Q_NULLPTR));
        label_oops->setText(QApplication::translate("page_error_wifi", "TextLabel", Q_NULLPTR));
        label_error_occured->setText(QApplication::translate("page_error_wifi", "TextLabel", Q_NULLPTR));
        label_check_back->setText(QApplication::translate("page_error_wifi", "TextLabel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class page_error_wifi: public Ui_page_error_wifi {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_ERROR_WIFI_H
