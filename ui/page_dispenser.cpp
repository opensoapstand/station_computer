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
#include "payment/commands.h"

extern QString transactionLogging;
// extern std::string CTROUTD;
extern std::string MAC_KEY;
extern std::string MAC_LABEL;
extern std::string AUTH_CODE;
// extern std::string SAF_NUM;
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
}

/*
 * Page Tracking reference to Payment page and completed payment
 */
void page_dispenser::setPage(page_qr_payment *page_qr_payment, page_payment_tap_serial *page_payment_tap_serial, page_payment_tap_tcp *page_payment_tap_tcp, page_end *page_end, page_idle *pageIdle, page_sendFeedback *pageFeedback)
{
    this->thanksPage = page_end;
    this->paymentPage = page_qr_payment;
    this->p_page_payment_tap_tcp = page_payment_tap_tcp;
    this->p_page_idle = pageIdle;
    this->feedbackPage = pageFeedback;
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
    }

    if (msgBox_problems != nullptr)
    {
        msgBox_problems->hide();
        msgBox_problems->deleteLater();
    }

    p_page_idle->thisMachine->pageTransition(this, pageToShow);
}
void page_dispenser::showEvent(QShowEvent *event)
{
    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    qDebug() << "Selected slot: " << QString::number(p_page_idle->thisMachine->selectedProduct->getSlot());
    QWidget::showEvent(event);

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

    transactionLogging += "\n 6: Station Unlocked - True";

    animationStepForwardElseBackward = true;

    // important to set to nullptr, to check at timeout if it was initialized (displayed...) or not.
    msgBox_problems = nullptr;
    msgBox_abort = nullptr;

    this->isDispensing = false;
    askForFeedbackAtEnd = false;

    ui->label_moving_bottle_fill_effect->move(380, 889);
    ui->pushButton_problems->move(120, 40);

    previousDispenseStatus = "NO STATE";

    if (p_page_idle->thisMachine->getSlotCount() == 1)
    {
        // single spout
        p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_DISPENSE_INSTRUCTIONS_BACKGROUND_PATH);
    }
    else
    {
        p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_DISPENSE_INSTRUCTIONS_MULTISPOUT_BACKGROUND_PATH);
    }

    // p_page_idle->thisMachine->addPictureToLabel(ui->label_indicate_active_spout, p_page_idle->thisMachine->getTemplatePathFromName(PAGE_DISPENSE_INSTRUCTIONS_SPOUT_INDICATOR_DOWN));

    if (p_page_idle->thisMachine->isDispenseAreaBelowElseBesideScreen())
    {
        // show on bottom
        p_page_idle->thisMachine->addPictureToLabel(ui->label_indicate_active_spout, p_page_idle->thisMachine->getTemplatePathFromName(PAGE_DISPENSE_INSTRUCTIONS_SPOUT_INDICATOR_DOWN));

        // indicate spout position with arrow.
        // ui->label_indicate_active_spout->move( (p_page_idle->thisMachine->selectedProduct->getSlot() -1) * 290 + 10, ui->label_indicate_active_spout->y()); // although it is well spaced out in theory, in reality, spout 2 and 3 are not clearly distinguished.
        int x = 0;
        switch (p_page_idle->thisMachine->selectedProduct->getSlot())
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
        ui->label_indicate_active_spout->move(x, ui->label_indicate_active_spout->y());
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

    dispenseIdleTimer->start(1000);
    arrowAnimationStepTimer->start();
    resetDispenseTimeout();

    p_page_idle->thisMachine->selectedProduct->resetVolumeDispensed();
    updatelabel_volume_dispensed_ml(p_page_idle->thisMachine->selectedProduct->getVolumeDispensedMl());
    paymentMethod = p_page_idle->thisMachine->selectedProduct->getPaymentMethod();

    fsmSendStartDispensing();
}

void page_dispenser::updatelabel_volume_dispensed_ml(double dispensed)
{
    QString dispensedVolumeUnitsCorrected;
    QString units = p_page_idle->thisMachine->selectedProduct->getUnitsForSlot();

    if (units == "oz")
    {
        dispensedVolumeUnitsCorrected = QString::number(ceil(dispensed * (double)ML_TO_OZ * 1.0));
    }
    else
    {
        dispensedVolumeUnitsCorrected = QString::number(ceil(dispensed));
    }

    if (p_page_idle->thisMachine->selectedProduct->getSize() == SIZE_CUSTOM_INDEX)
    {

        double unitprice = (p_page_idle->thisMachine->selectedProduct->getBasePrice());
        current_price = p_page_idle->thisMachine->getPriceWithDiscount(dispensed * unitprice);
        ui->label_volume_dispensed_ml->setText(dispensedVolumeUnitsCorrected + " " + units + " ( $" + QString::number(current_price, 'f', 2) + " )");
    }
    else
    {
        QString totalVolume = p_page_idle->thisMachine->selectedProduct->getSizeToVolumeWithCorrectUnits(p_page_idle->thisMachine->selectedProduct->getSize(), true, true);
        ui->label_volume_dispensed_ml->setText(dispensedVolumeUnitsCorrected + " " + units + "/ " + totalVolume);
        current_price = p_page_idle->thisMachine->selectedProduct->getBasePrice();
    }
}

