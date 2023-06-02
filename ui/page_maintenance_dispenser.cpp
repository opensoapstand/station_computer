#include "page_maintenance_dispenser.h"
#include "ui_page_maintenance_dispenser.h"
#include "page_idle.h"

#include <QInputDialog>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QQuickView>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QSlider>

// CTOR
page_maintenance_dispenser::page_maintenance_dispenser(QWidget *parent) : QWidget(parent),
                                                                          ui(new Ui::page_maintenance_dispenser)
{
    volume_per_tick_buffer = 0.0;
    // Fullscreen background setup
    ui->setupUi(this);

    QPalette palette;
    palette.setBrush(QPalette::Background, Qt::white);
    this->setPalette(palette);

    ui->pumpLabel->setText("Pump manual mode OFF.");
    ui->calibration_instructions_label->setText("Flowsensor calibration instructions:\n1.Enable the pump\n2.Take a measuring cup and dispense until the 1liter mark\n3.Check the calibration value\n4.Update the calibration value if different");
    ui->pushButton_pump->setText("ENABLE PUMP");


    maintainProductPageEndTimer = new QTimer(this);
    maintainProductPageEndTimer->setInterval(1000);
    connect(maintainProductPageEndTimer, SIGNAL(timeout()), this, SLOT(onMaintainProductPageTimeoutTick()));

    // maintainProductPageEndTimer->start(1000); // best to disable timeout. Maintenance page can be used as a permanent page in industrial or maintenance situations.

    dispenseTimer = new QTimer(this);
    dispenseTimer->setInterval(100);
    connect(dispenseTimer, SIGNAL(timeout()), this, SLOT(onDispenseTimerTick()));

    connect(ui->pwmSlider, SIGNAL(valueChanged(int)), this, SLOT(pwmSliderMoved(int)));

    connect(ui->buttonGroup_edit_product, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroup_edit_product_Pressed(int)));
}

// DTOR
page_maintenance_dispenser::~page_maintenance_dispenser()
{
    delete ui;
}

void page_maintenance_dispenser::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    dispense_test_end(true);
    maintainProductPageEndTimer->stop();
    p_page_idle->pageTransition(this, pageToShow);
}

void page_maintenance_dispenser::showEvent(QShowEvent *event)
{
    p_page_idle->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);


    QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);
    //ui->refillButton->setStyleSheet("QPushButton {font-size: 36px;}");
    ui->refillButton->setStyleSheet(styleSheet);
    //ui->pushButton_pump->setStyleSheet("QPushButton { background-color: #AAAAAA;font-size: 20px;  }");
    ui->pushButton_pump->setProperty("class", "pump_enable");
    ui->pushButton_pump->setStyleSheet(styleSheet);




    ui->calibration_result_label->setText("Calibration value (if 1000ml dispensed): "); // calibration constant

    qDebug() << "call db from maintenance select dispenser page";
    DbManager db(DB_PATH);
    this->units_selected_product = this->p_page_idle->selectedProduct->getUnitsForSlot();
    volume_per_tick_buffer = p_page_idle->selectedProduct->getVolumePerTickForSlot();
    p_page_idle->selectedProduct->setBiggestEnabledSizeIndex();


    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

    reset_all_dispense_stats();
    update_volume_received_dispense_stats(0);

    ui->pluLabel_s->setText(p_page_idle->selectedProduct->getPLU('s'));
    ui->pluLabel_m->setText(p_page_idle->selectedProduct->getPLU('m'));
    ui->pluLabel_l->setText(p_page_idle->selectedProduct->getPLU('l'));
    ui->pluLabel_c->setText(p_page_idle->selectedProduct->getPLU('c'));
    ui->numberEntry->hide();
    ui->errorLabel->setText("");
    ui->titleLabel->setText("");

    // if (p_page_idle->selectedProduct->getPaymentMethod() == "plu" || p_page_idle->selectedProduct->getPaymentMethod() == "barcode")  //|| paymentMethod == "barcode_EAN-2 " || paymentMethod == "barcode_EAN-13"
    // {
        ui->pluButton_s->setVisible(true);
        ui->pluLabel_s->setVisible(true);
        ui->pluButton_s->setEnabled(true);
        ui->pluButton_m->setVisible(true);
        ui->pluLabel_m->setVisible(true);
        ui->pluButton_m->setEnabled(true);
        ui->pluButton_l->setVisible(true);
        ui->pluLabel_l->setVisible(true);
        ui->pluButton_l->setEnabled(true);
        ui->pluButton_c->setVisible(true);
        ui->pluLabel_c->setVisible(true);
        ui->pluButton_c->setEnabled(true);
  

    QString p = p_page_idle->selectedProduct->getProductPicturePath();
    p_page_idle->dfUtility->fileExists(p);
    QPixmap im(p);
    QIcon qi(im);
    ui->productPhotoButton->setIcon(qi);
    ui->productPhotoButton->setIconSize(QSize(271, 391));

    refreshLabels();
    setpushButton_soldOutText();

    isDispenseButtonPressed = false;
}

