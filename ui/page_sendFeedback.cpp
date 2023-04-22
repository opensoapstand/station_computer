
//***************************************
//
// page_product.cpp
// GUI class for user to select size and
// payment for drink.
//
// Allows navigation to First product page
// payment page and page_idle page
//
// created: 05-04-2022
// by: Lode Ameije & Ash Singla & Udbhav Kansal
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "page_sendFeedback.h"
#include "ui_page_sendFeedback.h"
#include <iostream>
#include <string>
#include <cmath>

#include "page_qr_payment.h"
#include "page_select_product.h"
#include "page_idle.h"
#include <curl/curl.h>
#include <json.hpp>

using json = nlohmann::json;

// CTOR
page_sendFeedback::page_sendFeedback(QWidget *parent) : QWidget(parent),
                                                        ui(new Ui::page_sendFeedback)
{
    qDebug() << "IN send feedback";
    ui->setupUi(this);

    ui->feedback_Input_Button->setStyleSheet("QPushButton { border: 1px solid #5E8580}");
    ui->feedback_Input_Button->setText("Type Here");
    ui->feedbackText->setStyleSheet("QPushButton { background-color: transparent; border: 1px solid #5E8580 }");   // ui->page_qr_payment_Button->show();
    ui->feedbackKeyboard->hide();
    ui->label_thanks_for_feedback->hide();

    // ui->feedback_Input_Button->hide();
    ui->label_enter_feedback->show();
    // ui->label_type_here->hide();
    ui->previousPage_Button->setStyleSheet("QPushButton { color:#555555; background-color: transparent; border: 0px }");
    ui->previousPage_Button->setStyleSheet(
        "QPushButton {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 32px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "color: #003840;"
        "text-align: left;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");

    ui->previousPage_Button->setText("<-back");

    // ui->mainPage_Button->hide();
    ui->mainPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    ui->label_select_problem->setStyleSheet("QLabel { color:#555555; ; border: 1px }");
    ui->label_select_problem->setStyleSheet(
        "QLabel {"
        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 55px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "color: #003840;"
        "text-align: center;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");
    ui->label_select_problem->setText("Please provide feedback");
    ui->label_select_problem->setWordWrap(true);
    ui->label_problem_options->setStyleSheet("background-color: #5E8680; border-radius: 30px;");
    QString checkBoxLabelStyling = ("QPushButton {"
                                    "font-family: 'Brevia';"
                                    "font-style: normal;"
                                    "font-weight: 75;"
                                    "font-size: 32px;"
                                    "line-height: 99px;"
                                    "letter-spacing: 1.5px;"
                                    "color: #FFFFFF;"
                                    "text-align: left;"
                                    "qproperty-alignment: AlignCenter;"
                                    "border: none;"
                                    "wordWrap:true;"
                                    "}");

    ui->label_still_cant_find->setStyleSheet(
        "QLabel {"
        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 43px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "color: #438080;"
        "text-align: center;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");
    ui->label_still_cant_find->setText("Still can't find it?");

    ui->label_email->setStyleSheet(
        // "QLabel {"
        // "font-family: 'Montserrat';"
        // "font-style: normal;"
        // "font-weight: 75;"
        // "font-size: 35px;"
        // "line-height: 99px;"
        // "letter-spacing: 1.5px;"
        // "color: #024B51;"
        // "text-align: center;"
        // "qproperty-alignment: AlignCenter;"
        // "border: none;"
        // "wordWrap:true;"
        // "}");
         "QLabel {"
        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 43px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "color: #438080;"
        "text-align: center;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");
    ui->label_email->setText("Email us at: sales@soapstand.com");

    ui->checkBox_1_Label->setStyleSheet(checkBoxLabelStyling);
    ui->checkBox_1_Label->setText("The station never dispensed soap");

    ui->checkBox_2_Label->setStyleSheet(checkBoxLabelStyling);
    ui->checkBox_2_Label->setText("Wrong amount charged");

    ui->checkBox_3_Label->setStyleSheet(checkBoxLabelStyling);
    ui->checkBox_3_Label->setText("The station is too slow");

    ui->checkBox_4_Label->setStyleSheet(checkBoxLabelStyling);
    ui->checkBox_4_Label->setText("I got charged more than once");

    ui->checkBox_5_Label->setStyleSheet(checkBoxLabelStyling);
    ui->checkBox_5_Label->setText("Screen was frozen");

    ui->label_enter_feedback->setStyleSheet(checkBoxLabelStyling);
    ui->label_enter_feedback->setText("Enter other feedback below");

    ui->feedback_Input_Button->raise();
    ui->feedback_Input_Button->setStyleSheet("QPushButton { border: 1px solid #FFFFFF}");
    // ui->feedback_Input_Button->setText("Type here");
    // ui->label_type_here->show();
    // ui->label_type_here->setText("Type here");;

    // ui->promoKeyboard->hide();

    ui->feedbackText->clear();
    ui->feedbackText->show();
    ui->feedbackText->setEchoMode(QLineEdit::Normal);

    ui->label_thanks_for_feedback->setStyleSheet(
        "QLabel {"
        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 65px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "color: #003840;"
        "text-align: center;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");
    ui->label_thanks_for_feedback->setText("Thank you for <br> your feedback");

    // QString keyboard = KEYBOARD_IMAGE_PATH;
    // QString keyboard_style_sheet = " background-image: url(" + keyboard + "); }";
    // ui->feedbackKeyboard->setStyleSheet(keyboard_style_sheet);
    {
        selectIdleTimer = new QTimer(this);
        selectIdleTimer->setInterval(40);
        // connect(ui->promoButton, SIGNAL(clicked()), this, SLOT(on_applyPromo_Button_clicked()));
        // connect(ui->feedback_Input_Button, SIGNAL(clicked()), this, SLOT(on_feedback_Input_Button_clicked()));
        connect(ui->buttonGroup, SIGNAL(buttonPressed(int)), this, SLOT(keyboardButtonPressed(int)));
        connect(selectIdleTimer, SIGNAL(timeout()), this, SLOT(onSelectTimeoutTick()));
    }

    QSize size(30, 30);
    ui->checkBox_1->setIconSize(size);
    ui->checkBox_2->setIconSize(size);
    ui->checkBox_3->setIconSize(size);
    ui->checkBox_4->setIconSize(size);
    ui->checkBox_5->setIconSize(size);

    ui->label_send->setStyleSheet("QLabel {"
                                  "font-family: 'Brevia';"
                                  "font-style: normal;"
                                  "font-weight: 75;"
                                  "font-size: 35px;"
                                  "line-height: 99px;"
                                  "letter-spacing: 1.5px;"
                                  "background-color: #5E8680;"
                                  "color: #FFFFFF;"
                                  "text-align: center;"
                                  "qproperty-alignment: AlignCenter;"
                                  "border-radius: 30px;"
                                  "padding: 10px;"
                                  "border: 2px solid #004D54;"
                                  "}");

    ui->label_send->setText("SEND");
    ui->send_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    ui->label_send->setStyleSheet("QLabel {"
                                  "font-family: 'Brevia';"
                                  "font-style: normal;"
                                  "font-weight: 75;"
                                  "font-size: 35px;"
                                  "line-height: 99px;"
                                  "letter-spacing: 1.5px;"
                                  "background-color: #5E8680;"
                                  "color: #FFFFFF;"
                                  "text-align: center;"
                                  "qproperty-alignment: AlignCenter;"
                                  "border-radius: 30px;"
                                  "padding: 10px;"
                                  "border: 2px solid #004D54;"
                                  "}");

    ui->label_send->setText("SEND");
}

