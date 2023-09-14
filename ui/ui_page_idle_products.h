/********************************************************************************
** Form generated from reading UI file 'page_idle_products.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_IDLE_PRODUCTS_H
#define UI_PAGE_IDLE_PRODUCTS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_page_idle_products
{
public:
    QLabel *label_product_1_photo;
    QLabel *label_product_2_photo;
    QLabel *label_product_3_photo;
    QLabel *label_product_4_photo;
    QLabel *label_product1_price;
    QLabel *label_product2_price;
    QLabel *label_product3_price;
    QLabel *label_product4_price;
    QLabel *label_product_3_type;
    QLabel *label_product_2_type;
    QLabel *label_product_4_type;
    QLabel *label_product_1_type;
    QLabel *label_product_1_overlay_text;
    QLabel *label_product_2_overlay_text;
    QLabel *label_product_4_overlay_text;
    QLabel *label_product_3_overlay_text;
    QLabel *label_product_1_overlay;
    QLabel *label_product_2_overlay;
    QLabel *label_product_4_overlay;
    QLabel *label_product_3_overlay;
    QLabel *label_title;
    QLabel *label_customer_logo;
    QLabel *label_printer_status;
    QPushButton *pushButton_to_select_product_page;

    void setupUi(QWidget *page_idle_products)
    {
        if (page_idle_products->objectName().isEmpty())
            page_idle_products->setObjectName(QStringLiteral("page_idle_products"));
        page_idle_products->resize(1080, 1920);
        page_idle_products->setStyleSheet(QStringLiteral(""));
        label_product_1_photo = new QLabel(page_idle_products);
        label_product_1_photo->setObjectName(QStringLiteral("label_product_1_photo"));
        label_product_1_photo->setGeometry(QRect(53, 334, 450, 450));
        QFont font;
        font.setPointSize(48);
        font.setBold(true);
        font.setWeight(75);
        label_product_1_photo->setFont(font);
        label_product_1_photo->setStyleSheet(QStringLiteral(""));
        label_product_1_photo->setAlignment(Qt::AlignCenter);
        label_product_1_photo->setWordWrap(true);
        label_product_2_photo = new QLabel(page_idle_products);
        label_product_2_photo->setObjectName(QStringLiteral("label_product_2_photo"));
        label_product_2_photo->setGeometry(QRect(562, 334, 450, 450));
        label_product_2_photo->setFont(font);
        label_product_2_photo->setLayoutDirection(Qt::LeftToRight);
        label_product_2_photo->setStyleSheet(QStringLiteral(""));
        label_product_2_photo->setAlignment(Qt::AlignCenter);
        label_product_2_photo->setWordWrap(true);
        label_product_3_photo = new QLabel(page_idle_products);
        label_product_3_photo->setObjectName(QStringLiteral("label_product_3_photo"));
        label_product_3_photo->setGeometry(QRect(53, 1032, 450, 450));
        label_product_3_photo->setFont(font);
        label_product_3_photo->setLayoutDirection(Qt::LeftToRight);
        label_product_3_photo->setStyleSheet(QStringLiteral(""));
        label_product_3_photo->setAlignment(Qt::AlignCenter);
        label_product_3_photo->setWordWrap(true);
        label_product_4_photo = new QLabel(page_idle_products);
        label_product_4_photo->setObjectName(QStringLiteral("label_product_4_photo"));
        label_product_4_photo->setGeometry(QRect(562, 1032, 450, 450));
        label_product_4_photo->setFont(font);
        label_product_4_photo->setLayoutDirection(Qt::LeftToRight);
        label_product_4_photo->setStyleSheet(QStringLiteral(""));
        label_product_4_photo->setAlignment(Qt::AlignCenter);
        label_product_4_photo->setWordWrap(true);
        label_product1_price = new QLabel(page_idle_products);
        label_product1_price->setObjectName(QStringLiteral("label_product1_price"));
        label_product1_price->setGeometry(QRect(50, 820, 450, 71));
        label_product2_price = new QLabel(page_idle_products);
        label_product2_price->setObjectName(QStringLiteral("label_product2_price"));
        label_product2_price->setGeometry(QRect(562, 820, 450, 81));
        label_product3_price = new QLabel(page_idle_products);
        label_product3_price->setObjectName(QStringLiteral("label_product3_price"));
        label_product3_price->setGeometry(QRect(53, 1520, 450, 71));
        label_product4_price = new QLabel(page_idle_products);
        label_product4_price->setObjectName(QStringLiteral("label_product4_price"));
        label_product4_price->setGeometry(QRect(562, 1520, 450, 71));
        label_product_3_type = new QLabel(page_idle_products);
        label_product_3_type->setObjectName(QStringLiteral("label_product_3_type"));
        label_product_3_type->setGeometry(QRect(53, 1490, 450, 48));
        label_product_2_type = new QLabel(page_idle_products);
        label_product_2_type->setObjectName(QStringLiteral("label_product_2_type"));
        label_product_2_type->setGeometry(QRect(562, 780, 450, 46));
        label_product_4_type = new QLabel(page_idle_products);
        label_product_4_type->setObjectName(QStringLiteral("label_product_4_type"));
        label_product_4_type->setGeometry(QRect(562, 1490, 450, 46));
        label_product_1_type = new QLabel(page_idle_products);
        label_product_1_type->setObjectName(QStringLiteral("label_product_1_type"));
        label_product_1_type->setGeometry(QRect(53, 780, 450, 46));
        label_product_1_overlay_text = new QLabel(page_idle_products);
        label_product_1_overlay_text->setObjectName(QStringLiteral("label_product_1_overlay_text"));
        label_product_1_overlay_text->setGeometry(QRect(53, 334, 450, 450));
        label_product_1_overlay_text->setFont(font);
        label_product_1_overlay_text->setStyleSheet(QStringLiteral("color:#707173;"));
        label_product_1_overlay_text->setAlignment(Qt::AlignCenter);
        label_product_1_overlay_text->setWordWrap(true);
        label_product_2_overlay_text = new QLabel(page_idle_products);
        label_product_2_overlay_text->setObjectName(QStringLiteral("label_product_2_overlay_text"));
        label_product_2_overlay_text->setGeometry(QRect(562, 334, 450, 450));
        label_product_2_overlay_text->setFont(font);
        label_product_2_overlay_text->setStyleSheet(QStringLiteral("color:#707173;"));
        label_product_2_overlay_text->setAlignment(Qt::AlignCenter);
        label_product_2_overlay_text->setWordWrap(true);
        label_product_4_overlay_text = new QLabel(page_idle_products);
        label_product_4_overlay_text->setObjectName(QStringLiteral("label_product_4_overlay_text"));
        label_product_4_overlay_text->setGeometry(QRect(570, 1030, 450, 450));
        label_product_4_overlay_text->setFont(font);
        label_product_4_overlay_text->setStyleSheet(QStringLiteral("color:#707173;"));
        label_product_4_overlay_text->setAlignment(Qt::AlignCenter);
        label_product_4_overlay_text->setWordWrap(true);
        label_product_3_overlay_text = new QLabel(page_idle_products);
        label_product_3_overlay_text->setObjectName(QStringLiteral("label_product_3_overlay_text"));
        label_product_3_overlay_text->setGeometry(QRect(50, 1030, 450, 450));
        label_product_3_overlay_text->setFont(font);
        label_product_3_overlay_text->setStyleSheet(QStringLiteral("color:#707173;"));
        label_product_3_overlay_text->setAlignment(Qt::AlignCenter);
        label_product_3_overlay_text->setWordWrap(true);
        label_product_1_overlay = new QLabel(page_idle_products);
        label_product_1_overlay->setObjectName(QStringLiteral("label_product_1_overlay"));
        label_product_1_overlay->setGeometry(QRect(60, 280, 450, 161));
        label_product_2_overlay = new QLabel(page_idle_products);
        label_product_2_overlay->setObjectName(QStringLiteral("label_product_2_overlay"));
        label_product_2_overlay->setGeometry(QRect(562, 334, 450, 450));
        label_product_4_overlay = new QLabel(page_idle_products);
        label_product_4_overlay->setObjectName(QStringLiteral("label_product_4_overlay"));
        label_product_4_overlay->setGeometry(QRect(562, 1032, 450, 450));
        label_product_3_overlay = new QLabel(page_idle_products);
        label_product_3_overlay->setObjectName(QStringLiteral("label_product_3_overlay"));
        label_product_3_overlay->setGeometry(QRect(53, 1032, 450, 450));
        label_title = new QLabel(page_idle_products);
        label_title->setObjectName(QStringLiteral("label_title"));
        label_title->setGeometry(QRect(0, 80, 1080, 191));
        label_customer_logo = new QLabel(page_idle_products);
        label_customer_logo->setObjectName(QStringLiteral("label_customer_logo"));
        label_customer_logo->setGeometry(QRect(446, 1732, 191, 181));
        label_printer_status = new QLabel(page_idle_products);
        label_printer_status->setObjectName(QStringLiteral("label_printer_status"));
        label_printer_status->setGeometry(QRect(50, 850, 980, 120));
        pushButton_to_select_product_page = new QPushButton(page_idle_products);
        pushButton_to_select_product_page->setObjectName(QStringLiteral("pushButton_to_select_product_page"));
        pushButton_to_select_product_page->setGeometry(QRect(0, 0, 1080, 1920));
        pushButton_to_select_product_page->setStyleSheet(QStringLiteral(""));
        pushButton_to_select_product_page->raise();
        label_product_1_type->raise();
        label_product_2_photo->raise();
        label_product_2_overlay_text->raise();
        label_product_1_photo->raise();
        label_product_1_overlay_text->raise();
        label_product_3_photo->raise();
        label_product_4_photo->raise();
        label_product1_price->raise();
        label_product2_price->raise();
        label_product3_price->raise();
        label_product4_price->raise();
        label_product_3_type->raise();
        label_product_2_type->raise();
        label_product_4_type->raise();
        label_product_4_overlay_text->raise();
        label_product_3_overlay_text->raise();
        label_product_1_overlay->raise();
        label_product_2_overlay->raise();
        label_product_4_overlay->raise();
        label_product_3_overlay->raise();
        label_title->raise();
        label_customer_logo->raise();
        label_printer_status->raise();

        retranslateUi(page_idle_products);

        QMetaObject::connectSlotsByName(page_idle_products);
    } // setupUi

    void retranslateUi(QWidget *page_idle_products)
    {
        page_idle_products->setWindowTitle(QApplication::translate("page_idle_products", "Form", Q_NULLPTR));
        label_product_1_photo->setText(QString());
        label_product_2_photo->setText(QString());
        label_product_3_photo->setText(QString());
        label_product_4_photo->setText(QString());
        label_product1_price->setText(QString());
        label_product2_price->setText(QString());
        label_product3_price->setText(QString());
        label_product4_price->setText(QString());
        label_product_3_type->setText(QApplication::translate("page_idle_products", "TextLabel", Q_NULLPTR));
        label_product_2_type->setText(QApplication::translate("page_idle_products", "TextLabel", Q_NULLPTR));
        label_product_4_type->setText(QApplication::translate("page_idle_products", "TextLabel", Q_NULLPTR));
        label_product_1_type->setText(QApplication::translate("page_idle_products", "TextLabel", Q_NULLPTR));
        label_product_1_overlay_text->setText(QString());
        label_product_2_overlay_text->setText(QString());
        label_product_4_overlay_text->setText(QString());
        label_product_3_overlay_text->setText(QString());
        label_product_1_overlay->setText(QApplication::translate("page_idle_products", "TextLabel", Q_NULLPTR));
        label_product_2_overlay->setText(QApplication::translate("page_idle_products", "TextLabel", Q_NULLPTR));
        label_product_4_overlay->setText(QApplication::translate("page_idle_products", "TextLabel", Q_NULLPTR));
        label_product_3_overlay->setText(QApplication::translate("page_idle_products", "TextLabel", Q_NULLPTR));
        label_title->setText(QApplication::translate("page_idle_products", "TextLabel", Q_NULLPTR));
        label_customer_logo->setText(QString());
        label_printer_status->setText(QString());
        pushButton_to_select_product_page->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class page_idle_products: public Ui_page_idle_products {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_IDLE_PRODUCTS_H