void page_maintenance_dispenser::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

/*
 * Page Tracking reference
 */
void page_maintenance_dispenser::setPage(page_maintenance *pageMaintenance, page_idle *pageIdle, page_idle_products *p_page_idle_products)
{

    this->p_page_maintenance = pageMaintenance;
    this->p_page_idle = pageIdle;
}

void page_maintenance_dispenser::refreshLabels()
{
    ui->pwmSlider->hide();

    volume_per_tick_buffer = p_page_idle->selectedProduct->getVolumePerTickForSlot();

    ui->volume_per_tick->setText(p_page_idle->selectedProduct->getVolumePerTickAsStringForSlot());

    ui->name->setText(p_page_idle->selectedProduct->getProductName());

    ui->pushButton_set_restock_volume->setText("Define restock volume.");

    ui->price_small->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_SMALL_INDEX)));
    ui->price_medium->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_MEDIUM_INDEX)));
    ui->price_large->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_LARGE_INDEX)));
    ui->price_custom->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_CUSTOM_INDEX)));

    ui->target_volume_s->setText(p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(SIZE_SMALL_INDEX, false, true));
    ui->target_volume_m->setText(p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(SIZE_MEDIUM_INDEX, false, true));
    ui->target_volume_l->setText(p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(SIZE_LARGE_INDEX, false, true));

    ui->full_volume->setText("Restock volume                          : " + p_page_idle->selectedProduct->getFullVolumeCorrectUnits(true));

    ui->label_volume_dispensed_total->setText("Volume dispensed total                : " + p_page_idle->selectedProduct->getTotalDispensedCorrectUnits());
    ui->label_volume_dispensed_since_restock->setText("Volume dispensed since restock : " + p_page_idle->selectedProduct->getVolumeDispensedSinceRestockCorrectUnits());
    ui->label_volume_remaining->setText("Volume remaining                        : " + p_page_idle->selectedProduct->getVolumeRemainingCorrectUnits());

    ui->pwmLabel->setText(QString::number(p_page_idle->selectedProduct->getDispenseSpeedPercentage()) + "%");

    int product_slot___ = p_page_idle->selectedProduct->getSlot();
    qDebug() << "db... refresh labels";
    DbManager db(DB_PATH);
    ui->label_restock_timestamp->setText("Most recent restock                     : " + db.getLastRestockDate(product_slot___));
    ui->pluLabel_s->setText(p_page_idle->selectedProduct->getPLU('s'));
    ui->pluLabel_m->setText(p_page_idle->selectedProduct->getPLU('m'));
    ui->pluLabel_l->setText(p_page_idle->selectedProduct->getPLU('l'));
    ui->pluLabel_c->setText(p_page_idle->selectedProduct->getPLU('c'));

    // db.closeDB();

    ui->temperatureButton->setVisible(false);
    ui->temperatureLabel->setVisible(false);
    activeEditField = "";
}

void page_maintenance_dispenser::setpushButton_soldOutText()
{
    qDebug() << "db call from pushButton_soldOutsetting";
    int slot = p_page_idle->selectedProduct->getSlotEnabled();
    DbManager db(DB_PATH);
    bool isSlotEnabled = db.getSlotEnabled(slot);
    db.closeDB();
    QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);

    if (isSlotEnabled)
    {

        ui->pushButton_soldOut->setText("Make \n unavailable");
//        ui->pushButton_soldOut->setStyleSheet("QPushButton { background-color: #5E8680;font-size: 36px; }");
        ui->pushButton_soldOut->setProperty("class", "pushButton_soldOut_unavailable");
        ui->pushButton_soldOut->setStyleSheet(styleSheet);
    }
    else
    {
        ui->pushButton_soldOut->setText("Make \n available");
//        ui->pushButton_soldOut->setStyleSheet("QPushButton { background-color: #E0A0A0;font-size: 36px;  }");
        ui->pushButton_soldOut->setProperty("class", "pushButton_soldOut_available");
        ui->pushButton_soldOut->setStyleSheet(styleSheet);

    }
}

