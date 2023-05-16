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
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "page_dispenser.h"
#include "ui_page_dispenser.h"
#include "includefiles.h"
#include "page_idle.h"
#include "pagethankyou.h"
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
    ui->setupUi(this);

    ui->finishTransactionMessage->setStyleSheet(
        "QLabel {"
        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 100;"
        "font-size: 52px;"
        "text-align: centre;"
        "line-height: auto;"
        "letter-spacing: 0px;"
        "qproperty-alignment: AlignCenter;"
        "border-radius: 20px;"
        "color: #5e8580;"
        "border: none;"
        "}");

    // ui->finish_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->debug_Button->setStyleSheet("QPushButton { color:#5E8580; background-color: transparent; border: 5px;border-color:#5E8580; }");
    ui->abortButton->setStyleSheet(
        "QPushButton {"
        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 100;"
        "background-color: #5E8580;"
        "font-size: 42px;"
        "text-align: centre;"
        "line-height: auto;"
        "letter-spacing: 0px;"
        "qproperty-alignment: AlignCenter;"
        "border-radius: 20px;"
        "color: white;"
        "border: none;"
        "}");

    // ui->abortButton->setStyleSheet("QPushButton { color:#FFFFFF;background-color: #5E8580; border: 1px solid #3D6675;box-sizing: border-box;border-radius: 20px;}");
    QString volumeDispensedStylesheet = "QLabel {"

                                        "font-family: 'Brevia';"
                                        "font-style: normal;"
                                        "font-weight: 100;"
                                        "font-size: 42px;"
                                        "text-align: centre;"
                                        "line-height: auto;"
                                        "letter-spacing: 0px;"
                                        "qproperty-alignment: AlignCenter;"
                                        "border-radius: 20px;"
                                        "color: #5e8580;"
                                        "border: none;"
                                        "}";
    ui->volumeDispensedLabel->setStyleSheet(volumeDispensedStylesheet);
    ui->label_to_refill->setStyleSheet(
        "QLabel {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 85px;"
        "background-color: transparent;"
        "border: 0px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "color: #FFFFFF;"
        "text-align: center;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");

    ui->label_instructions_container->setStyleSheet(
        "QLabel {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 55px;"
        "background-color: transparent;"
        "border: 0px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "color: #FFFFFF;"
        "text-align: center;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");

    ui->label_press->setStyleSheet(
        "QLabel {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 55px;"
        "background-color: transparent;"
        "border: 0px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "color: #FFFFFF;"
        "text-align: center;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");

    ui->finishTransactionMessage->hide();
    ui->label_to_refill->setText("to refill");
    ui->label_instructions_container->setText("bring container to nozzle");
    ui->label_press->setText("press and hold <br>the button");

    ui->label_dispense_message->setStyleSheet(
        "QLabel {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 42px;"
        "background-color: transparent;"
        "border: 0px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "color: #FFFFFF;"
        "text-align: center;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");
    ui->button_problems->setStyleSheet(
        "QPushButton {"
        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "background-color: #5E8580;"
        "font-size: 42px;"
        "text-align: centre;"
        "line-height: auto;"
        "letter-spacing: 0px;"
        "qproperty-alignment: AlignCenter;"
        "border-radius: 20px;"
        "color: white;"
        "border: none;"
        "}");
    ui->button_report->setStyleSheet(
        "QPushButton {"
        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "background-color: #5E8580;"
        "font-size: 42px;"
        "text-align: centre;"
        "line-height: auto;"
        "letter-spacing: 0px;"
        "qproperty-alignment: AlignCenter;"
        "border-radius: 20px;"
        "color: white;"
        "border: none;"
        "}");

    ui->abortButton->raise();
    ui->button_problems->raise();
    ui->button_problems->setText("Tap here if you notice a problem.");
    ui->button_report->setText("Report");
    ui->label_volume_dispensed->setStyleSheet(volumeDispensedStylesheet);
    dispenseIdleTimer = new QTimer(this);
    dispenseIdleTimer->setInterval(1000);
    connect(dispenseIdleTimer, SIGNAL(timeout()), this, SLOT(onDispenseIdleTick()));
}

/*
 * Page Tracking reference to Payment page and completed payment
 */
