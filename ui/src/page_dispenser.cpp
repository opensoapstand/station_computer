//***************************************
//
// page_dispenser.cpp
// GUI class to dispense drink.
// Coordinates User input to payment class
// then communcates results to FSM IP_thread
//
// Allows navigation to paymentpage and
// thankyou page
//
// created: 05-04-2022
// by: Lode Ameije, Ash Singla & Daniel C.
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "page_dispenser.h"
#include "ui_page_dispenser.h"
#include "includefiles.h"
#include "page_idle.h"
#include "page_end.h"
#include "page_product.h"
#include "page_product_mixing.h"
#include "payment/commands.h"
extern bool isFreeEmailOrder;
extern QString transactionLogging;
extern std::string MAC_KEY;
extern std::string MAC_LABEL;
extern std::string AUTH_CODE;
extern std::string socketAddr;
extern std::map<std::string, std::string> tapPaymentObject;
double current_price;
// CTOR
page_dispenser::page_dispenser(QWidget *parent) : QWidget(parent),
                                                  ui(new Ui::page_dispenser)
{
    qDebug() << "constructor page_dispenser";
    ui->setupUi(this);

    ui->pushButton_abort->raise();
    ui->pushButton_problems->raise();
    dispenseIdleTimer = new QTimer(this);
    dispenseIdleTimer->setInterval(1000);
    connect(dispenseIdleTimer, SIGNAL(timeout()), this, SLOT(onDispenseIdleTick()));
    this->isDispensing = false;

    arrowAnimationStepTimer = new QTimer(this);
    arrowAnimationStepTimer->setInterval(50);
    connect(arrowAnimationStepTimer, SIGNAL(timeout()), this, SLOT(onArrowAnimationStepTimerTick()));
    statusbarLayout = new QVBoxLayout(this);
}

/*
 * Page Tracking reference to Payment page and completed payment
 */
void page_dispenser::setPage(page_qr_payment *page_qr_payment,page_offline_payment *page_offline_payment, page_payment_tap_serial *page_payment_tap_serial, page_payment_tap_tcp *page_payment_tap_tcp, page_end *page_end, page_idle *pageIdle, page_sendFeedback *pageFeedback, statusbar *p_statusbar)
{
    this->thanksPage = page_end;
    this->paymentPage = page_qr_payment;
    this->paymentOfflinePage = page_offline_payment;
    this->p_page_payment_tap_tcp = page_payment_tap_tcp;
    this->p_page_idle = pageIdle;
    this->feedbackPage = pageFeedback;
    this->p_statusbar = p_statusbar;
}

// DTOR
page_dispenser::~page_dispenser()
{
    delete ui;
}

