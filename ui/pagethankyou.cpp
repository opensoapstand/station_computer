//***************************************
//
// pagethankyou.cpp
// GUI class to show user dispense has been
// completed and route back to page_idle
//
// created: 05-04-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "pagethankyou.h"
#include "ui_pagethankyou.h"
#include "page_sendFeedback.h"

extern QString transactionLogging;

// CTOR
pagethankyou::pagethankyou(QWidget *parent) : QWidget(parent),
                                              ui(new Ui::pagethankyou)
{
    ui->setupUi(this);


    ui->extra_message_label->hide();
    connect(ui->notifyUs_Button, SIGNAL(clicked()), this, SLOT(on_notifyUs_Button_clicked()));

    thankYouEndTimer = new QTimer(this);
    thankYouEndTimer->setInterval(1000);
    connect(thankYouEndTimer, SIGNAL(timeout()), this, SLOT(onThankyouTimeoutTick()));

    is_in_state_thank_you = false;

    
}

/*
 * Page Tracking reference
 */
void pagethankyou::setPage(page_dispenser *page_dispenser, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_sendFeedback *page_sendFeedback)
{
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->paymentPage = page_qr_payment;
    this->p_page_sendFeedback = page_sendFeedback;
}

// DTOR
pagethankyou::~pagethankyou()
{
    delete ui;
}

void pagethankyou::showEvent(QShowEvent *event)
{

    QString styleSheet = p_page_idle->getCSS(PAGETHANKYOU_CSS);

    ui->pushButton_to_idle->setStyleSheet(styleSheet);
    ui->thank_you_message_label->setStyleSheet(styleSheet);
    ui->thank_you_subtitle_message_label->setStyleSheet(styleSheet);

    ui->volumeDispensedLabel->setProperty("class", "volumeDispensedStylesheet");//set property goes first!!
    ui->label_volume_dispensed->setProperty("class", "volumeDispensedStylesheet");//set property goes first!!

    ui->volumeDispensedLabel->setStyleSheet(styleSheet);
    ui->label_volume_dispensed->setStyleSheet(styleSheet);
    ui->notifyUs_Button->setStyleSheet(styleSheet);



    qDebug() << "<<<<<<< Page Enter: Thank you >>>>>>>>>";

    QWidget::showEvent(event);

   
    ui->pushButton_to_idle->setEnabled(true);
    ui->pushButton_to_idle->raise();

    QFont font;
    font.setFamily(QStringLiteral("Brevia"));
    font.setPointSize(20);
    // font.setBold(true);
    // font.setWeight(75);
    font.setWeight(50);
    ui->notifyUs_Button->setFont(font);
    ui->notifyUs_Button->setText("Provide Feedback");
    ui->notifyUs_Button->raise();

    // popup
    // ui->popup = new QDialogButtonBox(this);
    // ui->popup->setWindowTitle("Pop-up Window");
    // ui->popup->setFixedSize(400, 200);

    p_page_idle->addCompanyLogoToLabel(ui->thank_you_logo_label);

    ui->thank_you_logo_label->hide();

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_THANK_YOU_BACKGROUND_PATH);

    qDebug() << "db for receipt printer check";
    QString paymentMethod = p_page_idle->selectedProduct->getPaymentMethod();

    DbManager db(DB_PATH);
    bool hasReceiptPrinter = db.hasReceiptPrinter();
    db.closeDB();

    if (hasReceiptPrinter)
    {
        ui->thank_you_message_label->setText("Please take <br>your receipt!");
        ui->thank_you_subtitle_message_label->setText("By refilling you've helped keep a<br>plastic bottle out of our landfills.<br><br>Thank you!");
    }
    else if (paymentMethod == "qr" || paymentMethod == "tapTcp")
    {
        ui->thank_you_message_label->setText("Thank you!");
        ui->thank_you_subtitle_message_label->setText("By refilling you've helped keep a<br>plastic bottle out of our landfills.");
    }
    else
    {
        ui->thank_you_message_label->setText("Thank you!");
        ui->thank_you_subtitle_message_label->setText("By refilling you've helped keep a<br>plastic bottle out of our landfills.");
    }

    is_in_state_thank_you = true;

    // reset promovalue
    p_page_idle->selectedProduct->setDiscountPercentageFraction(0.0);

    is_controller_finished = false;
    is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = false;
    exitIsForceable = false;

    if (paymentMethod == "qr")
    {
        sendDispenseEndToCloud();
    }
    else
    {
        is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
    }
    _thankYouTimeoutSec = PAGE_THANK_YOU_TIMEOUT_SECONDS;
    thankYouEndTimer->start();
    p_page_idle->addPictureToLabel(ui->drinkfill_logo_label2, DRINKFILL_LOGO_VERTICAL_PATH);

    QString units = p_page_idle->selectedProduct->getUnitsForSlot();
    QString dispensed_correct_units = df_util::getConvertedStringVolumeFromMl(p_page_idle->selectedProduct->getVolumeDispensedMl(), units, false, true);

    double price = p_page_idle->selectedProduct->getPriceCorrected();

    if (p_page_idle->selectedProduct->getSize() == SIZE_CUSTOM_INDEX)
    {
        price = p_page_idle->selectedProduct->getVolumeDispensedMl() * price;
    }
    ui->volumeDispensedLabel->setText(dispensed_correct_units + " ( $" + QString::number(price, 'f', 2) + " )");
}

