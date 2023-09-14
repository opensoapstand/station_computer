/********************************************************************************
** Form generated from reading UI file 'page_maintenance_general.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_MAINTENANCE_GENERAL_H
#define UI_PAGE_MAINTENANCE_GENERAL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_page_maintenance_general
{
public:
    QPushButton *pushButton_back;
    QPushButton *pushButton_minimize;
    QPushButton *pushButton_reboot;
    QPushButton *pushButton_shutdown;
    QLabel *printer_hasPaper_label;
    QLabel *printer_isOnline_label;
    QLabel *label_receipt_printer;
    QCheckBox *checkBox_enable_pump_ramping;
    QCheckBox *checkBox_enable_empty_container;
    QLabel *label_wifi_internet;
    QPushButton *pushButton_wifi_networks;
    QTableWidget *table_wifi_networks;
    QLabel *label_wifi_status;
    QLabel *label_wifi_ip_address;
    QWidget *keyboard_2;
    QGridLayout *keyboard;
    QPushButton *pushButton_206;
    QPushButton *pushButton_205;
    QPushButton *pushButton_255;
    QPushButton *pushButton_223;
    QPushButton *pushButton_242;
    QPushButton *pushButton_252;
    QPushButton *pushButton_199;
    QPushButton *pushButton_195;
    QPushButton *pushButton_204;
    QPushButton *pushButton_219;
    QPushButton *pushButton_245;
    QPushButton *pushButton_254;
    QPushButton *pushButton_207;
    QPushButton *pushButton_217;
    QPushButton *pushButton_237;
    QPushButton *pushButton_220;
    QPushButton *pushButton_201;
    QLineEdit *keyboardTextEntry;
    QPushButton *pushButton_253;
    QPushButton *pushButton_226;
    QPushButton *pushButton_222;
    QPushButton *pushButton_197;
    QPushButton *pushButton_239;
    QPushButton *pushButton_235;
    QPushButton *pushButton_250;
    QPushButton *pushButton_214;
    QPushButton *pushButton_211;
    QPushButton *pushButton_230;
    QPushButton *pushButton_209;
    QPushButton *pushButton_208;
    QPushButton *pushButton_240;
    QPushButton *pushButton_236;
    QPushButton *pushButton_241;
    QPushButton *pushButton_232;
    QPushButton *pushButton_221;
    QPushButton *pushButton_243;
    QPushButton *pushButton_238;
    QPushButton *pushButton_213;
    QPushButton *pushButton_256;
    QPushButton *pushButton_224;
    QPushButton *pushButton_229;
    QPushButton *pushButton_198;
    QPushButton *pushButton_200;
    QPushButton *pushButton_203;
    QPushButton *pushButton_225;
    QPushButton *pushButton_212;
    QPushButton *pushButton_193;
    QPushButton *pushButton_244;
    QPushButton *pushButton_218;
    QPushButton *pushButton_202;
    QPushButton *pushButton_210;
    QPushButton *pushButton_258;
    QPushButton *pushButton_194;
    QPushButton *pushButton_216;
    QPushButton *pushButton_215;
    QPushButton *pushButton_196;
    QPushButton *pushButton_257;
    QPushButton *pushButton_249;
    QPushButton *pushButton_246;
    QPushButton *pushButton_247;
    QPushButton *pushButton_251;
    QPushButton *pushButton_248;
    QPushButton *pushButton_233;
    QPushButton *pushButton_228;
    QPushButton *pushButton_234;
    QPushButton *pushButton_227;
    QPushButton *pushButton_231;
    QLabel *wifiPassLabel;
    QLabel *label_settings;
    QLabel *label_connectivity;
    QLabel *label_4;
    QPushButton *pushButton_printer_check_status;
    QPushButton *pushButton_printer_test_print;
    QPushButton *pushButton_rtunnel_restart;
    QLabel *label_status_feedback;
    QLabel *label_feedback;
    QPushButton *pushButton_network_status;
    QPushButton *pushButton_restart_electronics;
    QPushButton *pushButton_restart_UI;
    QLabel *label_aws_port;
    QLabel *label_machine_id;
    QLabel *label_wifi_name;
    QButtonGroup *buttonGroup;

    void setupUi(QWidget *page_maintenance_general)
    {
        if (page_maintenance_general->objectName().isEmpty())
            page_maintenance_general->setObjectName(QStringLiteral("page_maintenance_general"));
        page_maintenance_general->resize(1080, 1920);
        page_maintenance_general->setStyleSheet(QStringLiteral(""));
        pushButton_back = new QPushButton(page_maintenance_general);
        pushButton_back->setObjectName(QStringLiteral("pushButton_back"));
        pushButton_back->setGeometry(QRect(70, 40, 401, 41));
        pushButton_minimize = new QPushButton(page_maintenance_general);
        pushButton_minimize->setObjectName(QStringLiteral("pushButton_minimize"));
        pushButton_minimize->setGeometry(QRect(60, 940, 221, 61));
        pushButton_reboot = new QPushButton(page_maintenance_general);
        pushButton_reboot->setObjectName(QStringLiteral("pushButton_reboot"));
        pushButton_reboot->setGeometry(QRect(290, 940, 221, 61));
        pushButton_shutdown = new QPushButton(page_maintenance_general);
        pushButton_shutdown->setObjectName(QStringLiteral("pushButton_shutdown"));
        pushButton_shutdown->setGeometry(QRect(290, 1010, 221, 61));
        pushButton_shutdown->setAutoDefault(false);
        pushButton_shutdown->setFlat(false);
        printer_hasPaper_label = new QLabel(page_maintenance_general);
        printer_hasPaper_label->setObjectName(QStringLiteral("printer_hasPaper_label"));
        printer_hasPaper_label->setGeometry(QRect(310, 690, 151, 17));
        printer_isOnline_label = new QLabel(page_maintenance_general);
        printer_isOnline_label->setObjectName(QStringLiteral("printer_isOnline_label"));
        printer_isOnline_label->setGeometry(QRect(310, 650, 201, 17));
        label_receipt_printer = new QLabel(page_maintenance_general);
        label_receipt_printer->setObjectName(QStringLiteral("label_receipt_printer"));
        label_receipt_printer->setGeometry(QRect(60, 620, 111, 17));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label_receipt_printer->setFont(font);
        checkBox_enable_pump_ramping = new QCheckBox(page_maintenance_general);
        checkBox_enable_pump_ramping->setObjectName(QStringLiteral("checkBox_enable_pump_ramping"));
        checkBox_enable_pump_ramping->setGeometry(QRect(60, 810, 911, 23));
        checkBox_enable_empty_container = new QCheckBox(page_maintenance_general);
        checkBox_enable_empty_container->setObjectName(QStringLiteral("checkBox_enable_empty_container"));
        checkBox_enable_empty_container->setGeometry(QRect(60, 780, 911, 23));
        label_wifi_internet = new QLabel(page_maintenance_general);
        label_wifi_internet->setObjectName(QStringLiteral("label_wifi_internet"));
        label_wifi_internet->setGeometry(QRect(310, 410, 391, 31));
        pushButton_wifi_networks = new QPushButton(page_maintenance_general);
        pushButton_wifi_networks->setObjectName(QStringLiteral("pushButton_wifi_networks"));
        pushButton_wifi_networks->setGeometry(QRect(60, 370, 221, 71));
        table_wifi_networks = new QTableWidget(page_maintenance_general);
        if (table_wifi_networks->columnCount() < 1)
            table_wifi_networks->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        table_wifi_networks->setHorizontalHeaderItem(0, __qtablewidgetitem);
        table_wifi_networks->setObjectName(QStringLiteral("table_wifi_networks"));
        table_wifi_networks->setGeometry(QRect(60, 120, 591, 241));
        table_wifi_networks->horizontalHeader()->setDefaultSectionSize(500);
        label_wifi_status = new QLabel(page_maintenance_general);
        label_wifi_status->setObjectName(QStringLiteral("label_wifi_status"));
        label_wifi_status->setGeometry(QRect(310, 440, 621, 31));
        label_wifi_ip_address = new QLabel(page_maintenance_general);
        label_wifi_ip_address->setObjectName(QStringLiteral("label_wifi_ip_address"));
        label_wifi_ip_address->setGeometry(QRect(310, 470, 711, 31));
        keyboard_2 = new QWidget(page_maintenance_general);
        keyboard_2->setObjectName(QStringLiteral("keyboard_2"));
        keyboard_2->setEnabled(true);
        keyboard_2->setGeometry(QRect(-10, 1170, 1051, 441));
        keyboard_2->setStyleSheet(QStringLiteral("background-color:white;"));
        keyboard = new QGridLayout(keyboard_2);
        keyboard->setObjectName(QStringLiteral("keyboard"));
        pushButton_206 = new QPushButton(keyboard_2);
        buttonGroup = new QButtonGroup(page_maintenance_general);
        buttonGroup->setObjectName(QStringLiteral("buttonGroup"));
        buttonGroup->addButton(pushButton_206);
        pushButton_206->setObjectName(QStringLiteral("pushButton_206"));
        pushButton_206->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_206, 3, 0, 1, 1);

        pushButton_205 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_205);
        pushButton_205->setObjectName(QStringLiteral("pushButton_205"));
        pushButton_205->setMaximumSize(QSize(87, 50));

        keyboard->addWidget(pushButton_205, 2, 12, 1, 1);

        pushButton_255 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_255);
        pushButton_255->setObjectName(QStringLiteral("pushButton_255"));
        pushButton_255->setMaximumSize(QSize(87, 50));

        keyboard->addWidget(pushButton_255, 6, 12, 1, 1);

        pushButton_223 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_223);
        pushButton_223->setObjectName(QStringLiteral("pushButton_223"));
        pushButton_223->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_223, 4, 4, 1, 1);

        pushButton_242 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_242);
        pushButton_242->setObjectName(QStringLiteral("pushButton_242"));
        pushButton_242->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_242, 5, 11, 1, 1);

        pushButton_252 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_252);
        pushButton_252->setObjectName(QStringLiteral("pushButton_252"));
        pushButton_252->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_252, 6, 9, 1, 1);

        pushButton_199 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_199);
        pushButton_199->setObjectName(QStringLiteral("pushButton_199"));
        pushButton_199->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_199, 2, 6, 1, 1);

        pushButton_195 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_195);
        pushButton_195->setObjectName(QStringLiteral("pushButton_195"));
        pushButton_195->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_195, 2, 2, 1, 1);

        pushButton_204 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_204);
        pushButton_204->setObjectName(QStringLiteral("pushButton_204"));
        pushButton_204->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_204, 2, 11, 1, 1);

        pushButton_219 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_219);
        pushButton_219->setObjectName(QStringLiteral("pushButton_219"));
        pushButton_219->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_219, 4, 0, 1, 1);

        pushButton_245 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_245);
        pushButton_245->setObjectName(QStringLiteral("pushButton_245"));
        pushButton_245->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_245, 6, 2, 1, 1);

        pushButton_254 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_254);
        pushButton_254->setObjectName(QStringLiteral("pushButton_254"));
        pushButton_254->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_254, 6, 11, 1, 1);

        pushButton_207 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_207);
        pushButton_207->setObjectName(QStringLiteral("pushButton_207"));
        pushButton_207->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_207, 3, 1, 1, 1);

        pushButton_217 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_217);
        pushButton_217->setObjectName(QStringLiteral("pushButton_217"));
        pushButton_217->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_217, 3, 11, 1, 1);

        pushButton_237 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_237);
        pushButton_237->setObjectName(QStringLiteral("pushButton_237"));
        pushButton_237->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_237, 5, 6, 1, 1);

        pushButton_220 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_220);
        pushButton_220->setObjectName(QStringLiteral("pushButton_220"));
        pushButton_220->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_220, 4, 1, 1, 1);

        pushButton_201 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_201);
        pushButton_201->setObjectName(QStringLiteral("pushButton_201"));
        pushButton_201->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_201, 2, 8, 1, 1);

        keyboardTextEntry = new QLineEdit(keyboard_2);
        keyboardTextEntry->setObjectName(QStringLiteral("keyboardTextEntry"));
        keyboardTextEntry->setMinimumSize(QSize(0, 50));

        keyboard->addWidget(keyboardTextEntry, 1, 0, 1, 12);

        pushButton_253 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_253);
        pushButton_253->setObjectName(QStringLiteral("pushButton_253"));
        pushButton_253->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_253, 6, 10, 1, 1);

        pushButton_226 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_226);
        pushButton_226->setObjectName(QStringLiteral("pushButton_226"));
        pushButton_226->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_226, 4, 7, 1, 1);

        pushButton_222 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_222);
        pushButton_222->setObjectName(QStringLiteral("pushButton_222"));
        pushButton_222->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_222, 4, 3, 1, 1);

        pushButton_197 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_197);
        pushButton_197->setObjectName(QStringLiteral("pushButton_197"));
        pushButton_197->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_197, 2, 4, 1, 1);

        pushButton_239 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_239);
        pushButton_239->setObjectName(QStringLiteral("pushButton_239"));
        pushButton_239->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_239, 5, 8, 1, 1);

        pushButton_235 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_235);
        pushButton_235->setObjectName(QStringLiteral("pushButton_235"));
        pushButton_235->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_235, 5, 4, 1, 1);

        pushButton_250 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_250);
        pushButton_250->setObjectName(QStringLiteral("pushButton_250"));
        pushButton_250->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_250, 6, 7, 1, 1);

        pushButton_214 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_214);
        pushButton_214->setObjectName(QStringLiteral("pushButton_214"));
        pushButton_214->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_214, 3, 8, 1, 1);

        pushButton_211 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_211);
        pushButton_211->setObjectName(QStringLiteral("pushButton_211"));
        pushButton_211->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_211, 3, 5, 1, 1);

        pushButton_230 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_230);
        pushButton_230->setObjectName(QStringLiteral("pushButton_230"));
        pushButton_230->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_230, 4, 11, 1, 1);

        pushButton_209 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_209);
        pushButton_209->setObjectName(QStringLiteral("pushButton_209"));
        pushButton_209->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_209, 3, 3, 1, 1);

        pushButton_208 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_208);
        pushButton_208->setObjectName(QStringLiteral("pushButton_208"));
        pushButton_208->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_208, 3, 2, 1, 1);

        pushButton_240 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_240);
        pushButton_240->setObjectName(QStringLiteral("pushButton_240"));
        pushButton_240->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_240, 5, 9, 1, 1);

        pushButton_236 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_236);
        pushButton_236->setObjectName(QStringLiteral("pushButton_236"));
        pushButton_236->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_236, 5, 5, 1, 1);

        pushButton_241 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_241);
        pushButton_241->setObjectName(QStringLiteral("pushButton_241"));
        pushButton_241->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_241, 5, 10, 1, 1);

        pushButton_232 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_232);
        pushButton_232->setObjectName(QStringLiteral("pushButton_232"));
        pushButton_232->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_232, 5, 1, 1, 1);

        pushButton_221 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_221);
        pushButton_221->setObjectName(QStringLiteral("pushButton_221"));
        pushButton_221->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_221, 4, 2, 1, 1);

        pushButton_243 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_243);
        pushButton_243->setObjectName(QStringLiteral("pushButton_243"));
        pushButton_243->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_243, 6, 0, 1, 1);

        pushButton_238 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_238);
        pushButton_238->setObjectName(QStringLiteral("pushButton_238"));
        pushButton_238->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_238, 5, 7, 1, 1);

        pushButton_213 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_213);
        pushButton_213->setObjectName(QStringLiteral("pushButton_213"));
        pushButton_213->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_213, 3, 7, 1, 1);

        pushButton_256 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_256);
        pushButton_256->setObjectName(QStringLiteral("pushButton_256"));
        pushButton_256->setMaximumSize(QSize(87, 50));

        keyboard->addWidget(pushButton_256, 7, 0, 1, 2);

        pushButton_224 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_224);
        pushButton_224->setObjectName(QStringLiteral("pushButton_224"));
        pushButton_224->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_224, 4, 5, 1, 1);

        pushButton_229 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_229);
        pushButton_229->setObjectName(QStringLiteral("pushButton_229"));
        pushButton_229->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_229, 4, 10, 1, 1);

        pushButton_198 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_198);
        pushButton_198->setObjectName(QStringLiteral("pushButton_198"));
        pushButton_198->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_198, 2, 5, 1, 1);

        pushButton_200 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_200);
        pushButton_200->setObjectName(QStringLiteral("pushButton_200"));
        pushButton_200->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_200, 2, 7, 1, 1);

        pushButton_203 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_203);
        pushButton_203->setObjectName(QStringLiteral("pushButton_203"));
        pushButton_203->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_203, 2, 10, 1, 1);

        pushButton_225 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_225);
        pushButton_225->setObjectName(QStringLiteral("pushButton_225"));
        pushButton_225->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_225, 4, 6, 1, 1);

        pushButton_212 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_212);
        pushButton_212->setObjectName(QStringLiteral("pushButton_212"));
        pushButton_212->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_212, 3, 6, 1, 1);

        pushButton_193 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_193);
        pushButton_193->setObjectName(QStringLiteral("pushButton_193"));
        pushButton_193->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_193, 2, 0, 1, 1);

        pushButton_244 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_244);
        pushButton_244->setObjectName(QStringLiteral("pushButton_244"));
        pushButton_244->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_244, 6, 1, 1, 1);

        pushButton_218 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_218);
        pushButton_218->setObjectName(QStringLiteral("pushButton_218"));
        pushButton_218->setMaximumSize(QSize(87, 50));

        keyboard->addWidget(pushButton_218, 3, 12, 1, 1);

        pushButton_202 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_202);
        pushButton_202->setObjectName(QStringLiteral("pushButton_202"));
        pushButton_202->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_202, 2, 9, 1, 1);

        pushButton_210 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_210);
        pushButton_210->setObjectName(QStringLiteral("pushButton_210"));
        pushButton_210->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_210, 3, 4, 1, 1);

        pushButton_258 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_258);
        pushButton_258->setObjectName(QStringLiteral("pushButton_258"));
        pushButton_258->setMaximumSize(QSize(87, 50));

        keyboard->addWidget(pushButton_258, 7, 12, 1, 1);

        pushButton_194 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_194);
        pushButton_194->setObjectName(QStringLiteral("pushButton_194"));
        pushButton_194->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_194, 2, 1, 1, 1);

        pushButton_216 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_216);
        pushButton_216->setObjectName(QStringLiteral("pushButton_216"));
        pushButton_216->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_216, 3, 10, 1, 1);

        pushButton_215 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_215);
        pushButton_215->setObjectName(QStringLiteral("pushButton_215"));
        pushButton_215->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_215, 3, 9, 1, 1);

        pushButton_196 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_196);
        pushButton_196->setObjectName(QStringLiteral("pushButton_196"));
        pushButton_196->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_196, 2, 3, 1, 1);

        pushButton_257 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_257);
        pushButton_257->setObjectName(QStringLiteral("pushButton_257"));
        pushButton_257->setMaximumSize(QSize(10000, 50));

        keyboard->addWidget(pushButton_257, 7, 2, 1, 9);

        pushButton_249 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_249);
        pushButton_249->setObjectName(QStringLiteral("pushButton_249"));
        pushButton_249->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_249, 6, 6, 1, 1);

        pushButton_246 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_246);
        pushButton_246->setObjectName(QStringLiteral("pushButton_246"));
        pushButton_246->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_246, 6, 3, 1, 1);

        pushButton_247 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_247);
        pushButton_247->setObjectName(QStringLiteral("pushButton_247"));
        pushButton_247->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_247, 6, 4, 1, 1);

        pushButton_251 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_251);
        pushButton_251->setObjectName(QStringLiteral("pushButton_251"));
        pushButton_251->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_251, 6, 8, 1, 1);

        pushButton_248 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_248);
        pushButton_248->setObjectName(QStringLiteral("pushButton_248"));
        pushButton_248->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_248, 6, 5, 1, 1);

        pushButton_233 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_233);
        pushButton_233->setObjectName(QStringLiteral("pushButton_233"));
        pushButton_233->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_233, 5, 2, 1, 1);

        pushButton_228 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_228);
        pushButton_228->setObjectName(QStringLiteral("pushButton_228"));
        pushButton_228->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_228, 4, 9, 1, 1);

        pushButton_234 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_234);
        pushButton_234->setObjectName(QStringLiteral("pushButton_234"));
        pushButton_234->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_234, 5, 3, 1, 1);

        pushButton_227 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_227);
        pushButton_227->setObjectName(QStringLiteral("pushButton_227"));
        pushButton_227->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_227, 4, 8, 1, 1);

        pushButton_231 = new QPushButton(keyboard_2);
        buttonGroup->addButton(pushButton_231);
        pushButton_231->setObjectName(QStringLiteral("pushButton_231"));
        pushButton_231->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_231, 5, 0, 1, 1);

        wifiPassLabel = new QLabel(keyboard_2);
        wifiPassLabel->setObjectName(QStringLiteral("wifiPassLabel"));

        keyboard->addWidget(wifiPassLabel, 0, 0, 1, 12);

        label_settings = new QLabel(page_maintenance_general);
        label_settings->setObjectName(QStringLiteral("label_settings"));
        label_settings->setGeometry(QRect(60, 760, 111, 17));
        label_settings->setFont(font);
        label_connectivity = new QLabel(page_maintenance_general);
        label_connectivity->setObjectName(QStringLiteral("label_connectivity"));
        label_connectivity->setGeometry(QRect(60, 100, 111, 17));
        label_connectivity->setFont(font);
        label_4 = new QLabel(page_maintenance_general);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(60, 850, 111, 17));
        label_4->setFont(font);
        pushButton_printer_check_status = new QPushButton(page_maintenance_general);
        pushButton_printer_check_status->setObjectName(QStringLiteral("pushButton_printer_check_status"));
        pushButton_printer_check_status->setGeometry(QRect(60, 640, 221, 41));
        pushButton_printer_test_print = new QPushButton(page_maintenance_general);
        pushButton_printer_test_print->setObjectName(QStringLiteral("pushButton_printer_test_print"));
        pushButton_printer_test_print->setGeometry(QRect(60, 690, 221, 41));
        pushButton_rtunnel_restart = new QPushButton(page_maintenance_general);
        pushButton_rtunnel_restart->setObjectName(QStringLiteral("pushButton_rtunnel_restart"));
        pushButton_rtunnel_restart->setGeometry(QRect(60, 530, 221, 61));
        label_status_feedback = new QLabel(page_maintenance_general);
        label_status_feedback->setObjectName(QStringLiteral("label_status_feedback"));
        label_status_feedback->setGeometry(QRect(60, 1140, 661, 471));
        label_status_feedback->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_feedback = new QLabel(page_maintenance_general);
        label_feedback->setObjectName(QStringLiteral("label_feedback"));
        label_feedback->setGeometry(QRect(60, 1100, 111, 17));
        label_feedback->setFont(font);
        pushButton_network_status = new QPushButton(page_maintenance_general);
        pushButton_network_status->setObjectName(QStringLiteral("pushButton_network_status"));
        pushButton_network_status->setGeometry(QRect(60, 450, 221, 61));
        pushButton_restart_electronics = new QPushButton(page_maintenance_general);
        pushButton_restart_electronics->setObjectName(QStringLiteral("pushButton_restart_electronics"));
        pushButton_restart_electronics->setGeometry(QRect(60, 870, 221, 61));
        pushButton_restart_UI = new QPushButton(page_maintenance_general);
        pushButton_restart_UI->setObjectName(QStringLiteral("pushButton_restart_UI"));
        pushButton_restart_UI->setGeometry(QRect(290, 870, 221, 61));
        label_aws_port = new QLabel(page_maintenance_general);
        label_aws_port->setObjectName(QStringLiteral("label_aws_port"));
        label_aws_port->setGeometry(QRect(310, 510, 711, 17));
        label_machine_id = new QLabel(page_maintenance_general);
        label_machine_id->setObjectName(QStringLiteral("label_machine_id"));
        label_machine_id->setGeometry(QRect(310, 540, 711, 17));
        label_wifi_name = new QLabel(page_maintenance_general);
        label_wifi_name->setObjectName(QStringLiteral("label_wifi_name"));
        label_wifi_name->setGeometry(QRect(310, 376, 711, 41));
        pushButton_back->raise();
        pushButton_minimize->raise();
        pushButton_reboot->raise();
        pushButton_shutdown->raise();
        printer_hasPaper_label->raise();
        printer_isOnline_label->raise();
        label_receipt_printer->raise();
        checkBox_enable_empty_container->raise();
        label_wifi_internet->raise();
        pushButton_wifi_networks->raise();
        table_wifi_networks->raise();
        label_wifi_status->raise();
        label_wifi_ip_address->raise();
        keyboard_2->raise();
        label_settings->raise();
        label_connectivity->raise();
        label_4->raise();
        pushButton_printer_check_status->raise();
        pushButton_printer_test_print->raise();
        pushButton_rtunnel_restart->raise();
        label_status_feedback->raise();
        label_feedback->raise();
        pushButton_network_status->raise();
        pushButton_restart_electronics->raise();
        pushButton_restart_UI->raise();
        checkBox_enable_pump_ramping->raise();
        label_aws_port->raise();
        label_machine_id->raise();
        label_wifi_name->raise();

        retranslateUi(page_maintenance_general);

        pushButton_shutdown->setDefault(false);


        QMetaObject::connectSlotsByName(page_maintenance_general);
    } // setupUi

    void retranslateUi(QWidget *page_maintenance_general)
    {
        page_maintenance_general->setWindowTitle(QApplication::translate("page_maintenance_general", "Form", Q_NULLPTR));
        pushButton_back->setText(QApplication::translate("page_maintenance_general", "back", Q_NULLPTR));
        pushButton_minimize->setText(QApplication::translate("page_maintenance_general", "Minimize Soapstand App", Q_NULLPTR));
        pushButton_reboot->setText(QApplication::translate("page_maintenance_general", "Restart Computer", Q_NULLPTR));
        pushButton_shutdown->setText(QApplication::translate("page_maintenance_general", "Shut down (unplug AC is ok too)", Q_NULLPTR));
        printer_hasPaper_label->setText(QApplication::translate("page_maintenance_general", "paper status", Q_NULLPTR));
        printer_isOnline_label->setText(QApplication::translate("page_maintenance_general", "printer connection status", Q_NULLPTR));
        label_receipt_printer->setText(QApplication::translate("page_maintenance_general", "Receipt Printer", Q_NULLPTR));
        checkBox_enable_pump_ramping->setText(QApplication::translate("page_maintenance_general", "Enable pump ramp up / ramp down", Q_NULLPTR));
        checkBox_enable_empty_container->setText(QApplication::translate("page_maintenance_general", "Enable empty stock detection", Q_NULLPTR));
        label_wifi_internet->setText(QApplication::translate("page_maintenance_general", "wifi connectivity", Q_NULLPTR));
        pushButton_wifi_networks->setText(QString());
        QTableWidgetItem *___qtablewidgetitem = table_wifi_networks->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("page_maintenance_general", "SSID", Q_NULLPTR));
        label_wifi_status->setText(QApplication::translate("page_maintenance_general", "wifi status", Q_NULLPTR));
        label_wifi_ip_address->setText(QApplication::translate("page_maintenance_general", "wifi IP ", Q_NULLPTR));
        pushButton_206->setText(QApplication::translate("page_maintenance_general", "1", Q_NULLPTR));
        pushButton_205->setText(QApplication::translate("page_maintenance_general", "Backspace", Q_NULLPTR));
        pushButton_255->setText(QApplication::translate("page_maintenance_general", "CAPS", Q_NULLPTR));
        pushButton_223->setText(QApplication::translate("page_maintenance_general", "T", Q_NULLPTR));
        pushButton_242->setText(QApplication::translate("page_maintenance_general", "|", Q_NULLPTR));
        pushButton_252->setText(QApplication::translate("page_maintenance_general", "?", Q_NULLPTR));
        pushButton_199->setText(QApplication::translate("page_maintenance_general", "&&", Q_NULLPTR));
        pushButton_195->setText(QApplication::translate("page_maintenance_general", "#", Q_NULLPTR));
        pushButton_204->setText(QApplication::translate("page_maintenance_general", "=", Q_NULLPTR));
        pushButton_219->setText(QApplication::translate("page_maintenance_general", "Q", Q_NULLPTR));
        pushButton_245->setText(QApplication::translate("page_maintenance_general", "C", Q_NULLPTR));
        pushButton_254->setText(QApplication::translate("page_maintenance_general", "~", Q_NULLPTR));
        pushButton_207->setText(QApplication::translate("page_maintenance_general", "2", Q_NULLPTR));
        pushButton_217->setText(QApplication::translate("page_maintenance_general", "+", Q_NULLPTR));
        pushButton_237->setText(QApplication::translate("page_maintenance_general", "J", Q_NULLPTR));
        pushButton_220->setText(QApplication::translate("page_maintenance_general", "W", Q_NULLPTR));
        pushButton_201->setText(QApplication::translate("page_maintenance_general", "(", Q_NULLPTR));
        pushButton_253->setText(QApplication::translate("page_maintenance_general", "`", Q_NULLPTR));
        pushButton_226->setText(QApplication::translate("page_maintenance_general", "I", Q_NULLPTR));
        pushButton_222->setText(QApplication::translate("page_maintenance_general", "R", Q_NULLPTR));
        pushButton_197->setText(QApplication::translate("page_maintenance_general", "%", Q_NULLPTR));
        pushButton_239->setText(QApplication::translate("page_maintenance_general", "L", Q_NULLPTR));
        pushButton_235->setText(QApplication::translate("page_maintenance_general", "G", Q_NULLPTR));
        pushButton_250->setText(QApplication::translate("page_maintenance_general", ",", Q_NULLPTR));
        pushButton_214->setText(QApplication::translate("page_maintenance_general", "9", Q_NULLPTR));
        pushButton_211->setText(QApplication::translate("page_maintenance_general", "6", Q_NULLPTR));
        pushButton_230->setText(QApplication::translate("page_maintenance_general", "}", Q_NULLPTR));
        pushButton_209->setText(QApplication::translate("page_maintenance_general", "4", Q_NULLPTR));
        pushButton_208->setText(QApplication::translate("page_maintenance_general", "3", Q_NULLPTR));
        pushButton_240->setText(QApplication::translate("page_maintenance_general", ":", Q_NULLPTR));
        pushButton_236->setText(QApplication::translate("page_maintenance_general", "H", Q_NULLPTR));
        pushButton_241->setText(QApplication::translate("page_maintenance_general", "\"", Q_NULLPTR));
        pushButton_232->setText(QApplication::translate("page_maintenance_general", "S", Q_NULLPTR));
        pushButton_221->setText(QApplication::translate("page_maintenance_general", "E", Q_NULLPTR));
        pushButton_243->setText(QApplication::translate("page_maintenance_general", "Z", Q_NULLPTR));
        pushButton_238->setText(QApplication::translate("page_maintenance_general", "K", Q_NULLPTR));
        pushButton_213->setText(QApplication::translate("page_maintenance_general", "8", Q_NULLPTR));
        pushButton_256->setText(QApplication::translate("page_maintenance_general", "Cancel", Q_NULLPTR));
        pushButton_224->setText(QApplication::translate("page_maintenance_general", "Y", Q_NULLPTR));
        pushButton_229->setText(QApplication::translate("page_maintenance_general", "{", Q_NULLPTR));
        pushButton_198->setText(QApplication::translate("page_maintenance_general", "^", Q_NULLPTR));
        pushButton_200->setText(QApplication::translate("page_maintenance_general", "*", Q_NULLPTR));
        pushButton_203->setText(QApplication::translate("page_maintenance_general", "_", Q_NULLPTR));
        pushButton_225->setText(QApplication::translate("page_maintenance_general", "U", Q_NULLPTR));
        pushButton_212->setText(QApplication::translate("page_maintenance_general", "7", Q_NULLPTR));
        pushButton_193->setText(QApplication::translate("page_maintenance_general", "!", Q_NULLPTR));
        pushButton_244->setText(QApplication::translate("page_maintenance_general", "X", Q_NULLPTR));
        pushButton_218->setText(QApplication::translate("page_maintenance_general", "Clear", Q_NULLPTR));
        pushButton_202->setText(QApplication::translate("page_maintenance_general", ")", Q_NULLPTR));
        pushButton_210->setText(QApplication::translate("page_maintenance_general", "5", Q_NULLPTR));
        pushButton_258->setText(QApplication::translate("page_maintenance_general", "Done", Q_NULLPTR));
        pushButton_194->setText(QApplication::translate("page_maintenance_general", "@", Q_NULLPTR));
        pushButton_216->setText(QApplication::translate("page_maintenance_general", "-", Q_NULLPTR));
        pushButton_215->setText(QApplication::translate("page_maintenance_general", "0", Q_NULLPTR));
        pushButton_196->setText(QApplication::translate("page_maintenance_general", "$", Q_NULLPTR));
        pushButton_257->setText(QApplication::translate("page_maintenance_general", "Space", Q_NULLPTR));
        pushButton_249->setText(QApplication::translate("page_maintenance_general", "M", Q_NULLPTR));
        pushButton_246->setText(QApplication::translate("page_maintenance_general", "V", Q_NULLPTR));
        pushButton_247->setText(QApplication::translate("page_maintenance_general", "B", Q_NULLPTR));
        pushButton_251->setText(QApplication::translate("page_maintenance_general", ".", Q_NULLPTR));
        pushButton_248->setText(QApplication::translate("page_maintenance_general", "N", Q_NULLPTR));
        pushButton_233->setText(QApplication::translate("page_maintenance_general", "D", Q_NULLPTR));
        pushButton_228->setText(QApplication::translate("page_maintenance_general", "P", Q_NULLPTR));
        pushButton_234->setText(QApplication::translate("page_maintenance_general", "F", Q_NULLPTR));
        pushButton_227->setText(QApplication::translate("page_maintenance_general", "O", Q_NULLPTR));
        pushButton_231->setText(QApplication::translate("page_maintenance_general", "A", Q_NULLPTR));
        wifiPassLabel->setText(QString());
        label_settings->setText(QApplication::translate("page_maintenance_general", "Settings", Q_NULLPTR));
        label_connectivity->setText(QString());
        label_4->setText(QApplication::translate("page_maintenance_general", "Control", Q_NULLPTR));
        pushButton_printer_check_status->setText(QApplication::translate("page_maintenance_general", "Check Status", Q_NULLPTR));
        pushButton_printer_test_print->setText(QApplication::translate("page_maintenance_general", "Test Print", Q_NULLPTR));
        pushButton_rtunnel_restart->setText(QApplication::translate("page_maintenance_general", "Reset Backend Connection", Q_NULLPTR));
        label_status_feedback->setText(QApplication::translate("page_maintenance_general", "Command feedback", Q_NULLPTR));
        label_feedback->setText(QApplication::translate("page_maintenance_general", "Feedback", Q_NULLPTR));
        pushButton_network_status->setText(QApplication::translate("page_maintenance_general", "Check network status", Q_NULLPTR));
        pushButton_restart_electronics->setText(QApplication::translate("page_maintenance_general", "Restart Soapstand App (ui+fsm)", Q_NULLPTR));
        pushButton_restart_UI->setText(QApplication::translate("page_maintenance_general", "Restart UI only", Q_NULLPTR));
        label_aws_port->setText(QApplication::translate("page_maintenance_general", "aws port", Q_NULLPTR));
        label_machine_id->setText(QApplication::translate("page_maintenance_general", "machine id", Q_NULLPTR));
        label_wifi_name->setText(QApplication::translate("page_maintenance_general", "wifi name", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class page_maintenance_general: public Ui_page_maintenance_general {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_MAINTENANCE_GENERAL_H