void page_dispenser::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{

    this->isDispensing = false;

    dispenseIdleTimer->stop();
    arrowAnimationStepTimer->stop();

    if (msgBox_abort != nullptr)
    {
        msgBox_abort->hide();
        msgBox_abort->deleteLater();
        msgBox_abort = nullptr;
    }
    // else
    // {
    //     qDebug() << "msgBox_abort was not active ";
    // }

    if (msgBox_problems != nullptr)
    {
        msgBox_problems->hide();
        msgBox_problems->deleteLater();
        msgBox_abort = nullptr;
    }
    // else
    // {
    //     qDebug() << "msgBox_problems was not active ";
    // }

    qDebug() << "msgBox done. ";
    cancelPayment = false;

    statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.

    p_page_idle->thisMachine->pageTransition(this, pageToShow);
}
void page_dispenser::showEvent(QShowEvent *event)
{
    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    qDebug() << "Selected product: " << QString::number(p_page_idle->thisMachine->getSelectedProduct()->getPNumber());
    QWidget::showEvent(event);

    statusbarLayout->addWidget(p_statusbar);            // Only one instance can be shown. So, has to be added/removed per page.
    statusbarLayout->setContentsMargins(0, 1874, 0, 0); // int left, int top, int right, int bottom);

    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget

    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_problems);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_to_refill);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_instructions_container);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_press);
    p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_abort, "abort");
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_volume_dispensed);

    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_DISPENSER_CSS);
    ui->pushButton_problems->setProperty("class", "normal");
    ui->label_volume_dispensed->setProperty("class", "normal");
    ui->pushButton_problems->setStyleSheet(styleSheet);
    ui->label_volume_dispensed->setProperty("class", "normal");
    ui->label_finishTransactionMessage->setStyleSheet(styleSheet);
    ui->pushButton_debug_Button->setStyleSheet(styleSheet);
    ui->pushButton_abort->setStyleSheet(styleSheet);
    ui->label_volume_dispensed_ml->setProperty("class", "label_volume_dispensed_ml"); // set property goes first!!
    ui->label_volume_dispensed->setProperty("class", "label_volume_dispensed_ml");
    ui->label_volume_dispensed_ml->setStyleSheet(styleSheet);
    ui->label_volume_dispensed->setStyleSheet(styleSheet);
    ui->label_to_refill->setStyleSheet(styleSheet);
    ui->label_instructions_container->setStyleSheet(styleSheet);
    ui->label_press->setStyleSheet(styleSheet);
    ui->label_dispense_message->setStyleSheet(styleSheet);

    p_page_idle->thisMachine->addToTransactionLogging("\n 6: Station Unlocked - True");
    // transactionLogging += "\n 6: Station Unlocked - True";

    animationStepForwardElseBackward = true;

    // important to set to nullptr, to check at timeout if it was initialized (displayed...) or not.
    msgBox_problems = nullptr;
    msgBox_abort = nullptr;

    this->isDispensing = false;
    askForFeedbackAtEnd = false;



    previousDispenseStatus = SLOT_STATE_INVALID;

    // if (p_page_idle->thisMachine->getSlotCount() == 1)
    // {
    //     // single spout
    //     p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_DISPENSE_INSTRUCTIONS_BACKGROUND_PATH);
    // }
    // else
    // {
        p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_DISPENSE_INSTRUCTIONS_BACKGROUND_PATH);
    // }

    // p_page_idle->thisMachine->addPictureToLabel(ui->label_indicate_active_spout, p_page_idle->thisMachine->getTemplatePathFromName(PAGE_DISPENSE_INSTRUCTIONS_SPOUT_INDICATOR_DOWN));

    if (p_page_idle->thisMachine->isDispenseAreaBelowElseBesideScreen())
    {
        // show on bottom
        p_page_idle->thisMachine->addPictureToLabel(ui->label_indicate_active_spout, p_page_idle->thisMachine->getTemplatePathFromName(PAGE_DISPENSE_INSTRUCTIONS_SPOUT_INDICATOR_DOWN));

        // indicate spout position with arrow.
        // ui->label_indicate_active_spout->move( (p_page_idle->thisMachine->getSelectedSlot()->getSlotId() -1) * 290 + 10, ui->label_indicate_active_spout->y()); // although it is well spaced out in theory, in reality, spout 2 and 3 are not clearly distinguished.
        int x = 0;
        switch (p_page_idle->thisMachine->getSelectedSlot()->getSlotId())
        {
        case 1:
            x = 10;
            break;
        case 2:
            x = 250;
            break;
        case 3:
            x = 640;
            break;
        case 4:
            x = 880;
            break;
        }
        if(p_page_idle->thisMachine->hasMixing()){
            ui->label_indicate_active_spout->move(x, 1700);
        }else{
            ui->label_indicate_active_spout->move(x, 1600);
            // ui->label_indicate_active_spout->move(x, ui->label_indicate_active_spout->y());
        }
    }
    else
    {
        // show on side
        p_page_idle->thisMachine->addPictureToLabel(ui->label_indicate_active_spout, p_page_idle->thisMachine->getTemplatePathFromName(PAGE_DISPENSE_INSTRUCTIONS_SPOUT_INDICATOR_RIGHT));
    }

    p_page_idle->thisMachine->addClientLogoToLabel(ui->label_logo);
    ui->label_logo->hide();

    p_page_idle->thisMachine->addPictureToLabel(ui->label_background_during_dispense_animation, p_page_idle->thisMachine->getTemplatePathFromName(PAGE_DISPENSE_BACKGROUND_PATH));

    p_page_idle->thisMachine->addPictureToLabel(ui->label_moving_bottle_fill_effect, p_page_idle->thisMachine->getTemplatePathFromName(PAGE_DISPENSE_FILL_ANIMATION));

    ui->pushButton_abort->show();
    ui->label_press->show();
    ui->label_to_refill->show();
    ui->label_instructions_container->show();
    ui->label_finishTransactionMessage->hide();
    ui->label_background_during_dispense_animation->hide();
    ui->label_moving_bottle_fill_effect->hide();
    ui->label_indicate_active_spout->show();

    ui->label_dispense_message->hide();
    ui->pushButton_problems->show();

    if(p_page_idle->thisMachine->hasMixing()){
        ui->label_moving_bottle_fill_effect->move(296, 663);
        // not needed
        ui->label_press->hide();
        ui->label_product_summary_background->setStyleSheet(styleSheet);
        ui->label_product_name->setStyleSheet(styleSheet);
        ui->label_product_selected->setStyleSheet(styleSheet);

        p_page_idle->thisMachine->addPictureToLabel(ui->label_product_icon, p_page_idle->thisMachine->getTemplatePathFromName(PAGE_DISPENSE_PRODUCT_ICON));
        p_page_idle->thisMachine->addPictureToLabel(ui->label_volume_icon, p_page_idle->thisMachine->getTemplatePathFromName(PAGE_DISPENSE_VOLUME_ICON));

        ui->label_product_name->setText(p_page_idle->thisMachine->getSelectedProduct()->getProductName());
        QString selected_volume = p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(p_page_idle->thisMachine->getSelectedProduct()->getSelectedSize(), true, true);
        double selectedPrice = p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize();
        double selectedPriceCorrected;
        if (p_page_idle->thisMachine->getSelectedProduct()->getSelectedSize() == SIZE_CUSTOM_INDEX){
            double discountFraction = p_page_idle->thisMachine->getDiscountPercentageFraction();
            QString units = p_page_idle->thisMachine->getSizeUnit();
            if (units == "ml")
            {
                units = "100ml";
                selectedPrice = selectedPrice * 100;
            }
            else if (units == "g")
            {
                if (p_page_idle->thisMachine->getSelectedProduct()->getVolumeBySize(SIZE_CUSTOM_INDEX) == VOLUME_TO_TREAT_CUSTOM_DISPENSE_AS_PER_100G)
                {
                    units = "100g";
                    selectedPrice = selectedPrice * 100;
                }
                else
                {
                    units = "kg";
                    selectedPrice = selectedPrice * 1000;
                }
            }
            else if (units == "oz")
            {
                units = "oz";
                selectedPrice = selectedPrice * OZ_TO_ML;
            }
            selectedPriceCorrected = selectedPrice * (1 - discountFraction);
            QString base = p_page_idle->thisMachine->getTemplateTextByElementNameAndPageAndIdentifier(ui->label_product_selected, "custom_volume");
            ui->label_product_selected->setText(base.arg(selected_volume)+ "<br>$" + QString::number(selectedPriceCorrected, 'f', 2) + "/" + units);
        }else{
            selectedPriceCorrected = p_page_idle->thisMachine->getPriceWithDiscount(selectedPrice);
            ui->label_product_selected->setText(selected_volume + " ($" + QString::number(selectedPriceCorrected, 'f', 2) + ")");
        }
        ui->label_dispense_message_background->setStyleSheet(styleSheet);
        p_page_idle->thisMachine->addPictureToLabel(ui->label_dispense_message_icon, p_page_idle->thisMachine->getTemplatePathFromName(PAGE_DISPENSE_ALERT_ICON));
        ui->label_dispense_message_background->hide();
        ui->label_dispense_message_icon->hide();
    }else{
        ui->label_moving_bottle_fill_effect->move(380, 889);
        ui->pushButton_problems->move(120, 40);
        ui->label_product_summary_background->hide();
        ui->label_product_icon->hide();
        ui->label_product_name->hide();
        ui->label_product_selected->hide();
        ui->label_volume_icon->hide();
        ui->label_dispense_message_background->hide();
        ui->label_dispense_message_icon->hide();
    }

    dispenseIdleTimer->start(1000);
    arrowAnimationStepTimer->start();
    resetDispenseTimeout();

    p_page_idle->thisMachine->getSelectedProduct()->resetVolumeDispensed();
    updatelabel_volume_dispensed_ml(p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl());
    paymentMethod = p_page_idle->thisMachine->getSelectedPaymentMethod();

    fsmSendStartDispensing();
}

