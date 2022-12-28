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

extern QString transactionLogging;
// CTOR
page_dispenser::page_dispenser(QWidget *parent) : QWidget(parent),
                                                  ui(new Ui::page_dispenser)
{

    this->isDispensing = false;
    ui->setupUi(this);

    // ui->finish_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    // ui->abortButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->abortButton->setStyleSheet("QPushButton { color:#FFFFFF;background-color: #5E8580; border: 1px solid #3D6675;box-sizing: border-box;border-radius: 20px;}");

    dispenseIdleTimer = new QTimer(this);
    dispenseIdleTimer->setInterval(1000);
    connect(dispenseIdleTimer, SIGNAL(timeout()), this, SLOT(onDispenseIdleTick()));
    
}

/*
 * Page Tracking reference to Payment page and completed payment
 */
void page_dispenser::setPage(page_payment *page_payment, pagethankyou *pageThankYou, page_idle *pageIdle)
{
    this->thanksPage = pageThankYou;
    this->paymentPage = page_payment;
    this->p_page_idle = pageIdle;
    selectedProductOrder = p_page_idle->currentProductOrder;

    // p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_DISPENSE_INSTRUCTIONS_BACKGROUND_PATH);

    // QPixmap background(PAGE_DISPENSE_INSTRUCTIONS_BACKGROUND_PATH);
    // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    // QPalette palette;
    // palette.setBrush(QPalette::Background, background);
    // this->setPalette(palette);


}

// DTOR
page_dispenser::~page_dispenser()
{
    delete ui;
}

void page_dispenser::showEvent(QShowEvent *event)
{
    this->isDispensing = false;
    qDebug() << "<<<<<<< Page Enter: Dispenser >>>>>>>>>";
    
    QWidget::showEvent(event);

    qDebug() << "selected slot: " << QString::number(selectedProductOrder->getSelectedSlot());

    qDebug() << "db check dispense buttons count:";
    DbManager db(DB_PATH);
    
    transactionLogging +="\n Dispense Page Shown to the user";
    qDebug() << transactionLogging;

    int button_count = db.getDispenseButtonCount();
    db.closeDB();

    if (button_count==1){
        // single spout 
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_DISPENSE_INSTRUCTIONS_BACKGROUND_PATH);
    }else{
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_DISPENSE_INSTRUCTIONS_MULTISPOUT_BACKGROUND_PATH);

    }

#ifdef ENABLE_DYNAMIC_UI
    p_page_idle->addPictureToLabel(ui->dispense_bottle_label, p_page_idle->getTemplatePathFromName(PAGE_DISPENSE_BACKGROUND_PATH));
#else
    p_page_idle->addPictureToLabel(ui->dispense_bottle_label, PAGE_DISPENSE_BACKGROUND_PATH);

#endif
    ui->dispense_bottle_label->hide();
    ui->fill_animation_label->hide();

    startDispensing();
    ui->abortButton->setText("cancel");
    ui->abortButton->raise();

    if (nullptr == dispenseIdleTimer)
    {
        dispenseIdleTimer = new QTimer(this);
        dispenseIdleTimer->setInterval(1000);
        connect(dispenseIdleTimer, SIGNAL(timeout()), this, SLOT(onDispenseIdleTick()));
    }

    dispenseIdleTimer->start(1000);
    _dispenseIdleTimeoutSec = 120;
}