void page_maintenance_dispenser::dispense_test_start()
{

    qDebug() << "Start dispense in maintenance mode. (FYI: if app crashes, it's probably about the update volume interrupts caused by the controller sending data.)";
    QString command = QString::number(p_page_idle->selectedProduct->getSlot());
    command.append("t");
    command.append(SEND_DISPENSE_START);

    reset_all_dispense_stats();
    dispenseTimer->start(100);
    update_volume_received_dispense_stats(0);

    p_page_idle->dfUtility->send_command_to_FSM(command);

    pumping = true;

    ui->pumpLabel->setText("Manual Pump ready. Press dispense button.");
    ui->pushButton_pump->setText("DISABLE PUMP");
    
    QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);
    //ui->pushButton_pump->setStyleSheet("QPushButton { background-color: #E0A0A0;font-size: 20px;  }");
    ui->pushButton_pump->setProperty("class", "pump_disable");
    ui->pushButton_pump->setStyleSheet(styleSheet);
        
}

void page_maintenance_dispenser::dispense_test_end(bool sendStopToController)
{
    QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);

    if (pumping)
    {
        dispenseTimer->stop();
        pumping = false;
        ui->pumpLabel->setText("Pump manual mode OFF.");
        ui->pushButton_pump->setText("ENABLE PUMP");
        //ui->pushButton_pump->setStyleSheet("QPushButton { background-color: #AAAAAA;font-size: 20px;  }");
        ui->pushButton_pump->setProperty("class", "pump_enable");
        ui->pushButton_pump->setStyleSheet(styleSheet);

        if (sendStopToController)
        {
            qDebug() << "Manually finish dispense in maintenance mode.";
            QString command = QString::number(p_page_idle->selectedProduct->getSlot());
            command.append("t");
            command.append(SEND_DISPENSE_STOP);
            p_page_idle->dfUtility->send_command_to_FSM(command);
        }
        else
        {
            qDebug() << "controller sent stop dispensing signal in maintenance mode.";
        }

        refreshLabels();
    }
}

void page_maintenance_dispenser::on_autoDispenseLarge_clicked()
{
    autoDispenseStart(SIZE_LARGE_INDEX);
}

void page_maintenance_dispenser::on_autoDispenseMedium_clicked()
{
    autoDispenseStart(SIZE_MEDIUM_INDEX);
}

void page_maintenance_dispenser::on_autoDispenseSmallButton_clicked()
{
    autoDispenseStart(SIZE_SMALL_INDEX);
}

void page_maintenance_dispenser::autoDispenseStart(int size)
{
    QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);

    if (!pumping)
    {

        ui->pushButton_pump->setText("DISABLE PUMP");
        //ui->pushButton_pump->setStyleSheet("QPushButton { background-color: #E0A0A0;font-size: 20px;  }");
        ui->pushButton_pump->setProperty("class", "pump_disable");
        ui->pushButton_pump->setStyleSheet(styleSheet);
        qDebug() << "Autofill small quantity pressed.";
        QString command = QString::number(this->p_page_idle->selectedProduct->getSlot());

        switch (size)
        {
        case SIZE_SMALL_INDEX:
        {
            command.append("s");
        }
        break;
        case SIZE_MEDIUM_INDEX:
        {
            command.append("m");
        }
        break;
        case SIZE_LARGE_INDEX:
        {
            command.append("l");
        }
        break;
        default:
        {
            qDebug() << "ERROR: size type not specified. will chose small size.";
            command.append("s");
        }
        break;
        }
        command.append(SEND_DISPENSE_AUTOFILL);

        reset_all_dispense_stats();
        dispenseTimer->start(100);
        update_volume_received_dispense_stats(0);

        p_page_idle->dfUtility->send_command_to_FSM(command);

        pumping = true;
    }
    else
    {
    }
}