void page_dispenser::updatelabel_volume_dispensed_ml(double dispensed)
{
    QString dispensedVolumeUnitsCorrected;
    QString units = p_page_idle->thisMachine->getSizeUnit();

    if (units == "oz")
    {
        dispensedVolumeUnitsCorrected = QString::number(ceil(dispensed * (double)ML_TO_OZ * 1.0));
    }
    else
    {
        dispensedVolumeUnitsCorrected = QString::number(ceil(dispensed));
    }

    if (p_page_idle->thisMachine->getSelectedProduct()->getSelectedSize() == SIZE_CUSTOM_INDEX)
    {

        double unitprice = (p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_CUSTOM_INDEX));
        current_price = p_page_idle->thisMachine->getPriceWithDiscount(dispensed * unitprice);
        ui->label_volume_dispensed_ml->setText(dispensedVolumeUnitsCorrected + " " + units + " ( $" + QString::number(current_price, 'f', 2) + " )");
    }
    else
    {
        QString totalVolume = p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(p_page_idle->thisMachine->getSelectedProduct()->getSelectedSize(), true, true);
        ui->label_volume_dispensed_ml->setText(dispensedVolumeUnitsCorrected + " " + units + "/ " + totalVolume);
        current_price = p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize();
    }
}

bool page_dispenser::sendToUX410()
{
    int waitForAck = 0;
    while (waitForAck < 3)
    {
        cout << "Wait for ACK counter: " << waitForAck << endl;
        com.sendPacket(pktToSend, uint(pktToSend.size()));
        std::cout << "sendtoUX410 Electronic Card Reader: " << paymentPacket.getSendPacket() << endl;

        // read back what is responded
        pktResponded = com.readForAck();
        readPacket.packetReadFromUX(pktResponded);
        pktResponded.clear();
        waitForAck++;

        cout << "Waiting for TAP" << endl;
        cout << readPacket << endl;
        if (readPacket.getAckOrNak() == communicationPacketField::ACK)
        {
            return true;
        }
        usleep(50000);
    }
    return false;
}

