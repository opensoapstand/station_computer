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
extern std::string CTROUTD;
extern std::string MAC_KEY;
extern std::string MAC_LABEL;
extern std::string AUTH_CODE;
extern std::string SAF_NUM;
extern std::string socketAddr;
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
}

/*
 * Page Tracking reference to Payment page and completed payment
 */
void page_dispenser::setPage(page_qr_payment *page_qr_payment, page_tap_payment *page_tap_payment, page_end *page_end, page_idle *pageIdle, page_sendFeedback *pageFeedback)
{
    this->thanksPage = page_end;
    this->paymentPage = page_qr_payment;
    this->p_page_payment_tap = page_tap_payment;
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
    if (dispenseIdleTimer != nullptr)
    {
        dispenseIdleTimer->stop();
    }

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

    p_page_idle->pageTransition(this, pageToShow);
}
void page_dispenser::showEvent(QShowEvent *event)
{
    p_page_idle->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    qDebug() << "Selected slot: " << QString::number(p_page_idle->selectedProduct->getSlot());
    QWidget::showEvent(event);

    p_page_idle->setTemplateTextToObject(ui->label_to_refill);
    p_page_idle->setTemplateTextToObject(ui->label_instructions_container);
    p_page_idle->setTemplateTextToObject(ui->label_press);
    p_page_idle->setTemplateTextToObject(ui->pushButton_problems);
    p_page_idle->setTemplateTextWithIdentifierToObject(ui->pushButton_abort, "abort");
    p_page_idle->setTemplateTextToObject(ui->label_volume_dispensed);

    QString styleSheet = p_page_idle->getCSS(PAGE_DISPENSER_CSS);
    ui->pushButton_problems->setProperty("class", "normal");
    ui->label_volume_dispensed->setProperty("class", "normal");
    ui->pushButton_problems->setStyleSheet(styleSheet);
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

    // important to set to nullptr, to check at timeout if it was initialized (displayed...) or not.
    msgBox_problems = nullptr;
    msgBox_abort = nullptr;

    this->isDispensing = false;
    askForFeedbackAtEnd = false;

    ui->label_moving_bottle_fill_effect->move(380, 889);
    ui->pushButton_problems->move(120, 40);

    previousDispenseStatus = "NO STATE";


    if (p_page_idle->thisMachine.getDispensersCount() == 1)
    {
        // single spout
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_DISPENSE_INSTRUCTIONS_BACKGROUND_PATH);
    }
    else
    {
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_DISPENSE_INSTRUCTIONS_MULTISPOUT_BACKGROUND_PATH);
    }

    p_page_idle->addCustomerLogoToLabel(ui->label_logo);
    ui->label_logo->hide();

    p_page_idle->addPictureToLabel(ui->label_background_during_dispense_animation, p_page_idle->thisMachine.getTemplatePathFromName(PAGE_DISPENSE_BACKGROUND_PATH));

    p_page_idle->addPictureToLabel(ui->label_moving_bottle_fill_effect, p_page_idle->thisMachine.getTemplatePathFromName(PAGE_DISPENSE_FILL_ANIMATION));
    

    ui->pushButton_abort->show();
    ui->label_press->show();
    ui->label_to_refill->show();
    ui->label_instructions_container->show();
    ui->label_finishTransactionMessage->hide();
    ui->label_background_during_dispense_animation->hide();
    ui->label_moving_bottle_fill_effect->hide();

    ui->label_dispense_message->hide();
    ui->pushButton_problems->show();

    dispenseIdleTimer->start(1000);
    resetDispenseTimeout();

    p_page_idle->selectedProduct->resetVolumeDispensed();
    updatelabel_volume_dispensed_ml(p_page_idle->selectedProduct->getVolumeDispensedMl());

    fsmSendStartDispensing();
}