bool page_dispenser::sendToUX410()
{
    int waitForAck = 0;
    while (waitForAck < 3)
    {
        cout << "Wait for ACK counter: " << waitForAck << endl;
        qDebug() << "Wait for ACK counter: " << endl;
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
        //        QCoreApplication::processEvents();
        // cout << readPacket << endl;
        if (pktResponded[0] != 0x02)
        {
            pktResponded.clear();
            pktResponded = com.readPacket();
            usleep(10);
        }
        else
        {
            //  pktResponded = com.readPacket();
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
    qDebug() << "volume dispensed" << p_page_idle->thisMachine->selectedProduct->getVolumeDispensedMl();
    if (p_page_idle->thisMachine->selectedProduct->getVolumeDispensedMl() < MINIMUM_DISPENSE_VOLUME_ML)
    {
        cancelPayment = true;
    }
    if (cancelPayment && (paymentMethod == PAYMENT_TAP_TCP || paymentMethod == PAYMENT_TAP_SERIAL))
    {
        ui->label_indicate_active_spout->hide();
        ui->label_to_refill->hide();
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_finishTransactionMessage, "no_pay");
        p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_CANCEL);

        std::map<std::string, std::string> response;
        qDebug() << "dispense end: tap payment No volume dispensed.";
        // REVERSE PAYMENT.
        if (paymentMethod == PAYMENT_TAP_TCP)
        {
            if (tapPaymentObject.find("SAF_NUM") != tapPaymentObject.end())
            {
                std::cout << "Voiding transaction";
                qDebug() << "SAF_NUM" << QString::fromStdString(tapPaymentObject["SAF_NUM"]);
                tapPaymentObject["CTROUT_SAF"] = tapPaymentObject["SAF_NUM"];
                response = voidTransactionOffline(std::stoi(socketAddr), MAC_LABEL, MAC_KEY, tapPaymentObject["SAF_NUM"]);
            }
            else if (tapPaymentObject.find("CTROUTD") != tapPaymentObject.end())
            {   
                qDebug() << "CTROUTD" << QString::fromStdString(tapPaymentObject["CTROUTD"]);
                response = voidTransaction(std::stoi(socketAddr), MAC_LABEL, MAC_KEY, tapPaymentObject["CTROUTD"]);
                tapPaymentObject["CTROUT_SAF"] = tapPaymentObject["CTROUTD"];
            }
            finishSession(std::stoi(socketAddr), MAC_LABEL, MAC_KEY);
        }
        if (paymentMethod == PAYMENT_TAP_SERIAL)
        {
            com.page_init();
            pktToSend = paymentPacket.reversePurchasePacket();
            if (sendToUX410())
            {
                waitForUX410();
                qDebug() << "Payment Reversed" << endl;
                pktResponded.clear();
                com.flushSerial();
            }
        }
    }
    else if (((paymentMethod == PAYMENT_TAP_TCP || paymentMethod == PAYMENT_TAP_SERIAL)))
    {

        QString base_text = p_page_idle->thisMachine->getTemplateTextByElementNameAndPageAndIdentifier(ui->label_finishTransactionMessage, "display_price");
        ui->label_finishTransactionMessage->setText(base_text.arg(QString::number(current_price, 'f', 2))); // will replace %1 character in string by the provide text
        p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_GENERIC);
        if (paymentMethod == PAYMENT_TAP_TCP)
        {
            if (tapPaymentObject.find("CTROUTD") != tapPaymentObject.end())
            {
                qDebug() << "CTROUTD" << QString::fromStdString(tapPaymentObject["CTROUTD"]);
                tapPaymentObject["CTROUT_SAF"] = tapPaymentObject["CTROUTD"];
                std::map<std::string, std::string> testResponse = capture(std::stoi(socketAddr), MAC_LABEL, MAC_KEY, tapPaymentObject["CTROUTD"], stream.str());
                // tapPaymentObject["AMOUNT"] = stream.str();
            }
            else if (tapPaymentObject.find("SAF_NUM") != tapPaymentObject.end())
            {
                qDebug() << "SAF_NUM" << QString::fromStdString(tapPaymentObject["SAF_NUM"]);
                tapPaymentObject["CTROUT_SAF"] = tapPaymentObject["SAF_NUM"];
                std::map<std::string, std::string> testResponse = editSaf(std::stoi(socketAddr), MAC_LABEL, MAC_KEY, tapPaymentObject["SAF_NUM"], stream.str(), "ELIGIBLE");
                // tapPaymentObject["AMOUNT"] = stream.str();
            }
            tapPaymentObject["STATUS"] = "CAPTURED";

            p_page_idle->thisMachine->getDb()->setPaymentTransaction(tapPaymentObject);

            finishSession(std::stoi(socketAddr), MAC_LABEL, MAC_KEY);
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

QString page_dispenser::getStartDispensingCommand()
{
    // build up command that will be sent to fsm
    QString command = QString::number(p_page_idle->thisMachine->selectedProduct->getSlot());
    command.append(p_page_idle->thisMachine->selectedProduct->getSizeAsChar());
    command.append(SEND_DISPENSE_START);
    return command;
}

void page_dispenser::fsmSendStartDispensing()
{
    QString dispenseCommand = getStartDispensingCommand();
    QString priceCommand = QString::number(p_page_idle->thisMachine->getPriceWithDiscount(p_page_idle->thisMachine->selectedProduct->getBasePrice()));
    QString promoCommand = p_page_idle->thisMachine->getCouponCode();

    QString delimiter = QString("|");
    QString preamble = "Order";
    QString command = preamble + delimiter + dispenseCommand + delimiter + priceCommand + delimiter + promoCommand + delimiter;

    qDebug() << "Send start command to FSM: " << command;
    p_page_idle->thisMachine->dfUtility->send_command_to_FSM(command, true);
    this->isDispensing = true;
    qDebug() << "Dispensing started.";
}

void page_dispenser::fsmSendStopDispensing()
{
    qDebug() << "Send STOP dispensing to fsm";
    this->isDispensing = false;

    QString command = QString::number(p_page_idle->thisMachine->selectedProduct->getSlot());
    command.append(p_page_idle->thisMachine->selectedProduct->getSizeAsChar());
    command.append(SEND_DISPENSE_STOP);
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
//     QString units = p_page_idle->thisMachine->selectedProduct->getUnitsForSelectedSlot();

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
    QString dispenseStatus = status;
    ui->label_dispense_status->setText(dispenseStatus);
    ui->label_dispense_status->hide();

    if (dispenseStatus != previousDispenseStatus)
    {
        qDebug() << "Dispense status received from FSM: " << dispenseStatus;

        if (dispenseStatus == "SLOT_STATE_WARNING_PRIMING")
        {
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_dispense_message, "priming");
            p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_problems, "alert", PAGE_DISPENSER_CSS);
            ui->label_dispense_message->show();
        }
        else if (dispenseStatus == "SLOT_STATE_PROBLEM_EMPTY")
        {
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_dispense_message, "out_of_stock");
            p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_problems, "alert", PAGE_DISPENSER_CSS);
            // p_page_idle->thisMachine->setSlotEnabled(p_page_idle->thisMachine->selectedProduct->getSlot(), false);
            p_page_idle->thisMachine->setSlotEnabled(p_page_idle->thisMachine->getSelectedProduct()->getSlot(), false);
            ui->label_dispense_message->show();
        }
        else if (dispenseStatus == "SLOT_STATE_PROBLEM_NEEDS_ATTENTION")
        {
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_dispense_message, "needs_attention");
            p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_problems, "alert", PAGE_DISPENSER_CSS);
            ui->label_dispense_message->show();
        }
        else if (dispenseStatus == "SLOT_STATE_AVAILABLE")
        {
            p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_problems, "normal", PAGE_DISPENSER_CSS);
            // normal status
            // ui->pushButton_problems->hide();
            ui->label_dispense_message->hide();
            // p_page_idle->thisMachine->setSlotEnabled(p_page_idle->thisMachine->selectedProduct->getSlot(), true);
            p_page_idle->thisMachine->setSlotEnabled(p_page_idle->thisMachine->getSelectedProduct()->getSlot(), true);
        }
        else
        {
        }
    }

    previousDispenseStatus = dispenseStatus;
};

