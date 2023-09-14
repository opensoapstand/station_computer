/********************************************************************************
** Form generated from reading UI file 'page_transactions.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_TRANSACTIONS_H
#define UI_PAGE_TRANSACTIONS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_page_transactions
{
public:
    QPushButton *pushButton_back;
    QListWidget *list_transactions;
    QPushButton *pushButton_print;
    QLabel *label_title;

    void setupUi(QWidget *page_transactions)
    {
        if (page_transactions->objectName().isEmpty())
            page_transactions->setObjectName(QStringLiteral("page_transactions"));
        page_transactions->resize(1080, 1920);
        pushButton_back = new QPushButton(page_transactions);
        pushButton_back->setObjectName(QStringLiteral("pushButton_back"));
        pushButton_back->setGeometry(QRect(70, 40, 401, 41));
        list_transactions = new QListWidget(page_transactions);
        list_transactions->setObjectName(QStringLiteral("list_transactions"));
        list_transactions->setGeometry(QRect(40, 420, 1001, 901));
        QFont font;
        font.setFamily(QStringLiteral("Courier"));
        font.setPointSize(14);
        font.setBold(true);
        font.setWeight(75);
        list_transactions->setFont(font);
        list_transactions->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        list_transactions->setAlternatingRowColors(true);
        pushButton_print = new QPushButton(page_transactions);
        pushButton_print->setObjectName(QStringLiteral("pushButton_print"));
        pushButton_print->setGeometry(QRect(280, 1360, 521, 81));
        label_title = new QLabel(page_transactions);
        label_title->setObjectName(QStringLiteral("label_title"));
        label_title->setGeometry(QRect(0, 178, 1080, 86));

        retranslateUi(page_transactions);

        QMetaObject::connectSlotsByName(page_transactions);
    } // setupUi

    void retranslateUi(QWidget *page_transactions)
    {
        page_transactions->setWindowTitle(QApplication::translate("page_transactions", "Form", Q_NULLPTR));
        pushButton_back->setText(QApplication::translate("page_transactions", "Back", Q_NULLPTR));
        pushButton_print->setText(QApplication::translate("page_transactions", "Print Selected Receipt", Q_NULLPTR));
        label_title->setText(QApplication::translate("page_transactions", "TextLabel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class page_transactions: public Ui_page_transactions {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_TRANSACTIONS_H
