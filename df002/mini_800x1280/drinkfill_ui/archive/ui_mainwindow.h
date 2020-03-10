/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QFormLayout *formLayout;
    QSpacerItem *horizontalSpacer_4;
    QStackedWidget *pages;
    QWidget *main;
    QLabel *companyName;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *productToDispense;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout;
    QToolButton *sparklingwater;
    QToolButton *kombucha;
    QToolButton *coffee;
    QWidget *selected;
    QPushButton *back;
    QLabel *label_2;
    QLabel *productImage;
    QGroupBox *buttonGroup_2;
    QPushButton *pushButton_1;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QWidget *dispense;
    QProgressBar *progressBar;
    QWidget *thanks;
    QLabel *thankstext;
    QWidget *error;
    QTextEdit *textEdit;
    QButtonGroup *buttonGroup;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->setWindowModality(Qt::NonModal);
        MainWindow->resize(1920, 1080);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(1920, 1080));
        MainWindow->setCursor(QCursor(Qt::PointingHandCursor));
        MainWindow->setContextMenuPolicy(Qt::NoContextMenu);
        MainWindow->setWindowTitle(QStringLiteral("MainWindow"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->setAutoFillBackground(false);
        formLayout = new QFormLayout(centralWidget);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        horizontalSpacer_4 = new QSpacerItem(10, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        formLayout->setItem(0, QFormLayout::LabelRole, horizontalSpacer_4);

        pages = new QStackedWidget(centralWidget);
        pages->setObjectName(QStringLiteral("pages"));
        pages->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(190);
        sizePolicy1.setVerticalStretch(110);
        sizePolicy1.setHeightForWidth(pages->sizePolicy().hasHeightForWidth());
        pages->setSizePolicy(sizePolicy1);
        pages->setMinimumSize(QSize(1920, 1080));
        pages->setMaximumSize(QSize(1920, 16777214));
        pages->setLayoutDirection(Qt::LeftToRight);
        pages->setStyleSheet(QStringLiteral("left-margin:0 px;"));
        pages->setLineWidth(0);
        main = new QWidget();
        main->setObjectName(QStringLiteral("main"));
        main->setMinimumSize(QSize(1920, 1080));
        companyName = new QLabel(main);
        companyName->setObjectName(QStringLiteral("companyName"));
        companyName->setGeometry(QRect(30, 30, 239, 77));
        companyName->setMaximumSize(QSize(250, 120));
        QFont font;
        font.setFamily(QStringLiteral("Ubuntu"));
        font.setPointSize(51);
        companyName->setFont(font);
        companyName->setToolTipDuration(-1);
        companyName->setLineWidth(0);
        companyName->setAlignment(Qt::AlignCenter);
        companyName->setIndent(0);
        layoutWidget = new QWidget(main);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(-9, 150, 1921, 122));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        productToDispense = new QLabel(layoutWidget);
        productToDispense->setObjectName(QStringLiteral("productToDispense"));
        productToDispense->setEnabled(true);

        horizontalLayout_2->addWidget(productToDispense);

        horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        label = new QLabel(main);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(1770, 1010, 121, 51));
        QFont font1;
        font1.setPointSize(15);
        label->setFont(font1);
        layoutWidget1 = new QWidget(main);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(-2, 111, 1941, 871));
        horizontalLayout = new QHBoxLayout(layoutWidget1);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        sparklingwater = new QToolButton(layoutWidget1);
        sparklingwater->setObjectName(QStringLiteral("sparklingwater"));
        sizePolicy.setHeightForWidth(sparklingwater->sizePolicy().hasHeightForWidth());
        sparklingwater->setSizePolicy(sizePolicy);
        sparklingwater->setMinimumSize(QSize(550, 720));
        sparklingwater->setMaximumSize(QSize(550, 720));
        QFont font2;
        font2.setPointSize(25);
        font2.setBold(false);
        font2.setUnderline(false);
        font2.setWeight(50);
        font2.setKerning(true);
        font2.setStyleStrategy(QFont::PreferAntialias);
        sparklingwater->setFont(font2);
        sparklingwater->setAutoFillBackground(false);
        sparklingwater->setStyleSheet(QStringLiteral(""));
        QIcon icon;
        icon.addFile(QStringLiteral("images/page0/carbonated-button.png"), QSize(), QIcon::Normal, QIcon::Off);
        sparklingwater->setIcon(icon);
        sparklingwater->setIconSize(QSize(550, 720));
        sparklingwater->setToolButtonStyle(Qt::ToolButtonIconOnly);
        sparklingwater->setAutoRaise(false);

        horizontalLayout->addWidget(sparklingwater);

        kombucha = new QToolButton(layoutWidget1);
        kombucha->setObjectName(QStringLiteral("kombucha"));
        sizePolicy.setHeightForWidth(kombucha->sizePolicy().hasHeightForWidth());
        kombucha->setSizePolicy(sizePolicy);
        kombucha->setMinimumSize(QSize(550, 720));
        kombucha->setMaximumSize(QSize(550, 720));
        QFont font3;
        font3.setPointSize(25);
        kombucha->setFont(font3);
        QIcon icon1;
        icon1.addFile(QStringLiteral("images/page0/kombucha-button.png"), QSize(), QIcon::Normal, QIcon::Off);
        kombucha->setIcon(icon1);
        kombucha->setIconSize(QSize(550, 720));
        kombucha->setToolButtonStyle(Qt::ToolButtonIconOnly);

        horizontalLayout->addWidget(kombucha);

        coffee = new QToolButton(layoutWidget1);
        coffee->setObjectName(QStringLiteral("coffee"));
        sizePolicy.setHeightForWidth(coffee->sizePolicy().hasHeightForWidth());
        coffee->setSizePolicy(sizePolicy);
        coffee->setMinimumSize(QSize(550, 720));
        coffee->setMaximumSize(QSize(550, 720));
        coffee->setFont(font3);
        coffee->setAutoFillBackground(false);
        coffee->setStyleSheet(QStringLiteral(""));
        QIcon icon2;
        icon2.addFile(QStringLiteral("images/page0/Coldbrew-button.png"), QSize(), QIcon::Normal, QIcon::Off);
        coffee->setIcon(icon2);
        coffee->setIconSize(QSize(550, 720));
        coffee->setToolButtonStyle(Qt::ToolButtonIconOnly);

        horizontalLayout->addWidget(coffee);

        pages->addWidget(main);
        selected = new QWidget();
        selected->setObjectName(QStringLiteral("selected"));
        selected->setMinimumSize(QSize(1900, 1080));
        back = new QPushButton(selected);
        back->setObjectName(QStringLiteral("back"));
        back->setGeometry(QRect(12, 12, 80, 50));
        back->setMinimumSize(QSize(50, 50));
        label_2 = new QLabel(selected);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(12, 796, 67, 17));
        productImage = new QLabel(selected);
        productImage->setObjectName(QStringLiteral("productImage"));
        productImage->setGeometry(QRect(20, 120, 520, 720));
        sizePolicy.setHeightForWidth(productImage->sizePolicy().hasHeightForWidth());
        productImage->setSizePolicy(sizePolicy);
        productImage->setMinimumSize(QSize(350, 712));
        productImage->setMaximumSize(QSize(520, 720));
        QFont font4;
        font4.setPointSize(1);
        productImage->setFont(font4);
        productImage->setTextFormat(Qt::RichText);
        productImage->setScaledContents(true);
        productImage->setTextInteractionFlags(Qt::NoTextInteraction);
        buttonGroup_2 = new QGroupBox(selected);
        buttonGroup_2->setObjectName(QStringLiteral("buttonGroup_2"));
        buttonGroup_2->setGeometry(QRect(740, 170, 901, 311));
        buttonGroup_2->setAutoFillBackground(true);
        pushButton_1 = new QPushButton(buttonGroup_2);
        buttonGroup = new QButtonGroup(MainWindow);
        buttonGroup->setObjectName(QStringLiteral("buttonGroup"));
        buttonGroup->addButton(pushButton_1);
        pushButton_1->setObjectName(QStringLiteral("pushButton_1"));
        pushButton_1->setGeometry(QRect(50, 30, 250, 250));
        pushButton_1->setMinimumSize(QSize(250, 250));
        pushButton_1->setFont(font3);
        pushButton_1->setStyleSheet(QLatin1String("QPushButton {\n"
"    color: #333;\n"
"    border: 0px solid #555;\n"
"    border-radius: 125px;\n"
"    border-style: outset;\n"
"    background: white;\n"
"    padding: 5px;\n"
"    }\n"
""));
        pushButton_2 = new QPushButton(buttonGroup_2);
        buttonGroup->addButton(pushButton_2);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(340, 30, 250, 250));
        pushButton_2->setMinimumSize(QSize(250, 250));
        pushButton_2->setFont(font3);
        pushButton_2->setStyleSheet(QLatin1String("QPushButton {\n"
"    color: #333;\n"
"    border: 0px solid #555;\n"
"    border-radius: 125px;\n"
"    border-style: outset;\n"
"    background: white;\n"
"    padding: 5px;\n"
"    }\n"
""));
        pushButton_3 = new QPushButton(buttonGroup_2);
        buttonGroup->addButton(pushButton_3);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setGeometry(QRect(630, 30, 250, 250));
        pushButton_3->setMinimumSize(QSize(250, 250));
        pushButton_3->setFont(font3);
        pushButton_3->setStyleSheet(QLatin1String("QPushButton {\n"
"    color: #333;\n"
"    border: 0px solid #555;\n"
"    border-radius: 125px;\n"
"    border-style: outset;\n"
"    background: white;\n"
"    padding: 5px;\n"
"    }\n"
""));
        pages->addWidget(selected);
        productImage->raise();
        back->raise();
        label_2->raise();
        buttonGroup_2->raise();
        dispense = new QWidget();
        dispense->setObjectName(QStringLiteral("dispense"));
        dispense->setMinimumSize(QSize(1900, 1080));
        progressBar = new QProgressBar(dispense);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setGeometry(QRect(670, 320, 931, 201));
        progressBar->setLayoutDirection(Qt::LeftToRight);
        progressBar->setMaximum(100);
        progressBar->setValue(100);
        progressBar->setOrientation(Qt::Horizontal);
        pages->addWidget(dispense);
        thanks = new QWidget();
        thanks->setObjectName(QStringLiteral("thanks"));
        thanks->setMinimumSize(QSize(1900, 1080));
        thankstext = new QLabel(thanks);
        thankstext->setObjectName(QStringLiteral("thankstext"));
        thankstext->setGeometry(QRect(600, 340, 591, 101));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(thankstext->sizePolicy().hasHeightForWidth());
        thankstext->setSizePolicy(sizePolicy2);
        QFont font5;
        font5.setPointSize(30);
        thankstext->setFont(font5);
        thankstext->setAlignment(Qt::AlignCenter);
        pages->addWidget(thanks);
        error = new QWidget();
        error->setObjectName(QStringLiteral("error"));
        error->setMinimumSize(QSize(1900, 1080));
        textEdit = new QTextEdit(error);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(120, 160, 621, 231));
        QFont font6;
        font6.setPointSize(20);
        textEdit->setFont(font6);
        textEdit->setStyleSheet(QLatin1String("background: transparent;\n"
""));
        pages->addWidget(error);

        formLayout->setWidget(0, QFormLayout::FieldRole, pages);

        MainWindow->setCentralWidget(centralWidget);
        QWidget::setTabOrder(coffee, kombucha);
        QWidget::setTabOrder(kombucha, textEdit);
        QWidget::setTabOrder(textEdit, back);

        retranslateUi(MainWindow);

        pages->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        companyName->setText(QApplication::translate("MainWindow", "drinkfill", nullptr));
        productToDispense->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        label->setText(QApplication::translate("MainWindow", "CONTACT US", nullptr));
        sparklingwater->setText(QApplication::translate("MainWindow", "WATER", nullptr));
        kombucha->setText(QApplication::translate("MainWindow", "KOMBUCHA", nullptr));
        coffee->setText(QApplication::translate("MainWindow", "COLD COFFEE", nullptr));
        back->setText(QApplication::translate("MainWindow", "back", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        productImage->setText(QApplication::translate("MainWindow", "<html><head/><body><p><img src=\":/images/page1/carbonated.png\"/></p></body></html>", nullptr));
        buttonGroup_2->setTitle(QString());
        pushButton_1->setText(QApplication::translate("MainWindow", "16oz\n"
"473ml", nullptr));
        pushButton_2->setText(QApplication::translate("MainWindow", "24oz\n"
"709ml", nullptr));
        pushButton_3->setText(QApplication::translate("MainWindow", "32oz\n"
"946ml", nullptr));
        progressBar->setFormat(QApplication::translate("MainWindow", " %v ml", nullptr));
        thankstext->setText(QApplication::translate("MainWindow", "Thank you for using Drinkfill!", nullptr));
        textEdit->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:20pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:24pt;\">There has been an error. Sorry for the inconvenience, please wait for someone to come service this machine.</span></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:24pt;\"><br /></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:24pt;\">Thank you for your patience!</sp"
                        "an></p></body></html>", nullptr));
        Q_UNUSED(MainWindow);
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