bool page_dispenser::waitForUX410()
{
    bool waitResponse = false;
    while (!waitResponse)
    {

        if (pktResponded[0] != 0x02)
        {
            pktResponded.clear();
            pktResponded = com.readPacket();
            usleep(10);
        }
        else
        {
            readPacket.packetReadFromUX(pktResponded);
            std::cout << readPacket;
            com.sendAck();
            waitResponse = true;
        }
    }
    return waitResponse;
}

/*
 * Page Tracking reference to Payment page and completed payment
 */
void page_dispenser::dispensing_end_admin()
{
    qDebug() << "Dispense end admin start";
    this->isDispensing = false;
    ui->label_background_during_dispense_animation->hide();
    ui->label_moving_bottle_fill_effect->hide();
    ui->pushButton_abort->hide();
    ui->label_finishTransactionMessage->show();
    ui->label_finishTransactionMessage->raise();

    double price = current_price;

    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << price;
    qDebug() << "Minimum volume dispensed" << MINIMUM_DISPENSE_VOLUME_ML;
    qDebug() << "volume dispensed" << p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl();
    if (p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl() < MINIMUM_DISPENSE_VOLUME_ML)
    {
        this->cancelPayment = true;
    }
    if (isFreeEmailOrder)
    {
        qDebug() << "Free email order";
    }
    else
    {
        switch (paymentMethod)
        {
        // If payment method is Tap canada
        case 1:
        {
            if (this->cancelPayment)
            {
                ui->label_indicate_active_spout->hide();
                ui->label_to_refill->hide();
                p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_finishTransactionMessage, "no_pay");
                p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_CANCEL);
                std::map<std::string, std::string> response;
                qDebug() << "dispense end: tap payment No volume dispensed.";
                com.page_init();
                pktToSend = paymentPacket.reversePurchasePacket();
                if (sendToUX410())
                {
                    waitForUX410();
                    qDebug() << "Payment Reversed";
                    pktResponded.clear();
                    com.flushSerial();
                }
                tapPaymentObject["status"] = "Voided";
                p_page_idle->thisMachine->getDb()->setPaymentTransaction(tapPaymentObject);
            }
            else
            {
                // Successful transaction. Capturing payment
                QString base_text = p_page_idle->thisMachine->getTemplateTextByElementNameAndPageAndIdentifier(ui->label_finishTransactionMessage, "display_price");
                ui->label_finishTransactionMessage->setText(base_text.arg(QString::number(current_price, 'f', 2))); // will replace %1 character in string by the provide text
                p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_GENERIC);
                tapPaymentObject["status"] = "CAPTURED";
                p_page_idle->thisMachine->getDb()->setPaymentTransaction(tapPaymentObject);
            }
            break;
        }
        case 2:
        {
            // If active payment method is Tap USA
            if (this->cancelPayment)
            {
                // Reversing the payment back to the Tapped card
                ui->label_indicate_active_spout->hide();
                ui->label_to_refill->hide();
                p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_finishTransactionMessage, "no_pay");
                p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_CANCEL);
                std::map<std::string, std::string> response;
                qDebug() << "dispense end: tap payment No volume dispensed.";
                if (tapPaymentObject.find("saf_num") != tapPaymentObject.end())
                {
                    std::cout << "Voiding transaction";
                    qDebug() << "SAF_NUM" << QString::fromStdString(tapPaymentObject["saf_num"]);
                    tapPaymentObject["ctrout_saf"] = tapPaymentObject["saf_num"];
                    response = voidTransactionOffline(std::stoi(socketAddr), MAC_LABEL, MAC_KEY, tapPaymentObject["saf_num"]);
                }
                else if (tapPaymentObject.find("ctroutd") != tapPaymentObject.end())
                {
                    qDebug() << "CTROUTD" << QString::fromStdString(tapPaymentObject["ctroutd"]);
                    response = voidTransaction(std::stoi(socketAddr), MAC_LABEL, MAC_KEY, tapPaymentObject["ctroutd"]);
                    tapPaymentObject["ctrout_saf"] = tapPaymentObject["ctroutd"];
                }
                tapPaymentObject["amount"] = stream.str();
                tapPaymentObject["status"] = "Voided";
                p_page_idle->thisMachine->getDb()->setPaymentTransaction(tapPaymentObject);
                finishSession(std::stoi(socketAddr), MAC_LABEL, MAC_KEY);
            }
            else
            {
                QString base_text = p_page_idle->thisMachine->getTemplateTextByElementNameAndPageAndIdentifier(ui->label_finishTransactionMessage, "display_price");
                ui->label_finishTransactionMessage->setText(base_text.arg(QString::number(current_price, 'f', 2))); // will replace %1 character in string by the provide text
                p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_GENERIC);
                if (tapPaymentObject.find("ctroutd") != tapPaymentObject.end())
                {
                    qDebug() << "CTROUTD" << QString::fromStdString(tapPaymentObject["ctroutd"]);
                    tapPaymentObject["ctrout_saf"] = tapPaymentObject["ctroutd"];
                    std::map<std::string, std::string> testResponse = capture(std::stoi(socketAddr), MAC_LABEL, MAC_KEY, tapPaymentObject["ctroutd"], stream.str());
                    tapPaymentObject["amount"] = stream.str();
                }
                else if (tapPaymentObject.find("saf_num") != tapPaymentObject.end())
                {
                    qDebug() << "SAF_NUM" << QString::fromStdString(tapPaymentObject["saf_num"]);
                    tapPaymentObject["ctrout_saf"] = tapPaymentObject["saf_num"];
                    std::map<std::string, std::string> testResponse = editSaf(std::stoi(socketAddr), MAC_LABEL, MAC_KEY, tapPaymentObject["saf_num"], stream.str(), "ELIGIBLE");
                    tapPaymentObject["amount"] = stream.str();
                }
                tapPaymentObject["status"] = "CAPTURED";
                p_page_idle->thisMachine->getDb()->setPaymentTransaction(tapPaymentObject);
                finishSession(std::stoi(socketAddr), MAC_LABEL, MAC_KEY);
            }
            break;
        }
        default:
        {
            break;
        }
        }
    }
    
    qDebug() << "Finished dispense admin handling";
    current_price = 0.0;
    if (askForFeedbackAtEnd)
    {
        hideCurrentPageAndShowProvided(feedbackPage);
    }
    else
    {
        hideCurrentPageAndShowProvided(thanksPage);
    }
}

