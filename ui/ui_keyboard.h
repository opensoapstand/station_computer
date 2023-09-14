/********************************************************************************
** Form generated from reading UI file 'keyboard.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KEYBOARD_H
#define UI_KEYBOARD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_keyboardWidget
{
public:
    QWidget *widget;
    QGridLayout *keyboard;
    QLabel *wifiPassLabel;
    QLineEdit *keyboardTextEntry;
    QPushButton *pushButton_21;
    QPushButton *pushButton_25;
    QPushButton *pushButton_29;
    QPushButton *pushButton_33;
    QPushButton *pushButton_37;
    QPushButton *pushButton_47;
    QPushButton *pushButton_48;
    QPushButton *pushButton_50;
    QPushButton *pushButton_53;
    QPushButton *pushButton_56;
    QPushButton *pushButton_61;
    QPushButton *pushButton_62;
    QPushButton *pushButton;
    QPushButton *pushButton_6;
    QPushButton *pushButton_17;
    QPushButton *pushButton_16;
    QPushButton *pushButton_15;
    QPushButton *pushButton_14;
    QPushButton *pushButton_13;
    QPushButton *pushButton_12;
    QPushButton *pushButton_11;
    QPushButton *pushButton_3;
    QPushButton *pushButton_2;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_60;
    QPushButton *pushButton_7;
    QPushButton *pushButton_18;
    QPushButton *pushButton_22;
    QPushButton *pushButton_26;
    QPushButton *pushButton_30;
    QPushButton *pushButton_34;
    QPushButton *pushButton_38;
    QPushButton *pushButton_40;
    QPushButton *pushButton_41;
    QPushButton *pushButton_42;
    QPushButton *pushButton_43;
    QPushButton *pushButton_63;
    QPushButton *pushButton_8;
    QPushButton *pushButton_19;
    QPushButton *pushButton_23;
    QPushButton *pushButton_27;
    QPushButton *pushButton_31;
    QPushButton *pushButton_35;
    QPushButton *pushButton_44;
    QPushButton *pushButton_51;
    QPushButton *pushButton_54;
    QPushButton *pushButton_57;
    QPushButton *pushButton_58;
    QPushButton *pushButton_67;
    QPushButton *pushButton_9;
    QPushButton *pushButton_20;
    QPushButton *pushButton_24;
    QPushButton *pushButton_28;
    QPushButton *pushButton_32;
    QPushButton *pushButton_36;
    QPushButton *pushButton_45;
    QPushButton *pushButton_52;
    QPushButton *pushButton_55;
    QPushButton *pushButton_59;
    QPushButton *pushButton_10;
    QPushButton *pushButton_39;
    QPushButton *pushButton_66;
    QPushButton *pushButton_65;
    QPushButton *pushButton_68;
    QPushButton *pushButton_64;

    void setupUi(QWidget *keyboardWidget)
    {
        if (keyboardWidget->objectName().isEmpty())
            keyboardWidget->setObjectName(QStringLiteral("keyboardWidget"));
        keyboardWidget->resize(1080, 639);
        keyboardWidget->setStyleSheet(QStringLiteral("background-color:white;"));
        widget = new QWidget(keyboardWidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(10, 170, 1051, 441));
        keyboard = new QGridLayout(widget);
        keyboard->setObjectName(QStringLiteral("keyboard"));
        keyboard->setContentsMargins(0, 0, 0, 0);
        wifiPassLabel = new QLabel(widget);
        wifiPassLabel->setObjectName(QStringLiteral("wifiPassLabel"));

        keyboard->addWidget(wifiPassLabel, 0, 0, 1, 12);

        keyboardTextEntry = new QLineEdit(widget);
        keyboardTextEntry->setObjectName(QStringLiteral("keyboardTextEntry"));
        keyboardTextEntry->setMinimumSize(QSize(0, 50));

        keyboard->addWidget(keyboardTextEntry, 1, 0, 1, 12);

        pushButton_21 = new QPushButton(widget);
        pushButton_21->setObjectName(QStringLiteral("pushButton_21"));
        pushButton_21->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_21, 2, 0, 1, 1);

        pushButton_25 = new QPushButton(widget);
        pushButton_25->setObjectName(QStringLiteral("pushButton_25"));
        pushButton_25->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_25, 2, 1, 1, 1);

        pushButton_29 = new QPushButton(widget);
        pushButton_29->setObjectName(QStringLiteral("pushButton_29"));
        pushButton_29->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_29, 2, 2, 1, 1);

        pushButton_33 = new QPushButton(widget);
        pushButton_33->setObjectName(QStringLiteral("pushButton_33"));
        pushButton_33->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_33, 2, 3, 1, 1);

        pushButton_37 = new QPushButton(widget);
        pushButton_37->setObjectName(QStringLiteral("pushButton_37"));
        pushButton_37->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_37, 2, 4, 1, 1);

        pushButton_47 = new QPushButton(widget);
        pushButton_47->setObjectName(QStringLiteral("pushButton_47"));
        pushButton_47->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_47, 2, 5, 1, 1);

        pushButton_48 = new QPushButton(widget);
        pushButton_48->setObjectName(QStringLiteral("pushButton_48"));
        pushButton_48->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_48, 2, 6, 1, 1);

        pushButton_50 = new QPushButton(widget);
        pushButton_50->setObjectName(QStringLiteral("pushButton_50"));
        pushButton_50->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_50, 2, 7, 1, 1);

        pushButton_53 = new QPushButton(widget);
        pushButton_53->setObjectName(QStringLiteral("pushButton_53"));
        pushButton_53->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_53, 2, 8, 1, 1);

        pushButton_56 = new QPushButton(widget);
        pushButton_56->setObjectName(QStringLiteral("pushButton_56"));
        pushButton_56->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_56, 2, 9, 1, 1);

        pushButton_61 = new QPushButton(widget);
        pushButton_61->setObjectName(QStringLiteral("pushButton_61"));
        pushButton_61->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_61, 2, 10, 1, 1);

        pushButton_62 = new QPushButton(widget);
        pushButton_62->setObjectName(QStringLiteral("pushButton_62"));
        pushButton_62->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_62, 2, 11, 1, 1);

        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setMaximumSize(QSize(87, 50));

        keyboard->addWidget(pushButton, 2, 12, 1, 1);

        pushButton_6 = new QPushButton(widget);
        pushButton_6->setObjectName(QStringLiteral("pushButton_6"));
        pushButton_6->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_6, 3, 0, 1, 1);

        pushButton_17 = new QPushButton(widget);
        pushButton_17->setObjectName(QStringLiteral("pushButton_17"));
        pushButton_17->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_17, 3, 1, 1, 1);

        pushButton_16 = new QPushButton(widget);
        pushButton_16->setObjectName(QStringLiteral("pushButton_16"));
        pushButton_16->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_16, 3, 2, 1, 1);

        pushButton_15 = new QPushButton(widget);
        pushButton_15->setObjectName(QStringLiteral("pushButton_15"));
        pushButton_15->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_15, 3, 3, 1, 1);

        pushButton_14 = new QPushButton(widget);
        pushButton_14->setObjectName(QStringLiteral("pushButton_14"));
        pushButton_14->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_14, 3, 4, 1, 1);

        pushButton_13 = new QPushButton(widget);
        pushButton_13->setObjectName(QStringLiteral("pushButton_13"));
        pushButton_13->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_13, 3, 5, 1, 1);

        pushButton_12 = new QPushButton(widget);
        pushButton_12->setObjectName(QStringLiteral("pushButton_12"));
        pushButton_12->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_12, 3, 6, 1, 1);

        pushButton_11 = new QPushButton(widget);
        pushButton_11->setObjectName(QStringLiteral("pushButton_11"));
        pushButton_11->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_11, 3, 7, 1, 1);

        pushButton_3 = new QPushButton(widget);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_3, 3, 8, 1, 1);

        pushButton_2 = new QPushButton(widget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_2, 3, 9, 1, 1);

        pushButton_4 = new QPushButton(widget);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));
        pushButton_4->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_4, 3, 10, 1, 1);

        pushButton_5 = new QPushButton(widget);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));
        pushButton_5->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_5, 3, 11, 1, 1);

        pushButton_60 = new QPushButton(widget);
        pushButton_60->setObjectName(QStringLiteral("pushButton_60"));
        pushButton_60->setMaximumSize(QSize(87, 50));

        keyboard->addWidget(pushButton_60, 3, 12, 1, 1);

        pushButton_7 = new QPushButton(widget);
        pushButton_7->setObjectName(QStringLiteral("pushButton_7"));
        pushButton_7->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_7, 4, 0, 1, 1);

        pushButton_18 = new QPushButton(widget);
        pushButton_18->setObjectName(QStringLiteral("pushButton_18"));
        pushButton_18->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_18, 4, 1, 1, 1);

        pushButton_22 = new QPushButton(widget);
        pushButton_22->setObjectName(QStringLiteral("pushButton_22"));
        pushButton_22->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_22, 4, 2, 1, 1);

        pushButton_26 = new QPushButton(widget);
        pushButton_26->setObjectName(QStringLiteral("pushButton_26"));
        pushButton_26->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_26, 4, 3, 1, 1);

        pushButton_30 = new QPushButton(widget);
        pushButton_30->setObjectName(QStringLiteral("pushButton_30"));
        pushButton_30->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_30, 4, 4, 1, 1);

        pushButton_34 = new QPushButton(widget);
        pushButton_34->setObjectName(QStringLiteral("pushButton_34"));
        pushButton_34->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_34, 4, 5, 1, 1);

        pushButton_38 = new QPushButton(widget);
        pushButton_38->setObjectName(QStringLiteral("pushButton_38"));
        pushButton_38->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_38, 4, 6, 1, 1);

        pushButton_40 = new QPushButton(widget);
        pushButton_40->setObjectName(QStringLiteral("pushButton_40"));
        pushButton_40->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_40, 4, 7, 1, 1);

        pushButton_41 = new QPushButton(widget);
        pushButton_41->setObjectName(QStringLiteral("pushButton_41"));
        pushButton_41->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_41, 4, 8, 1, 1);

        pushButton_42 = new QPushButton(widget);
        pushButton_42->setObjectName(QStringLiteral("pushButton_42"));
        pushButton_42->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_42, 4, 9, 1, 1);

        pushButton_43 = new QPushButton(widget);
        pushButton_43->setObjectName(QStringLiteral("pushButton_43"));
        pushButton_43->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_43, 4, 10, 1, 1);

        pushButton_63 = new QPushButton(widget);
        pushButton_63->setObjectName(QStringLiteral("pushButton_63"));
        pushButton_63->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_63, 4, 11, 1, 1);

        pushButton_8 = new QPushButton(widget);
        pushButton_8->setObjectName(QStringLiteral("pushButton_8"));
        pushButton_8->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_8, 5, 0, 1, 1);

        pushButton_19 = new QPushButton(widget);
        pushButton_19->setObjectName(QStringLiteral("pushButton_19"));
        pushButton_19->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_19, 5, 1, 1, 1);

        pushButton_23 = new QPushButton(widget);
        pushButton_23->setObjectName(QStringLiteral("pushButton_23"));
        pushButton_23->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_23, 5, 2, 1, 1);

        pushButton_27 = new QPushButton(widget);
        pushButton_27->setObjectName(QStringLiteral("pushButton_27"));
        pushButton_27->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_27, 5, 3, 1, 1);

        pushButton_31 = new QPushButton(widget);
        pushButton_31->setObjectName(QStringLiteral("pushButton_31"));
        pushButton_31->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_31, 5, 4, 1, 1);

        pushButton_35 = new QPushButton(widget);
        pushButton_35->setObjectName(QStringLiteral("pushButton_35"));
        pushButton_35->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_35, 5, 5, 1, 1);

        pushButton_44 = new QPushButton(widget);
        pushButton_44->setObjectName(QStringLiteral("pushButton_44"));
        pushButton_44->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_44, 5, 6, 1, 1);

        pushButton_51 = new QPushButton(widget);
        pushButton_51->setObjectName(QStringLiteral("pushButton_51"));
        pushButton_51->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_51, 5, 7, 1, 1);

        pushButton_54 = new QPushButton(widget);
        pushButton_54->setObjectName(QStringLiteral("pushButton_54"));
        pushButton_54->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_54, 5, 8, 1, 1);

        pushButton_57 = new QPushButton(widget);
        pushButton_57->setObjectName(QStringLiteral("pushButton_57"));
        pushButton_57->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_57, 5, 9, 1, 1);

        pushButton_58 = new QPushButton(widget);
        pushButton_58->setObjectName(QStringLiteral("pushButton_58"));
        pushButton_58->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_58, 5, 10, 1, 1);

        pushButton_67 = new QPushButton(widget);
        pushButton_67->setObjectName(QStringLiteral("pushButton_67"));
        pushButton_67->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_67, 5, 11, 1, 1);

        pushButton_9 = new QPushButton(widget);
        pushButton_9->setObjectName(QStringLiteral("pushButton_9"));
        pushButton_9->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_9, 6, 0, 1, 1);

        pushButton_20 = new QPushButton(widget);
        pushButton_20->setObjectName(QStringLiteral("pushButton_20"));
        pushButton_20->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_20, 6, 1, 1, 1);

        pushButton_24 = new QPushButton(widget);
        pushButton_24->setObjectName(QStringLiteral("pushButton_24"));
        pushButton_24->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_24, 6, 2, 1, 1);

        pushButton_28 = new QPushButton(widget);
        pushButton_28->setObjectName(QStringLiteral("pushButton_28"));
        pushButton_28->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_28, 6, 3, 1, 1);

        pushButton_32 = new QPushButton(widget);
        pushButton_32->setObjectName(QStringLiteral("pushButton_32"));
        pushButton_32->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_32, 6, 4, 1, 1);

        pushButton_36 = new QPushButton(widget);
        pushButton_36->setObjectName(QStringLiteral("pushButton_36"));
        pushButton_36->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_36, 6, 5, 1, 1);

        pushButton_45 = new QPushButton(widget);
        pushButton_45->setObjectName(QStringLiteral("pushButton_45"));
        pushButton_45->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_45, 6, 6, 1, 1);

        pushButton_52 = new QPushButton(widget);
        pushButton_52->setObjectName(QStringLiteral("pushButton_52"));
        pushButton_52->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_52, 6, 7, 1, 1);

        pushButton_55 = new QPushButton(widget);
        pushButton_55->setObjectName(QStringLiteral("pushButton_55"));
        pushButton_55->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_55, 6, 8, 1, 1);

        pushButton_59 = new QPushButton(widget);
        pushButton_59->setObjectName(QStringLiteral("pushButton_59"));
        pushButton_59->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_59, 6, 9, 1, 1);

        pushButton_10 = new QPushButton(widget);
        pushButton_10->setObjectName(QStringLiteral("pushButton_10"));
        pushButton_10->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_10, 6, 10, 1, 1);

        pushButton_39 = new QPushButton(widget);
        pushButton_39->setObjectName(QStringLiteral("pushButton_39"));
        pushButton_39->setMaximumSize(QSize(50, 50));

        keyboard->addWidget(pushButton_39, 6, 11, 1, 1);

        pushButton_66 = new QPushButton(widget);
        pushButton_66->setObjectName(QStringLiteral("pushButton_66"));
        pushButton_66->setMaximumSize(QSize(87, 50));

        keyboard->addWidget(pushButton_66, 6, 12, 1, 1);

        pushButton_65 = new QPushButton(widget);
        pushButton_65->setObjectName(QStringLiteral("pushButton_65"));
        pushButton_65->setMaximumSize(QSize(87, 50));

        keyboard->addWidget(pushButton_65, 7, 0, 1, 2);

        pushButton_68 = new QPushButton(widget);
        pushButton_68->setObjectName(QStringLiteral("pushButton_68"));
        pushButton_68->setMaximumSize(QSize(10000, 50));

        keyboard->addWidget(pushButton_68, 7, 2, 1, 9);

        pushButton_64 = new QPushButton(widget);
        pushButton_64->setObjectName(QStringLiteral("pushButton_64"));
        pushButton_64->setMaximumSize(QSize(87, 50));

        keyboard->addWidget(pushButton_64, 7, 12, 1, 1);


        retranslateUi(keyboardWidget);

        QMetaObject::connectSlotsByName(keyboardWidget);
    } // setupUi

    void retranslateUi(QWidget *keyboardWidget)
    {
        keyboardWidget->setWindowTitle(QApplication::translate("keyboardWidget", "Form", Q_NULLPTR));
        wifiPassLabel->setText(QString());
        pushButton_21->setText(QApplication::translate("keyboardWidget", "!", Q_NULLPTR));
        pushButton_25->setText(QApplication::translate("keyboardWidget", "@", Q_NULLPTR));
        pushButton_29->setText(QApplication::translate("keyboardWidget", "#", Q_NULLPTR));
        pushButton_33->setText(QApplication::translate("keyboardWidget", "$", Q_NULLPTR));
        pushButton_37->setText(QApplication::translate("keyboardWidget", "%", Q_NULLPTR));
        pushButton_47->setText(QApplication::translate("keyboardWidget", "^", Q_NULLPTR));
        pushButton_48->setText(QApplication::translate("keyboardWidget", "&&", Q_NULLPTR));
        pushButton_50->setText(QApplication::translate("keyboardWidget", "*", Q_NULLPTR));
        pushButton_53->setText(QApplication::translate("keyboardWidget", "(", Q_NULLPTR));
        pushButton_56->setText(QApplication::translate("keyboardWidget", ")", Q_NULLPTR));
        pushButton_61->setText(QApplication::translate("keyboardWidget", "_", Q_NULLPTR));
        pushButton_62->setText(QApplication::translate("keyboardWidget", "=", Q_NULLPTR));
        pushButton->setText(QApplication::translate("keyboardWidget", "Backspace", Q_NULLPTR));
        pushButton_6->setText(QApplication::translate("keyboardWidget", "1", Q_NULLPTR));
        pushButton_17->setText(QApplication::translate("keyboardWidget", "2", Q_NULLPTR));
        pushButton_16->setText(QApplication::translate("keyboardWidget", "3", Q_NULLPTR));
        pushButton_15->setText(QApplication::translate("keyboardWidget", "4", Q_NULLPTR));
        pushButton_14->setText(QApplication::translate("keyboardWidget", "5", Q_NULLPTR));
        pushButton_13->setText(QApplication::translate("keyboardWidget", "6", Q_NULLPTR));
        pushButton_12->setText(QApplication::translate("keyboardWidget", "7", Q_NULLPTR));
        pushButton_11->setText(QApplication::translate("keyboardWidget", "8", Q_NULLPTR));
        pushButton_3->setText(QApplication::translate("keyboardWidget", "9", Q_NULLPTR));
        pushButton_2->setText(QApplication::translate("keyboardWidget", "0", Q_NULLPTR));
        pushButton_4->setText(QApplication::translate("keyboardWidget", "-", Q_NULLPTR));
        pushButton_5->setText(QApplication::translate("keyboardWidget", "+", Q_NULLPTR));
        pushButton_60->setText(QApplication::translate("keyboardWidget", "Clear", Q_NULLPTR));
        pushButton_7->setText(QApplication::translate("keyboardWidget", "Q", Q_NULLPTR));
        pushButton_18->setText(QApplication::translate("keyboardWidget", "W", Q_NULLPTR));
        pushButton_22->setText(QApplication::translate("keyboardWidget", "E", Q_NULLPTR));
        pushButton_26->setText(QApplication::translate("keyboardWidget", "R", Q_NULLPTR));
        pushButton_30->setText(QApplication::translate("keyboardWidget", "T", Q_NULLPTR));
        pushButton_34->setText(QApplication::translate("keyboardWidget", "Y", Q_NULLPTR));
        pushButton_38->setText(QApplication::translate("keyboardWidget", "U", Q_NULLPTR));
        pushButton_40->setText(QApplication::translate("keyboardWidget", "I", Q_NULLPTR));
        pushButton_41->setText(QApplication::translate("keyboardWidget", "O", Q_NULLPTR));
        pushButton_42->setText(QApplication::translate("keyboardWidget", "P", Q_NULLPTR));
        pushButton_43->setText(QApplication::translate("keyboardWidget", "{", Q_NULLPTR));
        pushButton_63->setText(QApplication::translate("keyboardWidget", "}", Q_NULLPTR));
        pushButton_8->setText(QApplication::translate("keyboardWidget", "A", Q_NULLPTR));
        pushButton_19->setText(QApplication::translate("keyboardWidget", "S", Q_NULLPTR));
        pushButton_23->setText(QApplication::translate("keyboardWidget", "D", Q_NULLPTR));
        pushButton_27->setText(QApplication::translate("keyboardWidget", "F", Q_NULLPTR));
        pushButton_31->setText(QApplication::translate("keyboardWidget", "G", Q_NULLPTR));
        pushButton_35->setText(QApplication::translate("keyboardWidget", "H", Q_NULLPTR));
        pushButton_44->setText(QApplication::translate("keyboardWidget", "J", Q_NULLPTR));
        pushButton_51->setText(QApplication::translate("keyboardWidget", "K", Q_NULLPTR));
        pushButton_54->setText(QApplication::translate("keyboardWidget", "L", Q_NULLPTR));
        pushButton_57->setText(QApplication::translate("keyboardWidget", ":", Q_NULLPTR));
        pushButton_58->setText(QApplication::translate("keyboardWidget", "\"", Q_NULLPTR));
        pushButton_67->setText(QApplication::translate("keyboardWidget", "|", Q_NULLPTR));
        pushButton_9->setText(QApplication::translate("keyboardWidget", "Z", Q_NULLPTR));
        pushButton_20->setText(QApplication::translate("keyboardWidget", "X", Q_NULLPTR));
        pushButton_24->setText(QApplication::translate("keyboardWidget", "C", Q_NULLPTR));
        pushButton_28->setText(QApplication::translate("keyboardWidget", "V", Q_NULLPTR));
        pushButton_32->setText(QApplication::translate("keyboardWidget", "B", Q_NULLPTR));
        pushButton_36->setText(QApplication::translate("keyboardWidget", "N", Q_NULLPTR));
        pushButton_45->setText(QApplication::translate("keyboardWidget", "M", Q_NULLPTR));
        pushButton_52->setText(QApplication::translate("keyboardWidget", ",", Q_NULLPTR));
        pushButton_55->setText(QApplication::translate("keyboardWidget", ".", Q_NULLPTR));
        pushButton_59->setText(QApplication::translate("keyboardWidget", "?", Q_NULLPTR));
        pushButton_10->setText(QApplication::translate("keyboardWidget", "`", Q_NULLPTR));
        pushButton_39->setText(QApplication::translate("keyboardWidget", "~", Q_NULLPTR));
        pushButton_66->setText(QApplication::translate("keyboardWidget", "CAPS", Q_NULLPTR));
        pushButton_65->setText(QApplication::translate("keyboardWidget", "Cancel", Q_NULLPTR));
        pushButton_68->setText(QApplication::translate("keyboardWidget", "Space", Q_NULLPTR));
        pushButton_64->setText(QApplication::translate("keyboardWidget", "Done", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class keyboardWidget: public Ui_keyboardWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KEYBOARD_H