void page_maintenance_dispenser::reset_all_dispense_stats()
{
    dispenserEnabledSecs = 0.0;
    dispenserPumpingSecs = 0.0; // reset pumping timer
    ui->dispenseTimeLabel->setText("Enabled time: " + QString::number(dispenserEnabledSecs, 'f', 1) + "s");
    ui->dispenseTimeLabelButton->setText("Button time: " + QString::number(dispenserPumpingSecs, 'f', 1) + "s");
    ui->flowRateLabel->setText("Flow rate (2s): ");
    setButtonPressCountLabel(true);

    DbManager db(DB_PATH);
    QString slotStatus = db.getStatusText(this->p_page_idle->selectedProduct->getSlot());
    db.closeDB();
    ui->dispense_status_label->setText(slotStatus);
}

void page_maintenance_dispenser::update_volume_received_dispense_stats(double dispensed)
{
    double vol_dispensed = dispensed;

    if (this->units_selected_product == "oz")
    {
        ui->ticksLabel->setText("fdvbc x (" + QString::number(df_util::convertMlToOz(volume_per_tick_buffer), 'f', 2) + "oz/tick): " + QString::number(vol_dispensed / volume_per_tick_buffer));
        ui->vol_dispensed_label->setText("Volume Dispensed: " + df_util::getConvertedStringVolumeFromMl(vol_dispensed, "oz", false, true));
    }
    else
    {
        ui->ticksLabel->setText("Ticks (" + QString::number(volume_per_tick_buffer) + "ml/tick): " + QString::number(vol_dispensed / volume_per_tick_buffer));
        ui->vol_dispensed_label->setText("Volume Dispensed: " + QString::number(vol_dispensed) + "ml");
    }

    if (vol_dispensed > 0)
    {

        QString vol_per_tick_for_1000ml = QString::number(1000 / (vol_dispensed / volume_per_tick_buffer), 'f', 2);

        ui->calibration_result_label->setText("Calibration value (if 1000ml dispensed): " + vol_per_tick_for_1000ml + "ml/tick"); // calibration constant if 1000ml were dispensed.
    }
}

void page_maintenance_dispenser::fsmReceivedVolumeDispensed(double dispensed, bool isFull)
{

    if (pumping)
    {
        update_volume_received_dispense_stats(dispensed);
    }
}

void page_maintenance_dispenser::fsmReceiveDispenseRate(double flowrate)
{
    qDebug() << "Dispense flow rate received from FSM: " << QString::number(flowrate, 'f', 2);
    ui->flowRateLabel->setText("Flow rate (2s): " + QString::number(flowrate, 'f', 2) + "ml/s");
};
void page_maintenance_dispenser::fsmReceiveDispenserStatus(QString status)
{
    QString dispenseStatus = status;
    qDebug() << "Dispense status received from FSM: " << dispenseStatus;
    ui->dispense_status_label->setText(dispenseStatus);
};

void page_maintenance_dispenser::setButtonPressCountLabel(bool init)
{
    if (init)
    {
        this->button_press_count = 0;
    }
    ui->dispense_button_presses_label->setText("Button press count: " + QString::number(this->button_press_count));
}

void page_maintenance_dispenser::fsmReceiveDispenseButtonPressedPositiveEdge()
{
    qDebug() << "Signal: dispense button pressed. (positive edge)";
    this->button_press_count++;
    setButtonPressCountLabel(false);
    isDispenseButtonPressed = true;
}

void page_maintenance_dispenser::fsmReceiveDispenseButtonPressedNegativeEdge()
{
    qDebug() << "Signal: dispense button unpressed. (negative edge)";
    isDispenseButtonPressed = false;
}

void page_maintenance_dispenser::fsmReceiveTargetVolumeReached()
{

    dispense_test_end(false);
}
void page_maintenance_dispenser::fsmReceiveNoFlowAbort()
{
    // gets called from the controller.
    qDebug() << "Received no flow from controller (in maintenance mode). Will not take any action";
}