void page_dispenser::force_finish_dispensing()
{
    fsmSendStopDispensing();
    dispensing_end_admin();
}

void page_dispenser::fsmSendStartDispensing()
{
    QString dispenseCommand = QString::number(p_page_idle->thisMachine->getSelectedSlot()->getSlotId());
    dispenseCommand.append(p_page_idle->thisMachine->getSelectedProduct()->getSelectedSizeAsChar());
    dispenseCommand.append(SEND_DISPENSE_START);

    qDebug() << "base price: " << QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize());
    QString price = QString::number(p_page_idle->thisMachine->getPriceWithDiscount(p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize()));
    qDebug() << "discounted price: " << price;
    QString promoCode = p_page_idle->thisMachine->getCouponCode();

    QString delimiter = QString("|");
    QString preamble = "orderDetails";
    QString order_command = preamble + delimiter + price + delimiter + promoCode + delimiter;
    qDebug() << "Send order details to FSM: " << order_command;
    p_page_idle->thisMachine->dfUtility->send_command_to_FSM(order_command, true);
    QThread::msleep(50); // Sleep for 50 milliseconds

    bool isCustomMix = p_page_idle->thisMachine->getSelectedProduct()->isCustomMix();
    int pNumberSelectedProduct = p_page_idle->thisMachine->getSelectedProduct()->getPNumber();
    QString command;
    if (isCustomMix)
    {
        QString pNumbersAsCsvString = this->p_page_idle->thisMachine->getSelectedProduct()->getMixPNumbersAsCsv();
        QString pNumberRatiosAsCsvString = this->p_page_idle->thisMachine->getSelectedProduct()->getMixRatiosAsCsv(this->p_page_idle->thisMachine->getSelectedProduct()->getCustomMixRatios());
        command = "dispenseCustomMix|" + dispenseCommand + "|" + QString::number(pNumberSelectedProduct) + "|" + pNumbersAsCsvString + "|" + pNumberRatiosAsCsvString + "|"; // dipenseMix|slot|dispensePNumber|pnumberscsv|ratioscsv
    }
    else
    {
        command = "dispensePNumber|" + dispenseCommand + "|" + QString::number(pNumberSelectedProduct) + "|"; // dispensePNumber|slot|dispensePNumber
    }
    p_page_idle->thisMachine->dfUtility->send_command_to_FSM(command, true);
    
    this->isDispensing = true;
    qDebug() << "Dispensing started.";
}

void page_dispenser::fsmSendStopDispensing()
{
    qDebug() << "Send STOP dispensing to fsm";
    this->isDispensing = false;

    // QString command = QString::number(p_page_idle->thisMachine->getSelectedSlot()->getSlotId());
    // command.append(p_page_idle->thisMachine->getSelectedProduct()->getSelectedSizeAsChar());
    // command.append(SEND_DISPENSE_STOP);
    QString command = "stopDispense";
    p_page_idle->thisMachine->dfUtility->send_command_to_FSM(command, true);
    
}

void page_dispenser::onArrowAnimationStepTimerTick()
{

    arrow_animation_step_counter++;

    if (arrow_animation_step_counter > 10)
    {
        arrow_animation_step_counter = 0;
        animationStepForwardElseBackward = !animationStepForwardElseBackward;
    }

    if (animationStepForwardElseBackward)
    {
        ui->label_indicate_active_spout->move(ui->label_indicate_active_spout->x() + 1, ui->label_indicate_active_spout->y() + 1);
    }
    else
    {
        ui->label_indicate_active_spout->move(ui->label_indicate_active_spout->x() - 1, ui->label_indicate_active_spout->y() - 1);
    }
}