void page_dispenser::updatelabel_volume_dispensed_ml(double dispensed)
{
    QString dispensedVolumeUnitsCorrected;
    QString units = p_page_idle->selectedProduct->getUnitsForSlot();

    if (units == "oz")
    {
        dispensedVolumeUnitsCorrected = QString::number(ceil(dispensed * (double)ML_TO_OZ * 1.0));
    }
    else
    {
        dispensedVolumeUnitsCorrected = QString::number(ceil(dispensed));
    }

    if (p_page_idle->selectedProduct->getSize() == SIZE_CUSTOM_INDEX)
    {

        double unitprice = p_page_idle->thisMachine.getPriceWithDiscount(p_page_idle->selectedProduct->getBasePrice());
        current_price = dispensed * unitprice;
        ui->label_volume_dispensed_ml->setText(dispensedVolumeUnitsCorrected + " " + units + " ( $" + QString::number(current_price, 'f', 2) + " )");
    }
    else
    {
        QString totalVolume = p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(p_page_idle->selectedProduct->getSize(), true, true);
        ui->label_volume_dispensed_ml->setText(dispensedVolumeUnitsCorrected + " " + units + "/ " + totalVolume);
        current_price = p_page_idle->selectedProduct->getBasePrice();
    }
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
    qDebug() << "volume dispensed" << p_page_idle->selectedProduct->getVolumeDispensedMl();
    if (p_page_idle->selectedProduct->getVolumeDispensedMl() < MINIMUM_DISPENSE_VOLUME_ML && (p_page_idle->selectedProduct->getPaymentMethod()) == "tapTcp")
    {
        p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_finishTransactionMessage, "no_pay");
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_GENERIC);
        std::map<std::string, std::string> response;
        qDebug() << "dispense end: tap payment No volume dispensed.";
        // REVERSE PAYMENT.
        if (SAF_NUM != "")
        {
            std::cout << "Voiding transaction";
            response = voidTransactionOffline(std::stoi(socketAddr), MAC_LABEL, MAC_KEY, SAF_NUM);
        }
        else if (CTROUTD != "")
        {
            response = voidTransaction(std::stoi(socketAddr), MAC_LABEL, MAC_KEY, CTROUTD);
        }
        finishSession(std::stoi(socketAddr), MAC_LABEL, MAC_KEY);
    }
    else if ((p_page_idle->selectedProduct->getPaymentMethod() == "tapTcp") && p_page_idle->selectedProduct->getVolumeDispensedMl() >= MINIMUM_DISPENSE_VOLUME_ML)
    {

        
        QString base_text = p_page_idle->getTemplateTextByElementNameAndPageAndIdentifier(ui->label_finishTransactionMessage, "display_price" );
        ui->label_finishTransactionMessage->setText(base_text.arg(QString::number(current_price, 'f', 2))); // will replace %1 character in string by the provide text
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_GENERIC);
        if (CTROUTD != "")
        {
            std::map<std::string, std::string> testResponse = capture(std::stoi(socketAddr), MAC_LABEL, MAC_KEY, CTROUTD, stream.str());
        }
        else if (SAF_NUM != "")
        {

            std::map<std::string, std::string> testResponse = editSaf(std::stoi(socketAddr), MAC_LABEL, MAC_KEY, SAF_NUM, stream.str(), "ELIGIBLE");
        }
        finishSession(std::stoi(socketAddr), MAC_LABEL, MAC_KEY);
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
    QString command = QString::number(p_page_idle->selectedProduct->getSlot());
    command.append(p_page_idle->selectedProduct->getSizeAsChar());
    command.append(SEND_DISPENSE_START);
    return command;
}

void page_dispenser::fsmSendStartDispensing()
{
    QString dispenseCommand = getStartDispensingCommand();
    QString priceCommand = QString::number(p_page_idle->thisMachine.getPriceWithDiscount(p_page_idle->selectedProduct->getBasePrice()));
    QString promoCommand = p_page_idle->thisMachine.getPromoCode();

    QString delimiter = QString("|");
    QString preamble = "Order";
    QString command = preamble + delimiter + dispenseCommand + delimiter + priceCommand + delimiter + promoCommand + delimiter;

    qDebug() << "Send start command to FSM: " << command;
    p_page_idle->dfUtility->send_command_to_FSM(command);
    this->isDispensing = true;
    qDebug() << "Dispensing started.";
}

void page_dispenser::fsmSendStopDispensing()
{
    qDebug() << "Send STOP dispensing to fsm";
    this->isDispensing = false;

    QString command = QString::number(p_page_idle->selectedProduct->getSlot());
    command.append(p_page_idle->selectedProduct->getSizeAsChar());
    command.append(SEND_DISPENSE_STOP);
    p_page_idle->dfUtility->send_command_to_FSM(command);
}

// void page_dispenser::fsmSendPrice()
// {
//     qDebug() << "Send Price to fsm";
//     std::string prefix = "$";
//     QString command = QString::fromStdString(prefix);
//     command.append(QString::number(this->p_page_idle->selectedProduct->getSelectedPriceCorrected()));
//     p_page_idle->dfUtility->send_command_to_FSM(command);
// }

// void page_dispenser::fsmSendPromo()
// {
//     qDebug() << "Send Promo to fsm";
//     std::string prefix = "Promo:";
//     QString command = QString::fromStdString(prefix);
//     command.append(this->p_page_idle->selectedProduct->getPromoCode());
//     p_page_idle->dfUtility->send_command_to_FSM(command);
// }

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
//     QString units = p_page_idle->selectedProduct->getUnitsForSelectedSlot();

//     return df_util::getConvertedStringVolumeFromMl(volumeDispensed, units, false, false);
// }