void page_dispenser::setPage(page_qr_payment *page_qr_payment, page_tap_payment *page_tap_payment, pagethankyou *pageThankYou, page_idle *pageIdle, page_sendFeedback *pageFeedback)
{
    this->thanksPage = pageThankYou;
    this->paymentPage = page_qr_payment;
    this->paymentTapPage = page_tap_payment;
    this->p_page_idle = pageIdle;
    this->feedbackPage = pageFeedback;
    // selectedProductOrder = p_page_idle->selectedProduct;
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

    if (msgBox != nullptr)
    {
        msgBox->hide();
        msgBox->deleteLater();
    }
    if (msgBox2 != nullptr)
    {
        msgBox2->hide();
        msgBox2->deleteLater();
    }

    p_page_idle->pageTransition(this, pageToShow);
}
void page_dispenser::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Dispenser >>>>>>>>>";
    qDebug() << "Selected slot: " << QString::number(selectedProductOrder->getSelectedSlot());
    transactionLogging += "\n 6: Station Unlocked - True";
    QWidget::showEvent(event);

    // important to set to nullptr, to check at timeout if it was initialized (displayed...) or not.
    msgBox = nullptr;
    msgBox2 = nullptr;

    this->isDispensing = false;
    askForFeedbackAtEnd = false;

    ui->fill_animation_label->move(380, 889);
    ui->button_problems->move(120,40);

    qDebug() << "db check dispense buttons count:";
    DbManager db(DB_PATH);
    int button_count = db.getDispenseButtonCount();
    db.closeDB();

    if (button_count == 1)
    {
        // single spout
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_DISPENSE_INSTRUCTIONS_BACKGROUND_PATH);
    }
    else
    {
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_DISPENSE_INSTRUCTIONS_MULTISPOUT_BACKGROUND_PATH);
    }
    // p_page_idle->addCompanyLogoToLabel(ui->label_logo);
    
    p_page_idle->addPictureToLabel(ui->dispense_bottle_label, p_page_idle->getTemplatePathFromName(PAGE_DISPENSE_BACKGROUND_PATH));

    ui->abortButton->setText("Abort");
    ui->abortButton->show();
    ui->label_press->show();
    ui->label_to_refill->show();
    ui->label_instructions_container->show();
    ui->finishTransactionMessage->hide();
    ui->dispense_bottle_label->hide();
    ui->fill_animation_label->hide();

    ui->label_dispense_message->hide();
    ui->button_problems->show();
    ui->button_report->hide();

    dispenseIdleTimer->start(1000);
    resetDispenseTimeout();

    selectedProductOrder->resetSelectedVolumeDispensed();
    updateVolumeDispensedLabel(selectedProductOrder->getSelectedVolumeDispensedMl());

    QString dispenseCommand = getStartDispensingCommand();
    QString priceCommand = QString::number(this->selectedProductOrder->getSelectedPriceCorrected());
    QString promoCommand = this->selectedProductOrder->getPromoCode();

    QString delimiter = QString("|");
    QString preamble = "Order";
    QString command = preamble + delimiter + dispenseCommand + delimiter + priceCommand + delimiter + promoCommand + delimiter;

    qDebug() << "Send start command to FSM: " << command;
    p_page_idle->dfUtility->send_command_to_FSM(command);
    this->isDispensing = true;
    qDebug() << "Dispensing started.";
}

void page_dispenser::updateVolumeDispensedLabel(double dispensed)
{
    QString dispensedVolumeUnitsCorrected;
    QString units = selectedProductOrder->getUnitsForSelectedSlot();

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

        double unitprice = p_page_idle->selectedProduct->getPriceCorrected();
        current_price = dispensed * unitprice;
        ui->volumeDispensedLabel->setText(dispensedVolumeUnitsCorrected + " " + units + " ( $" + QString::number(current_price, 'f', 2) + " )");
    }
    else
    {
        QString totalVolume = selectedProductOrder->getSizeToVolumeWithCorrectUnitsForSelectedSlot(selectedProductOrder->getSelectedSize(), true, true);
        ui->volumeDispensedLabel->setText(dispensedVolumeUnitsCorrected + " " + units + "/ " + totalVolume);
        current_price = selectedProductOrder->getSelectedPrice();
    }
}

/*
 * Page Tracking reference to Payment page and completed payment
 */