void page_dispenser::updateVolumeDisplayed(double dispensed, bool isFull)
{

    if (p_page_idle->thisMachine->selectedProduct->getVolumeDispensedMl() != dispensed)
    {
        // only reset idle timer if volume has changed.
        resetDispenseTimeout();
    }
    p_page_idle->thisMachine->selectedProduct->setVolumeDispensedMl(dispensed);

    // volumeDispensed = dispensed;
    qDebug() << "Signal: dispensed " << dispensed << " of " << p_page_idle->thisMachine->selectedProduct->getVolumeOfSelectedSize();

    if (p_page_idle->thisMachine->selectedProduct->getVolumeDispensedMl() >= MINIMUM_DISPENSE_VOLUME_ML)
    {

        ui->label_indicate_active_spout->hide();
        updatelabel_volume_dispensed_ml(p_page_idle->thisMachine->selectedProduct->getVolumeDispensedMl());

        double percentage = p_page_idle->thisMachine->selectedProduct->getVolumeDispensedMl() / (p_page_idle->thisMachine->selectedProduct->getVolumeOfSelectedSize()) * 100;
        if (isFull)
        {
            percentage = 100;
        }

        this->ui->label_moving_bottle_fill_effect->move(380, 900 - 3 * percentage);
        // ui->pushButton_problems->move(120, 450);
        // transition from instructions to dispensing at first receival of volume.
        ui->label_background_during_dispense_animation->show();
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
        updateVolumeDisplayed(p_page_idle->thisMachine->selectedProduct->getVolumeOfSelectedSize(), true); // make sure the fill bottle graphics are completed
        transactionLogging += "\n 8: Target Reached - True";
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
        transactionLogging += "\n 7: Cancel Button - True";
        force_finish_dispensing();
    }
}