// QString page_dispenser::getPromoCodeUsed()
// {
//     QString promoCode = p_page_idle->getPromoCode();

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
            p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_dispense_message, "priming");
            p_page_idle->addCssClassToObject(ui->pushButton_problems, "alert", PAGE_DISPENSER_CSS);
            ui->label_dispense_message->show();
        }
        else if (dispenseStatus == "SLOT_STATE_PROBLEM_EMPTY")
        {
            p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_dispense_message, "out_of_stock");
            p_page_idle->addCssClassToObject(ui->pushButton_problems, "alert", PAGE_DISPENSER_CSS);
            ui->label_dispense_message->show();
        }
        else if (dispenseStatus == "SLOT_STATE_PROBLEM_NEEDS_ATTENTION")
        {
            p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_dispense_message, "needs_attention");
            p_page_idle->addCssClassToObject(ui->pushButton_problems, "alert", PAGE_DISPENSER_CSS);
            ui->label_dispense_message->show();
        }
        else if (dispenseStatus == "SLOT_STATE_AVAILABLE")
        {
            p_page_idle->addCssClassToObject(ui->pushButton_problems, "normal", PAGE_DISPENSER_CSS);
            // normal status
            // ui->pushButton_problems->hide();
            ui->label_dispense_message->hide();
        }
        else
        {
        }
    }

    previousDispenseStatus = dispenseStatus;
};

void page_dispenser::updateVolumeDisplayed(double dispensed, bool isFull)
{

    if (p_page_idle->selectedProduct->getVolumeDispensedMl() != dispensed)
    {
        // only reset idle timer if volume has changed.
        resetDispenseTimeout();
    }
    p_page_idle->selectedProduct->setVolumeDispensedMl(dispensed);

    // volumeDispensed = dispensed;
    qDebug() << "Signal: dispensed " << dispensed << " of " << p_page_idle->selectedProduct->getVolumeOfSelectedSize();

    if (p_page_idle->selectedProduct->getVolumeDispensedMl() >= MINIMUM_DISPENSE_VOLUME_ML)
    {

        updatelabel_volume_dispensed_ml(p_page_idle->selectedProduct->getVolumeDispensedMl());

        double percentage = p_page_idle->selectedProduct->getVolumeDispensedMl() / (p_page_idle->selectedProduct->getVolumeOfSelectedSize()) * 100;
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

        p_page_idle->setTemplateTextWithIdentifierToObject(ui->pushButton_abort, "complete");
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
        updateVolumeDisplayed(p_page_idle->selectedProduct->getVolumeOfSelectedSize(), true); // make sure the fill bottle graphics are completed
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
    if (p_page_idle->selectedProduct->getVolumeDispensedMl() < MINIMUM_DISPENSE_VOLUME_ML)
    {
        msgBox_abort = new QMessageBox();
        msgBox_abort->setObjectName("msgBox_abort");
        msgBox_abort->setWindowFlags(Qt::FramelessWindowHint); // do not show messagebox header with program name
        QString payment = p_page_idle->selectedProduct->getPaymentMethod();

        if (payment == "qr" || payment == "tapTcp")
        {
            QString searchString = this->objectName() + "->" + msgBox_abort->objectName() + "->" + "qr_tap";
            p_page_idle->setTextToObject(msgBox_abort, p_page_idle->getTemplateText(searchString));
        }
        else
        {
            QString searchString = this->objectName() + "->" + msgBox_abort->objectName() + "->" + "default";
            p_page_idle->setTextToObject(msgBox_abort, p_page_idle->getTemplateText(searchString));
        }

        p_page_idle->addCssClassToObject(msgBox_abort, "msgBoxbutton msgBox", PAGE_DISPENSER_CSS);
        msgBox_abort->setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        int ret = msgBox_abort->exec();
        bool success;
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

    QString payment = p_page_idle->selectedProduct->getPaymentMethod();

    if (payment == "qr" || payment == "tapTcp")
    {
        QString searchString = this->objectName() + "->" + msgBox_problems->objectName() + "->" + "qr_tap";
        p_page_idle->setTextToObject(msgBox_problems, p_page_idle->getTemplateText(searchString));
    }
    else
    {
        QString searchString = this->objectName() + "->" + msgBox_problems->objectName() + "->" + "default";
        p_page_idle->setTextToObject(msgBox_problems, p_page_idle->getTemplateText(searchString));
    }

    p_page_idle->addCssClassToObject(msgBox_problems, "msgBoxbutton msgBox", PAGE_DISPENSER_CSS);
    msgBox_problems->setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    int ret = msgBox_problems->exec();
    bool success;
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
        p_page_idle->dfUtility->send_command_to_FSM(SEND_REPAIR_PCA);
        break;
    }
    }
}