/*
 * Page Tracking reference to Select Drink, Payment Page and Idle page
 */
void page_sendFeedback::setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_help *pageHelp, pageProduct *page_product, pagethankyou *page_thankyou )
{
    this->p_page_select_product = pageSelect;
    this->paymentPage = page_qr_payment;
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->p_page_help = pageHelp;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_product = page_product;
    // ui->promoCode->clear();
    // ui->promoCode->hide();

    
    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_SEND_FEEDBACK_PATH);
    ui->send_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_SELECT_PRODUCT_BACKGROUND_PATH);
    QString full_path = p_page_idle->getTemplatePathFromName(IMAGE_BUTTON_HELP);
    p_page_idle->addPictureToLabel(ui->label_help, full_path);
    // ui->label_help->raise();
    // couponHandler();
}

// DTOR
page_sendFeedback::~page_sendFeedback()
{
    delete ui;
}

/* GUI */

void page_sendFeedback::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Send Feedback>>>>>>>>>";
    QWidget::showEvent(event);

    // selectedProductOrder->loadSelectedProductProperties();
    reset_and_show_page_elements();
}

void page_sendFeedback::resizeEvent(QResizeEvent *event)
{
    qDebug() << "\n---Page Send Feedback: resizeEvent";
    // QWidget::resizeEvent(event);
    // selectedProductOrder->loadSelectedProductProperties();
    // reset_and_show_page_elements();
}