void page_maintenance_dispenser::on_refillButton_clicked()
{
        QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);

    qDebug() << "refill clicked. slot: " << QString::number(this->p_page_idle->selectedProduct->getSlot());
    qDebug() << "refill clicked. size: " << QString::number(this->p_page_idle->selectedProduct->getVolume());

    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

    // ARE YOU SURE YOU WANT TO COMPLETE?
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    msgBox.setText("<p align=center>Are you sure you want to refill?</p>");
    
    msgBox.setProperty("class", "msgBoxbutton msgBox"); // set property goes first!!

    msgBox.setStyleSheet(styleSheet);
    //msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int ret = msgBox.exec();
    bool success;
    switch (ret)
    {
    case QMessageBox::Yes:
    {
        DbManager db(DB_PATH);
        success = db.restockProduct(p_page_idle->selectedProduct->getSlot());
        db.closeDB();

        if (success)
        {
            sendRestockToCloud();
            refreshLabels();
            ui->infoLabel->setText("Refill Succesfull");

            DbManager db(DB_PATH);
            bool isEnabled = db.getSlotEnabled(this->p_page_idle->selectedProduct->getSlot());
            bool success = db.updateSlotAvailability(this->p_page_idle->selectedProduct->getSlot(), isEnabled, "SLOT_STATE_AVAILABLE");
            db.closeDB();
        }
        else
        {
            ui->infoLabel->setText("Refill ERROR");
        }
    }
    break;
    case QMessageBox::No:
    {
        msgBox.hide();
    }
    break;
    }

    DbManager db(DB_PATH);
    QString slotStatus = db.getStatusText(this->p_page_idle->selectedProduct->getSlot());
    db.closeDB();

    ui->dispense_status_label->setText(slotStatus);
}

void page_maintenance_dispenser::on_pushButton_soldOut_clicked()
{
    qDebug() << "soldout clicked. slot: " << QString::number(this->p_page_idle->selectedProduct->getSlot());

    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

    DbManager db(DB_PATH);
    bool slotEnabled = db.getSlotEnabled(this->p_page_idle->selectedProduct->getSlot());
    QString slotStatus = db.getStatusText(this->p_page_idle->selectedProduct->getSlot());
    db.closeDB();

    QString infoLabelText = "";

    if (slotEnabled)
    {

        // ARE YOU SURE YOU WANT TO COMPLETE?
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint);
        msgBox.setText("<p align=center>Are you sure you want to Disable product?</p>");
        //msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

        QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);
        msgBox.setProperty("class", "msgBoxbutton msgBox"); // set property goes first!!
        msgBox.setStyleSheet(styleSheet);

        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();

        switch (ret)
        {
        case QMessageBox::Yes:
        {
            QMessageBox msgBox_set_availabilty;
            msgBox_set_availabilty.setWindowFlags(Qt::FramelessWindowHint);
            msgBox_set_availabilty.setText("<p align=center>Should the product be labeled as coming soon?</p>");
            //msgBox_set_availabilty.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");
            msgBox_set_availabilty.setProperty("class", "msgBoxbutton msgBox"); // set property goes first!!
            msgBox_set_availabilty.setStyleSheet(styleSheet);

            msgBox_set_availabilty.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            int ret2 = msgBox_set_availabilty.exec();
            switch (ret2)
            {
            case QMessageBox::Yes:
            {
                slotStatus = "SLOT_STATE_DISABLED_COMING_SOON";
            }
            break;
            case QMessageBox::No:
            {
                slotStatus = "SLOT_STATE_DISABLED";
            }
            break;
            }


            slotEnabled = false;
            break;
        }
        case QMessageBox::No:
        {

            msgBox.hide();
        }
        break;
        }
    }
    else
    {
        // ARE YOU SURE YOU WANT TO COMPLETE?
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint);
        msgBox.setText("<p align=center>Are you sure you want to Enable Product? (This will reset technical problems messages too)</p>");
        //msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

        QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);
        msgBox.setProperty("class", "msgBoxbutton msgBox"); // set property goes first!!
        msgBox.setStyleSheet(styleSheet);

        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();

        switch (ret)
        {
        case QMessageBox::Yes:
        {
         

            slotEnabled = true;
            slotStatus = "SLOT_STATE_AVAILABLE";
            break;
        }
        case QMessageBox::No:
        {
            msgBox.hide();
            break;
        }
        }
    }

    DbManager db3(DB_PATH);
    bool success = db3.updateSlotAvailability(p_page_idle->selectedProduct->getSlot(), slotEnabled, slotStatus);
    db3.closeDB();
    if (!success)
    {
        infoLabelText = "Set Enabled ERROR";
    }
    else
    {
        infoLabelText = "Slot Status set to\n" + slotStatus;
    }

    ui->infoLabel->setText(infoLabelText);

    setpushButton_soldOutText();
    ui->dispense_status_label->setText(slotStatus);
}

