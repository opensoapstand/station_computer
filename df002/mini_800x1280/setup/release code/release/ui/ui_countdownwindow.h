/********************************************************************************
** Form generated from reading UI file 'countdownwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COUNTDOWNWINDOW_H
#define UI_COUNTDOWNWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_countdownWindow
{
public:
    QLabel *bottleLabelArrow;
    QLabel *pourLabel;
    QLabel *fillLabel;
    QProgressBar *progressBar;
    QLabel *fillLabelArrow;
    QLabel *bottleLabel;
    QPushButton *pushButton;
    QLabel *timeLabel;
    QLabel *maskLabel;

    void setupUi(QWidget *countdownWindow)
    {
        if (countdownWindow->objectName().isEmpty())
            countdownWindow->setObjectName(QString::fromUtf8("countdownWindow"));
        countdownWindow->resize(800, 1280);
        countdownWindow->setStyleSheet(QString::fromUtf8(""));
        bottleLabelArrow = new QLabel(countdownWindow);
        bottleLabelArrow->setObjectName(QString::fromUtf8("bottleLabelArrow"));
        bottleLabelArrow->setGeometry(QRect(840, 1190, 200, 100));
        bottleLabelArrow->setStyleSheet(QString::fromUtf8("\n"
"image: url(:/new/prefix/ASSETS/side_arrow.png);"));
        bottleLabelArrow->setPixmap(QPixmap(QString::fromUtf8(":/assets/side_arrow.png")));
        pourLabel = new QLabel(countdownWindow);
        pourLabel->setObjectName(QString::fromUtf8("pourLabel"));
        pourLabel->setGeometry(QRect(50, 110, 700, 230));
        fillLabel = new QLabel(countdownWindow);
        fillLabel->setObjectName(QString::fromUtf8("fillLabel"));
        fillLabel->setGeometry(QRect(0, 400, 800, 121));
        progressBar = new QProgressBar(countdownWindow);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(0, 929, 800, 221));
        QFont font;
        font.setPointSize(30);
        font.setUnderline(false);
        font.setStrikeOut(false);
        font.setKerning(false);
        progressBar->setFont(font);
        progressBar->setStyleSheet(QString::fromUtf8(""));
        progressBar->setMaximum(500);
        progressBar->setValue(0);
        progressBar->setAlignment(Qt::AlignCenter);
        progressBar->setTextVisible(false);
        progressBar->setOrientation(Qt::Vertical);
        progressBar->setInvertedAppearance(false);
        fillLabelArrow = new QLabel(countdownWindow);
        fillLabelArrow->setObjectName(QString::fromUtf8("fillLabelArrow"));
        fillLabelArrow->setGeometry(QRect(860, 650, 200, 100));
        fillLabelArrow->setStyleSheet(QString::fromUtf8("\n"
"image: url(:/new/prefix/ASSETS/up_arrow.png);"));
        fillLabelArrow->setPixmap(QPixmap(QString::fromUtf8(":/assets/up_arrow.png")));
        bottleLabel = new QLabel(countdownWindow);
        bottleLabel->setObjectName(QString::fromUtf8("bottleLabel"));
        bottleLabel->setGeometry(QRect(0, 600, 800, 120));
        bottleLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(238, 238, 236);"));
        pushButton = new QPushButton(countdownWindow);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(570, 1210, 200, 40));
        pushButton->setStyleSheet(QString::fromUtf8("font: 15pt \"Ubuntu\";\n"
"color: rgb(255, 255, 255);\n"
"border-image: url(:/assets/V5/rect_button.png);"));
        timeLabel = new QLabel(countdownWindow);
        timeLabel->setObjectName(QString::fromUtf8("timeLabel"));
        timeLabel->setGeometry(QRect(50, 260, 700, 81));
        QFont font1;
        font1.setPointSize(25);
        timeLabel->setFont(font1);
        maskLabel = new QLabel(countdownWindow);
        maskLabel->setObjectName(QString::fromUtf8("maskLabel"));
        maskLabel->setGeometry(QRect(0, 0, 800, 1280));
        maskLabel->setPixmap(QPixmap(QString::fromUtf8(":/assets/V6/progressbarmask_800x1280.png")));
        progressBar->raise();
        maskLabel->raise();
        bottleLabelArrow->raise();
        pourLabel->raise();
        fillLabel->raise();
        fillLabelArrow->raise();
        bottleLabel->raise();
        pushButton->raise();
        timeLabel->raise();

        retranslateUi(countdownWindow);

        QMetaObject::connectSlotsByName(countdownWindow);
    } // setupUi

    void retranslateUi(QWidget *countdownWindow)
    {
        countdownWindow->setWindowTitle(QCoreApplication::translate("countdownWindow", "Form", nullptr));
        bottleLabelArrow->setText(QString());
        pourLabel->setText(QCoreApplication::translate("countdownWindow", "<html><head/><body><p align=\"center\"><span style=\" font-size:72pt; color:#ffffff;\">STILL THERE ?</span></p><p align=\"center\"><span style=\" font-size:36pt; color:#ffffff;\"><br/></span></p></body></html>", nullptr));
        fillLabel->setText(QCoreApplication::translate("countdownWindow", "<html><head/><body><p align=\"center\"><span style=\" font-size:40pt; color:#ffffff;\">PULL TO FILL</span></p></body></html>", nullptr));
        progressBar->setFormat(QCoreApplication::translate("countdownWindow", "%vml", nullptr));
        fillLabelArrow->setText(QString());
        bottleLabel->setText(QCoreApplication::translate("countdownWindow", "<html><head/><body><p align=\"center\"><span style=\" font-size:40pt; color:#ffffff;\">PLACE BOTTLE BELOW</span></p></body></html>", nullptr));
        pushButton->setText(QCoreApplication::translate("countdownWindow", "I'M DONE", nullptr));
        timeLabel->setText(QCoreApplication::translate("countdownWindow", "<html><head/><body><p align=\"center\"><span style=\" color:#2e3436;\">time display</span></p><p align=\"center\"><br/></p></body></html>", nullptr));
        maskLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class countdownWindow: public Ui_countdownWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COUNTDOWNWINDOW_H
