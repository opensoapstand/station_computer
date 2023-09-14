/********************************************************************************
** Form generated from reading UI file 'page_maintenance_dispenser.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_MAINTENANCE_DISPENSER_H
#define UI_PAGE_MAINTENANCE_DISPENSER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_page_maintenance_dispenser
{
public:
    QPushButton *pushButton_to_previous_page;
    QLabel *label_product_name;
    QLabel *label_volume_per_tick;
    QPushButton *pushButton_price_medium;
    QPushButton *pushButton_volume_per_tick;
    QPushButton *pushButton_price_small;
    QPushButton *pushButton_restock;
    QLabel *label_action_feedback;
    QPushButton *pushButton_set_status;
    QPushButton *pushButton_set_restock_volume;
    QLabel *label_restock_volume;
    QPushButton *pushButton_set_volume_remaining;
    QLabel *label_volume_dispensed_total;
    QLabel *label_volume_remaining;
    QLabel *label_restock_timestamp;
    QLabel *label_setting_temperature;
    QPushButton *pushButton_setting_temperature;
    QLabel *label_setting_speed_pwm;
    QPushButton *pushButton_setting_speed_pwm;
    QWidget *numberEntry;
    QLineEdit *textEntry;
    QPushButton *button1;
    QPushButton *button2;
    QPushButton *button3;
    QPushButton *button6;
    QPushButton *button5;
    QPushButton *button4;
    QPushButton *button9;
    QPushButton *button8;
    QPushButton *button7;
    QPushButton *button0;
    QPushButton *buttonBack;
    QPushButton *buttonPeriod;
    QPushButton *pushButton_cancel;
    QLabel *label_title;
    QLabel *errorLabel;
    QPushButton *pushButton_done;
    QPushButton *pushButton_enable_pump;
    QLabel *label_pump_enabled_status;
    QLabel *label_status_volume_dispensed;
    QPushButton *pushButton_target_volume_small;
    QPushButton *pushButton_target_volume_medium;
    QLabel *label_volume_dispensed_since_restock;
    QPushButton *pushButton_auto_dispense_small;
    QPushButton *pushButton_auto_dispense_medium;
    QPushButton *pushButton_auto_dispense_large;
    QLabel *label_status_dispense_button_press_count;
    QLabel *label_status_button_press_time;
    QLabel *label_status_flow_rate;
    QLabel *label_calibration_instructions;
    QPushButton *pushButton_price_large;
    QPushButton *pushButton_target_volume_large;
    QPushButton *pushButton_price_custom;
    QPushButton *pushButton_plu_medium;
    QPushButton *pushButton_plu_large;
    QPushButton *pushButton_plu_small;
    QPushButton *pushButton_plu_custom;
    QLabel *label_calibration_result;
    QLabel *label_status_dispenser;
    QFrame *line;
    QFrame *line_2;
    QFrame *line_3;
    QFrame *line_4;
    QFrame *line_5;
    QFrame *line_6;
    QLabel *label_product_picture;
    QLabel *label_column_price;
    QLabel *label_column_target_volume;
    QLabel *label_column_plu;
    QLabel *label_row_small;
    QLabel *label_row_medium;
    QLabel *label_row_large;
    QLabel *label_row_custom;
    QPushButton *pushButton_target_volume_custom;
    QLabel *label_column_size;
    QFrame *line_7;
    QFrame *line_8;
    QFrame *line_9;
    QFrame *line_10;
    QFrame *line_11;
    QFrame *line_12;
    QFrame *line_13;
    QFrame *line_14;
    QFrame *line_15;
    QFrame *line_16;
    QFrame *line_17;
    QFrame *line_18;
    QFrame *line_19;
    QFrame *line_20;
    QFrame *line_21;
    QFrame *line_22;
    QFrame *line_24;
    QFrame *line_26;
    QFrame *line_27;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *labelText_volume_remaining;
    QLabel *labelText_volume_dispensed_since_restock;
    QLabel *labelText_volume_dispensed_grand_total;
    QLabel *labelText_restock_time;
    QLabel *labelText_restock_volume;
    QLabel *label_11;
    QPushButton *pushButton_clear_problem;
    QLabel *label_status_dispenser_elaborated;
    QFrame *line_23;
    QLabel *label_column_enabled;
    QCheckBox *checkBox_enable_small;
    QCheckBox *checkBox_enable_medium;
    QCheckBox *checkBox_enable_large;
    QCheckBox *checkBox_enable_custom;
    QLabel *label_setting_temperature_2;
    QButtonGroup *buttonGroup_keypad;
    QButtonGroup *buttonGroup_edit_product;

    void setupUi(QWidget *page_maintenance_dispenser)
    {
        if (page_maintenance_dispenser->objectName().isEmpty())
            page_maintenance_dispenser->setObjectName(QStringLiteral("page_maintenance_dispenser"));
        page_maintenance_dispenser->resize(1080, 1920);
        pushButton_to_previous_page = new QPushButton(page_maintenance_dispenser);
        pushButton_to_previous_page->setObjectName(QStringLiteral("pushButton_to_previous_page"));
        pushButton_to_previous_page->setGeometry(QRect(70, 40, 291, 41));
        label_product_name = new QLabel(page_maintenance_dispenser);
        label_product_name->setObjectName(QStringLiteral("label_product_name"));
        label_product_name->setGeometry(QRect(20, 650, 311, 51));
        label_volume_per_tick = new QLabel(page_maintenance_dispenser);
        label_volume_per_tick->setObjectName(QStringLiteral("label_volume_per_tick"));
        label_volume_per_tick->setGeometry(QRect(630, 990, 121, 16));
        pushButton_price_medium = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product = new QButtonGroup(page_maintenance_dispenser);
        buttonGroup_edit_product->setObjectName(QStringLiteral("buttonGroup_edit_product"));
        buttonGroup_edit_product->addButton(pushButton_price_medium);
        pushButton_price_medium->setObjectName(QStringLiteral("pushButton_price_medium"));
        pushButton_price_medium->setGeometry(QRect(490, 660, 181, 41));
        pushButton_volume_per_tick = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product->addButton(pushButton_volume_per_tick);
        pushButton_volume_per_tick->setObjectName(QStringLiteral("pushButton_volume_per_tick"));
        pushButton_volume_per_tick->setGeometry(QRect(830, 980, 201, 41));
        pushButton_price_small = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product->addButton(pushButton_price_small);
        pushButton_price_small->setObjectName(QStringLiteral("pushButton_price_small"));
        pushButton_price_small->setGeometry(QRect(490, 620, 181, 41));
        pushButton_restock = new QPushButton(page_maintenance_dispenser);
        pushButton_restock->setObjectName(QStringLiteral("pushButton_restock"));
        pushButton_restock->setGeometry(QRect(829, 40, 201, 201));
        label_action_feedback = new QLabel(page_maintenance_dispenser);
        label_action_feedback->setObjectName(QStringLiteral("label_action_feedback"));
        label_action_feedback->setGeometry(QRect(340, 250, 691, 91));
        QFont font;
        font.setPointSize(20);
        label_action_feedback->setFont(font);
        label_action_feedback->setWordWrap(true);
        pushButton_set_status = new QPushButton(page_maintenance_dispenser);
        pushButton_set_status->setObjectName(QStringLiteral("pushButton_set_status"));
        pushButton_set_status->setGeometry(QRect(610, 40, 201, 201));
        pushButton_set_restock_volume = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product->addButton(pushButton_set_restock_volume);
        pushButton_set_restock_volume->setObjectName(QStringLiteral("pushButton_set_restock_volume"));
        pushButton_set_restock_volume->setGeometry(QRect(830, 500, 200, 41));
        label_restock_volume = new QLabel(page_maintenance_dispenser);
        label_restock_volume->setObjectName(QStringLiteral("label_restock_volume"));
        label_restock_volume->setGeometry(QRect(629, 510, 191, 21));
        pushButton_set_volume_remaining = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product->addButton(pushButton_set_volume_remaining);
        pushButton_set_volume_remaining->setObjectName(QStringLiteral("pushButton_set_volume_remaining"));
        pushButton_set_volume_remaining->setGeometry(QRect(830, 340, 201, 41));
        label_volume_dispensed_total = new QLabel(page_maintenance_dispenser);
        label_volume_dispensed_total->setObjectName(QStringLiteral("label_volume_dispensed_total"));
        label_volume_dispensed_total->setGeometry(QRect(629, 430, 401, 21));
        label_volume_remaining = new QLabel(page_maintenance_dispenser);
        label_volume_remaining->setObjectName(QStringLiteral("label_volume_remaining"));
        label_volume_remaining->setGeometry(QRect(630, 340, 191, 31));
        label_restock_timestamp = new QLabel(page_maintenance_dispenser);
        label_restock_timestamp->setObjectName(QStringLiteral("label_restock_timestamp"));
        label_restock_timestamp->setGeometry(QRect(630, 470, 421, 21));
        label_setting_temperature = new QLabel(page_maintenance_dispenser);
        label_setting_temperature->setObjectName(QStringLiteral("label_setting_temperature"));
        label_setting_temperature->setGeometry(QRect(340, 1190, 361, 16));
        pushButton_setting_temperature = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product->addButton(pushButton_setting_temperature);
        pushButton_setting_temperature->setObjectName(QStringLiteral("pushButton_setting_temperature"));
        pushButton_setting_temperature->setGeometry(QRect(830, 1180, 200, 41));
        label_setting_speed_pwm = new QLabel(page_maintenance_dispenser);
        label_setting_speed_pwm->setObjectName(QStringLiteral("label_setting_speed_pwm"));
        label_setting_speed_pwm->setGeometry(QRect(340, 1151, 300, 16));
        pushButton_setting_speed_pwm = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product->addButton(pushButton_setting_speed_pwm);
        pushButton_setting_speed_pwm->setObjectName(QStringLiteral("pushButton_setting_speed_pwm"));
        pushButton_setting_speed_pwm->setGeometry(QRect(830, 1140, 200, 41));
        numberEntry = new QWidget(page_maintenance_dispenser);
        numberEntry->setObjectName(QStringLiteral("numberEntry"));
        numberEntry->setGeometry(QRect(0, 820, 331, 631));
        textEntry = new QLineEdit(numberEntry);
        textEntry->setObjectName(QStringLiteral("textEntry"));
        textEntry->setGeometry(QRect(10, 60, 311, 71));
        textEntry->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        button1 = new QPushButton(numberEntry);
        buttonGroup_keypad = new QButtonGroup(page_maintenance_dispenser);
        buttonGroup_keypad->setObjectName(QStringLiteral("buttonGroup_keypad"));
        buttonGroup_keypad->addButton(button1);
        button1->setObjectName(QStringLiteral("button1"));
        button1->setGeometry(QRect(10, 200, 89, 71));
        button2 = new QPushButton(numberEntry);
        buttonGroup_keypad->addButton(button2);
        button2->setObjectName(QStringLiteral("button2"));
        button2->setGeometry(QRect(120, 200, 89, 71));
        button3 = new QPushButton(numberEntry);
        buttonGroup_keypad->addButton(button3);
        button3->setObjectName(QStringLiteral("button3"));
        button3->setGeometry(QRect(230, 200, 89, 71));
        button6 = new QPushButton(numberEntry);
        buttonGroup_keypad->addButton(button6);
        button6->setObjectName(QStringLiteral("button6"));
        button6->setGeometry(QRect(230, 280, 89, 71));
        button5 = new QPushButton(numberEntry);
        buttonGroup_keypad->addButton(button5);
        button5->setObjectName(QStringLiteral("button5"));
        button5->setGeometry(QRect(120, 280, 89, 71));
        button4 = new QPushButton(numberEntry);
        buttonGroup_keypad->addButton(button4);
        button4->setObjectName(QStringLiteral("button4"));
        button4->setGeometry(QRect(10, 280, 89, 71));
        button9 = new QPushButton(numberEntry);
        buttonGroup_keypad->addButton(button9);
        button9->setObjectName(QStringLiteral("button9"));
        button9->setGeometry(QRect(230, 360, 89, 71));
        button8 = new QPushButton(numberEntry);
        buttonGroup_keypad->addButton(button8);
        button8->setObjectName(QStringLiteral("button8"));
        button8->setGeometry(QRect(120, 360, 89, 71));
        button7 = new QPushButton(numberEntry);
        buttonGroup_keypad->addButton(button7);
        button7->setObjectName(QStringLiteral("button7"));
        button7->setGeometry(QRect(10, 360, 89, 71));
        button0 = new QPushButton(numberEntry);
        buttonGroup_keypad->addButton(button0);
        button0->setObjectName(QStringLiteral("button0"));
        button0->setGeometry(QRect(120, 440, 89, 71));
        buttonBack = new QPushButton(numberEntry);
        buttonBack->setObjectName(QStringLiteral("buttonBack"));
        buttonBack->setGeometry(QRect(230, 440, 89, 71));
        buttonPeriod = new QPushButton(numberEntry);
        buttonPeriod->setObjectName(QStringLiteral("buttonPeriod"));
        buttonPeriod->setGeometry(QRect(10, 440, 89, 71));
        pushButton_cancel = new QPushButton(numberEntry);
        pushButton_cancel->setObjectName(QStringLiteral("pushButton_cancel"));
        pushButton_cancel->setGeometry(QRect(10, 550, 141, 71));
        label_title = new QLabel(numberEntry);
        label_title->setObjectName(QStringLiteral("label_title"));
        label_title->setGeometry(QRect(10, 10, 321, 41));
        QFont font1;
        font1.setFamily(QStringLiteral("Proxima Nova ScOsf"));
        font1.setPointSize(14);
        font1.setBold(true);
        font1.setWeight(75);
        label_title->setFont(font1);
        errorLabel = new QLabel(numberEntry);
        errorLabel->setObjectName(QStringLiteral("errorLabel"));
        errorLabel->setGeometry(QRect(0, 150, 321, 31));
        pushButton_done = new QPushButton(numberEntry);
        pushButton_done->setObjectName(QStringLiteral("pushButton_done"));
        pushButton_done->setGeometry(QRect(180, 550, 141, 71));
        pushButton_enable_pump = new QPushButton(page_maintenance_dispenser);
        pushButton_enable_pump->setObjectName(QStringLiteral("pushButton_enable_pump"));
        pushButton_enable_pump->setGeometry(QRect(40, 1500, 251, 251));
        label_pump_enabled_status = new QLabel(page_maintenance_dispenser);
        label_pump_enabled_status->setObjectName(QStringLiteral("label_pump_enabled_status"));
        label_pump_enabled_status->setGeometry(QRect(330, 1500, 351, 20));
        label_pump_enabled_status->setLayoutDirection(Qt::LeftToRight);
        label_pump_enabled_status->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_status_volume_dispensed = new QLabel(page_maintenance_dispenser);
        label_status_volume_dispensed->setObjectName(QStringLiteral("label_status_volume_dispensed"));
        label_status_volume_dispensed->setGeometry(QRect(630, 830, 191, 17));
        pushButton_target_volume_small = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product->addButton(pushButton_target_volume_small);
        pushButton_target_volume_small->setObjectName(QStringLiteral("pushButton_target_volume_small"));
        pushButton_target_volume_small->setGeometry(QRect(670, 620, 181, 41));
        pushButton_target_volume_medium = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product->addButton(pushButton_target_volume_medium);
        pushButton_target_volume_medium->setObjectName(QStringLiteral("pushButton_target_volume_medium"));
        pushButton_target_volume_medium->setGeometry(QRect(670, 660, 181, 41));
        label_volume_dispensed_since_restock = new QLabel(page_maintenance_dispenser);
        label_volume_dispensed_since_restock->setObjectName(QStringLiteral("label_volume_dispensed_since_restock"));
        label_volume_dispensed_since_restock->setGeometry(QRect(630, 380, 341, 31));
        pushButton_auto_dispense_small = new QPushButton(page_maintenance_dispenser);
        pushButton_auto_dispense_small->setObjectName(QStringLiteral("pushButton_auto_dispense_small"));
        pushButton_auto_dispense_small->setGeometry(QRect(330, 1240, 200, 200));
        pushButton_auto_dispense_medium = new QPushButton(page_maintenance_dispenser);
        pushButton_auto_dispense_medium->setObjectName(QStringLiteral("pushButton_auto_dispense_medium"));
        pushButton_auto_dispense_medium->setGeometry(QRect(580, 1240, 200, 200));
        pushButton_auto_dispense_large = new QPushButton(page_maintenance_dispenser);
        pushButton_auto_dispense_large->setObjectName(QStringLiteral("pushButton_auto_dispense_large"));
        pushButton_auto_dispense_large->setGeometry(QRect(830, 1240, 200, 200));
        label_status_dispense_button_press_count = new QLabel(page_maintenance_dispenser);
        label_status_dispense_button_press_count->setObjectName(QStringLiteral("label_status_dispense_button_press_count"));
        label_status_dispense_button_press_count->setGeometry(QRect(630, 910, 191, 17));
        label_status_button_press_time = new QLabel(page_maintenance_dispenser);
        label_status_button_press_time->setObjectName(QStringLiteral("label_status_button_press_time"));
        label_status_button_press_time->setGeometry(QRect(630, 950, 191, 17));
        label_status_flow_rate = new QLabel(page_maintenance_dispenser);
        label_status_flow_rate->setObjectName(QStringLiteral("label_status_flow_rate"));
        label_status_flow_rate->setGeometry(QRect(630, 870, 191, 17));
        label_calibration_instructions = new QLabel(page_maintenance_dispenser);
        label_calibration_instructions->setObjectName(QStringLiteral("label_calibration_instructions"));
        label_calibration_instructions->setGeometry(QRect(340, 1060, 581, 81));
        pushButton_price_large = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product->addButton(pushButton_price_large);
        pushButton_price_large->setObjectName(QStringLiteral("pushButton_price_large"));
        pushButton_price_large->setGeometry(QRect(490, 700, 181, 41));
        pushButton_target_volume_large = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product->addButton(pushButton_target_volume_large);
        pushButton_target_volume_large->setObjectName(QStringLiteral("pushButton_target_volume_large"));
        pushButton_target_volume_large->setGeometry(QRect(670, 700, 181, 41));
        pushButton_price_custom = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product->addButton(pushButton_price_custom);
        pushButton_price_custom->setObjectName(QStringLiteral("pushButton_price_custom"));
        pushButton_price_custom->setGeometry(QRect(490, 740, 181, 41));
        pushButton_plu_medium = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product->addButton(pushButton_plu_medium);
        pushButton_plu_medium->setObjectName(QStringLiteral("pushButton_plu_medium"));
        pushButton_plu_medium->setGeometry(QRect(850, 660, 181, 41));
        pushButton_plu_large = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product->addButton(pushButton_plu_large);
        pushButton_plu_large->setObjectName(QStringLiteral("pushButton_plu_large"));
        pushButton_plu_large->setGeometry(QRect(850, 700, 181, 41));
        pushButton_plu_small = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product->addButton(pushButton_plu_small);
        pushButton_plu_small->setObjectName(QStringLiteral("pushButton_plu_small"));
        pushButton_plu_small->setGeometry(QRect(850, 620, 181, 41));
        pushButton_plu_custom = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product->addButton(pushButton_plu_custom);
        pushButton_plu_custom->setObjectName(QStringLiteral("pushButton_plu_custom"));
        pushButton_plu_custom->setGeometry(QRect(850, 740, 181, 41));
        label_calibration_result = new QLabel(page_maintenance_dispenser);
        label_calibration_result->setObjectName(QStringLiteral("label_calibration_result"));
        label_calibration_result->setGeometry(QRect(630, 1030, 591, 17));
        label_status_dispenser = new QLabel(page_maintenance_dispenser);
        label_status_dispenser->setObjectName(QStringLiteral("label_status_dispenser"));
        label_status_dispenser->setGeometry(QRect(20, 250, 311, 81));
        line = new QFrame(page_maintenance_dispenser);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(340, 410, 691, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(page_maintenance_dispenser);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setGeometry(QRect(340, 450, 691, 20));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        line_3 = new QFrame(page_maintenance_dispenser);
        line_3->setObjectName(QStringLiteral("line_3"));
        line_3->setGeometry(QRect(340, 370, 691, 20));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        line_4 = new QFrame(page_maintenance_dispenser);
        line_4->setObjectName(QStringLiteral("line_4"));
        line_4->setGeometry(QRect(340, 490, 691, 20));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);
        line_5 = new QFrame(page_maintenance_dispenser);
        line_5->setObjectName(QStringLiteral("line_5"));
        line_5->setGeometry(QRect(340, 530, 691, 20));
        line_5->setFrameShape(QFrame::HLine);
        line_5->setFrameShadow(QFrame::Sunken);
        line_6 = new QFrame(page_maintenance_dispenser);
        line_6->setObjectName(QStringLiteral("line_6"));
        line_6->setGeometry(QRect(340, 330, 691, 20));
        line_6->setFrameShape(QFrame::HLine);
        line_6->setFrameShadow(QFrame::Sunken);
        label_product_picture = new QLabel(page_maintenance_dispenser);
        label_product_picture->setObjectName(QStringLiteral("label_product_picture"));
        label_product_picture->setGeometry(QRect(20, 340, 300, 300));
        label_column_price = new QLabel(page_maintenance_dispenser);
        label_column_price->setObjectName(QStringLiteral("label_column_price"));
        label_column_price->setGeometry(QRect(560, 590, 67, 17));
        label_column_target_volume = new QLabel(page_maintenance_dispenser);
        label_column_target_volume->setObjectName(QStringLiteral("label_column_target_volume"));
        label_column_target_volume->setGeometry(QRect(710, 590, 131, 17));
        label_column_plu = new QLabel(page_maintenance_dispenser);
        label_column_plu->setObjectName(QStringLiteral("label_column_plu"));
        label_column_plu->setGeometry(QRect(890, 590, 131, 17));
        label_row_small = new QLabel(page_maintenance_dispenser);
        label_row_small->setObjectName(QStringLiteral("label_row_small"));
        label_row_small->setGeometry(QRect(350, 630, 67, 17));
        label_row_medium = new QLabel(page_maintenance_dispenser);
        label_row_medium->setObjectName(QStringLiteral("label_row_medium"));
        label_row_medium->setGeometry(QRect(350, 670, 67, 17));
        label_row_large = new QLabel(page_maintenance_dispenser);
        label_row_large->setObjectName(QStringLiteral("label_row_large"));
        label_row_large->setGeometry(QRect(350, 710, 67, 17));
        label_row_custom = new QLabel(page_maintenance_dispenser);
        label_row_custom->setObjectName(QStringLiteral("label_row_custom"));
        label_row_custom->setGeometry(QRect(350, 750, 67, 17));
        pushButton_target_volume_custom = new QPushButton(page_maintenance_dispenser);
        buttonGroup_edit_product->addButton(pushButton_target_volume_custom);
        pushButton_target_volume_custom->setObjectName(QStringLiteral("pushButton_target_volume_custom"));
        pushButton_target_volume_custom->setGeometry(QRect(670, 740, 181, 41));
        label_column_size = new QLabel(page_maintenance_dispenser);
        label_column_size->setObjectName(QStringLiteral("label_column_size"));
        label_column_size->setGeometry(QRect(350, 590, 67, 17));
        line_7 = new QFrame(page_maintenance_dispenser);
        line_7->setObjectName(QStringLiteral("line_7"));
        line_7->setGeometry(QRect(340, 610, 691, 20));
        line_7->setFrameShape(QFrame::HLine);
        line_7->setFrameShadow(QFrame::Sunken);
        line_8 = new QFrame(page_maintenance_dispenser);
        line_8->setObjectName(QStringLiteral("line_8"));
        line_8->setGeometry(QRect(340, 650, 691, 20));
        line_8->setFrameShape(QFrame::HLine);
        line_8->setFrameShadow(QFrame::Sunken);
        line_9 = new QFrame(page_maintenance_dispenser);
        line_9->setObjectName(QStringLiteral("line_9"));
        line_9->setGeometry(QRect(340, 690, 691, 20));
        line_9->setFrameShape(QFrame::HLine);
        line_9->setFrameShadow(QFrame::Sunken);
        line_10 = new QFrame(page_maintenance_dispenser);
        line_10->setObjectName(QStringLiteral("line_10"));
        line_10->setGeometry(QRect(340, 730, 691, 20));
        line_10->setFrameShape(QFrame::HLine);
        line_10->setFrameShadow(QFrame::Sunken);
        line_11 = new QFrame(page_maintenance_dispenser);
        line_11->setObjectName(QStringLiteral("line_11"));
        line_11->setGeometry(QRect(340, 770, 691, 20));
        line_11->setFrameShape(QFrame::HLine);
        line_11->setFrameShadow(QFrame::Sunken);
        line_12 = new QFrame(page_maintenance_dispenser);
        line_12->setObjectName(QStringLiteral("line_12"));
        line_12->setGeometry(QRect(480, 580, 20, 201));
        line_12->setFrameShape(QFrame::VLine);
        line_12->setFrameShadow(QFrame::Sunken);
        line_13 = new QFrame(page_maintenance_dispenser);
        line_13->setObjectName(QStringLiteral("line_13"));
        line_13->setGeometry(QRect(660, 580, 20, 201));
        line_13->setFrameShape(QFrame::VLine);
        line_13->setFrameShadow(QFrame::Sunken);
        line_14 = new QFrame(page_maintenance_dispenser);
        line_14->setObjectName(QStringLiteral("line_14"));
        line_14->setGeometry(QRect(840, 580, 20, 201));
        line_14->setFrameShape(QFrame::VLine);
        line_14->setFrameShadow(QFrame::Sunken);
        line_15 = new QFrame(page_maintenance_dispenser);
        line_15->setObjectName(QStringLiteral("line_15"));
        line_15->setGeometry(QRect(1020, 540, 20, 201));
        line_15->setFrameShape(QFrame::VLine);
        line_15->setFrameShadow(QFrame::Sunken);
        line_16 = new QFrame(page_maintenance_dispenser);
        line_16->setObjectName(QStringLiteral("line_16"));
        line_16->setGeometry(QRect(340, 570, 691, 20));
        line_16->setFrameShape(QFrame::HLine);
        line_16->setFrameShadow(QFrame::Sunken);
        line_17 = new QFrame(page_maintenance_dispenser);
        line_17->setObjectName(QStringLiteral("line_17"));
        line_17->setGeometry(QRect(340, 890, 691, 20));
        line_17->setFrameShape(QFrame::HLine);
        line_17->setFrameShadow(QFrame::Sunken);
        line_18 = new QFrame(page_maintenance_dispenser);
        line_18->setObjectName(QStringLiteral("line_18"));
        line_18->setGeometry(QRect(340, 930, 691, 20));
        line_18->setFrameShape(QFrame::HLine);
        line_18->setFrameShadow(QFrame::Sunken);
        line_19 = new QFrame(page_maintenance_dispenser);
        line_19->setObjectName(QStringLiteral("line_19"));
        line_19->setGeometry(QRect(340, 850, 691, 20));
        line_19->setFrameShape(QFrame::HLine);
        line_19->setFrameShadow(QFrame::Sunken);
        line_20 = new QFrame(page_maintenance_dispenser);
        line_20->setObjectName(QStringLiteral("line_20"));
        line_20->setGeometry(QRect(340, 1010, 691, 20));
        line_20->setFrameShape(QFrame::HLine);
        line_20->setFrameShadow(QFrame::Sunken);
        line_21 = new QFrame(page_maintenance_dispenser);
        line_21->setObjectName(QStringLiteral("line_21"));
        line_21->setGeometry(QRect(340, 970, 691, 20));
        line_21->setFrameShape(QFrame::HLine);
        line_21->setFrameShadow(QFrame::Sunken);
        line_22 = new QFrame(page_maintenance_dispenser);
        line_22->setObjectName(QStringLiteral("line_22"));
        line_22->setGeometry(QRect(340, 810, 691, 20));
        line_22->setFrameShape(QFrame::HLine);
        line_22->setFrameShadow(QFrame::Sunken);
        line_24 = new QFrame(page_maintenance_dispenser);
        line_24->setObjectName(QStringLiteral("line_24"));
        line_24->setGeometry(QRect(340, 1130, 691, 20));
        line_24->setFrameShape(QFrame::HLine);
        line_24->setFrameShadow(QFrame::Sunken);
        line_26 = new QFrame(page_maintenance_dispenser);
        line_26->setObjectName(QStringLiteral("line_26"));
        line_26->setGeometry(QRect(340, 1170, 691, 20));
        line_26->setFrameShape(QFrame::HLine);
        line_26->setFrameShadow(QFrame::Sunken);
        line_27 = new QFrame(page_maintenance_dispenser);
        line_27->setObjectName(QStringLiteral("line_27"));
        line_27->setGeometry(QRect(340, 1210, 691, 20));
        line_27->setFrameShape(QFrame::HLine);
        line_27->setFrameShadow(QFrame::Sunken);
        label = new QLabel(page_maintenance_dispenser);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(340, 910, 161, 17));
        label_2 = new QLabel(page_maintenance_dispenser);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(340, 830, 161, 17));
        label_3 = new QLabel(page_maintenance_dispenser);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(340, 950, 201, 17));
        label_4 = new QLabel(page_maintenance_dispenser);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(340, 870, 171, 17));
        label_5 = new QLabel(page_maintenance_dispenser);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(340, 990, 261, 17));
        labelText_volume_remaining = new QLabel(page_maintenance_dispenser);
        labelText_volume_remaining->setObjectName(QStringLiteral("labelText_volume_remaining"));
        labelText_volume_remaining->setGeometry(QRect(340, 350, 241, 17));
        labelText_volume_dispensed_since_restock = new QLabel(page_maintenance_dispenser);
        labelText_volume_dispensed_since_restock->setObjectName(QStringLiteral("labelText_volume_dispensed_since_restock"));
        labelText_volume_dispensed_since_restock->setGeometry(QRect(340, 390, 241, 17));
        labelText_volume_dispensed_grand_total = new QLabel(page_maintenance_dispenser);
        labelText_volume_dispensed_grand_total->setObjectName(QStringLiteral("labelText_volume_dispensed_grand_total"));
        labelText_volume_dispensed_grand_total->setGeometry(QRect(340, 430, 241, 17));
        labelText_restock_time = new QLabel(page_maintenance_dispenser);
        labelText_restock_time->setObjectName(QStringLiteral("labelText_restock_time"));
        labelText_restock_time->setGeometry(QRect(340, 470, 241, 17));
        labelText_restock_volume = new QLabel(page_maintenance_dispenser);
        labelText_restock_volume->setObjectName(QStringLiteral("labelText_restock_volume"));
        labelText_restock_volume->setGeometry(QRect(340, 510, 241, 17));
        label_11 = new QLabel(page_maintenance_dispenser);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(340, 1030, 291, 17));
        pushButton_clear_problem = new QPushButton(page_maintenance_dispenser);
        pushButton_clear_problem->setObjectName(QStringLiteral("pushButton_clear_problem"));
        pushButton_clear_problem->setGeometry(QRect(390, 40, 201, 201));
        label_status_dispenser_elaborated = new QLabel(page_maintenance_dispenser);
        label_status_dispenser_elaborated->setObjectName(QStringLiteral("label_status_dispenser_elaborated"));
        label_status_dispenser_elaborated->setGeometry(QRect(330, 1520, 701, 51));
        line_23 = new QFrame(page_maintenance_dispenser);
        line_23->setObjectName(QStringLiteral("line_23"));
        line_23->setGeometry(QRect(410, 580, 20, 201));
        line_23->setFrameShape(QFrame::VLine);
        line_23->setFrameShadow(QFrame::Sunken);
        label_column_enabled = new QLabel(page_maintenance_dispenser);
        label_column_enabled->setObjectName(QStringLiteral("label_column_enabled"));
        label_column_enabled->setGeometry(QRect(430, 590, 67, 17));
        checkBox_enable_small = new QCheckBox(page_maintenance_dispenser);
        checkBox_enable_small->setObjectName(QStringLiteral("checkBox_enable_small"));
        checkBox_enable_small->setGeometry(QRect(440, 630, 16, 23));
        checkBox_enable_medium = new QCheckBox(page_maintenance_dispenser);
        checkBox_enable_medium->setObjectName(QStringLiteral("checkBox_enable_medium"));
        checkBox_enable_medium->setGeometry(QRect(440, 670, 16, 23));
        checkBox_enable_large = new QCheckBox(page_maintenance_dispenser);
        checkBox_enable_large->setObjectName(QStringLiteral("checkBox_enable_large"));
        checkBox_enable_large->setGeometry(QRect(440, 710, 16, 23));
        checkBox_enable_custom = new QCheckBox(page_maintenance_dispenser);
        checkBox_enable_custom->setObjectName(QStringLiteral("checkBox_enable_custom"));
        checkBox_enable_custom->setGeometry(QRect(440, 750, 16, 23));
        label_setting_temperature_2 = new QLabel(page_maintenance_dispenser);
        label_setting_temperature_2->setObjectName(QStringLiteral("label_setting_temperature_2"));
        label_setting_temperature_2->setGeometry(QRect(690, 1190, 361, 16));

        retranslateUi(page_maintenance_dispenser);

        QMetaObject::connectSlotsByName(page_maintenance_dispenser);
    } // setupUi

    void retranslateUi(QWidget *page_maintenance_dispenser)
    {
        page_maintenance_dispenser->setWindowTitle(QApplication::translate("page_maintenance_dispenser", "Form", Q_NULLPTR));
        pushButton_to_previous_page->setText(QApplication::translate("page_maintenance_dispenser", "Back", Q_NULLPTR));
        label_product_name->setText(QApplication::translate("page_maintenance_dispenser", "Product Name: ", Q_NULLPTR));
        label_volume_per_tick->setText(QApplication::translate("page_maintenance_dispenser", "Product Volume Per Tick: ", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_price_medium->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "Medium Price", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_price_medium->setText(QApplication::translate("page_maintenance_dispenser", "Medium Price", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_volume_per_tick->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "Volume per flow sensor tick", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_volume_per_tick->setText(QApplication::translate("page_maintenance_dispenser", "Edit", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_price_small->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "price small", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_price_small->setText(QApplication::translate("page_maintenance_dispenser", "Small Price", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_restock->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "Restock", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_restock->setText(QApplication::translate("page_maintenance_dispenser", "Restock", Q_NULLPTR));
        label_action_feedback->setText(QApplication::translate("page_maintenance_dispenser", "feedback", Q_NULLPTR));
        pushButton_set_status->setText(QApplication::translate("page_maintenance_dispenser", "Mark as Sold Out", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_set_restock_volume->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "Set full volume", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_set_restock_volume->setText(QApplication::translate("page_maintenance_dispenser", "Adjust", Q_NULLPTR));
        label_restock_volume->setText(QApplication::translate("page_maintenance_dispenser", "Restock Volume:", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_set_volume_remaining->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "Modify Remaining Stock", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_set_volume_remaining->setText(QApplication::translate("page_maintenance_dispenser", "Adjust", Q_NULLPTR));
        label_volume_dispensed_total->setText(QApplication::translate("page_maintenance_dispenser", "Total Dispensed:", Q_NULLPTR));
        label_volume_remaining->setText(QApplication::translate("page_maintenance_dispenser", "Remaining", Q_NULLPTR));
        label_restock_timestamp->setText(QApplication::translate("page_maintenance_dispenser", "Last Refill", Q_NULLPTR));
        label_setting_temperature->setText(QApplication::translate("page_maintenance_dispenser", "Temperature", Q_NULLPTR));
        pushButton_setting_temperature->setText(QApplication::translate("page_maintenance_dispenser", "Temperature:", Q_NULLPTR));
        label_setting_speed_pwm->setText(QApplication::translate("page_maintenance_dispenser", "Pump Speed", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_setting_speed_pwm->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "Pump Speed Percentage", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_setting_speed_pwm->setText(QApplication::translate("page_maintenance_dispenser", " Pump Speed", Q_NULLPTR));
        textEntry->setText(QString());
        button1->setText(QApplication::translate("page_maintenance_dispenser", "1", Q_NULLPTR));
        button2->setText(QApplication::translate("page_maintenance_dispenser", "2", Q_NULLPTR));
        button3->setText(QApplication::translate("page_maintenance_dispenser", "3", Q_NULLPTR));
        button6->setText(QApplication::translate("page_maintenance_dispenser", "6", Q_NULLPTR));
        button5->setText(QApplication::translate("page_maintenance_dispenser", "5", Q_NULLPTR));
        button4->setText(QApplication::translate("page_maintenance_dispenser", "4", Q_NULLPTR));
        button9->setText(QApplication::translate("page_maintenance_dispenser", "9", Q_NULLPTR));
        button8->setText(QApplication::translate("page_maintenance_dispenser", "8", Q_NULLPTR));
        button7->setText(QApplication::translate("page_maintenance_dispenser", "7", Q_NULLPTR));
        button0->setText(QApplication::translate("page_maintenance_dispenser", "0", Q_NULLPTR));
        buttonBack->setText(QApplication::translate("page_maintenance_dispenser", "<-", Q_NULLPTR));
        buttonPeriod->setText(QApplication::translate("page_maintenance_dispenser", ".", Q_NULLPTR));
        pushButton_cancel->setText(QApplication::translate("page_maintenance_dispenser", "Cancel", Q_NULLPTR));
        label_title->setText(QApplication::translate("page_maintenance_dispenser", "New Price:", Q_NULLPTR));
        errorLabel->setText(QString());
        pushButton_done->setText(QApplication::translate("page_maintenance_dispenser", "DONE", Q_NULLPTR));
        pushButton_enable_pump->setText(QApplication::translate("page_maintenance_dispenser", "Toggle Pump Enable", Q_NULLPTR));
        label_pump_enabled_status->setText(QApplication::translate("page_maintenance_dispenser", "Pump ON/OFF", Q_NULLPTR));
        label_status_volume_dispensed->setText(QApplication::translate("page_maintenance_dispenser", "ticks and volume ", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_target_volume_small->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "Small target volume", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_target_volume_small->setText(QApplication::translate("page_maintenance_dispenser", "Small\n"
" Target Volume", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_target_volume_medium->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "Medium Target volume", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_target_volume_medium->setText(QApplication::translate("page_maintenance_dispenser", "Medium\n"
" Target Volume", Q_NULLPTR));
        label_volume_dispensed_since_restock->setText(QApplication::translate("page_maintenance_dispenser", "TextLabel", Q_NULLPTR));
        pushButton_auto_dispense_small->setText(QApplication::translate("page_maintenance_dispenser", "Auto Dispense Small ", Q_NULLPTR));
        pushButton_auto_dispense_medium->setText(QApplication::translate("page_maintenance_dispenser", "Auto Dispense Medium", Q_NULLPTR));
        pushButton_auto_dispense_large->setText(QApplication::translate("page_maintenance_dispenser", "Auto Dispense Large ", Q_NULLPTR));
        label_status_dispense_button_press_count->setText(QApplication::translate("page_maintenance_dispenser", "Button presses:", Q_NULLPTR));
        label_status_button_press_time->setText(QApplication::translate("page_maintenance_dispenser", "Pump Time / Enable Time", Q_NULLPTR));
        label_status_flow_rate->setText(QApplication::translate("page_maintenance_dispenser", "Flow rate (2s)", Q_NULLPTR));
        label_calibration_instructions->setText(QApplication::translate("page_maintenance_dispenser", "calibration instructions", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_price_large->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "Large Price", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_price_large->setText(QApplication::translate("page_maintenance_dispenser", "Large Price", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_target_volume_large->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "Large target volume", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_target_volume_large->setText(QApplication::translate("page_maintenance_dispenser", "Large\n"
" Target Volume", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_price_custom->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "price custom per ml", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_price_custom->setText(QApplication::translate("page_maintenance_dispenser", "Custom Price (per ml)", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_plu_medium->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "Medium PLU", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_plu_medium->setText(QApplication::translate("page_maintenance_dispenser", "Medium PLU", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_plu_large->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "Large PLU", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_plu_large->setText(QApplication::translate("page_maintenance_dispenser", "Large PLU", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_plu_small->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "Small PLU", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_plu_small->setText(QApplication::translate("page_maintenance_dispenser", "Small PLU", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_plu_custom->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "Custom PLU", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_plu_custom->setText(QApplication::translate("page_maintenance_dispenser", "Custom PLU", Q_NULLPTR));
        label_calibration_result->setText(QApplication::translate("page_maintenance_dispenser", "Calibraton value (ml/tick if exactly 1000ml dispensed):", Q_NULLPTR));
        label_status_dispenser->setText(QApplication::translate("page_maintenance_dispenser", "Dispense Status", Q_NULLPTR));
        label_product_picture->setText(QApplication::translate("page_maintenance_dispenser", "TextLabel", Q_NULLPTR));
        label_column_price->setText(QApplication::translate("page_maintenance_dispenser", "Price", Q_NULLPTR));
        label_column_target_volume->setText(QApplication::translate("page_maintenance_dispenser", "Target Volume", Q_NULLPTR));
        label_column_plu->setText(QApplication::translate("page_maintenance_dispenser", "PLU/barcode", Q_NULLPTR));
        label_row_small->setText(QApplication::translate("page_maintenance_dispenser", "Small", Q_NULLPTR));
        label_row_medium->setText(QApplication::translate("page_maintenance_dispenser", "Medium", Q_NULLPTR));
        label_row_large->setText(QApplication::translate("page_maintenance_dispenser", "Large", Q_NULLPTR));
        label_row_custom->setText(QApplication::translate("page_maintenance_dispenser", "Custom", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        pushButton_target_volume_custom->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "Custom maximum volume", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        pushButton_target_volume_custom->setText(QApplication::translate("page_maintenance_dispenser", "Custom\n"
" Target Volume", Q_NULLPTR));
        label_column_size->setText(QApplication::translate("page_maintenance_dispenser", "Size", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        line_9->setAccessibleName(QApplication::translate("page_maintenance_dispenser", "price medium", Q_NULLPTR));
#endif // QT_NO_ACCESSIBILITY
        label->setText(QApplication::translate("page_maintenance_dispenser", "Button Press Count", Q_NULLPTR));
        label_2->setText(QApplication::translate("page_maintenance_dispenser", "Volume Dispensed", Q_NULLPTR));
        label_3->setText(QApplication::translate("page_maintenance_dispenser", "Pump Time / Enable Time", Q_NULLPTR));
        label_4->setText(QApplication::translate("page_maintenance_dispenser", "Flow Rate (2s avg)", Q_NULLPTR));
        label_5->setText(QApplication::translate("page_maintenance_dispenser", "Calibration Volume Per Tick", Q_NULLPTR));
        labelText_volume_remaining->setText(QApplication::translate("page_maintenance_dispenser", "Volume Remaining", Q_NULLPTR));
        labelText_volume_dispensed_since_restock->setText(QApplication::translate("page_maintenance_dispenser", "Volume Dispensed Since Restock", Q_NULLPTR));
        labelText_volume_dispensed_grand_total->setText(QApplication::translate("page_maintenance_dispenser", "Volume Dispensed Grand Total", Q_NULLPTR));
        labelText_restock_time->setText(QApplication::translate("page_maintenance_dispenser", "Restock Time", Q_NULLPTR));
        labelText_restock_volume->setText(QApplication::translate("page_maintenance_dispenser", "Restock Volume", Q_NULLPTR));
        label_11->setText(QApplication::translate("page_maintenance_dispenser", "Calibration value if exactly 1L dispensed", Q_NULLPTR));
        pushButton_clear_problem->setText(QApplication::translate("page_maintenance_dispenser", "Clear problem", Q_NULLPTR));
        label_status_dispenser_elaborated->setText(QApplication::translate("page_maintenance_dispenser", "Dispense Status", Q_NULLPTR));
        label_column_enabled->setText(QApplication::translate("page_maintenance_dispenser", "Enabled", Q_NULLPTR));
        checkBox_enable_small->setText(QString());
        checkBox_enable_medium->setText(QString());
        checkBox_enable_large->setText(QString());
        checkBox_enable_custom->setText(QString());
        label_setting_temperature_2->setText(QApplication::translate("page_maintenance_dispenser", "Temperature2", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class page_maintenance_dispenser: public Ui_page_maintenance_dispenser {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_MAINTENANCE_DISPENSER_H