void page_maintenance_dispenser::on_pushButton_set_restock_volume_clicked()
{
    //    qDebug() << "Full Volume button clicked" ;
    // full = true;
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_pushButton_set_volume_remaining_clicked()
{
    //    qDebug() << "Remaining button clicked" ;
    // modify_stock = true;
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
    ui->numberEntry->show();
    ui->textEntry->setText("");
    ui->titleLabel->setText("Adjust the remaining volume:");
}

void page_maintenance_dispenser::on_dispensedButton_clicked()
{
    //    qDebug() << "Remaining button clicked" ;
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

// void page_maintenance_dispenser::on_lastRefillButton_clicked()
// {
//     //    qDebug() << "Last Refill button clicked" ;
//     _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
// }

void page_maintenance_dispenser::on_temperatureButton_clicked()
{
    //    qDebug() << "Temperature button clicked" ;
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::onDispenseTimerTick()
{
    dispenserEnabledSecs += 0.1;

    if (isDispenseButtonPressed)
    {
        dispenserPumpingSecs += 0.1;
    }
    ui->dispenseTimeLabel->setText("Enabled time: " + QString::number(dispenserEnabledSecs, 'f', 1) + "s");
    ui->dispenseTimeLabelButton->setText("Button time: " + QString::number(dispenserPumpingSecs, 'f', 1) + "s");
}

void page_maintenance_dispenser::onMaintainProductPageTimeoutTick()
{

    if (--_maintainProductPageTimeoutSec >= 0)
    {
        //        qDebug() << "Maintain Product Tick Down: " << _maintainProductPageTimeoutSec ;
    }
    else
    {
        qDebug() << "Maintenance dispenser page timeout";
        hideCurrentPageAndShowProvided(p_page_idle);
    }
}

void page_maintenance_dispenser::on_pwmButton_clicked()
{
    // pwm = true;
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
    ui->numberEntry->show();
    ui->textEntry->setText("");
    ui->titleLabel->setText("New Pump Speed:");
    ui->buttonPoint->hide();
}

void page_maintenance_dispenser::on_button1_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "1");
}

void page_maintenance_dispenser::on_button2_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "2");
}

void page_maintenance_dispenser::on_button3_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "3");
}

void page_maintenance_dispenser::on_button4_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "4");
}

void page_maintenance_dispenser::on_button5_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "5");
}

void page_maintenance_dispenser::on_button6_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "6");
}

void page_maintenance_dispenser::on_button7_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "7");
}

void page_maintenance_dispenser::on_button8_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "8");
}

void page_maintenance_dispenser::on_button9_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "9");
}

void page_maintenance_dispenser::on_button0_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "0");
}

void page_maintenance_dispenser::on_buttonBack_clicked()
{
    ui->textEntry->backspace();
}

void page_maintenance_dispenser::on_buttonPoint_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + ".");
}

void page_maintenance_dispenser::on_buttonDone_clicked()
{
    if (pwm)
    {
        if (ui->textEntry->text().toInt() > 100)
        {
            ui->errorLabel->setText("Error: Enter number less than 100");
            ui->textEntry->setText("");
        }
        else
        {
            text_entered = ui->textEntry->text();
            ui->buttonPoint->show();
            ui->numberEntry->hide();
            updateValues();
        }
    }
    else
    {
        text_entered = ui->textEntry->text();
        ui->numberEntry->hide();
        updateValues();
    }
}

void page_maintenance_dispenser::on_buttonCancel_clicked()
{
    ui->buttonPoint->show();
    ui->numberEntry->hide();
    ui->textEntry->setText("");
    ui->errorLabel->setText("");
    text_entered = "";
    activeEditField = "";

}