void page_dispenser::onDispenseIdleTick()
{
    if (--_dispenseIdleTimeoutSec >= 0)
    {
        // qDebug() << "Tick Down: " << _dispenseIdleTimeoutSec;  
    }
    else
    {
        qDebug() << "Dispensing timeout. End dispensing.";
        force_finish_dispensing();
    }
}

void page_dispenser::resetDispenseTimeout(void)
{
    _dispenseIdleTimeoutSec = 120;
}

// QString page_dispenser::getMostRecentDispensed()
// {
//     QString units = p_page_idle->thisMachine->getSelectedProduct()->getUnitsForSelectedSlot();

//     return df_util::getConvertedStringVolumeFromMl(volumeDispensed, units, false, false);
// }

// QString page_dispenser::getCouponCodeUsed()
// {
//     QString promoCode = p_page_idle->getCouponCode();

//     return promoCode;
// }

void page_dispenser::fsmReceiveDispenseRate(double flowrate)
{
    qDebug() << "Dispense flow rate received from FSM: " << QString::number(flowrate, 'f', 2);
    ui->label_dispense_flowrate->setText(QString::number(flowrate, 'f', 2) + "ml/s");
    ui->label_dispense_flowrate->hide();
};

void page_dispenser::fsmReceiveDispenserStatus(QString status)
{

    //////////////////////////////////////////////////////
    //     dispense status DOES NOT equal slot status.  // 
    //////////////////////////////////////////////////////

    QString dispenseStatusStr = status;
    QString dispenseStatus = dispenseStatusStr;

    // SlotState dispenseStatus = SlotStateStringMap[dispenseStatusStr];
    // SlotState dispenseStatus = df_util::convertSlotStatusToString(dispenseStatusStr);

    ui->label_dispense_status->setText(dispenseStatusStr);
    ui->label_dispense_status->hide();


    if (dispenseStatus != previousDispenseStatus)
    {
        qDebug() << "Dispense status received from FSM: " << dispenseStatus;

        if (dispenseStatus == "SLOT_STATE_WARNING_PRIMING")
        {
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_dispense_message, "priming");
            p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_problems, "alert", PAGE_DISPENSER_CSS);
            ui->label_dispense_message->show();
            if(p_page_idle->thisMachine->hasMixing()){
                ui->label_dispense_message_background->show();
                ui->label_dispense_message_icon->show();
                ui->label_dispense_message->raise();
            }
        }
        else if (dispenseStatus == "SLOT_STATE_PROBLEM_EMPTY")
        {
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_dispense_message, "out_of_stock");
            p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_problems, "alert", PAGE_DISPENSER_CSS);
            p_page_idle->thisMachine->getSelectedSlot()->setSlotEnabled(false);
            ui->label_dispense_message->show();
            if(p_page_idle->thisMachine->hasMixing()){
                ui->label_dispense_message_background->show();
                ui->label_dispense_message_icon->show();
                ui->label_dispense_message->raise();
            }
        }
        else if (dispenseStatus == "SLOT_STATE_PROBLEM_NEEDS_ATTENTION")
        {
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_dispense_message, "needs_attention");
            p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_problems, "alert", PAGE_DISPENSER_CSS);
            ui->label_dispense_message->show();
            if(p_page_idle->thisMachine->hasMixing()){
                ui->label_dispense_message_background->show();
                ui->label_dispense_message_icon->show();
                ui->label_dispense_message->raise();
            }
        }
        else if (dispenseStatus == "SLOT_STATE_AVAILABLE")
        {
            p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_problems, "normal", PAGE_DISPENSER_CSS);
            // normal status
            // ui->pushButton_problems->hide();
            ui->label_dispense_message->hide();
            if(p_page_idle->thisMachine->hasMixing()){
                ui->label_dispense_message_background->hide();
                ui->label_dispense_message_icon->hide();
            }
            p_page_idle->thisMachine->getSelectedSlot()->setSlotEnabled(true);
        }
        else
        {
        }
    }

    previousDispenseStatus = dispenseStatus;
};

