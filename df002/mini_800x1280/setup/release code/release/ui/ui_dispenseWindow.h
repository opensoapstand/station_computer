/********************************************************************************
** Form generated from reading UI file 'dispenseWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISPENSEWINDOW_H
#define UI_DISPENSEWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DispenseWindow
{
public:
    QLabel *fillLabel;
    QLabel *fillLabelArrow;
    QLabel *bottleLabel;
    QLabel *bottleLabelArrow;
    QLabel *pourLabel;
    QProgressBar *progressBar;
    QPushButton *pushButton;
    QLabel *maskLabel;

    void setupUi(QWidget *DispenseWindow)
    {
        if (DispenseWindow->objectName().isEmpty())
            DispenseWindow->setObjectName(QString::fromUtf8("DispenseWindow"));
        DispenseWindow->resize(800, 1280);
        DispenseWindow->setStyleSheet(QString::fromUtf8(""));
        fillLabel = new QLabel(DispenseWindow);
        fillLabel->setObjectName(QString::fromUtf8("fillLabel"));
        fillLabel->setGeometry(QRect(0, 640, 800, 121));
        fillLabelArrow = new QLabel(DispenseWindow);
        fillLabelArrow->setObjectName(QString::fromUtf8("fillLabelArrow"));
        fillLabelArrow->setGeometry(QRect(350, 920, 200, 100));
        fillLabelArrow->setStyleSheet(QString::fromUtf8("image: url(:/new/prefix/ASSETS/up_arrow.png);"));
        fillLabelArrow->setPixmap(QPixmap(QString::fromUtf8(":/assets/up_arrow.png")));
        bottleLabel = new QLabel(DispenseWindow);
        bottleLabel->setObjectName(QString::fromUtf8("bottleLabel"));
        bottleLabel->setGeometry(QRect(0, 800, 800, 120));
        bottleLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(238, 238, 236);"));
        bottleLabelArrow = new QLabel(DispenseWindow);
        bottleLabelArrow->setObjectName(QString::fromUtf8("bottleLabelArrow"));
        bottleLabelArrow->setGeometry(QRect(840, 1190, 200, 100));
        bottleLabelArrow->setStyleSheet(QString::fromUtf8("\n"
"image: url(:/new/prefix/ASSETS/side_arrow.png);"));
        bottleLabelArrow->setPixmap(QPixmap(QString::fromUtf8(":/assets/side_arrow.png")));
        pourLabel = new QLabel(DispenseWindow);
        pourLabel->setObjectName(QString::fromUtf8("pourLabel"));
        pourLabel->setEnabled(true);
        pourLabel->setGeometry(QRect(0, 50, 800, 231));
        progressBar = new QProgressBar(DispenseWindow);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setEnabled(true);
        progressBar->setGeometry(QRect(0, 929, 800, 221));
        QFont font;
        font.setPointSize(30);
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(50);
        font.setKerning(false);
        progressBar->setFont(font);
        progressBar->setTabletTracking(false);
        progressBar->setFocusPolicy(Qt::NoFocus);
        progressBar->setContextMenuPolicy(Qt::PreventContextMenu);
        progressBar->setInputMethodHints(Qt::ImhNone);
        progressBar->setMaximum(500);
        progressBar->setValue(0);
        progressBar->setTextVisible(false);
        progressBar->setOrientation(Qt::Vertical);
        progressBar->setInvertedAppearance(false);
        progressBar->setTextDirection(QProgressBar::BottomToTop);
        pushButton = new QPushButton(DispenseWindow);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(570, 1210, 200, 40));
        pushButton->setStyleSheet(QString::fromUtf8("font: 15pt \"Ubuntu\";\n"
"color: rgb(255, 255, 255);\n"
"border-image: url(:/assets/V5/rect_button.png);"));
        maskLabel = new QLabel(DispenseWindow);
        maskLabel->setObjectName(QString::fromUtf8("maskLabel"));
        maskLabel->setGeometry(QRect(0, 0, 800, 1280));
        maskLabel->setPixmap(QPixmap(QString::fromUtf8(":/assets/V6/progressbarmask_800x1280.png")));
        progressBar->raise();
        maskLabel->raise();
        fillLabel->raise();
        bottleLabel->raise();
        pourLabel->raise();
        pushButton->raise();
        fillLabelArrow->raise();
        bottleLabelArrow->raise();

        retranslateUi(DispenseWindow);

        QMetaObject::connectSlotsByName(DispenseWindow);
    } // setupUi

    void retranslateUi(QWidget *DispenseWindow)
    {
        DispenseWindow->setWindowTitle(QCoreApplication::translate("DispenseWindow", "Form", nullptr));
        fillLabel->setText(QCoreApplication::translate("DispenseWindow", "<html><head/><body><p align=\"center\"><span style=\" font-size:40pt; color:#ffffff;\">PULL TO FILL</span></p></body></html>", nullptr));
        fillLabelArrow->setText(QString());
        bottleLabel->setText(QCoreApplication::translate("DispenseWindow", "<html><head/><body><p align=\"center\"><span style=\" font-size:40pt; color:#ffffff;\">PLACE BOTTLE BELOW</span></p></body></html>", nullptr));
        bottleLabelArrow->setText(QString());
        pourLabel->setText(QCoreApplication::translate("DispenseWindow", "<html><head/><body><p align=\"center\"><span style=\" font-size:72pt; color:#ffffff;\">POURING</span></p><p align=\"center\"><span style=\" font-size:72pt; color:#ffffff;\">TIME!</span></p></body></html>", nullptr));
        progressBar->setFormat(QCoreApplication::translate("DispenseWindow", "%vml", nullptr));
        pushButton->setText(QCoreApplication::translate("DispenseWindow", "DONE", nullptr));
        maskLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class DispenseWindow: public Ui_DispenseWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISPENSEWINDOW_H
