/********************************************************************************
** Form generated from reading UI file 'page_dispenser.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_DISPENSER_H
#define UI_PAGE_DISPENSER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_page_dispenser
{
public:
    QLabel *label_background_during_dispense_animation;
    QLabel *label_moving_bottle_fill_effect;
    QPushButton *pushButton_abort;
    QLabel *label_volume_dispensed;
    QLabel *label_volume_dispensed_ml;
    QLabel *label_to_refill;
    QLabel *label_instructions_container;
    QLabel *label_press;
    QPushButton *pushButton_debug_Button;
    QLabel *label_finishTransactionMessage;
    QLabel *label_dispense_status;
    QLabel *label_dispense_flowrate;
    QPushButton *pushButton_problems;
    QLabel *label_dispense_message;
    QLabel *label_logo;

    void setupUi(QWidget *page_dispenser)
    {
        if (page_dispenser->objectName().isEmpty())
            page_dispenser->setObjectName(QStringLiteral("page_dispenser"));
        page_dispenser->resize(1080, 1920);
        page_dispenser->setLayoutDirection(Qt::RightToLeft);
        page_dispenser->setStyleSheet(QStringLiteral(""));
        label_background_during_dispense_animation = new QLabel(page_dispenser);
        label_background_during_dispense_animation->setObjectName(QStringLiteral("label_background_during_dispense_animation"));
        label_background_during_dispense_animation->setGeometry(QRect(0, 0, 1080, 1920));
        label_moving_bottle_fill_effect = new QLabel(page_dispenser);
        label_moving_bottle_fill_effect->setObjectName(QStringLiteral("label_moving_bottle_fill_effect"));
        label_moving_bottle_fill_effect->setGeometry(QRect(0, 400, 301, 671));
        label_moving_bottle_fill_effect->setStyleSheet(QStringLiteral(""));
        pushButton_abort = new QPushButton(page_dispenser);
        pushButton_abort->setObjectName(QStringLiteral("pushButton_abort"));
        pushButton_abort->setGeometry(QRect(340, 1380, 400, 101));
        QFont font;
        font.setFamily(QStringLiteral("Brevia"));
        font.setPointSize(9);
        font.setBold(true);
        font.setWeight(75);
        pushButton_abort->setFont(font);
        pushButton_abort->setFlat(false);
        label_volume_dispensed = new QLabel(page_dispenser);
        label_volume_dispensed->setObjectName(QStringLiteral("label_volume_dispensed"));
        label_volume_dispensed->setGeometry(QRect(240, 1530, 600, 71));
        label_volume_dispensed_ml = new QLabel(page_dispenser);
        label_volume_dispensed_ml->setObjectName(QStringLiteral("label_volume_dispensed_ml"));
        label_volume_dispensed_ml->setGeometry(QRect(240, 1620, 600, 71));
        label_to_refill = new QLabel(page_dispenser);
        label_to_refill->setObjectName(QStringLiteral("label_to_refill"));
        label_to_refill->setGeometry(QRect(0, 140, 1080, 151));
        label_to_refill->setTextFormat(Qt::RichText);
        label_instructions_container = new QLabel(page_dispenser);
        label_instructions_container->setObjectName(QStringLiteral("label_instructions_container"));
        label_instructions_container->setGeometry(QRect(0, 280, 1080, 91));
        label_press = new QLabel(page_dispenser);
        label_press->setObjectName(QStringLiteral("label_press"));
        label_press->setGeometry(QRect(0, 1180, 1080, 161));
        pushButton_debug_Button = new QPushButton(page_dispenser);
        pushButton_debug_Button->setObjectName(QStringLiteral("pushButton_debug_Button"));
        pushButton_debug_Button->setGeometry(QRect(1000, 240, 81, 71));
        pushButton_debug_Button->setFont(font);
        pushButton_debug_Button->setFlat(false);
        label_finishTransactionMessage = new QLabel(page_dispenser);
        label_finishTransactionMessage->setObjectName(QStringLiteral("label_finishTransactionMessage"));
        label_finishTransactionMessage->setGeometry(QRect(240, 600, 600, 300));
        label_finishTransactionMessage->setWordWrap(true);
        label_dispense_status = new QLabel(page_dispenser);
        label_dispense_status->setObjectName(QStringLiteral("label_dispense_status"));
        label_dispense_status->setGeometry(QRect(660, 410, 391, 51));
        label_dispense_flowrate = new QLabel(page_dispenser);
        label_dispense_flowrate->setObjectName(QStringLiteral("label_dispense_flowrate"));
        label_dispense_flowrate->setGeometry(QRect(650, 280, 411, 51));
        pushButton_problems = new QPushButton(page_dispenser);
        pushButton_problems->setObjectName(QStringLiteral("pushButton_problems"));
        pushButton_problems->setGeometry(QRect(120, 40, 840, 71));
        label_dispense_message = new QLabel(page_dispenser);
        label_dispense_message->setObjectName(QStringLiteral("label_dispense_message"));
        label_dispense_message->setGeometry(QRect(0, 1710, 1051, 191));
        label_logo = new QLabel(page_dispenser);
        label_logo->setObjectName(QStringLiteral("label_logo"));
        label_logo->setGeometry(QRect(600, 180, 151, 131));
        pushButton_abort->raise();
        label_moving_bottle_fill_effect->raise();
        label_background_during_dispense_animation->raise();
        label_volume_dispensed->raise();
        label_volume_dispensed_ml->raise();
        label_to_refill->raise();
        label_instructions_container->raise();
        label_press->raise();
        pushButton_debug_Button->raise();
        label_finishTransactionMessage->raise();
        label_dispense_status->raise();
        label_dispense_flowrate->raise();
        pushButton_problems->raise();
        label_dispense_message->raise();
        label_logo->raise();

        retranslateUi(page_dispenser);

        QMetaObject::connectSlotsByName(page_dispenser);
    } // setupUi

    void retranslateUi(QWidget *page_dispenser)
    {
        page_dispenser->setWindowTitle(QApplication::translate("page_dispenser", "Form", Q_NULLPTR));
        label_background_during_dispense_animation->setText(QString());
        label_moving_bottle_fill_effect->setText(QApplication::translate("page_dispenser", "<html><head/><body><p><img src=\"/home/df-admin/production/references/templates/default/bottle_fill_for_animation.png\"/></p></body></html>", Q_NULLPTR));
        pushButton_abort->setText(QString());
        label_volume_dispensed->setText(QApplication::translate("page_dispenser", "volume dispensed", Q_NULLPTR));
        label_volume_dispensed_ml->setText(QApplication::translate("page_dispenser", "0.0", Q_NULLPTR));
        label_to_refill->setText(QApplication::translate("page_dispenser", "TextLabel", Q_NULLPTR));
        label_instructions_container->setText(QApplication::translate("page_dispenser", "TextLabel", Q_NULLPTR));
        label_press->setText(QApplication::translate("page_dispenser", "TextLabel", Q_NULLPTR));
        pushButton_debug_Button->setText(QString());
        label_finishTransactionMessage->setText(QString());
        label_dispense_status->setText(QString());
        label_dispense_flowrate->setText(QString());
        pushButton_problems->setText(QApplication::translate("page_dispenser", "Problem detected", Q_NULLPTR));
        label_dispense_message->setText(QApplication::translate("page_dispenser", "TextLabel", Q_NULLPTR));
        label_logo->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class page_dispenser: public Ui_page_dispenser {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_DISPENSER_H