void page_maintenance_dispenser::on_image_clicked()
{
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_pushButton_pump_clicked()
{
    int slot = p_page_idle->selectedProduct->getSlot();
    if (slot < 1 || slot > SLOT_COUNT)
    {
        qDebug() << "ASSERT ERROR: Invalid slot : " << QString::number(slot);
    }

    if (!pumping)
    {
        dispense_test_start();
    }
    else
    {
        dispense_test_end(true);
    }

    // refreshLabels(); // fsm did not yet respond at this time. wait for feedback.
}

void page_maintenance_dispenser::on_backButton_clicked()
{
    hideCurrentPageAndShowProvided(p_page_maintenance);
}

void page_maintenance_dispenser::on_nameButton_clicked()
{
    //    qDebug() << "Name button clicked" ;
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_priceButton_s_clicked()
{
    // price_small = true;
    ui->textEntry->setText(QString::number(p_page_idle->selectedProduct->getPrice(SIZE_SMALL_INDEX)));
    // _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_priceButton_m_clicked()
{
    // price_medium = true;
    ui->textEntry->setText(QString::number(p_page_idle->selectedProduct->getPrice(SIZE_MEDIUM_INDEX)));
    // _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_priceButton_l_clicked()
{
    // price_large = true;
    ui->textEntry->setText(QString::number(p_page_idle->selectedProduct->getPrice(SIZE_LARGE_INDEX)));
    // _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_priceButton_c_clicked()
{
    // price_custom = true;
    ui->textEntry->setText(QString::number(p_page_idle->selectedProduct->getPrice(SIZE_CUSTOM_INDEX)));
    // _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_target_volumeButton_s_clicked()
{
    // target_s = true;
    ui->textEntry->setText(p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(SIZE_SMALL_INDEX, false, false));
    // _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_target_volumeButton_m_clicked()
{
    // target_m = true;
    ui->textEntry->setText(p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(SIZE_MEDIUM_INDEX, false, false));
    // _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_target_volumeButton_l_clicked()
{
    // target_l = true;
    ui->textEntry->setText(p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(SIZE_LARGE_INDEX, false, false));
}

void page_maintenance_dispenser::on_vol_per_tickButton_clicked()
{

    //    qDebug() << "Volume Per Tick button clicked" ;
    vol_per_tick = true;
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
    ui->numberEntry->show();
    ui->textEntry->setText("");
    ui->titleLabel->setText("New Volume Per Tick:");
}

void page_maintenance_dispenser::updateValues()
{
    if (text_entered != "")
    {
        if (activeEditField == "price_small")
        {
            p_page_idle->selectedProduct->setPrice(SIZE_SMALL_INDEX, text_entered.toDouble());
            ui->price_small->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_SMALL_INDEX)));
            ui->titleLabel->setText("Price Small:");
        }
        else if (activeEditField == "price_medium")
        {
            ui->titleLabel->setText("Price Medium:");
            p_page_idle->selectedProduct->setPrice(SIZE_MEDIUM_INDEX, text_entered.toDouble());
            ui->price_medium->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_MEDIUM_INDEX)));
        }
        else if (activeEditField == "price_large")
        {
            p_page_idle->selectedProduct->setPrice(SIZE_LARGE_INDEX, text_entered.toDouble());
            ui->price_large->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_LARGE_INDEX)));
        }
        else if (activeEditField == "price_custom")
        {
            p_page_idle->selectedProduct->setPrice(SIZE_CUSTOM_INDEX, text_entered.toDouble());
            ui->price_custom->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_CUSTOM_INDEX)));
        }
        else if (activeEditField == "target_s")
        {
            p_page_idle->selectedProduct->setSizeToVolumeForSlot(text_entered, SIZE_SMALL_INDEX);
        }
        else if (activeEditField == "target_m")
        {
            p_page_idle->selectedProduct->setSizeToVolumeForSlot(text_entered, SIZE_MEDIUM_INDEX);
        }
        else if (activeEditField == "target_l")
        {
            p_page_idle->selectedProduct->setSizeToVolumeForSlot(text_entered, SIZE_LARGE_INDEX);
        }
        else if (activeEditField == "vol_per_tick")
        {
            p_page_idle->selectedProduct->setVolumePerTickForSlot(text_entered);
        }
        else if (activeEditField == "full")
        {
            p_page_idle->selectedProduct->setFullVolumeCorrectUnits(text_entered);
        }
        else if (activeEditField == "modify_stock")
        {
            double vol_as_ml = p_page_idle->selectedProduct->inputTextToMlConvertUnits(text_entered);
            qDebug() << "db... set volume remaining";
            DbManager db(DB_PATH);
            db.setVolumeRemaining(p_page_idle->selectedProduct->getSlot(), vol_as_ml);
            db.closeDB();
        }
        else if (activeEditField == "pwm")
        {
            p_page_idle->selectedProduct->setDispenseSpeedPercentage(text_entered.toInt());
        }

        refreshLabels();
    }

    ui->textEntry->setText("");
    ui->titleLabel->setText("");
    ui->errorLabel->setText("");
}

void page_maintenance_dispenser::pwmSliderMoved(int percentage)
{
    qDebug() << "Slider not implemented";
 
}

