/********************************************************************************
** Form generated from reading UI file 'page_init.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_INIT_H
#define UI_PAGE_INIT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_page_init
{
public:
    QLabel *label_init_message;
    QLabel *label_fail_message;

    void setupUi(QWidget *page_init)
    {
        if (page_init->objectName().isEmpty())
            page_init->setObjectName(QStringLiteral("page_init"));
        page_init->resize(1080, 1920);
        page_init->setStyleSheet(QStringLiteral(""));
        label_init_message = new QLabel(page_init);
        label_init_message->setObjectName(QStringLiteral("label_init_message"));
        label_init_message->setGeometry(QRect(30, 1190, 951, 111));
        QFont font;
        font.setPointSize(33);
        label_init_message->setFont(font);
        label_init_message->setStyleSheet(QStringLiteral("color:white;"));
        label_init_message->setAlignment(Qt::AlignCenter);
        label_fail_message = new QLabel(page_init);
        label_fail_message->setObjectName(QStringLiteral("label_fail_message"));
        label_fail_message->setGeometry(QRect(270, 1430, 531, 111));
        label_fail_message->setFont(font);
        label_fail_message->setStyleSheet(QStringLiteral("color:white;"));

        retranslateUi(page_init);

        QMetaObject::connectSlotsByName(page_init);
    } // setupUi

    void retranslateUi(QWidget *page_init)
    {
        page_init->setWindowTitle(QApplication::translate("page_init", "Form", Q_NULLPTR));
        label_init_message->setText(QApplication::translate("page_init", "Initializing", Q_NULLPTR));
        label_fail_message->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class page_init: public Ui_page_init {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_INIT_H
