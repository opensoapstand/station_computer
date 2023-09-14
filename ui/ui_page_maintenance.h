/********************************************************************************
** Form generated from reading UI file 'page_maintenance.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_MAINTENANCE_H
#define UI_PAGE_MAINTENANCE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_page_maintenance
{
public:
    QPushButton *pushButton_product_4;
    QPushButton *pushButton_product_1;
    QLabel *label_product_3_name;
    QLabel *label_product_2_name;
    QPushButton *pushButton_product_2;
    QPushButton *pushButton_product_3;
    QLabel *label_product_4_name;
    QLabel *label_product_1_name;
    QPushButton *pushButton_to_previous_page;
    QLabel *label_title_maintenance_mode;
    QLabel *label_machine_id;
    QLabel *label_product_1_status;
    QLabel *label_product_2_status;
    QLabel *label_product_3_status;
    QLabel *label_product_4_status;
    QLabel *label_ui_version;
    QPushButton *pushButton_general_settings;
    QLabel *label_product_1_position;
    QLabel *label_product_2_position;
    QLabel *label_product_3_position;
    QLabel *label_product_4_position;
    QFrame *line;
    QLabel *label_role;

    void setupUi(QWidget *page_maintenance)
    {
        if (page_maintenance->objectName().isEmpty())
            page_maintenance->setObjectName(QStringLiteral("page_maintenance"));
        page_maintenance->resize(1080, 1920);
        page_maintenance->setAutoFillBackground(true);
        pushButton_product_4 = new QPushButton(page_maintenance);
        pushButton_product_4->setObjectName(QStringLiteral("pushButton_product_4"));
        pushButton_product_4->setGeometry(QRect(580, 780, 281, 341));
        pushButton_product_1 = new QPushButton(page_maintenance);
        pushButton_product_1->setObjectName(QStringLiteral("pushButton_product_1"));
        pushButton_product_1->setGeometry(QRect(220, 320, 281, 341));
        label_product_3_name = new QLabel(page_maintenance);
        label_product_3_name->setObjectName(QStringLiteral("label_product_3_name"));
        label_product_3_name->setGeometry(QRect(230, 1140, 261, 81));
        QFont font;
        font.setPointSize(13);
        label_product_3_name->setFont(font);
        label_product_3_name->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        label_product_3_name->setWordWrap(true);
        label_product_2_name = new QLabel(page_maintenance);
        label_product_2_name->setObjectName(QStringLiteral("label_product_2_name"));
        label_product_2_name->setGeometry(QRect(590, 670, 261, 81));
        label_product_2_name->setFont(font);
        label_product_2_name->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        label_product_2_name->setWordWrap(true);
        pushButton_product_2 = new QPushButton(page_maintenance);
        pushButton_product_2->setObjectName(QStringLiteral("pushButton_product_2"));
        pushButton_product_2->setGeometry(QRect(580, 320, 281, 341));
        pushButton_product_3 = new QPushButton(page_maintenance);
        pushButton_product_3->setObjectName(QStringLiteral("pushButton_product_3"));
        pushButton_product_3->setGeometry(QRect(220, 780, 281, 341));
        label_product_4_name = new QLabel(page_maintenance);
        label_product_4_name->setObjectName(QStringLiteral("label_product_4_name"));
        label_product_4_name->setGeometry(QRect(580, 1140, 261, 81));
        label_product_4_name->setFont(font);
        label_product_4_name->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        label_product_4_name->setWordWrap(true);
        label_product_1_name = new QLabel(page_maintenance);
        label_product_1_name->setObjectName(QStringLiteral("label_product_1_name"));
        label_product_1_name->setGeometry(QRect(230, 670, 271, 81));
        label_product_1_name->setFont(font);
        label_product_1_name->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        label_product_1_name->setWordWrap(true);
        pushButton_to_previous_page = new QPushButton(page_maintenance);
        pushButton_to_previous_page->setObjectName(QStringLiteral("pushButton_to_previous_page"));
        pushButton_to_previous_page->setGeometry(QRect(70, 40, 401, 41));
        label_title_maintenance_mode = new QLabel(page_maintenance);
        label_title_maintenance_mode->setObjectName(QStringLiteral("label_title_maintenance_mode"));
        label_title_maintenance_mode->setGeometry(QRect(0, 178, 1080, 86));
        QFont font1;
        font1.setPointSize(40);
        label_title_maintenance_mode->setFont(font1);
        label_title_maintenance_mode->setAlignment(Qt::AlignCenter);
        label_machine_id = new QLabel(page_maintenance);
        label_machine_id->setObjectName(QStringLiteral("label_machine_id"));
        label_machine_id->setGeometry(QRect(220, 1320, 391, 50));
        QFont font2;
        font2.setPointSize(15);
        label_machine_id->setFont(font2);
        label_product_1_status = new QLabel(page_maintenance);
        label_product_1_status->setObjectName(QStringLiteral("label_product_1_status"));
        label_product_1_status->setGeometry(QRect(220, 619, 281, 31));
        label_product_2_status = new QLabel(page_maintenance);
        label_product_2_status->setObjectName(QStringLiteral("label_product_2_status"));
        label_product_2_status->setGeometry(QRect(580, 619, 281, 31));
        label_product_3_status = new QLabel(page_maintenance);
        label_product_3_status->setObjectName(QStringLiteral("label_product_3_status"));
        label_product_3_status->setEnabled(true);
        label_product_3_status->setGeometry(QRect(220, 1079, 281, 31));
        label_product_4_status = new QLabel(page_maintenance);
        label_product_4_status->setObjectName(QStringLiteral("label_product_4_status"));
        label_product_4_status->setGeometry(QRect(580, 1079, 281, 31));
        label_ui_version = new QLabel(page_maintenance);
        label_ui_version->setObjectName(QStringLiteral("label_ui_version"));
        label_ui_version->setGeometry(QRect(220, 1370, 351, 30));
        pushButton_general_settings = new QPushButton(page_maintenance);
        pushButton_general_settings->setObjectName(QStringLiteral("pushButton_general_settings"));
        pushButton_general_settings->setGeometry(QRect(610, 40, 401, 41));
        label_product_1_position = new QLabel(page_maintenance);
        label_product_1_position->setObjectName(QStringLiteral("label_product_1_position"));
        label_product_1_position->setGeometry(QRect(340, 290, 51, 31));
        label_product_1_position->setAlignment(Qt::AlignCenter);
        label_product_2_position = new QLabel(page_maintenance);
        label_product_2_position->setObjectName(QStringLiteral("label_product_2_position"));
        label_product_2_position->setGeometry(QRect(685, 290, 71, 31));
        label_product_2_position->setAlignment(Qt::AlignCenter);
        label_product_3_position = new QLabel(page_maintenance);
        label_product_3_position->setObjectName(QStringLiteral("label_product_3_position"));
        label_product_3_position->setGeometry(QRect(340, 750, 51, 31));
        label_product_3_position->setAlignment(Qt::AlignCenter);
        label_product_4_position = new QLabel(page_maintenance);
        label_product_4_position->setObjectName(QStringLiteral("label_product_4_position"));
        label_product_4_position->setGeometry(QRect(680, 750, 71, 31));
        label_product_4_position->setAlignment(Qt::AlignCenter);
        line = new QFrame(page_maintenance);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(220, 1300, 641, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        label_role = new QLabel(page_maintenance);
        label_role->setObjectName(QStringLiteral("label_role"));
        label_role->setGeometry(QRect(220, 1410, 351, 30));
        label_product_3_name->raise();
        label_product_2_name->raise();
        label_product_4_name->raise();
        label_product_1_name->raise();
        pushButton_to_previous_page->raise();
        label_title_maintenance_mode->raise();
        label_machine_id->raise();
        label_ui_version->raise();
        pushButton_general_settings->raise();
        label_product_1_position->raise();
        label_product_2_position->raise();
        label_product_3_position->raise();
        label_product_4_position->raise();
        pushButton_product_1->raise();
        pushButton_product_2->raise();
        pushButton_product_4->raise();
        pushButton_product_3->raise();
        label_product_3_status->raise();
        label_product_1_status->raise();
        label_product_4_status->raise();
        label_product_2_status->raise();
        line->raise();
        label_role->raise();

        retranslateUi(page_maintenance);

        QMetaObject::connectSlotsByName(page_maintenance);
    } // setupUi

    void retranslateUi(QWidget *page_maintenance)
    {
        page_maintenance->setWindowTitle(QApplication::translate("page_maintenance", "Form", Q_NULLPTR));
        pushButton_product_4->setText(QString());
        pushButton_product_1->setText(QString());
        label_product_3_name->setText(QApplication::translate("page_maintenance", "Product 3", Q_NULLPTR));
        label_product_2_name->setText(QApplication::translate("page_maintenance", "Product 2", Q_NULLPTR));
        pushButton_product_2->setText(QString());
        pushButton_product_3->setText(QString());
        label_product_4_name->setText(QApplication::translate("page_maintenance", "Product 4", Q_NULLPTR));
        label_product_1_name->setText(QApplication::translate("page_maintenance", "Product 1", Q_NULLPTR));
        pushButton_to_previous_page->setText(QApplication::translate("page_maintenance", "back", Q_NULLPTR));
        label_title_maintenance_mode->setText(QApplication::translate("page_maintenance", "MAINTENANCE MODE", Q_NULLPTR));
        label_machine_id->setText(QApplication::translate("page_maintenance", "machine serial", Q_NULLPTR));
        label_product_1_status->setText(QApplication::translate("page_maintenance", "TextLabel", Q_NULLPTR));
        label_product_2_status->setText(QApplication::translate("page_maintenance", "TextLabel", Q_NULLPTR));
        label_product_3_status->setText(QApplication::translate("page_maintenance", "TextLabel", Q_NULLPTR));
        label_product_4_status->setText(QApplication::translate("page_maintenance", "TextLabel", Q_NULLPTR));
        label_ui_version->setText(QApplication::translate("page_maintenance", "UI version", Q_NULLPTR));
        pushButton_general_settings->setText(QApplication::translate("page_maintenance", "General Settings", Q_NULLPTR));
        label_product_1_position->setText(QApplication::translate("page_maintenance", "P1", Q_NULLPTR));
        label_product_2_position->setText(QApplication::translate("page_maintenance", "P2", Q_NULLPTR));
        label_product_3_position->setText(QApplication::translate("page_maintenance", "P3", Q_NULLPTR));
        label_product_4_position->setText(QApplication::translate("page_maintenance", "P4", Q_NULLPTR));
        label_role->setText(QApplication::translate("page_maintenance", "Logged in as", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class page_maintenance: public Ui_page_maintenance {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_MAINTENANCE_H
