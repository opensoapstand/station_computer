/********************************************************************************
** Form generated from reading UI file 'survey.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SURVEY_H
#define UI_SURVEY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_survey
{
public:
    QLabel *surveyLabel;
    QLabel *logoLabel;
    QLineEdit *drinkEdit;
    QLabel *drinkLabel;
    QPushButton *saveButton;
    QLineEdit *emailEdit;
    QLabel *emailLabel;
    QLabel *invalidLabel;
    QPushButton *goBackButton;
    QLabel *bottleSizeLabel;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_3;
    QRadioButton *ozRB_12;
    QRadioButton *ozRB_16;
    QVBoxLayout *verticalLayout;
    QRadioButton *ozRB_24;
    QRadioButton *ozRB_NA;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *ozRB_DK;
    QRadioButton *hiddenButton;

    void setupUi(QDialog *survey)
    {
        if (survey->objectName().isEmpty())
            survey->setObjectName(QString::fromUtf8("survey"));
        survey->resize(800, 1280);
        surveyLabel = new QLabel(survey);
        surveyLabel->setObjectName(QString::fromUtf8("surveyLabel"));
        surveyLabel->setGeometry(QRect(0, 150, 1080, 130));
        logoLabel = new QLabel(survey);
        logoLabel->setObjectName(QString::fromUtf8("logoLabel"));
        logoLabel->setGeometry(QRect(100, 1600, 880, 200));
        drinkEdit = new QLineEdit(survey);
        drinkEdit->setObjectName(QString::fromUtf8("drinkEdit"));
        drinkEdit->setGeometry(QRect(100, 550, 880, 40));
        drinkLabel = new QLabel(survey);
        drinkLabel->setObjectName(QString::fromUtf8("drinkLabel"));
        drinkLabel->setGeometry(QRect(100, 350, 879, 181));
        saveButton = new QPushButton(survey);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));
        saveButton->setGeometry(QRect(96, 1300, 880, 80));
        emailEdit = new QLineEdit(survey);
        emailEdit->setObjectName(QString::fromUtf8("emailEdit"));
        emailEdit->setGeometry(QRect(100, 1100, 880, 40));
        emailLabel = new QLabel(survey);
        emailLabel->setObjectName(QString::fromUtf8("emailLabel"));
        emailLabel->setGeometry(QRect(100, 900, 879, 181));
        invalidLabel = new QLabel(survey);
        invalidLabel->setObjectName(QString::fromUtf8("invalidLabel"));
        invalidLabel->setGeometry(QRect(100, 1170, 879, 111));
        goBackButton = new QPushButton(survey);
        goBackButton->setObjectName(QString::fromUtf8("goBackButton"));
        goBackButton->setGeometry(QRect(440, 1450, 200, 81));
        bottleSizeLabel = new QLabel(survey);
        bottleSizeLabel->setObjectName(QString::fromUtf8("bottleSizeLabel"));
        bottleSizeLabel->setGeometry(QRect(100, 610, 879, 181));
        horizontalLayoutWidget = new QWidget(survey);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(90, 780, 881, 85));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        ozRB_12 = new QRadioButton(horizontalLayoutWidget);
        ozRB_12->setObjectName(QString::fromUtf8("ozRB_12"));
        ozRB_12->setStyleSheet(QString::fromUtf8("QRadioButton {\n"
"    background-color:       transparent;\n"
"    color:                  white;\n"
"	\n"
"}\n"
"\n"
"QRadioButton::indicator {\n"
"    width:                  10px;\n"
"    height:                 10px;\n"
"    border-radius:          7px;\n"
"}\n"
"\n"
"QRadioButton::indicator:checked {\n"
"    background-color:       black;\n"
"    border:                 2px solid white;\n"
"}\n"
"\n"
"QRadioButton::indicator:unchecked {\n"
"    background-color:      white;\n"
"    border:                 2px solid white;\n"
"}"));

        verticalLayout_3->addWidget(ozRB_12);

        ozRB_16 = new QRadioButton(horizontalLayoutWidget);
        ozRB_16->setObjectName(QString::fromUtf8("ozRB_16"));
        ozRB_16->setStyleSheet(QString::fromUtf8("QRadioButton {\n"
"    background-color:       transparent;\n"
"    color:                  white;\n"
"}\n"
"\n"
"QRadioButton::indicator {\n"
"    width:                  10px;\n"
"    height:                 10px;\n"
"    border-radius:          7px;\n"
"}\n"
"\n"
"QRadioButton::indicator:checked {\n"
"    background-color:       black;\n"
"    border:                 2px solid white;\n"
"}\n"
"\n"
"QRadioButton::indicator:unchecked {\n"
"    background-color:      white;\n"
"    border:                 2px solid white;\n"
"}"));

        verticalLayout_3->addWidget(ozRB_16);


        horizontalLayout->addLayout(verticalLayout_3);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        ozRB_24 = new QRadioButton(horizontalLayoutWidget);
        ozRB_24->setObjectName(QString::fromUtf8("ozRB_24"));
        ozRB_24->setStyleSheet(QString::fromUtf8("QRadioButton {\n"
"    background-color:       transparent;\n"
"    color:                  white;\n"
"}\n"
"\n"
"QRadioButton::indicator {\n"
"    width:                  10px;\n"
"    height:                 10px;\n"
"    border-radius:          7px;\n"
"}\n"
"\n"
"QRadioButton::indicator:checked {\n"
"    background-color:       black;\n"
"    border:                 2px solid white;\n"
"}\n"
"\n"
"QRadioButton::indicator:unchecked {\n"
"    background-color:      white;\n"
"    border:                 2px solid white;\n"
"}"));

        verticalLayout->addWidget(ozRB_24);

        ozRB_NA = new QRadioButton(horizontalLayoutWidget);
        ozRB_NA->setObjectName(QString::fromUtf8("ozRB_NA"));
        ozRB_NA->setStyleSheet(QString::fromUtf8("QRadioButton {\n"
"    background-color:       transparent;\n"
"    color:                  white;\n"
"}\n"
"\n"
"QRadioButton::indicator {\n"
"    width:                  10px;\n"
"    height:                 10px;\n"
"    border-radius:          7px;\n"
"}\n"
"\n"
"QRadioButton::indicator:checked {\n"
"    background-color:       black;\n"
"    border:                 2px solid white;\n"
"}\n"
"\n"
"QRadioButton::indicator:unchecked {\n"
"    background-color:      white;\n"
"    border:                 2px solid white;\n"
"}"));

        verticalLayout->addWidget(ozRB_NA);


        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        ozRB_DK = new QRadioButton(horizontalLayoutWidget);
        ozRB_DK->setObjectName(QString::fromUtf8("ozRB_DK"));
        ozRB_DK->setStyleSheet(QString::fromUtf8("QRadioButton {\n"
"    background-color:       transparent;\n"
"    color:                  white;\n"
"}\n"
"\n"
"QRadioButton::indicator {\n"
"    width:                  10px;\n"
"    height:                 10px;\n"
"    border-radius:          7px;\n"
"}\n"
"\n"
"QRadioButton::indicator:checked {\n"
"    background-color:       black;\n"
"    border:                 2px solid white;\n"
"}\n"
"\n"
"QRadioButton::indicator:unchecked {\n"
"    background-color:      white;\n"
"    border:                 2px solid white;\n"
"}"));

        verticalLayout_2->addWidget(ozRB_DK);

        hiddenButton = new QRadioButton(horizontalLayoutWidget);
        hiddenButton->setObjectName(QString::fromUtf8("hiddenButton"));

        verticalLayout_2->addWidget(hiddenButton);


        horizontalLayout->addLayout(verticalLayout_2);


        retranslateUi(survey);

        QMetaObject::connectSlotsByName(survey);
    } // setupUi

    void retranslateUi(QDialog *survey)
    {
        survey->setWindowTitle(QCoreApplication::translate("survey", "Dialog", nullptr));
        surveyLabel->setText(QCoreApplication::translate("survey", "TextLabel", nullptr));
        logoLabel->setText(QCoreApplication::translate("survey", "TextLabel", nullptr));
        drinkLabel->setText(QCoreApplication::translate("survey", "TextLabel", nullptr));
        saveButton->setText(QCoreApplication::translate("survey", "Save $1.00", nullptr));
        emailLabel->setText(QCoreApplication::translate("survey", "TextLabel", nullptr));
        invalidLabel->setText(QCoreApplication::translate("survey", "TextLabel", nullptr));
        goBackButton->setText(QCoreApplication::translate("survey", "PushButton", nullptr));
        bottleSizeLabel->setText(QCoreApplication::translate("survey", "TextLabel", nullptr));
        ozRB_12->setText(QCoreApplication::translate("survey", "12oz (355mL)", nullptr));
        ozRB_16->setText(QCoreApplication::translate("survey", "16oz (473mL)", nullptr));
        ozRB_24->setText(QCoreApplication::translate("survey", "24oz (710mL)", nullptr));
        ozRB_NA->setText(QCoreApplication::translate("survey", "None of the Above", nullptr));
        ozRB_DK->setText(QCoreApplication::translate("survey", "Don't Really Know", nullptr));
        hiddenButton->setText(QCoreApplication::translate("survey", "hidden", nullptr));
    } // retranslateUi

};

namespace Ui {
    class survey: public Ui_survey {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SURVEY_H
