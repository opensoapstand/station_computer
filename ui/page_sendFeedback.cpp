
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

   

    ui->textEdit_custom_message->hide();
    // ui->textEdit_custom_message->setEchoMode(QTextEdit::Normal);
    ui->textEdit_custom_message->setAlignment(Qt::AlignTop | Qt::AlignLeft);


    ui->label_enter_feedback->show();
    //ui->pushButton_previous_page->setStyleSheet("QPushButton { color:#555555; background-color: transparent; border: 0px }");


    ui->pushButton_previous_page->setText("<-back");
    ui->feedbackText->hide();

    ui->label_select_problem->setText("Please provide feedback");
    // ui->label_select_problem->setWordWrap(true);
    

    //ui->label_problem_options->setStyleSheet("background-color: #5E8680; border-radius: 30px;");
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

    
    ui->label_still_cant_find->setText("Still can't find it?");

    
    ui->label_email->setText("Email us at: sales@soapstand.com");

    ui->checkBox_1_Label->setStyleSheet(checkBoxLabelStyling);
    ui->checkBox_1_Label->setText("I love this, keep me in the loop.");

    ui->checkBox_2_Label->setStyleSheet(checkBoxLabelStyling);
    // ui->checkBox_2_Label->setText("No soap was dispensed");
    ui->checkBox_2_Label->setText("Payment issue");

    ui->checkBox_3_Label->setStyleSheet(checkBoxLabelStyling);
    ui->checkBox_3_Label->setText("User interface problem");
    // ui->checkBox_3_Label->setText("Wrong amount of soap");

    ui->checkBox_4_Label->setStyleSheet(checkBoxLabelStyling);
    ui->checkBox_4_Label->setText("Soap dispensing problem");
    // ui->checkBox_4_Label->setText("Payment issue");

    ui->checkBox_5_Label->setStyleSheet(checkBoxLabelStyling);
    ui->checkBox_5_Label->setText("Other");

    ui->label_enter_feedback->setStyleSheet(checkBoxLabelStyling);
    ui->label_enter_feedback->setText("Please enter details or comments below.\nProvide email for feedback.");

    ui->pushButton_start_input->raise();
    // ui->pushButton_start_input->setStyleSheet("QPushButton { border: 1px solid #FFFFFF}");

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


    ui->textEdit_custom_message->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #5E8580;border-color:#5E8580;");

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
    QString styleSheet = p_page_idle->getCSS(PAGE_FEEDBACK_CSS);

    ui->pushButton_send->setStyleSheet(styleSheet);
    ui->pushButton_send->setText("SEND");
    
    ui->pushButton_start_input->setProperty("class", "buttonTransparent");
    ui->pushButton_start_input->setStyleSheet(styleSheet);
    
    ui->textEdit_custom_message->setStyleSheet(styleSheet);
    
    ui->pushButton_previous_page->setProperty( "class", "buttonPreviousPage");
    ui->pushButton_previous_page->setStyleSheet(styleSheet);

    ui->label_select_problem->setProperty("class", "labelSelectProblem");
    ui->label_select_problem->setStyleSheet(styleSheet);

     //ui->label_problem_options->setStyleSheet("background-color: #5E8680; border-radius: 30px;");
     ui->label_problem_options->setStyleSheet(styleSheet);

    //ui->pushButton_to_idle->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->pushButton_to_idle->setStyleSheet(styleSheet);
    ui->label_still_cant_find->setStyleSheet(styleSheet);
    /*ui->label_email->setStyleSheet(
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
*/
    ui->label_email->setStyleSheet(styleSheet);

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
        hideCurrentPageAndShowProvided(p_page_idle);
    }
}

