
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

#include "page_payment.h"
#include "page_select_product.h"
#include "page_idle.h"
#include <curl/curl.h>
#include <json.hpp>

using json = nlohmann::json;

// CTOR
page_sendFeedback::page_sendFeedback(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::page_sendFeedback)
{
    qDebug()<< "IN send feedback";
    ui->setupUi(this);
    
    ui->previousPage_Button->setStyleSheet("QPushButton { color:#555555; background-color: transparent; border: 0px }");
    ui->previousPage_Button->setStyleSheet(
                "QPushButton {"

                "font-family: 'Brevia';"
                "font-style: normal;"
                "font-weight: 75;"
                "font-size: 32px;"
                "line-height: 99px;"
                "letter-spacing: 1.5px;"
                "text-transform: lowercase;"
                "color: #003840;"
                "text-align: center;"
                "qproperty-alignment: AlignCenter;"
                "border: none;"
                "}");
        // ui->previousPage_Button->setText("<-Back");
        ui->mainPage_Button->hide();
        
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
        ui->label_select_problem->setText("Select problem(s) from the options below");
        ui->label_select_problem->setWordWrap(true);
        ui->label_problem_options->setStyleSheet("background-color: #5E8680; border-radius: 10px;");
        QString checkBoxLabelStyling = ("QPushButton {""font-family: 'Brevia';"
                                    "font-style: normal;"
                                    "font-weight: 75;"
                                    "font-size: 32px;"
                                    "line-height: 99px;"
                                    "letter-spacing: 1.5px;"
                                    "color: #FFFFFF;"
                                    "text-align: left;"
                                    "qproperty-alignment: AlignCenter;"
                                    "border: none;"
                                    "}");
    
        ui->checkBox_1_Label->setStyleSheet(checkBoxLabelStyling);
        ui->checkBox_1_Label->setText("The station never dispensed soap");
       
        ui->checkBox_2_Label->setStyleSheet(checkBoxLabelStyling);
        ui->checkBox_2_Label->setText("The station charged me more for my amount");
 
        ui->checkBox_3_Label->setStyleSheet(checkBoxLabelStyling);
        ui->checkBox_3_Label->setText("The station is too slow ");

        ui->checkBox_4_Label->setStyleSheet(checkBoxLabelStyling);
        ui->checkBox_4_Label->setText("I got charged more than once");

        ui->checkBox_5_Label->setStyleSheet(checkBoxLabelStyling);
        ui->checkBox_5_Label->setText("Screen was frozen");

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
            "border-radius: 20px;"
            "padding: 10px;"
            "border: 2px solid #004D54;"
"}");
        ui->label_send->setText("SEND");
        ui->send_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

}
    
/*
 * Page Tracking reference to Select Drink, Payment Page and Idle page
 */
void page_sendFeedback::setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_payment *page_payment, page_help *pageHelp, pageProduct *page_product, pagethankyou *page_thankyou )
{
    this->p_page_select_product = pageSelect;
    this->paymentPage = page_payment;
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->p_page_help = pageHelp;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_product = page_product;
    // ui->promoCode->clear();
    // ui->promoCode->hide();
    // ui->label_invoice_discount_amount->hide();
    // ui->label_invoice_discount_name->hide();
    // ui->label_discount_tag->hide();

    // selectedProductOrder = p_page_idle->currentProductOrder;
     p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_SEND_FEEDBACK_PATH);
    ui->send_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    // couponHandler();
}

// DTOR
page_sendFeedback::~page_sendFeedback()
{
    delete ui;
}

void page_sendFeedback::setDefaultSize()
{

    bool default_size_set = false;

    for (uint8_t i = 0; i < 4; i++)
    {
        int size = product_sizes[i];
        if (selectedProductOrder->getLoadedProductSizeEnabled(size))
        {
            selectedProductOrder->setSelectedSize(product_sizes[i]);
        }
    }
}

/* GUI */

void page_sendFeedback::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Send Feedback>>>>>>>>>";
    QWidget::showEvent(event);
        
    
    // selectedProductOrder->loadSelectedProductProperties();
    // reset_and_show_page_elements();
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
    
}



bool page_sendFeedback::stopSelectTimers()
{
    if (selectIdleTimer != nullptr)
    {
        selectIdleTimer->stop();
        return true;
    }
    else
    {
        return false;
    }
}

void page_sendFeedback::mainPage()
{
    qDebug() << "page_sendFeedback: mainPage button" << endl;
    this->stopSelectTimers();
    selectIdleTimer->stop();
    // qDebug() << "product to idle";
    // p_page_idle->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_idle);
}

void page_sendFeedback::on_mainPage_Button_clicked()
{
    this->stopSelectTimers();
    selectIdleTimer->stop();
    // p_page_help->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_help);
}



void page_sendFeedback::on_send_Button_clicked()
{   
    qDebug() << "Send button pressed";
    QStringList problemList;
    if(ui->checkBox_1->isChecked()){
        problemList << ui->checkBox_1_Label->text();
    }
    if(ui->checkBox_2->isChecked()){
        problemList << ui->checkBox_2_Label->text();
    }
    if(ui->checkBox_3->isChecked()){
        problemList << ui->checkBox_3_Label->text();
    }
    if(ui->checkBox_4->isChecked()){
        problemList << ui->checkBox_4_Label->text();
    }
    if(ui->checkBox_5->isChecked()){
        problemList << ui->checkBox_5_Label->text();
    }
    qDebug() << problemList;
    qDebug() << "Send button stores values" << endl;
}