void page_dispenser::updateVolumeDisplayed(double dispensed, bool isFull)
{

    if (p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl() != dispensed)
    {
        // only reset idle timer if volume has changed.
        resetDispenseTimeout();
    }
    p_page_idle->thisMachine->getSelectedProduct()->setVolumeDispensedMl(dispensed);

    // volumeDispensed = dispensed;
    qDebug() << "Signal: dispensed " << dispensed << " of " << p_page_idle->thisMachine->getSelectedProduct()->getVolumeOfSelectedSize();

    if (p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl() >= MINIMUM_DISPENSE_VOLUME_ML)
    {

        ui->label_indicate_active_spout->hide();
        updatelabel_volume_dispensed_ml(p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl());

        double percentage = p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl() / (p_page_idle->thisMachine->getSelectedProduct()->getVolumeOfSelectedSize()) * 100;
        if (isFull)
        {
            percentage = 100;
        }

        // ui->pushButton_problems->move(120, 450);
        // transition from instructions to dispensing at first receival of volume.
        ui->label_background_during_dispense_animation->show();
        if(p_page_idle->thisMachine->hasMixing() && ui->label_background_during_dispense_animation->isVisible()){
            this->ui->label_moving_bottle_fill_effect->move(296, 663 - 4.93 * percentage);
            ui->label_product_summary_background->move(316, 1245);
            ui->label_product_name->move(377, 1275);
            ui->label_product_selected->move(377, 1324);
            ui->label_product_icon->move(337, 1284);
            ui->label_volume_dispensed->move(377, 1429);
            ui->label_volume_dispensed_ml->move(377, 1474);
            ui->label_volume_icon->move(337, 1438);
            ui->pushButton_abort->move(193, 1720);
            ui->pushButton_problems->move(565, 1720);
            ui->label_dispense_message_background->move(316, 1539);
            ui->label_dispense_message_icon->move(342, 1584);
            ui->label_dispense_message->move(415, 1562);

            ui->label_product_summary_background->raise();
            ui->label_product_name->raise();
            ui->label_product_selected->raise();
            ui->label_product_icon->raise();
            ui->label_volume_dispensed->raise();
            ui->label_volume_dispensed_ml->raise();
            ui->label_volume_icon->raise();
            ui->label_dispense_message_background->raise();
            ui->label_dispense_message_icon->raise();
            ui->label_dispense_message->raise();
        }else{
            this->ui->label_moving_bottle_fill_effect->move(380, 900 - 3 * percentage);
        }

        ui->label_press->hide();
        ui->label_to_refill->hide();
        ui->label_instructions_container->hide();

        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_abort, "complete");
        ui->label_moving_bottle_fill_effect->show();
        ui->pushButton_abort->raise();
        ui->pushButton_problems->raise();
    }
}

void page_dispenser::fsmReceivedVolumeDispensed(double dispensed, bool isFull)
{
    if (this->isDispensing)
    {
        updateVolumeDisplayed(dispensed, isFull);
    }
    else
    {
        qDebug() << "Signal: volume update in dispenser while not dispensing.";
    }
}

void page_dispenser::fsmReceiveTargetVolumeReached()
{
    if (this->isDispensing)
    {
        qDebug() << "Target reached from controller.";
        this->isDispensing = false;

        // The idea is to show the target number, but Lode believes it should show the real number dispensed.
        //updateVolumeDisplayed(p_page_idle->thisMachine->getSelectedProduct()->getVolumeOfSelectedSize(), true); // make sure the fill bottle graphics are completed
        p_page_idle->thisMachine->addToTransactionLogging("\n 8: Target Reached - True");
        dispensing_end_admin();
    }
    else
    {
        qDebug() << "target volumne reached signal received while not dispensing.";
    }
}

void page_dispenser::fsmReceiveNoFlowAbort()
{
    if (this->isDispensing)
    {
        this->isDispensing = false;
        updateVolumeDisplayed(1.0, true); // make sure the fill bottle graphics are completed
        dispensing_end_admin();
    }
    else
    {
        qDebug() << "No flow abort received while not dispensing.";
    }
}

void page_dispenser::on_cancelButton_clicked()
{
    qDebug() << "Pressed cancel dispensing.";
    if (this->isDispensing)
    {
        p_page_idle->thisMachine->addToTransactionLogging("\n 7: Cancel Button - True");
        // transactionLogging += "\n 7: Cancel Button - True";
        force_finish_dispensing();
    }
}

void page_dispenser::on_pushButton_debug_Button_clicked()
{
    qDebug() << "WARNING: ========= Debug button pressed. Fake dispensing of 100ml ==============";
    updateVolumeDisplayed(750.0, false); // make sure the fill bottle graphics are completed
}

