/********************************************************************************
** Form generated from reading UI file 'page_end.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_END_H
#define UI_PAGE_END_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_page_end
{
public:
    QPushButton *pushButton_to_idle;
    QLabel *label_message;
    QLabel *label_customer_logo;
    QLabel *label_message_2;
    QLabel *label_manufacturer_logo;
    QPushButton *pushButton_contact;
    QLabel *label_volume_dispensed;
    QLabel *label_volume_dispensed_ml;

    void setupUi(QWidget *page_end)
    {
        if (page_end->objectName().isEmpty())
            page_end->setObjectName(QStringLiteral("page_end"));
        page_end->resize(1080, 1920);
        pushButton_to_idle = new QPushButton(page_end);
        pushButton_to_idle->setObjectName(QStringLiteral("pushButton_to_idle"));
        pushButton_to_idle->setGeometry(QRect(0, 10, 1080, 1920));
        pushButton_to_idle->setAutoFillBackground(false);
        pushButton_to_idle->setStyleSheet(QStringLiteral("background-image: url(/home/df-admin/production/references/7_background_thank_you.png);"));
        label_message = new QLabel(page_end);
        label_message->setObjectName(QStringLiteral("label_message"));
        label_message->setGeometry(QRect(110, 590, 841, 420));
        label_message->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        label_customer_logo = new QLabel(page_end);
        label_customer_logo->setObjectName(QStringLiteral("label_customer_logo"));
        label_customer_logo->setGeometry(QRect(370, 1710, 351, 211));
        label_message_2 = new QLabel(page_end);
        label_message_2->setObjectName(QStringLiteral("label_message_2"));
        label_message_2->setGeometry(QRect(0, 1020, 1081, 480));
        label_manufacturer_logo = new QLabel(page_end);
        label_manufacturer_logo->setObjectName(QStringLiteral("label_manufacturer_logo"));
        label_manufacturer_logo->setGeometry(QRect(300, 250, 471, 331));
        pushButton_contact = new QPushButton(page_end);
        pushButton_contact->setObjectName(QStringLiteral("pushButton_contact"));
        pushButton_contact->setGeometry(QRect(340, 1380, 400, 101));
        pushButton_contact->setMaximumSize(QSize(16777214, 16777215));
        pushButton_contact->setToolTipDuration(-10);
        QIcon icon;
        icon.addFile(QStringLiteral("../../production/references/helpButton.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_contact->setIcon(icon);
        label_volume_dispensed = new QLabel(page_end);
        label_volume_dispensed->setObjectName(QStringLiteral("label_volume_dispensed"));
        label_volume_dispensed->setGeometry(QRect(240, 1530, 600, 71));
        label_volume_dispensed_ml = new QLabel(page_end);
        label_volume_dispensed_ml->setObjectName(QStringLiteral("label_volume_dispensed_ml"));
        label_volume_dispensed_ml->setGeometry(QRect(240, 1620, 600, 71));

        retranslateUi(page_end);

        QMetaObject::connectSlotsByName(page_end);
    } // setupUi

    void retranslateUi(QWidget *page_end)
    {
        page_end->setWindowTitle(QApplication::translate("page_end", "Form", Q_NULLPTR));
        pushButton_to_idle->setText(QString());
        label_message->setText(QString());
        label_customer_logo->setText(QString());
        label_message_2->setText(QString());
        label_manufacturer_logo->setText(QString());
        pushButton_contact->setText(QApplication::translate("page_end", "Help", Q_NULLPTR));
        label_volume_dispensed->setText(QApplication::translate("page_end", "Volume dispensed", Q_NULLPTR));
        label_volume_dispensed_ml->setText(QApplication::translate("page_end", "0.0", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class page_end: public Ui_page_end {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_END_H