void page_dispenser::dispensing_end_admin()
{
    qDebug() << "Dispense end admin start";
    this->isDispensing = false;
    ui->dispense_bottle_label->hide();
    ui->fill_animation_label->hide();
    ui->abortButton->hide();
    ui->finishTransactionMessage->show();
    ui->finishTransactionMessage->raise();

    double price = current_price;

    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << price;
    qDebug() << "Minimum volume dispensed" << MINIMUM_DISPENSE_VOLUME_ML;
    qDebug() << "volume dispensed" << selectedProductOrder->getSelectedVolumeDispensedMl();
    if (selectedProductOrder->getSelectedVolumeDispensedMl() < MINIMUM_DISPENSE_VOLUME_ML && (selectedProductOrder->getSelectedPaymentMethod()) == "tap")
    {
        ui->finishTransactionMessage->setText("Voiding payment");
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
    else if ((selectedProductOrder->getSelectedPaymentMethod() == "tap") && selectedProductOrder->getSelectedVolumeDispensedMl() >= MINIMUM_DISPENSE_VOLUME_ML)
    {
        ui->finishTransactionMessage->setText("Capturing payment: $" + QString::number(current_price, 'f', 2));
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
    // ui->finishTransactionMessage->setText("");
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
    QString command = QString::number(selectedProductOrder->getSelectedSlot());
    command.append(selectedProductOrder->getSelectedSizeAsChar());
    command.append(SEND_DISPENSE_START);
    return command;
}

void page_dispenser::fsmSendStartDispensing()
{
    qDebug() << "Send Start dispensing to fsm";
    p_page_idle->dfUtility->send_command_to_FSM(getStartDispensingCommand());

    this->isDispensing = true;
}

void page_dispenser::fsmSendStopDispensing()
{
    qDebug() << "Send STOP dispensing to fsm";
    this->isDispensing = false;

    QString command = QString::number(p_page_idle->selectedProduct->getSlot());
    command.append(selectedProductOrder->getSelectedSizeAsChar());
    command.append(SEND_DISPENSE_STOP);
    p_page_idle->dfUtility->send_command_to_FSM(command);
}


// void page_dispenser::fsmSendPrice()
// {
//     qDebug() << "Send Price to fsm";
//     std::string prefix = "$";
//     QString command = QString::fromStdString(prefix);
//     command.append(QString::number(this->selectedProductOrder->getSelectedPriceCorrected()));
//     p_page_idle->dfUtility->send_command_to_FSM(command);
// }

// void page_dispenser::fsmSendPromo()
// {
//     qDebug() << "Send Promo to fsm";
//     std::string prefix = "Promo:";
//     QString command = QString::fromStdString(prefix);
//     command.append(this->selectedProductOrder->getPromoCode());
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
//     QString units = selectedProductOrder->getUnitsForSelectedSlot();

//     return df_util::getConvertedStringVolumeFromMl(volumeDispensed, units, false, false);
// }

QString page_dispenser::getPromoCodeUsed()
{
    QString promoCode = selectedProductOrder->getPromoCode();

    return promoCode;
}

void page_dispenser::fsmReceiveDispenseRate(double flowrate)
{
    qDebug() << "Dispense flow rate received from FSM: " << QString::number(flowrate, 'f', 2);
    ui->label_dispense_flowrate->setText(QString::number(flowrate, 'f', 2) + "ml/s");
    ui->label_dispense_flowrate->hide();
};

void page_dispenser::fsmReceiveDispenseStatus(QString status)
{
    QString dispenseStatus = status;
    qDebug() << "Dispense status received from FSM: " << dispenseStatus;
    ui->label_dispense_status->setText(dispenseStatus);
    ui->label_dispense_status->hide();

    if (dispenseStatus == "FLOW_STATE_NOT_PUMPING_NOT_DISPENSING" || dispenseStatus == "FLOW_STATE_PRIME_FAIL_OR_EMPTY" || dispenseStatus == "FLOW_STATE_RAMP_UP" )
    {
        // stable status. do not change button visibility.
    }
    else if (dispenseStatus == "FLOW_STATE_EMPTY")
    {
        ui->label_dispense_message->show();
        ui->label_dispense_message->setText("It appears we're out of stock.\nTap the problem button in case of other issues.");
    }
    else if (dispenseStatus == "FLOW_STATE_PRIME_FAIL_OR_EMPTY")
    {
        ui->label_dispense_message->show();
        ui->label_dispense_message->setText("We can't get the dispensing started.\nWe're empty or the pump needs help to prime.\nTap the problem button in case of other issues.");
    }

    else if (dispenseStatus == "FLOW_STATE_PRIMING_OR_EMPTY")
    {
        ui->label_dispense_message->show();
        ui->label_dispense_message->setText("Please keep the button pressed.\nfor up to 15 seconds\nbefore the product starts dispensing.");
    }
    else if (dispenseStatus == "FLOW_STATE_DISPENSING")
    {
        // normal status
        // ui->button_problems->hide();
        ui->label_dispense_message->hide();
    }
    else
    {
    }
};

void page_dispenser::updateVolumeDisplayed(double dispensed, bool isFull)
{

    if (selectedProductOrder->getSelectedVolumeDispensedMl() != dispensed)
    {
        // only reset idle timer if volume has changed.
        resetDispenseTimeout();
    }
    selectedProductOrder->setSelectedVolumeDispensedMl(dispensed);

    // volumeDispensed = dispensed;
    qDebug() << "Signal: dispensed " << dispensed << " of " << selectedProductOrder->getSelectedVolume();

    if (selectedProductOrder->getSelectedVolumeDispensedMl() >= MINIMUM_DISPENSE_VOLUME_ML)
    {

        updateVolumeDispensedLabel(selectedProductOrder->getSelectedVolumeDispensedMl());

        double percentage = selectedProductOrder->getSelectedVolumeDispensedMl() / (selectedProductOrder->getSelectedVolume()) * 100;
        if (isFull)
        {
            percentage = 100;
        }

        this->ui->fill_animation_label->move(380, 900 - 3 * percentage);
        // ui->button_problems->move(120, 450);
        // transition from instructions to dispensing at first receival of volume.
        ui->dispense_bottle_label->show();
        ui->label_press->hide();
        ui->label_to_refill->hide();
        ui->label_instructions_container->hide();

        ui->abortButton->setText("Complete");
        ui->fill_animation_label->show();
        ui->abortButton->raise();
        ui->button_problems->raise();
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
        this->isDispensing = false;
        updateVolumeDisplayed(selectedProductOrder->getSelectedVolume(), true); // make sure the fill bottle graphics are completed
        transactionLogging += "\n 8: Target Reached - True";
        dispensing_end_admin();
        qDebug() << "Controller msg: Target reached.";
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

void page_dispenser::on_abortButton_clicked()
{
    qDebug() << "Pressed button abort/complete";

    transactionLogging += "\n 7: Complete Button - True";
    if (selectedProductOrder->getSelectedVolumeDispensedMl() < MINIMUM_DISPENSE_VOLUME_ML)
    {
        msgBox = new QMessageBox();
        msgBox->setWindowFlags(Qt::FramelessWindowHint); // do not show messagebox header with program name
        QString payment = selectedProductOrder->getSelectedPaymentMethod();
        if (payment == "qr" || payment == "tap")
        {
            msgBox->setText("<p align=center><br><br>Are you sure, you want to cancel?<br><br>To dispense, please press the green lit button on the machine. \
                                If you press Yes, you will not be charged for the order.<br></p>");
        }
        else
        {
            msgBox->setText("<p align=center><br><br>Are you sure, you want to cancel?<br><br>To dispense, please press the green lit button on the machine.<br></p>");
        }
        msgBox->setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px; font-weight: bold; font-style: normal;  font-family: 'Montserrat';} QPushButton{font-size: 24px; min-width: 300px; min-height: 300px;}");

        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox->exec();
        bool success;
        // qDebug() << "***********************************";
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

void page_dispenser::on_cancelButton_clicked()
{
    qDebug() << "Pressed cancel dispensing.";
    if (this->isDispensing)
    {
        transactionLogging += "\n 7: Cancel Button - True";
        force_finish_dispensing();
    }
}

void page_dispenser::on_debug_Button_clicked()
{
    qDebug() << "WARNING: ========= Debug button pressed. Fake dispensing of 100ml ==============";
    updateVolumeDisplayed(300.0, false); // make sure the fill bottle graphics are completed
}

void page_dispenser::on_button_problems_clicked()
{

    msgBox2 = new QMessageBox();
    msgBox2->setWindowFlags(Qt::FramelessWindowHint); // do not show messagebox header with program name
    QString payment = selectedProductOrder->getSelectedPaymentMethod();
    QString base = "If the pump is working and you tried to dispense for more than 15s without success, the container is probably empty or the pump is not primed. Seek assistance or report the issue. <br> <br> If no green light is on at any dispenser buttons, please press no and check again as the software will attempt to repair the issue. <br> <br> Are you sure you want to stop dispensing and go to the report page?<br>";

    if (payment == "qr" || payment == "tap")
    {

        msgBox2->setText("<p align=center><br><br>" + base + "<br><br>You will only be charged for the dispensed amount<br></p>");
    }
    else
    {
        msgBox2->setText("<p align=center><br>" + base + "</p>");
    }

    msgBox2->setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px; font-weight: bold; font-style: normal;  font-family: 'Montserrat';} QPushButton{font-size: 24px; min-width: 300px; min-height: 300px;}");

    msgBox2->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int ret = msgBox2->exec();
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

void page_dispenser::on_button_report_clicked()
{
}
