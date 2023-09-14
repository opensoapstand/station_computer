/********************************************************************************
** Form generated from reading UI file 'page_product.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_PRODUCT_H
#define UI_PAGE_PRODUCT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_page_product
{
public:
    QPushButton *pushButton_previous_page;
    QPushButton *pushButton_continue;
    QPushButton *pushButton_to_help;
    QLabel *label_price_custom;
    QLabel *label_price_large;
    QLabel *label_size_custom;
    QLabel *label_size_large;
    QPushButton *pushButton_order_small;
    QPushButton *pushButton_order_big;
    QPushButton *pushButton_order_medium;
    QPushButton *pushButton_order_custom;
    QLabel *label_size_medium;
    QLabel *label_price_medium;
    QLabel *label_size_small;
    QLabel *label_price_small;
    QLabel *label_product_title;
    QLabel *label_product_description;
    QLabel *label_product_ingredients;
    QLabel *label_product_photo;
    QLabel *label_product_ingredients_title;
    QLabel *label_background_small;
    QLabel *label_background_medium;
    QLabel *label_background_large;
    QLabel *label_background_custom;
    QLabel *label_select_quantity;
    QPushButton *pushButton_back;
    QLabel *label_help;

    void setupUi(QWidget *page_product)
    {
        if (page_product->objectName().isEmpty())
            page_product->setObjectName(QStringLiteral("page_product"));
        page_product->resize(1125, 1920);
        QFont font;
        font.setFamily(QStringLiteral("Montserrat"));
        font.setBold(false);
        font.setWeight(50);
        page_product->setFont(font);
        page_product->setStyleSheet(QLatin1String("color: #58595B;\n"
"font-family: Montserrat;\n"
"font-style: light;\n"
"font-weight: normal;\n"
"font-size: 23px;\n"
"line-height: 29px;"));
        pushButton_previous_page = new QPushButton(page_product);
        pushButton_previous_page->setObjectName(QStringLiteral("pushButton_previous_page"));
        pushButton_previous_page->setGeometry(QRect(68, 1757, 281, 100));
        pushButton_continue = new QPushButton(page_product);
        pushButton_continue->setObjectName(QStringLiteral("pushButton_continue"));
        pushButton_continue->setGeometry(QRect(188, 1541, 740, 92));
        pushButton_continue->setAutoFillBackground(false);
        pushButton_to_help = new QPushButton(page_product);
        pushButton_to_help->setObjectName(QStringLiteral("pushButton_to_help"));
        pushButton_to_help->setEnabled(true);
        pushButton_to_help->setGeometry(QRect(480, 1770, 125, 125));
        label_price_custom = new QLabel(page_product);
        label_price_custom->setObjectName(QStringLiteral("label_price_custom"));
        label_price_custom->setGeometry(QRect(550, 1160, 451, 101));
        label_price_custom->setLayoutDirection(Qt::LeftToRight);
        label_price_custom->setStyleSheet(QLatin1String("font-family: Montserrat;\n"
"background-image: url(background.png);\n"
"font-style: normal;\n"
"font-weight: bold;\n"
"font-size: 36px;\n"
"line-height: 44px;\n"
"color: #FFFFFF;\n"
"text-align: center;\n"
""));
        label_price_custom->setAlignment(Qt::AlignCenter);
        label_price_custom->setWordWrap(true);
        label_price_large = new QLabel(page_product);
        label_price_large->setObjectName(QStringLiteral("label_price_large"));
        label_price_large->setGeometry(QRect(860, 1040, 121, 41));
        label_price_large->setLayoutDirection(Qt::LeftToRight);
        label_price_large->setStyleSheet(QLatin1String("font-family: Montserrat;\n"
"background-image: url(/home/df-admin/production/references/background.png);\n"
"font-style: normal;\n"
"font-weight: bold;\n"
"font-size: 36px;\n"
"line-height: 44px;\n"
"color: #3D6675;\n"
"text-align: center;"));
        label_price_large->setAlignment(Qt::AlignCenter);
        label_size_custom = new QLabel(page_product);
        label_size_custom->setObjectName(QStringLiteral("label_size_custom"));
        label_size_custom->setGeometry(QRect(570, 1110, 401, 41));
        label_size_custom->setStyleSheet(QLatin1String("color: #5E8680;\n"
"font-family: Montserrat;\n"
"font-style: light;\n"
"font-weight: normal;\n"
"font-size: 20px;\n"
"line-height: 24px;\n"
"text-align: center;\n"
""));
        label_size_custom->setAlignment(Qt::AlignCenter);
        label_size_large = new QLabel(page_product);
        label_size_large->setObjectName(QStringLiteral("label_size_large"));
        label_size_large->setGeometry(QRect(860, 1000, 121, 31));
        label_size_large->setStyleSheet(QLatin1String("color: #5E8680;\n"
"font-family: Montserrat;\n"
"font-style: light;\n"
"font-weight: normal;\n"
"font-size: 20px;\n"
"line-height: 24px;\n"
"text-align: center;"));
        label_size_large->setAlignment(Qt::AlignCenter);
        pushButton_order_small = new QPushButton(page_product);
        pushButton_order_small->setObjectName(QStringLiteral("pushButton_order_small"));
        pushButton_order_small->setGeometry(QRect(564, 996, 130, 100));
        pushButton_order_small->setAutoFillBackground(false);
        pushButton_order_big = new QPushButton(page_product);
        pushButton_order_big->setObjectName(QStringLiteral("pushButton_order_big"));
        pushButton_order_big->setGeometry(QRect(858, 996, 130, 100));
        pushButton_order_medium = new QPushButton(page_product);
        pushButton_order_medium->setObjectName(QStringLiteral("pushButton_order_medium"));
        pushButton_order_medium->setGeometry(QRect(711, 996, 130, 100));
        pushButton_order_medium->setAutoFillBackground(false);
        pushButton_order_custom = new QPushButton(page_product);
        pushButton_order_custom->setObjectName(QStringLiteral("pushButton_order_custom"));
        pushButton_order_custom->setGeometry(QRect(563, 1113, 424, 191));
        pushButton_order_custom->setAutoFillBackground(false);
        label_size_medium = new QLabel(page_product);
        label_size_medium->setObjectName(QStringLiteral("label_size_medium"));
        label_size_medium->setGeometry(QRect(710, 1000, 131, 31));
        label_size_medium->setStyleSheet(QLatin1String("color: #5E8680;\n"
"font-family: Montserrat;\n"
"font-style: light;\n"
"font-weight: normal;\n"
"font-size: 20px;\n"
"line-height: 24px;\n"
"text-align: center;"));
        label_size_medium->setAlignment(Qt::AlignCenter);
        label_price_medium = new QLabel(page_product);
        label_price_medium->setObjectName(QStringLiteral("label_price_medium"));
        label_price_medium->setGeometry(QRect(710, 1040, 131, 41));
        label_price_medium->setLayoutDirection(Qt::LeftToRight);
        label_price_medium->setStyleSheet(QLatin1String("font-family: Montserrat;\n"
"background-image: url(/home/df-admin/production/references/background.png);\n"
"font-style: normal;\n"
"font-weight: bold;\n"
"font-size: 36px;\n"
"line-height: 44px;\n"
"color: #3D6675;\n"
"text-align: center;"));
        label_price_medium->setAlignment(Qt::AlignCenter);
        label_size_small = new QLabel(page_product);
        label_size_small->setObjectName(QStringLiteral("label_size_small"));
        label_size_small->setGeometry(QRect(560, 1000, 131, 31));
        label_size_small->setStyleSheet(QLatin1String("color: #5E8680;\n"
"font-family: Montserrat;\n"
"font-style: light;\n"
"font-weight: normal;\n"
"font-size: 20px;\n"
"line-height: 24px;\n"
"text-align: center;"));
        label_size_small->setAlignment(Qt::AlignCenter);
        label_price_small = new QLabel(page_product);
        label_price_small->setObjectName(QStringLiteral("label_price_small"));
        label_price_small->setGeometry(QRect(560, 1040, 131, 41));
        label_price_small->setLayoutDirection(Qt::LeftToRight);
        label_price_small->setStyleSheet(QLatin1String("font-family: Montserrat;\n"
"background-image: url(/home/df-admin/production/references/background.png);\n"
"font-style: normal;\n"
"font-weight: bold;\n"
"font-size: 36px;\n"
"line-height: 44px;\n"
"color: #3D6675;\n"
"text-align: center;"));
        label_price_small->setAlignment(Qt::AlignCenter);
        label_product_title = new QLabel(page_product);
        label_product_title->setObjectName(QStringLiteral("label_product_title"));
        label_product_title->setGeometry(QRect(90, 326, 881, 121));
        label_product_description = new QLabel(page_product);
        label_product_description->setObjectName(QStringLiteral("label_product_description"));
        label_product_description->setGeometry(QRect(88, 461, 901, 271));
        label_product_description->setFont(font);
        label_product_description->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_product_ingredients = new QLabel(page_product);
        label_product_ingredients->setObjectName(QStringLiteral("label_product_ingredients"));
        label_product_ingredients->setGeometry(QRect(100, 1280, 888, 200));
        label_product_ingredients->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_product_photo = new QLabel(page_product);
        label_product_photo->setObjectName(QStringLiteral("label_product_photo"));
        label_product_photo->setGeometry(QRect(94, 765, 450, 450));
        label_product_photo->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_product_ingredients_title = new QLabel(page_product);
        label_product_ingredients_title->setObjectName(QStringLiteral("label_product_ingredients_title"));
        label_product_ingredients_title->setGeometry(QRect(100, 1239, 351, 50));
        label_product_ingredients_title->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_background_small = new QLabel(page_product);
        label_background_small->setObjectName(QStringLiteral("label_background_small"));
        label_background_small->setGeometry(QRect(310, 230, 67, 17));
        label_background_medium = new QLabel(page_product);
        label_background_medium->setObjectName(QStringLiteral("label_background_medium"));
        label_background_medium->setGeometry(QRect(410, 230, 67, 17));
        label_background_large = new QLabel(page_product);
        label_background_large->setObjectName(QStringLiteral("label_background_large"));
        label_background_large->setGeometry(QRect(500, 240, 67, 17));
        label_background_custom = new QLabel(page_product);
        label_background_custom->setObjectName(QStringLiteral("label_background_custom"));
        label_background_custom->setGeometry(QRect(400, 280, 67, 17));
        label_select_quantity = new QLabel(page_product);
        label_select_quantity->setObjectName(QStringLiteral("label_select_quantity"));
        label_select_quantity->setGeometry(QRect(567, 949, 393, 44));
        pushButton_back = new QPushButton(page_product);
        pushButton_back->setObjectName(QStringLiteral("pushButton_back"));
        pushButton_back->setGeometry(QRect(70, 40, 401, 41));
        label_help = new QLabel(page_product);
        label_help->setObjectName(QStringLiteral("label_help"));
        label_help->setEnabled(true);
        label_help->setGeometry(QRect(478, 1773, 125, 125));
        label_help->raise();
        pushButton_continue->raise();
        pushButton_order_small->raise();
        pushButton_order_big->raise();
        pushButton_order_medium->raise();
        pushButton_order_custom->raise();
        label_size_medium->raise();
        label_price_medium->raise();
        label_size_small->raise();
        label_price_small->raise();
        label_price_custom->raise();
        label_size_custom->raise();
        label_price_large->raise();
        label_size_large->raise();
        label_product_title->raise();
        label_product_description->raise();
        label_product_photo->raise();
        label_product_ingredients_title->raise();
        label_background_small->raise();
        label_background_medium->raise();
        label_background_large->raise();
        label_background_custom->raise();
        label_product_ingredients->raise();
        label_select_quantity->raise();
        pushButton_back->raise();
        pushButton_to_help->raise();
        pushButton_previous_page->raise();

        retranslateUi(page_product);

        QMetaObject::connectSlotsByName(page_product);
    } // setupUi

    void retranslateUi(QWidget *page_product)
    {
        page_product->setWindowTitle(QApplication::translate("page_product", "Form", Q_NULLPTR));
        pushButton_previous_page->setText(QString());
        pushButton_continue->setText(QString());
        pushButton_to_help->setText(QString());
        label_price_custom->setText(QString());
        label_price_large->setText(QString());
        label_size_custom->setText(QString());
        label_size_large->setText(QString());
        pushButton_order_small->setText(QString());
        pushButton_order_big->setText(QString());
        pushButton_order_medium->setText(QString());
        pushButton_order_custom->setText(QString());
        label_size_medium->setText(QString());
        label_price_medium->setText(QString());
        label_size_small->setText(QString());
        label_price_small->setText(QString());
        label_product_title->setText(QApplication::translate("page_product", "TextLabel", Q_NULLPTR));
        label_product_description->setText(QApplication::translate("page_product", "TextLabel", Q_NULLPTR));
        label_product_ingredients->setText(QApplication::translate("page_product", "TextLabel", Q_NULLPTR));
        label_product_photo->setText(QApplication::translate("page_product", "picture", Q_NULLPTR));
        label_product_ingredients_title->setText(QApplication::translate("page_product", "Ingredients:", Q_NULLPTR));
        label_background_small->setText(QString());
        label_background_medium->setText(QString());
        label_background_large->setText(QString());
        label_background_custom->setText(QString());
        label_select_quantity->setText(QApplication::translate("page_product", "TextLabel", Q_NULLPTR));
        pushButton_back->setText(QApplication::translate("page_product", "back", Q_NULLPTR));
        label_help->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class page_product: public Ui_page_product {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_PRODUCT_H