void page_dispenser::on_pushButton_abort_clicked()
{
    qDebug() << "Pressed button abort/complete";
    p_page_idle->thisMachine->addToTransactionLogging("\n 7: Complete Button - True");
    // transactionLogging += "\n 7: Complete Button - True";
    if (p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl() < MINIMUM_DISPENSE_VOLUME_ML)
    {
        msgBox_abort = new QMessageBox();
        msgBox_abort->setObjectName("msgBox_abort");
        msgBox_abort->setWindowFlags(Qt::FramelessWindowHint| Qt::Dialog); // do not show messagebox header with program name
        switch (paymentMethod)
        {
        case qr:
        case tap_canada:
        case tap_usa:
        {
            QString searchString = this->objectName() + "->" + msgBox_abort->objectName() + "->" + "qr_tap";
            p_page_idle->thisMachine->setTextToObject(msgBox_abort, p_page_idle->thisMachine->getTemplateText(searchString));
            break;
        }
        default:
        {
            QString searchString = this->objectName() + "->" + msgBox_abort->objectName() + "->" + "default";
            p_page_idle->thisMachine->setTextToObject(msgBox_abort, p_page_idle->thisMachine->getTemplateText(searchString));
            break;
        }
        }

        p_page_idle->thisMachine->addCssClassToObject(msgBox_abort, "msgBoxbutton msgBox", PAGE_DISPENSER_CSS);
        msgBox_abort->setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        // Use a QTimer to hide and delete the message box after a timeout
        QTimer *timeoutTimer = new QTimer(msgBox_abort);
        QObject::connect(timeoutTimer, &QTimer::timeout, [this, timeoutTimer]()
                         {
                timeoutTimer->stop();
                timeoutTimer->deleteLater();
                if (msgBox_abort) // check if still exits.
                {
                    msgBox_abort->hide();
                    msgBox_abort->deleteLater();
                    msgBox_abort = nullptr;
                }
                qDebug() << "msgBox_abort timed out. "; });
        timeoutTimer->start(MESSAGE_BOX_TIMEOUT_DEFAULT_MILLIS); // Set the timeout duration in milliseconds (5000 = 5 seconds)

        int ret = msgBox_abort->exec();
        switch (ret)
        {
        case QMessageBox::Yes:
        {
            if (this->isDispensing)
            {
                force_finish_dispensing();
            }
            break;
        }
        }
    }
    else
    {
        if (this->isDispensing)
        {
            force_finish_dispensing();
        }
    }
}

void page_dispenser::on_pushButton_problems_clicked()
{
    qDebug() << "Clicked on msgBox_problems  ";
    msgBox_problems = new QMessageBox();
    msgBox_problems->setObjectName("msgBox_problems");
    msgBox_problems->setWindowFlags(Qt::FramelessWindowHint| Qt::Dialog); // do not show messagebox header with program name

    QString client_id = p_page_idle->thisMachine->getClientId();
    if (client_id == "C-1") // good-filling
    {
        QString searchString = this->objectName() + "->" + msgBox_problems->objectName() + "->" + "shopify";
        p_page_idle->thisMachine->setTextToObject(msgBox_problems, p_page_idle->thisMachine->getTemplateText(searchString));
    }
    switch (paymentMethod)
    {
    case qr:
    case tap_canada:
    case tap_usa:
    {
        QString searchString = this->objectName() + "->" + msgBox_problems->objectName() + "->" + "qr_tap";
        p_page_idle->thisMachine->setTextToObject(msgBox_problems, p_page_idle->thisMachine->getTemplateText(searchString));
        break;
    }
    default:
    {
        QString searchString = this->objectName() + "->" + msgBox_problems->objectName() + "->" + "default";
        p_page_idle->thisMachine->setTextToObject(msgBox_problems, p_page_idle->thisMachine->getTemplateText(searchString));
        break;
    }
    }

    p_page_idle->thisMachine->addCssClassToObject(msgBox_problems, "msgBoxbutton msgBox", PAGE_DISPENSER_CSS);

    msgBox_problems->setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    // Use a QTimer to hide and delete the message box after a timeout
    QTimer *timeoutTimer2 = new QTimer(msgBox_problems);

    QObject::connect(timeoutTimer2, &QTimer::timeout, [this, timeoutTimer2]()
                     {
                         timeoutTimer2->stop();
                         timeoutTimer2->deleteLater();

                         if (msgBox_problems != nullptr)
                         {
                             msgBox_problems->hide();
                             msgBox_problems->deleteLater();
                             msgBox_problems = nullptr; // Set to nullptr after deletion
                         }

                         qDebug() << "msgBox_problems timed out. end "; });
    timeoutTimer2->start(MESSAGE_BOX_TIMEOUT_DEFAULT_MILLIS); // Set the timeout duration in milliseconds (5000 = 5 seconds)

    // Connect message box finished signal to a slot that stops and deletes the timer
    QObject::connect(msgBox_problems, &QMessageBox::finished, [timeoutTimer2](int result)
                     {
                         // Stop and delete the timer when the message box is closed
                         timeoutTimer2->stop();
                         timeoutTimer2->deleteLater(); });

    int ret = msgBox_problems->exec();
    switch (ret)
    {
    case QMessageBox::Yes:
    {

        if (this->isDispensing)
        {
            askForFeedbackAtEnd = true;
            force_finish_dispensing();
        }
        break;
    }

    case QMessageBox::No:
    {
        // send repair command
        qDebug() << "Send repair command to fsm";
        p_page_idle->thisMachine->dfUtility->send_command_to_FSM(SEND_REPAIR_PCA, true);
        
        break;
    }
    default:
    {
        qDebug() << "Nothing chosen. User did not choose. ";
    }
    break;
    }

    if (msgBox_problems != nullptr)
    {
        msgBox_problems->hide();
        msgBox_problems->deleteLater();
        msgBox_problems = nullptr; // Set to nullptr after deletion
    }
}