void page_sendFeedback::reset_and_show_page_elements()
{

    ui->textEdit_custom_message->clear();
    ui->textEdit_custom_message->show();
    ui->textEdit_custom_message->setFocus(); // give focus to the textEdit_custom_message widget
    qDebug() << "Setting feedback text to:" << TEXTBOX_INVITE_TEXT;
    ui->textEdit_custom_message->setPlaceholderText(TEXTBOX_INVITE_TEXT);
    // ui->textEdit_custom_message->setText(TEXTBOX_INVITE_TEXT);

    ui->feedbackKeyboard->hide();

    ui->pushButton_start_input->raise();
    ui->pushButton_start_input->show();

    ui->label_thanks_for_feedback->hide();
    ui->label_thank_you_image->hide();

    ui->checkBox_1->setCheckState(Qt::Unchecked);
    ui->checkBox_2->setCheckState(Qt::Unchecked);
    ui->checkBox_3->setCheckState(Qt::Unchecked);
    ui->checkBox_4->setCheckState(Qt::Unchecked);
    ui->checkBox_5->setCheckState(Qt::Unchecked);

    // set keyboard to lower case
    foreach (QAbstractButton *button, ui->buttonGroup->buttons())
    {
        if (button->text() == "Space" || button->text() == "Done" || button->text() == "Cancel" || button->text() == "Clear" || button->text() == "Enter" || button->text() == "Backspace")
        {
            // qDebug() << "doing nothing";
        }
        else
        {
            button->setText(button->text().toLower());
        }
    }
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

void page_sendFeedback::on_pushButton_to_idle_clicked()
{
    hideCurrentPageAndShowProvided(p_page_help);
}

size_t WriteCallbackFeedback(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void page_sendFeedback::on_pushButton_send_clicked()
{
    qDebug() << "Send button pressed";
    QVBoxLayout *layout = new QVBoxLayout();

    // revert
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

    if (problems.length() != 0 || ( !(ui->textEdit_custom_message->toPlainText().isEmpty())  &&  (ui->textEdit_custom_message->toPlainText() != TEXTBOX_INVITE_TEXT )))
    {
        qDebug() << "Will send feedback to backend";

        // instant reaction by hiding the page into "thank you"
        ui->label_thank_you_image->show();
        ui->label_thank_you_image->raise();
        ui->label_thank_you_image->repaint(); // instant showing instead of waiting for function to be finished.
        
        ui->label_thanks_for_feedback->show();
        ui->label_thanks_for_feedback->raise();
        ui->label_thanks_for_feedback->repaint(); // instant showing instead of waiting for function to be finished.

        // send to backend
        QString MachineSerialNumber = p_page_idle->currentProductOrder->getMachineId();
        QString customFeedback = ui->textEdit_custom_message->toPlainText();
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
    qDebug() << "Feeback page Keyboard Button pressed";
    _selectIdleTimeoutSec = 120;
    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    QString buttonText = buttonpressed->text();

    if (buttonText == "Cancel")
    {
        ui->feedbackKeyboard->hide();
        ui->textEdit_custom_message->setText("");
        ui->pushButton_start_input->raise();
        ui->pushButton_start_input->show();
    }
    else if (buttonText == "CAPS")
    {
        foreach (QAbstractButton *button, ui->buttonGroup->buttons())
        {
            if (button->text() == "Space" || button->text() == "Done" || button->text() == "Cancel" || button->text() == "Clear" || button->text() == "Enter" || button->text() == "Backspace")
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
            if (button->text() == "Space" || button->text() == "Done" || button->text() == "Cancel" || button->text() == "Clear" || button->text() == "Enter" || button->text() == "Backspace")
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
        // ui->textEdit_custom_message->deletePreviousChar();
        QTextCursor cursor = ui->textEdit_custom_message->textCursor();
        cursor.deletePreviousChar();
    }
    else if (buttonText == "Clear")
    {
        ui->textEdit_custom_message->setPlainText("");
    }
    else if (buttonText == "Done")
    {
        qDebug() << "Keyboard: Done Clicked";
        QString textEntry = ui->textEdit_custom_message->toPlainText();
        ui->feedbackKeyboard->hide();
        ui->pushButton_start_input->raise();
        ui->pushButton_start_input->show();
    }
    else if (buttonText == "Space")
    {
        ui->textEdit_custom_message->insertPlainText(" ");
    }
    else if (buttonText == "&&")
    {
        ui->textEdit_custom_message->insertPlainText("&");
    }
    else if (buttonText == "Enter")
    {
        ui->textEdit_custom_message->insertPlainText("\n");
    }
    else
    {
        ui->textEdit_custom_message->insertPlainText(buttonText);
    }
}

void page_sendFeedback::on_pushButton_previous_page_clicked()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_sendFeedback::on_feedback_Text_Input_clicked()
{
    // on_pushButton_start_input_clicked();
}

void page_sendFeedback::on_pushButton_start_input_clicked()
{
    qDebug() << "Feedback button clicked, will show keyboard";
    
    ui->feedbackKeyboard->show();
    ui->pushButton_start_input->lower();
    ui->pushButton_start_input->hide();

    // starts with welcome message
    if (ui->textEdit_custom_message->toPlainText() == TEXTBOX_INVITE_TEXT)
    {
        ui->textEdit_custom_message->clear(); // clears init text
        //ui->textEdit_custom_message->setPlainText("fefef");
    }
}

void page_sendFeedback::on_feedbackText_cursorPositionChanged(int arg1, int arg2)
{
    // on_pushButton_start_input_clicked();
}