size_t WriteCallback3(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void page_maintenance_dispenser::sendRestockToCloud()
{
    QString curl_param = "pid=" + p_page_idle->selectedProduct->getProductId() + "&volume_full=" + p_page_idle->selectedProduct->getFullVolumeCorrectUnits(false);

    curl_param_array = curl_param.toLocal8Bit();
    curl_data = curl_param_array.data();

    curl = curl_easy_init();
    if (!curl)
    {
        qDebug() << "cURL failed to page_init at thank you end";
        restockTransactionToFile(curl_data);
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/resetStock");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curl_param_array.data());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback3);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);

    res = curl_easy_perform(curl);

    // error code 6 (cannot resolve host) showed up when not connected to wifi. Make distinct!
    if (res != CURLE_OK)
    {
        qDebug() << "pagemaintenancedispenser. cURL fail. (6=could not resolve host (no internet)) Error code: " + QString::number(res);
        restockTransactionToFile(curl_data);
    }
    else
    {

        QString feedback = QString::fromUtf8(readBuffer.c_str());
        qDebug() << "ERROR: pagemaintenancedispenser cURL success. Server feedback readbuffer: " << feedback;

        // readbuffer is a string. "true" or "false"
        if (readBuffer == "true")
        {
            // return data
        }
    }
    curl_easy_cleanup(curl);
    readBuffer = "";
}

void page_maintenance_dispenser::restockTransactionToFile(char *curl_params)
{
    qDebug() << "Write Restock transaction to file ";
    QString data_out = curl_params;
    p_page_idle->dfUtility->write_to_file(TRANSACTIONS_RESTOCK_OFFINE_PATH, data_out);
}

size_t WriteCallback4(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void page_maintenance_dispenser::on_update_portal_clicked()
{
    qDebug() << "update portal clicked ";
    QString curl_params = "productId=" + p_page_idle->selectedProduct->getProductId() + "&source=soapstandStation" +
     "&price_small=" + QString::number( p_page_idle->selectedProduct->getPrice(SIZE_SMALL_INDEX)) + 
     "&price_medium=" + QString::number( p_page_idle->selectedProduct->getPrice(SIZE_MEDIUM_INDEX)) + 
     "&price_large=" + QString::number( p_page_idle->selectedProduct->getPrice(SIZE_LARGE_INDEX)) + 
     "&price_custom=" + QString::number( p_page_idle->selectedProduct->getPrice(SIZE_CUSTOM_INDEX)) + 
     "&size_small=" + QString::number( p_page_idle->selectedProduct->getPrice(SIZE_SMALL_INDEX)) + 
     "&size_medium=" + QString::number( p_page_idle->selectedProduct->getPrice(SIZE_MEDIUM_INDEX)) + 
     "&size_large=" + QString::number( p_page_idle->selectedProduct->getPrice(SIZE_LARGE_INDEX));
    curl_param_array2 = curl_params.toLocal8Bit();

    curl2 = curl_easy_init();
    if (!curl2)
    {
        qDebug() << "cURL failed to page_init at thank you end";
        return;
    }
    qDebug() << "Before pushing";

    curl_easy_setopt(curl2, CURLOPT_URL, "https://soapstandportal.com/api/product/update_product_from_station");
    curl_easy_setopt(curl2, CURLOPT_POSTFIELDS, curl_param_array2.data());
    curl_easy_setopt(curl2, CURLOPT_WRITEFUNCTION, WriteCallback4);
    curl_easy_setopt(curl2, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl2, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);
    qDebug() << "Successful request generated";

    res2 = curl_easy_perform(curl2);

    // error code 6 (cannot resolve host) showed up when not connected to wifi. Make distinct!
    if (res2 != CURLE_OK)
    {
        qDebug() << "pagemaintenancedispenser. cURL fail. (6=could not resolve host (no internet)) Error code: " + QString::number(res);
        // restockTransactionToFile(curl_data);
    }
    else
    {
        QString feedback = QString::fromUtf8(readBuffer.c_str());
        qDebug() << "Pagemaintenancedispenser cURL success. Server feedback readbuffer: " << feedback;
        ui->infoLabel->setText("Portal Update Succesfull");
        // readbuffer is a string. "true" or "false"
        if (readBuffer == "true")
        {
            // return data
        }
    }
    curl_easy_cleanup(curl2);
    readBuffer = "";
}

void page_maintenance_dispenser::buttonGroup_edit_product_Pressed(int buttonId)
{
    QAbstractButton *buttonpressed = ui->buttonGroup_edit_product->button(buttonId);
    QString buttonText = buttonpressed->text();
    activeEditField = buttonpressed->accessibleName();
    ui->titleLabel->setText(buttonText);
    ui->numberEntry->show();
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

}