void page_sendFeedback::onSelectTimeoutTick()
{
    if (--_selectIdleTimeoutSec >= 0)
    {
        //        qDebug() << "page_sendFeedback: Tick Down - " << _selectIdleTimeoutSec << endl;
    }
    else
    {
        // qDebug() << "Timer Done!" << _selectIdleTimeoutSec << endl;
        selectIdleTimer->stop();

        mainPage();
    }
}

void page_sendFeedback::reset_and_show_page_elements()
{
    ui->feedback_Input_Button->raise();
    ui->feedbackText->clear();
    ui->feedbackText->show();
    
    ui->checkBox_1->setCheckState(Qt::Unchecked);
    ui->checkBox_2->setCheckState(Qt::Unchecked);
    ui->checkBox_3->setCheckState(Qt::Unchecked);
    ui->checkBox_4->setCheckState(Qt::Unchecked);
    ui->checkBox_5->setCheckState(Qt::Unchecked);
}


void page_sendFeedback::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
     if (selectIdleTimer != nullptr)
    {
        selectIdleTimer->stop();
    }
    p_page_idle->pageTransition(this, pageToShow);
}

void page_sendFeedback::mainPage()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_sendFeedback::on_mainPage_Button_clicked()
{
    hideCurrentPageAndShowProvided(p_page_help);
}

