/********************************************************************************
** Form generated from reading UI file 'page_select_product.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_SELECT_PRODUCT_H
#define UI_PAGE_SELECT_PRODUCT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_page_select_product
{
public:
    QPushButton *pushButton_selection1;
    QPushButton *pushButton_selection2;
    QPushButton *pushButton_to_maintenance;
    QPushButton *pushButton_selection3;
    QPushButton *pushButton_selection4;
    QPushButton *pushButton_to_idle;
    QPushButton *pushButton_help_page;
    QLabel *label_product_1_photo;
    QLabel *label_product_3_photo;
    QLabel *label_product_4_photo;
    QLabel *label_product_1_name;
    QLabel *label_product_2_name;
    QLabel *label_product_3_name;
    QLabel *label_product_4_name;
    QLabel *label_product_1_icon;
    QLabel *label_product_2_icon;
    QLabel *label_product_3_icon;
    QLabel *label_product_4_icon;
    QLabel *label_product_3_type;
    QLabel *label_product_2_type;
    QLabel *label_product_4_type;
    QLabel *label_product_1_type;
    QLabel *label_product_1_overlay_text;
    QLabel *label_product_2_overlay_text;
    QLabel *label_product_4_overlay_text;
    QLabel *label_product_3_overlay_text;
    QLabel *label_pick_soap;
    QLabel *label_help;
    QLabel *label_product_2_photo;

    void setupUi(QWidget *page_select_product)
    {
        if (page_select_product->objectName().isEmpty())
            page_select_product->setObjectName(QStringLiteral("page_select_product"));
        page_select_product->resize(1080, 1920);
        page_select_product->setStyleSheet(QStringLiteral(""));
        pushButton_selection1 = new QPushButton(page_select_product);
        pushButton_selection1->setObjectName(QStringLiteral("pushButton_selection1"));
        pushButton_selection1->setEnabled(true);
        pushButton_selection1->setGeometry(QRect(30, 290, 491, 651));
        pushButton_selection1->setMinimumSize(QSize(150, 300));
        pushButton_selection1->setMaximumSize(QSize(500, 700));
        pushButton_selection1->setBaseSize(QSize(150, 300));
        pushButton_selection1->setStyleSheet(QStringLiteral(""));
        pushButton_selection2 = new QPushButton(page_select_product);
        pushButton_selection2->setObjectName(QStringLiteral("pushButton_selection2"));
        pushButton_selection2->setEnabled(true);
        pushButton_selection2->setGeometry(QRect(550, 290, 481, 651));
        QFont font;
        font.setFamily(QStringLiteral("Montserrat"));
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(50);
        pushButton_selection2->setFont(font);
        pushButton_selection2->setStyleSheet(QStringLiteral(""));
        pushButton_selection2->setIconSize(QSize(20, 52));
        pushButton_to_maintenance = new QPushButton(page_select_product);
        pushButton_to_maintenance->setObjectName(QStringLiteral("pushButton_to_maintenance"));
        pushButton_to_maintenance->setGeometry(QRect(230, 10, 281, 241));
        pushButton_selection3 = new QPushButton(page_select_product);
        pushButton_selection3->setObjectName(QStringLiteral("pushButton_selection3"));
        pushButton_selection3->setEnabled(true);
        pushButton_selection3->setGeometry(QRect(30, 990, 491, 651));
        pushButton_selection3->setFont(font);
        pushButton_selection3->setStyleSheet(QStringLiteral(""));
        pushButton_selection4 = new QPushButton(page_select_product);
        pushButton_selection4->setObjectName(QStringLiteral("pushButton_selection4"));
        pushButton_selection4->setEnabled(true);
        pushButton_selection4->setGeometry(QRect(550, 990, 481, 651));
        pushButton_selection4->setFont(font);
        pushButton_selection4->setStyleSheet(QStringLiteral(""));
        pushButton_to_idle = new QPushButton(page_select_product);
        pushButton_to_idle->setObjectName(QStringLiteral("pushButton_to_idle"));
        pushButton_to_idle->setGeometry(QRect(70, 40, 401, 41));
        pushButton_help_page = new QPushButton(page_select_product);
        pushButton_help_page->setObjectName(QStringLiteral("pushButton_help_page"));
        pushButton_help_page->setGeometry(QRect(446, 1732, 201, 181));
        label_product_1_photo = new QLabel(page_select_product);
        label_product_1_photo->setObjectName(QStringLiteral("label_product_1_photo"));
        label_product_1_photo->setGeometry(QRect(50, 334, 450, 450));
        QFont font1;
        font1.setPointSize(48);
        font1.setBold(true);
        font1.setWeight(75);
        label_product_1_photo->setFont(font1);
        label_product_1_photo->setStyleSheet(QStringLiteral(""));
        label_product_1_photo->setAlignment(Qt::AlignCenter);
        label_product_1_photo->setWordWrap(true);
        label_product_3_photo = new QLabel(page_select_product);
        label_product_3_photo->setObjectName(QStringLiteral("label_product_3_photo"));
        label_product_3_photo->setGeometry(QRect(50, 1030, 450, 450));
        label_product_3_photo->setFont(font1);
        label_product_3_photo->setLayoutDirection(Qt::LeftToRight);
        label_product_3_photo->setStyleSheet(QStringLiteral(""));
        label_product_3_photo->setAlignment(Qt::AlignCenter);
        label_product_3_photo->setWordWrap(true);
        label_product_4_photo = new QLabel(page_select_product);
        label_product_4_photo->setObjectName(QStringLiteral("label_product_4_photo"));
        label_product_4_photo->setGeometry(QRect(562, 1030, 450, 450));
        label_product_4_photo->setFont(font1);
        label_product_4_photo->setLayoutDirection(Qt::LeftToRight);
        label_product_4_photo->setStyleSheet(QStringLiteral(""));
        label_product_4_photo->setAlignment(Qt::AlignCenter);
        label_product_4_photo->setWordWrap(true);
        label_product_1_name = new QLabel(page_select_product);
        label_product_1_name->setObjectName(QStringLiteral("label_product_1_name"));
        label_product_1_name->setGeometry(QRect(58, 875, 450, 81));
        label_product_2_name = new QLabel(page_select_product);
        label_product_2_name->setObjectName(QStringLiteral("label_product_2_name"));
        label_product_2_name->setGeometry(QRect(562, 875, 450, 81));
        label_product_3_name = new QLabel(page_select_product);
        label_product_3_name->setObjectName(QStringLiteral("label_product_3_name"));
        label_product_3_name->setGeometry(QRect(53, 1568, 450, 71));
        label_product_4_name = new QLabel(page_select_product);
        label_product_4_name->setObjectName(QStringLiteral("label_product_4_name"));
        label_product_4_name->setGeometry(QRect(562, 1568, 450, 71));
        label_product_1_icon = new QLabel(page_select_product);
        label_product_1_icon->setObjectName(QStringLiteral("label_product_1_icon"));
        label_product_1_icon->setGeometry(QRect(241, 294, 81, 81));
        label_product_2_icon = new QLabel(page_select_product);
        label_product_2_icon->setObjectName(QStringLiteral("label_product_2_icon"));
        label_product_2_icon->setGeometry(QRect(750, 294, 81, 81));
        label_product_3_icon = new QLabel(page_select_product);
        label_product_3_icon->setObjectName(QStringLiteral("label_product_3_icon"));
        label_product_3_icon->setGeometry(QRect(241, 994, 81, 81));
        label_product_4_icon = new QLabel(page_select_product);
        label_product_4_icon->setObjectName(QStringLiteral("label_product_4_icon"));
        label_product_4_icon->setGeometry(QRect(750, 991, 81, 81));
        label_product_3_type = new QLabel(page_select_product);
        label_product_3_type->setObjectName(QStringLiteral("label_product_3_type"));
        label_product_3_type->setGeometry(QRect(53, 1514, 450, 48));
        label_product_2_type = new QLabel(page_select_product);
        label_product_2_type->setObjectName(QStringLiteral("label_product_2_type"));
        label_product_2_type->setGeometry(QRect(562, 814, 450, 46));
        label_product_4_type = new QLabel(page_select_product);
        label_product_4_type->setObjectName(QStringLiteral("label_product_4_type"));
        label_product_4_type->setGeometry(QRect(562, 1514, 450, 46));
        label_product_1_type = new QLabel(page_select_product);
        label_product_1_type->setObjectName(QStringLiteral("label_product_1_type"));
        label_product_1_type->setGeometry(QRect(53, 814, 450, 46));
        label_product_1_overlay_text = new QLabel(page_select_product);
        label_product_1_overlay_text->setObjectName(QStringLiteral("label_product_1_overlay_text"));
        label_product_1_overlay_text->setGeometry(QRect(50, 334, 450, 450));
        label_product_1_overlay_text->setFont(font1);
        label_product_1_overlay_text->setStyleSheet(QStringLiteral(""));
        label_product_1_overlay_text->setAlignment(Qt::AlignCenter);
        label_product_1_overlay_text->setWordWrap(true);
        label_product_2_overlay_text = new QLabel(page_select_product);
        label_product_2_overlay_text->setObjectName(QStringLiteral("label_product_2_overlay_text"));
        label_product_2_overlay_text->setGeometry(QRect(562, 334, 450, 450));
        label_product_2_overlay_text->setFont(font1);
        label_product_2_overlay_text->setStyleSheet(QStringLiteral(""));
        label_product_2_overlay_text->setAlignment(Qt::AlignCenter);
        label_product_2_overlay_text->setWordWrap(true);
        label_product_4_overlay_text = new QLabel(page_select_product);
        label_product_4_overlay_text->setObjectName(QStringLiteral("label_product_4_overlay_text"));
        label_product_4_overlay_text->setGeometry(QRect(562, 1030, 450, 450));
        label_product_4_overlay_text->setFont(font1);
        label_product_4_overlay_text->setStyleSheet(QStringLiteral(""));
        label_product_4_overlay_text->setAlignment(Qt::AlignCenter);
        label_product_4_overlay_text->setWordWrap(true);
        label_product_3_overlay_text = new QLabel(page_select_product);
        label_product_3_overlay_text->setObjectName(QStringLiteral("label_product_3_overlay_text"));
        label_product_3_overlay_text->setGeometry(QRect(50, 1030, 450, 450));
        label_product_3_overlay_text->setFont(font1);
        label_product_3_overlay_text->setStyleSheet(QStringLiteral(""));
        label_product_3_overlay_text->setAlignment(Qt::AlignCenter);
        label_product_3_overlay_text->setWordWrap(true);
        label_pick_soap = new QLabel(page_select_product);
        label_pick_soap->setObjectName(QStringLiteral("label_pick_soap"));
        label_pick_soap->setGeometry(QRect(78, 178, 881, 86));
        label_help = new QLabel(page_select_product);
        label_help->setObjectName(QStringLiteral("label_help"));
        label_help->setGeometry(QRect(478, 1773, 125, 125));
        label_product_2_photo = new QLabel(page_select_product);
        label_product_2_photo->setObjectName(QStringLiteral("label_product_2_photo"));
        label_product_2_photo->setGeometry(QRect(562, 334, 450, 450));
        label_product_2_photo->setFont(font1);
        label_product_2_photo->setStyleSheet(QStringLiteral(""));
        label_product_2_photo->setAlignment(Qt::AlignCenter);
        label_product_2_photo->setWordWrap(true);
        pushButton_selection2->raise();
        label_product_2_overlay_text->raise();
        pushButton_selection1->raise();
        label_product_1_photo->raise();
        label_product_1_overlay_text->raise();
        pushButton_to_maintenance->raise();
        pushButton_selection3->raise();
        pushButton_selection4->raise();
        label_product_3_photo->raise();
        label_product_4_photo->raise();
        label_product_1_name->raise();
        label_product_2_name->raise();
        label_product_3_name->raise();
        label_product_4_name->raise();
        label_product_1_icon->raise();
        label_product_2_icon->raise();
        label_product_3_icon->raise();
        label_product_4_icon->raise();
        label_product_3_type->raise();
        label_product_2_type->raise();
        label_product_4_type->raise();
        label_product_1_type->raise();
        label_product_4_overlay_text->raise();
        label_product_3_overlay_text->raise();
        label_pick_soap->raise();
        label_help->raise();
        pushButton_help_page->raise();
        pushButton_to_idle->raise();
        label_product_2_photo->raise();

        retranslateUi(page_select_product);

        QMetaObject::connectSlotsByName(page_select_product);
    } // setupUi

    void retranslateUi(QWidget *page_select_product)
    {
        page_select_product->setWindowTitle(QApplication::translate("page_select_product", "Form", Q_NULLPTR));
        pushButton_selection1->setText(QString());
        pushButton_selection2->setText(QString());
        pushButton_to_maintenance->setText(QString());
        pushButton_selection3->setText(QString());
        pushButton_selection4->setText(QString());
        pushButton_to_idle->setText(QApplication::translate("page_select_product", "back", Q_NULLPTR));
        pushButton_help_page->setText(QString());
        label_product_1_photo->setText(QString());
        label_product_3_photo->setText(QString());
        label_product_4_photo->setText(QString());
        label_product_1_name->setText(QApplication::translate("page_select_product", "TextLabel", Q_NULLPTR));
        label_product_2_name->setText(QApplication::translate("page_select_product", "TextLabel", Q_NULLPTR));
        label_product_3_name->setText(QApplication::translate("page_select_product", "TextLabel", Q_NULLPTR));
        label_product_4_name->setText(QApplication::translate("page_select_product", "TextLabel", Q_NULLPTR));
        label_product_1_icon->setText(QApplication::translate("page_select_product", "TextLabel", Q_NULLPTR));
        label_product_2_icon->setText(QApplication::translate("page_select_product", "TextLabel", Q_NULLPTR));
        label_product_3_icon->setText(QApplication::translate("page_select_product", "TextLabel", Q_NULLPTR));
        label_product_4_icon->setText(QApplication::translate("page_select_product", "TextLabel", Q_NULLPTR));
        label_product_3_type->setText(QApplication::translate("page_select_product", "TextLabel", Q_NULLPTR));
        label_product_2_type->setText(QApplication::translate("page_select_product", "TextLabel", Q_NULLPTR));
        label_product_4_type->setText(QApplication::translate("page_select_product", "TextLabel", Q_NULLPTR));
        label_product_1_type->setText(QApplication::translate("page_select_product", "TextLabel", Q_NULLPTR));
        label_product_1_overlay_text->setText(QString());
        label_product_2_overlay_text->setText(QString());
        label_product_4_overlay_text->setText(QString());
        label_product_3_overlay_text->setText(QString());
        label_pick_soap->setText(QApplication::translate("page_select_product", "TextLabel", Q_NULLPTR));
        label_help->setText(QString());
        label_product_2_photo->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class page_select_product: public Ui_page_select_product {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_SELECT_PRODUCT_H