bool page_dispenser::sendToUX410()
{
    int waitForAck = 0;
    while (waitForAck < 3)
    {
        cout << "Wait for ACK counter: " << waitForAck << endl;
        qDebug() << "Wait for ACK counter: " << endl;
        // sleep(1);
        com.sendPacket(pktToSend, uint(pktToSend.size()));
        std::cout << "sendtoUX410 Electronic Card Reader: " << paymentPacket.getSendPacket() << endl;

        // read back what is responded
        pktResponded = com.readForAck();
        readPacket.packetReadFromUX(pktResponded);
        pktResponded.clear();
        waitForAck++;

        cout << "Waiting for TAP in dispenser" << endl;
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
        // sleep(1);
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

    if (volumeDispensed == 0 && (selectedProductOrder->getSelectedPaymentMethod()) == "tap")
    {
        qDebug() << "dispense end: tap payment No volume dispensed.";
        // REVERSE PAYMENT
        com.page_init();
        pktToSend = paymentPacket.reversePurchasePacket();
        if (sendToUX410())
        {
            waitForUX410();
            qDebug() << "Tap Payment Reversed";
            pktResponded.clear();
            com.flushSerial(); 
        }
    }
    else if ((selectedProductOrder->getSelectedPaymentMethod() == "tap") && volumeDispensed != 0)
    {
        // qDebug() << "dispense end: tap payment. Some volume dispensed.";
        // QMessageBox msgBox;
        // msgBox.setText("Complete!");
        // msgBox.setInformativeText("Would you like your receipt emailed to you?");
        // msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        // int ret = msgBox.exec();

        // switch (ret)
        // {
        // case QMessageBox::Yes:
        //     //            qDebug() << "This person wants their receipt emailed";
        //     break;
        // case QMessageBox::No:
        //     //            qDebug() << "This person doesn't want a receipt";
        //     break;
        // default:
        //     break;
        // }
    }
    stopDispenseTimer();
    // thanksPage->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, thanksPage);
    qDebug() << "Finished dispense admin handling";
}

void page_dispenser::force_finish_dispensing()
{
    fsmSendStopDispensing();
    dispensing_end_admin();
}

void page_dispenser::startDispensing()
{
    volumeDispensed = 0;
    targetVolume = selectedProductOrder->getSelectedVolume();

    fsmSendStartDispensing();
    fsmSendPrice();
    fsmSendPromo();
}

void page_dispenser::fsmSendStartDispensing()
{
    qDebug() << "Send Start dispensing to fsm";
    QString command = QString::number(selectedProductOrder->getSelectedSlot());
    command.append(selectedProductOrder->getSelectedSizeAsChar());
    command.append(SEND_DISPENSE_START);

    p_page_idle->dfUtility->send_command_to_FSM(command);

    this->isDispensing = true;
    

}

void page_dispenser::fsmSendStopDispensing()
{
    qDebug() << "Send STOP dispensing to fsm";
    this->isDispensing = false;

    QString command = QString::number(this->selectedProductOrder->getSelectedSlot());
    command.append(selectedProductOrder->getSelectedSizeAsChar());
    command.append(SEND_DISPENSE_STOP);
    p_page_idle->dfUtility->send_command_to_FSM(command);
}

void page_dispenser::fsmSendPrice()
{
    qDebug() << "Send Price to fsm";
    std::string prefix = "$";
    QString command = QString::fromStdString(prefix);
    command.append(QString::number(this->selectedProductOrder->getSelectedPriceCorrected()));
    p_page_idle->dfUtility->send_command_to_FSM(command);
}


void page_dispenser::fsmSendPromo()
{
    qDebug() << "Send Promo to fsm";
    std::string prefix = "Promo:";
    QString command = QString::fromStdString(prefix);
    command.append(this->selectedProductOrder->getPromoCode());
    p_page_idle->dfUtility->send_command_to_FSM(command);
}


// void page_dispenser::onRinseTimerTick()
// {
// }

void page_dispenser::stopDispenseTimer()
{
    this->isDispensing = false;
    //    qDebug() << "page_dispenser: Stop Timers" << endl;
    if (dispenseIdleTimer != nullptr)
    {
        dispenseIdleTimer->stop();
    }
    dispenseIdleTimer = nullptr;
}

void page_dispenser::onDispenseIdleTick()
{
    if (--_dispenseIdleTimeoutSec >= 0)
    {
        //        qDebug() << "page_dispenser: Idle Tick Down: " << _dispenseIdleTimeoutSec << endl;
    }
    else
    {
        //        qDebug() << "Timer Done!" << _dispenseIdleTimeoutSec << endl;
        //        dispenseIdleTimer->stop();
        qDebug() << "Dispensing timeout. End dispensing." << endl;
        force_finish_dispensing();
    }
}

QString page_dispenser::getMostRecentDispensed()
{
    QString units = selectedProductOrder->getUnitsForSelectedSlot();

    return df_util::getConvertedStringVolumeFromMl(volumeDispensed, units, false, false);
    // return volumeDispensed;
}

QString page_dispenser::getPromoCodeUsed()
{
    QString promoCode = selectedProductOrder->getPromoCode();

    return promoCode;
}

void page_dispenser::resetDispenseTimeout(void)
{
    _dispenseIdleTimeoutSec = 30;
}

void page_dispenser::updateVolumeDisplayed(double dispensed, bool isFull)
{
    //  qDebug() << "vol dispesnne " << dispensed;
    //  qDebug() << "vol is fulll  " << isFull;
    if (this->isDispensing)
    {
        // if (dispensed > 0.01){
        ui->abortButton->setText("complete");
        // }

        // qDebug() << "Signal: update vol in dispenser!" ;
        qDebug() << "Signal: dispensed " << dispensed << " of " << this->targetVolume;
        resetDispenseTimeout();

        volumeDispensed = dispensed;

        double percentage = dispensed / this->targetVolume * 100;
        if (isFull)
        {
            percentage = 100;
        }

#ifdef ENABLE_DYNAMIC_UI
        this->ui->fill_animation_label->move(380, 900 - 3 * percentage);
#else
        this->ui->fill_animation_label->move(380, 590 - 3 * percentage);
#endif

        ui->dispense_bottle_label->show();
        ui->fill_animation_label->show();
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
        // qDebug() << "Signal: Target volume reached."  << endl;
        updateVolumeDisplayed(1.0, true); // make sure the fill bottle graphics are completed
        transactionLogging += "\n Target Volume Reached";
        dispensing_end_admin();
        // qDebug() << "Finish dispense end admin."  << endl;
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
        // qDebug() << "Signal: Target volume reached."  << endl;
        updateVolumeDisplayed(1.0, true); // make sure the fill bottle graphics are completed

        dispensing_end_admin();
        // qDebug() << "Finish dispense end admin."  << endl;
    }
    else
    {
        qDebug() << "No flow abort received while not dispensing.";
    }
}

void page_dispenser::on_abortButton_clicked()
{
    qDebug() << "Pressed dispense complete.";
    transactionLogging += "\n Customer pressed complete button on screen";
    if (this->isDispensing)
    {
        force_finish_dispensing();
    }
}

void page_dispenser::on_cancelButton_clicked()
{
    qDebug() << "Pressed cancel dispensing.";
    if (this->isDispensing)
    {
        transactionLogging += "\n Customer pressed Cancel button on screen";
        force_finish_dispensing();
    }
}