size_t WriteCallbackFeedback(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void page_sendFeedback::on_send_Button_clicked()
{
    QVBoxLayout *layout = new QVBoxLayout();

    // revert
    QDialog *dialog = new QDialog(this);
    QTimer *timer = new QTimer(dialog);
    timer->setSingleShot(true);
    timer->start(3000);
    ui->feedbackText->clear();

    QObject::connect(timer, &QTimer::timeout, dialog, &QDialog::close);

    // to remove title bar
    dialog->setWindowFlags(Qt::CustomizeWindowHint);
    dialog->setLayout(layout);
    qDebug() << "Send button pressed";
    QStringList problemList;
    if (ui->checkBox_1->isChecked())
    {
        problemList << ui->checkBox_1_Label->text();
    }
    if (ui->checkBox_2->isChecked())
    {
        problemList << ui->checkBox_2_Label->text();
    }
    if (ui->checkBox_3->isChecked())
    {
        problemList << ui->checkBox_3_Label->text();
    }
    if (ui->checkBox_4->isChecked())
    {
        problemList << ui->checkBox_4_Label->text();
    }
    if (ui->checkBox_5->isChecked())
    {
        problemList << ui->checkBox_5_Label->text();
    }
    qDebug() << problemList;
    QString problems = problemList.join(",");
    if (problems.length() != 0 && ui->feedbackText->text().isEmpty())
    {
        QString MachineSerialNumber = p_page_idle->currentProductOrder->getMachineId();
        QString customFeedback = ui->feedbackText->text();
        QString curl_param = "problems=" + problems + " " + customFeedback + "&MachineSerialNumber=" + MachineSerialNumber;
        qDebug() << "Curl params" << curl_param << endl;
        curl_param_array = curl_param.toLocal8Bit();
        qDebug() << curl_param_array;
        curl = curl_easy_init();
        if (!curl)
        {
            qDebug() << "pagethankyou: cURL failed to init. parameters:" + curl_param;

            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/alert/sendFeedbackEmail");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curl_param_array.data());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackFeedback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            qDebug() << "ERROR: Transaction NOT sent to cloud. cURL fail. Error code: " + QString::number(res);
        }
        qDebug() << "Send button stores values" << endl;

        dialog->move(0, 100);
        dialog->resize(1080, 1920);
        dialog->setStyleSheet("background-image:  url(/home/df-admin/drinkfill/ui/references/templates/default/background_feedbacksent.png);");

        QLabel *label = new QLabel(dialog);
        label->setText("Thank you for <br>your feedback <br>");
        label->setStyleSheet("QLabel {"
                             "font-family: 'Brevia';"
                             "font-style: normal;"
                             "font-weight: 75;"
                             "font-size: 65px;"
                             "line-height: 99px;"
                             "letter-spacing: 1.5px;"
                             "color: #003840;"
                             "text-align: center;"
                             "qproperty-alignment: AlignCenter;"
                             "border: none;"
                             "}");
        // label->setAlignment(Qt::AlignCenter);
        label->setAlignment(Qt::AlignCenter | Qt::AlignTop); // Align label to the top
        label->setWordWrap(true);
        label->setMinimumHeight(dialog->height() / 2);
        label->setMinimumWidth(dialog->width());
        // Move the label up by 100 pixels
        // int xOffset = (dialog->width() - label->width()) / 2;
        // int yOffset = 100;
        // label->move(xOffset, yOffset);

        QVBoxLayout *layout = new QVBoxLayout(dialog);
        layout->addWidget(label);

        dialog->setLayout(layout);
        dialog->show();
        dialog->exec();

        // ui->label_thanks_for_feedback->show();
        // ui->label_thanks_for_feedback->raise();
    }

    else
    {
        dialog->move(0, 0);
        dialog->resize(1080, 1920);
        dialog->setStyleSheet("background-image:  url(/home/df-admin/drinkfill/ui/references/templates/default/background_feedbacksent.png);");

        QLabel *label = new QLabel(dialog);
        label->setText("Thank you for <br>your feedback <br>");
        label->setStyleSheet("QLabel {"
                             "font-family: 'Brevia';"
                             "font-style: normal;"
                             "font-weight: 75;"
                             "font-size: 65px;"
                             "line-height: 99px;"
                             "letter-spacing: 1.5px;"
                             "color: #003840;"
                             "text-align: center;"
                             "qproperty-alignment: AlignCenter;"
                             "border: none;"
                             "}");
        // label->setAlignment(Qt::AlignCenter);
        label->setAlignment(Qt::AlignCenter | Qt::AlignTop); // Align label to the top
        label->setWordWrap(true);
        label->setMinimumHeight(dialog->height() / 2);
        label->setMinimumWidth(dialog->width());
    }

    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_sendFeedback::keyboardButtonPressed(int buttonID)
{
    qDebug() << "maintenance password Keyboard Button pressed";

    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    QString buttonText = buttonpressed->text();

    if (buttonText == "Cancel")
    {
        ui->feedbackKeyboard->hide();
        ui->feedbackText->setText("");
    }
    else if (buttonText == "CAPS")
    {
        foreach (QAbstractButton *button, ui->buttonGroup->buttons())
        {
            if (button->text() == "Space" || button->text() == "Done" || button->text() == "Cancel" || button->text() == "Clear" || button->text() == "Backspace")
            {
                // qDebug() << "doing nothing";
            }
            else
            {
                button->setText(button->text().toLower());
            }
        }
    }
    else if (buttonText == "caps")
    {
        foreach (QAbstractButton *button, ui->buttonGroup->buttons())
        {
            if (button->text() == "Space" || button->text() == "Done" || button->text() == "Cancel" || button->text() == "Clear" || button->text() == "Backspace")
            {
                // doing nothing
            }
            else
            {
                button->setText(button->text().toUpper());
            }
        }
    }
    else if (buttonText == "Backspace")
    {
        ui->feedbackText->backspace();
    }
    else if (buttonText == "Clear")
    {
        ui->feedbackText->setText("");
    }
    else if (buttonText == "Done")
    {
        qDebug() << "DONE CLICKED";
        QString textEntry = ui->feedbackText->text();
        ui->feedbackKeyboard->hide();
        if (ui->feedbackText->text() == "")
        {
            ui->feedbackText->hide();
        }
    }
    else if (buttonText == "Space")
    {
        ui->feedbackText->setText(ui->feedbackText->text() + " ");
    }
    else if (buttonText == "&&")
    {
        ui->feedbackText->setText(ui->feedbackText->text() + "&");
    }
    else
    {
        ui->feedbackText->setText(ui->feedbackText->text() + buttonText);
    }
}
void page_sendFeedback::on_previousPage_Button_clicked()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}
void page_sendFeedback::on_feedback_Input_Button_clicked()
{
    //  QObject *button = QObject::sender();
    qDebug() << "feedback button clicked";
    ui->feedbackText->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #5E8580;border-color:#5E8580;");
    // ui->promoInputButton->hide();
    ui->feedbackKeyboard->show();
    ui->feedback_Input_Button->lower();
    qDebug() << "show promo keyboard.";
    ui->feedbackText->show();
}