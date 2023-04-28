
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
#define TEXTBOX_INVITE_TEXT "Touch this field to input a text message"
// CTOR
page_sendFeedback::page_sendFeedback(QWidget *parent) : QWidget(parent),
                                                        ui(new Ui::page_sendFeedback)
{


    qDebug() << "IN send feedback";
    ui->setupUi(this);

    ui->feedback_Input_Button->setStyleSheet("QPushButton { border: 1px solid #5E8580}");

    ui->feedbackTextEdit->hide();
    // ui->feedbackTextEdit->setEchoMode(QTextEdit::Normal);
    ui->feedbackTextEdit->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui->feedbackTextEdit->setStyleSheet(
        "QLineEdit {"

        "font-family: 'Brevia', sans-serif;"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 32px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "color: #003840;"
        "qproperty-alignment: AlignLeft;"
        "border: none;"
        "}");


    ui->label_enter_feedback->show();
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
    ui->feedbackText->hide();

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
    ui->checkBox_1_Label->setText("The station worked perfect, thanks!");

    ui->checkBox_2_Label->setStyleSheet(checkBoxLabelStyling);
    ui->checkBox_2_Label->setText("Payment issue");

    ui->checkBox_3_Label->setStyleSheet(checkBoxLabelStyling);
    ui->checkBox_3_Label->setText("User interface problem");

    ui->checkBox_4_Label->setStyleSheet(checkBoxLabelStyling);
    ui->checkBox_4_Label->setText("Soap dispensing problem");

    ui->checkBox_5_Label->setStyleSheet(checkBoxLabelStyling);
    ui->checkBox_5_Label->setText("Other");

    ui->label_enter_feedback->setStyleSheet(checkBoxLabelStyling);
    ui->label_enter_feedback->setText("Please enter details or comments below");

    ui->feedback_Input_Button->raise();
    ui->feedback_Input_Button->setStyleSheet("QPushButton { border: 1px solid #FFFFFF}");

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

    {
        selectIdleTimer = new QTimer(this);
        selectIdleTimer->setInterval(1000);

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
void page_sendFeedback::setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_help *pageHelp, pageProduct *page_product, pagethankyou *page_thankyou)
{
    this->p_page_select_product = pageSelect;
    this->paymentPage = page_qr_payment;
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->p_page_help = pageHelp;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_product = page_product;

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_SEND_FEEDBACK_PATH);
    ui->send_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_SELECT_PRODUCT_BACKGROUND_PATH);

    QString full_path = p_page_idle->getTemplatePathFromName(IMAGE_BUTTON_HELP);
    p_page_idle->addPictureToLabel(ui->label_help, full_path);

    full_path = p_page_idle->getTemplatePathFromName(THANK_YOU_FOR_YOUR_FEEDBACK);
    p_page_idle->addPictureToLabel(ui->label_thank_you_image, full_path);
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
    _selectIdleTimeoutSec = 60;
    selectIdleTimer->start();

    reset_and_show_page_elements();
}

void page_sendFeedback::resizeEvent(QResizeEvent *event)
{
    qDebug() << "\n---Page Send Feedback: resizeEvent";
}

void page_sendFeedback::onSelectTimeoutTick()
{
    if (--_selectIdleTimeoutSec >= 0)
    {
        //        qDebug() << "page_sendFeedback: Tick Down - " << _selectIdleTimeoutSec ;
    }
    else
    {
        selectIdleTimer->stop();
        hideCurrentPageAndShowProvided(p_page_idle);
    }
}

void page_sendFeedback::reset_and_show_page_elements()
{

    ui->feedbackTextEdit->clear();
    ui->feedbackTextEdit->setText(TEXTBOX_INVITE_TEXT);
    ui->feedbackTextEdit->show();

    ui->feedbackKeyboard->hide();
    
    ui->feedback_Input_Button->raise();
    ui->feedback_Input_Button->show();

    ui->label_thanks_for_feedback->hide();
    ui->label_thank_you_image->hide();

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

    if (problems.length() != 0 || !(ui->feedbackTextEdit->toPlainText().isEmpty()))
    {

        qDebug() << "Will send feedback to backend";

        // instant reaction by hiding the page into "thank you"
        ui->label_thank_you_image->show();
        ui->label_thank_you_image->raise();
        ui->label_thanks_for_feedback->show();
        ui->label_thanks_for_feedback->raise();

        // send to backend
        QString MachineSerialNumber = p_page_idle->currentProductOrder->getMachineId();
        QString customFeedback = ui->feedbackTextEdit->toPlainText();
        QString curl_param = "problems=" + problems + " " + customFeedback + "&MachineSerialNumber=" + MachineSerialNumber;
        qDebug() << "Curl params" << curl_param;
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

        // this will trigger the reversal to idle page (start only after curl command completed)
        _selectIdleTimeoutSec = 2;
    }
}

void page_sendFeedback::keyboardButtonPressed(int buttonID)
{
    qDebug() << "maintenance password Keyboard Button pressed";
    _selectIdleTimeoutSec = 60;
    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    QString buttonText = buttonpressed->text();

    if (buttonText == "Cancel")
    {
        ui->feedbackKeyboard->hide();
        ui->feedbackTextEdit->setText("");
        ui->feedback_Input_Button->raise();
        ui->feedback_Input_Button->show();
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
        // ui->feedbackTextEdit->deletePreviousChar();
        QTextCursor cursor = ui->feedbackTextEdit->textCursor();
        cursor.deletePreviousChar();

    }
    else if (buttonText == "Clear")
    {
        ui->feedbackTextEdit->setPlainText("");
    }
    else if (buttonText == "Done")
    {
        qDebug() << "DONE CLICKED";
        QString textEntry = ui->feedbackTextEdit->toPlainText();
        ui->feedbackKeyboard->hide();
        ui->feedback_Input_Button->raise();
        ui->feedback_Input_Button->show();
    }
    else if (buttonText == "Space")
    {
        ui->feedbackTextEdit->setPlainText(ui->feedbackTextEdit->toPlainText() + " ");
    }
    else if (buttonText == "&&")
    {
        ui->feedbackTextEdit->setPlainText(ui->feedbackTextEdit->toPlainText() + "&");
    }
    else
    {
        ui->feedbackTextEdit->setPlainText(ui->feedbackTextEdit->toPlainText() + buttonText);
    }
}

void page_sendFeedback::on_previousPage_Button_clicked()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_sendFeedback::on_feedback_Text_Input_clicked()
{
    // on_feedback_Input_Button_clicked();
}

void page_sendFeedback::on_feedback_Input_Button_clicked()
{
    qDebug() << "Feedback button clicked, will show keyboard";
    ui->feedbackTextEdit->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #5E8580;border-color:#5E8580;");

    ui->feedbackKeyboard->show();
    ui->feedback_Input_Button->lower();
    ui->feedback_Input_Button->hide();

    // ui->feedbackText->show();
    if (ui->feedbackTextEdit->toPlainText() == TEXTBOX_INVITE_TEXT)
    {
        ui->feedbackTextEdit->clear(); // clears init text
    }
}
void page_sendFeedback::on_feedbackText_cursorPositionChanged(int arg1, int arg2)
{
    // on_feedback_Input_Button_clicked();
}