size_t WriteCallback2(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void pagethankyou::sendDispenseEndToCloud()
{
    QString order_id = this->paymentPage->getOID();

    QString units = p_page_idle->selectedProduct->getUnitsForSlot();
    QString dispensed_correct_units = df_util::getConvertedStringVolumeFromMl(p_page_idle->selectedProduct->getVolumeDispensedMl(), units, false, false);

    QString promoCode = this->p_page_dispense->getPromoCodeUsed();
    qDebug() << "Send data at finish of order : " << order_id << ". Total dispensed: " << dispensed_correct_units << "corrected units send to soapstandportal: " << dispensed_correct_units;
    if (dispensed_correct_units == 0)
    {
        transactionLogging += "\n ERROR: No Volume dispensed";
    }
    QString curl_param = "oid=" + order_id + "&dispensed_amount=" + dispensed_correct_units + "&coupon=" + promoCode + "&logging=" + transactionLogging;
    ;
    qDebug() << "Curl params" << curl_param << endl;
    curl_param_array = curl_param.toLocal8Bit();
    curl_data = curl_param_array.data();

    curl = curl_easy_init();
    if (!curl)
    {
        qDebug() << "pagethankyou: cURL failed to init. parameters:" + curl_param;
        transactionToFile(curl_data);
        is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/updateOrder");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curl_param_array.data());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback2);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);
    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        qDebug() << "ERROR: Transaction NOT sent to cloud. cURL fail. Error code: " + QString::number(res);

        transactionToFile(curl_data);
        is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
    }
    else
    {
        QString feedback = QString::fromUtf8(readBuffer.c_str());
        qDebug() << "Transaction sent to cloud. cURL success. Server feedback readbuffer: " << feedback;

        // readbuffer is a string. "true" or "false"
        if (readBuffer == "true" || "Order Completed")
        {
            // return data
            is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
        }
    }
    curl_easy_cleanup(curl);
    readBuffer = "";
}

void pagethankyou::controllerFinishedTransaction()
{
    if (is_in_state_thank_you)
    {
        qDebug() << "Thank you page: Controller msg: All done for transaction.";
        is_controller_finished = true;
        // thankYouEndTimer->start(1000);
        _thankYouTimeoutSec = PAGE_THANK_YOU_TIMEOUT_SECONDS;
    }
    else
    {
        qDebug() << "WARNING: Transaction end received while not in thank you page.";
    }
}

void pagethankyou::transactionToFile(char *curl_params)
{
    QString data_out = curl_params;
    p_page_idle->dfUtility->write_to_file(TRANSACTION_DISPENSE_END_OFFINE_PATH, data_out);
}

void pagethankyou::onThankyouTimeoutTick()
{
    if (--_thankYouTimeoutSec >= 0)
    {
        qDebug() << QString::number(_thankYouTimeoutSec);
    }
    else
    {
        finishHandler();
        // once finishHandler is activated "unsuccessfully" make sure, next time it will finish for sure!
        exitIsForceable = true;
        _thankYouTimeoutSec = PAGE_THANK_YOU_TIMEOUT_SECONDS;
    }
}

void pagethankyou::on_pushButton_to_idle_clicked()
{
    qDebug() << "main page button clicked.";
    finishHandler();
}

void pagethankyou::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{

    is_in_state_thank_you = false;
    p_page_idle->selectedProduct->setPromoCode("");

    thankYouEndTimer->stop();
    p_page_idle->pageTransition(this, pageToShow);
}

void pagethankyou::finishHandler()
{
    transactionLogging = "";
    if ((is_controller_finished && is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER) || exitIsForceable)
    {
        if (exitIsForceable)
        {
            qDebug() << "ERROR?!:Forced exit. controller ok?: " << is_controller_finished << " is payment finished?:" << is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER;
        }

        hideCurrentPageAndShowProvided(p_page_idle);
    }
    else
    {

        ui->thank_you_message_label->setText("Finishing<br>transaction");
        ui->thank_you_subtitle_message_label->hide();
    }
}

void pagethankyou::on_notifyUs_Button_clicked()
{
    hideCurrentPageAndShowProvided(p_page_sendFeedback);
}