void page_dispenser::on_pushButton_debug_Button_clicked()
{
    qDebug() << "WARNING: ========= Debug button pressed. Fake dispensing of 100ml ==============";
    updateVolumeDisplayed(300.0, false); // make sure the fill bottle graphics are completed
}

void page_dispenser::on_pushButton_abort_clicked()
{
    qDebug() << "Pressed button abort/complete";

    transactionLogging += "\n 7: Complete Button - True";
    if (p_page_idle->thisMachine->selectedProduct->getVolumeDispensedMl() < MINIMUM_DISPENSE_VOLUME_ML)
    {
        msgBox_abort = new QMessageBox();
        msgBox_abort->setObjectName("msgBox_abort");
        msgBox_abort->setWindowFlags(Qt::FramelessWindowHint); // do not show messagebox header with program name

        if (paymentMethod == PAYMENT_QR || paymentMethod == PAYMENT_TAP_TCP || paymentMethod == PAYMENT_TAP_SERIAL)
        {
            QString searchString = this->objectName() + "->" + msgBox_abort->objectName() + "->" + "qr_tap";
            p_page_idle->thisMachine->setTextToObject(msgBox_abort, p_page_idle->thisMachine->getTemplateText(searchString));
        }
        else
        {
            QString searchString = this->objectName() + "->" + msgBox_abort->objectName() + "->" + "default";
            p_page_idle->thisMachine->setTextToObject(msgBox_abort, p_page_idle->thisMachine->getTemplateText(searchString));
        }

        p_page_idle->thisMachine->addCssClassToObject(msgBox_abort, "msgBoxbutton msgBox", PAGE_DISPENSER_CSS);
        msgBox_abort->setStandardButtons(QMessageBox::Yes | QMessageBox::No);

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

    msgBox_problems = new QMessageBox();
    msgBox_problems->setObjectName("msgBox_problems");
    msgBox_problems->setWindowFlags(Qt::FramelessWindowHint); // do not show messagebox header with program name

    QString client_id = p_page_idle->thisMachine->getClientId();
    if (client_id == "C-1") // good-filling
    {
        QString searchString = this->objectName() + "->" + msgBox_problems->objectName() + "->" + "shopify";
        p_page_idle->thisMachine->setTextToObject(msgBox_problems, p_page_idle->thisMachine->getTemplateText(searchString));
    }
    else if (paymentMethod == "qr" || paymentMethod == PAYMENT_TAP_TCP || paymentMethod == PAYMENT_TAP_SERIAL)
    {
        QString searchString = this->objectName() + "->" + msgBox_problems->objectName() + "->" + "qr_tap";
        p_page_idle->thisMachine->setTextToObject(msgBox_problems, p_page_idle->thisMachine->getTemplateText(searchString));
    }
    else
    {
        QString searchString = this->objectName() + "->" + msgBox_problems->objectName() + "->" + "default";
        p_page_idle->thisMachine->setTextToObject(msgBox_problems, p_page_idle->thisMachine->getTemplateText(searchString));
    }

    p_page_idle->thisMachine->addCssClassToObject(msgBox_problems, "msgBoxbutton msgBox", PAGE_DISPENSER_CSS);
    msgBox_problems->setStandardButtons(QMessageBox::Yes | QMessageBox::No);

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
    }
}
