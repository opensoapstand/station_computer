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

    // Fullscreen background setup
    ui->setupUi(this);

    QPalette palette;
    palette.setBrush(QPalette::Background, Qt::white);
    this->setPalette(palette);

    // ui->label_enabled_status->setText("Pump manual mode OFF.");
    // ui->label_calibration_instructions->setText("Flowsensor calibration instructions:\n1.Enable the pump\n2.Take a measuring cup and dispense until the 1liter mark\n3.Check the calibration value\n4.Update the calibration value if different");
    // ui->pushButton_enable_pump->setText("ENABLE PUMP");

    maintainProductPageEndTimer = new QTimer(this);
    maintainProductPageEndTimer->setInterval(1000);
    connect(maintainProductPageEndTimer, SIGNAL(timeout()), this, SLOT(onMaintainProductPageTimeoutTick()));

    // maintainProductPageEndTimer->start(1000); // best to disable timeout. Maintenance page can be used as a permanent page in industrial or maintenance situations.

    dispenseTimer = new QTimer(this);
    dispenseTimer->setInterval(100);
    connect(dispenseTimer, SIGNAL(timeout()), this, SLOT(onDispenseTimerTick()));

    connect(ui->buttonGroup_edit_product, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroup_edit_product_Pressed(int)));
    connect(ui->buttonGroup_keypad, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroup_keypad_Pressed(int)));
}

// DTOR
page_maintenance_dispenser::~page_maintenance_dispenser()
{
    delete ui;
}

void page_maintenance_dispenser::setPage(page_maintenance *pageMaintenance, page_idle *pageIdle, page_idle_products *p_page_idle_products)
{

    this->p_page_maintenance = pageMaintenance;
    this->p_page_idle = pageIdle;
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

    p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_enabled_status,"pump_off");
    //p_page_idle->setTemplateTextToObject(ui->label_enabled_status);
    p_page_idle->setTemplateTextToObject(ui->label_calibration_instructions);
    p_page_idle->setTemplateTextToObject(ui->pushButton_enable_pump);
    p_page_idle->setTemplateTextWithIdentifierToObject(ui->pushButton_enable_pump,"enable_pump");
    p_page_idle->setTemplateTextToObject(ui->pushButton_set_restock_volume);
    p_page_idle->setTemplateTextToObject(ui->pushButton_update_portal);
    ui->pushButton_done->setText(p_page_idle->getTemplateTextByPage(this, "pushButton_keypad_done"));
    ui->pushButton_cancel->setText(p_page_idle->getTemplateTextByPage(this, "pushButton_keypad_cancel"));
    // p_page_idle->setTemplateTextToObject(ui->pushButton_cancel);
    // p_page_idle->setTemplateTextToObject(ui->pushButton_done);

    //   ui->pushButton_cancel->setText("CANCEL");
    //     ui->pushButton_done->setText("DONE");

    ui->pushButton_restock->setStyleSheet(styleSheet);
    ui->pushButton_enable_pump->setProperty("class", "pump_enable");
    ui->pushButton_enable_pump->setStyleSheet(styleSheet);

    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

    ui->numberEntry->hide();
    // ui->errorLabel->setText("");
    ui->label_title->setText("");

    ui->pushButton_plu_small->setEnabled(true);
    ui->pushButton_plu_medium->setEnabled(true);
    ui->pushButton_plu_large->setEnabled(true);
    ui->pushButton_plu_custom->setEnabled(true);

    QString path = p_page_idle->selectedProduct->getProductPicturePath();
    p_page_idle->addPictureToLabel(ui->label_product_picture, path);

    ui->pushButton_setting_speed_pwm->hide();
    ui->label_setting_speed_pwm->hide();
    ui->pushButton_setting_temperature->setVisible(false);
    ui->label_setting_temperature->setVisible(false);

    isDispenseButtonPressed = false;
    activeEditField = "";

    reset_all_dispense_stats();
    update_volume_received_dispense_stats(0);
    updateProductLabelValues(false);
}

void page_maintenance_dispenser::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void page_maintenance_dispenser::updateProductLabelValues(bool reloadFromDb)
{
    if (reloadFromDb)
    {
        p_page_idle->loadDynamicContent();
    }

    this->units_selected_product = this->p_page_idle->selectedProduct->getUnitsForSlot();

    volume_per_tick_buffer = p_page_idle->selectedProduct->getVolumePerTickForSlot();

    ui->label_volume_per_tick->setText(p_page_idle->selectedProduct->getVolumePerTickAsStringForSlot() + "/tick");

    ui->label_name->setText(p_page_idle->selectedProduct->getProductName());

    ui->pushButton_price_small->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_SMALL_INDEX)));
    ui->pushButton_price_medium->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_MEDIUM_INDEX)));
    ui->pushButton_price_large->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_LARGE_INDEX)));
    ui->pushButton_price_custom->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_CUSTOM_INDEX)));

    ui->pushButton_target_volume_small->setText(p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(SIZE_SMALL_INDEX, false, true));
    ui->pushButton_target_volume_medium->setText(p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(SIZE_MEDIUM_INDEX, false, true));
    ui->pushButton_target_volume_large->setText(p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(SIZE_LARGE_INDEX, false, true));
    ui->pushButton_target_volume_custom->setText(p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(SIZE_CUSTOM_INDEX, false, true));

    ui->label_restock_volume->setText(p_page_idle->selectedProduct->getFullVolumeCorrectUnits(true));

    ui->label_volume_dispensed_total->setText(p_page_idle->selectedProduct->getTotalDispensedCorrectUnits());
    ui->label_volume_dispensed_since_restock->setText(p_page_idle->selectedProduct->getVolumeDispensedSinceRestockCorrectUnits());
    ui->label_volume_remaining->setText(p_page_idle->selectedProduct->getVolumeRemainingCorrectUnits());

    ui->label_setting_speed_pwm->setText(QString::number(p_page_idle->selectedProduct->getDispenseSpeedPercentage()) + "%");

    ui->label_restock_timestamp->setText(p_page_idle->selectedProduct->getLastRestockDate());

    ui->pushButton_plu_small->setText(p_page_idle->selectedProduct->getPlu(SIZE_SMALL_INDEX));
    ui->pushButton_plu_medium->setText(p_page_idle->selectedProduct->getPlu(SIZE_MEDIUM_INDEX));
    ui->pushButton_plu_large->setText(p_page_idle->selectedProduct->getPlu(SIZE_LARGE_INDEX));
    ui->pushButton_plu_custom->setText(p_page_idle->selectedProduct->getPlu(SIZE_CUSTOM_INDEX));

    ui->label_status_dispenser->setText(p_page_idle->selectedProduct->getStatusText());
    setpushButton_soldOutText();
}

void page_maintenance_dispenser::setpushButton_soldOutText()
{
    QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);

    if (p_page_idle->selectedProduct->getSlotEnabled())
    {

        p_page_idle->setTemplateTextWithIdentifierToObject(ui->pushButton_soldOut, "unavailable");

        // ui->pushButton_soldOut->setText("Make \n unavailable");
        // QString caption = p_page_idle->getTemplateTextByPage(this, "pushButton_soldOut->unavailable");
        // caption.replace("\\n", "\n");
        // ui->pushButton_soldOut->setText(caption);
        ui->pushButton_soldOut->setProperty("class", "pushButton_soldOut_unavailable");
        ui->pushButton_soldOut->setStyleSheet(styleSheet);
    }
    else
    {
        p_page_idle->setTemplateTextWithIdentifierToObject(ui->pushButton_soldOut, "available");
        // ui->pushButton_soldOut->setText("Make \n available");
        ui->pushButton_soldOut->setProperty("class", "pushButton_soldOut_available");
        ui->pushButton_soldOut->setStyleSheet(styleSheet);
    }
}

void page_maintenance_dispenser::on_image_clicked()
{
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_pushButton_setting_temperature_clicked()
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
    ui->label_status_button_press_time->setText(QString::number(dispenserPumpingSecs, 'f', 1) + "s / " + QString::number(dispenserEnabledSecs, 'f', 1) + "s");
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

// ****************************************************************
// ****************** DISPENSER PUMP ACTIONS **********************
// ****************************************************************

void page_maintenance_dispenser::on_pushButton_enable_pump_clicked()
{
    int slot = p_page_idle->selectedProduct->getSlot();
    if (slot < 1 || slot > SLOT_COUNT)
    {
        qDebug() << "ASSERT ERROR: Invalid slot : " << QString::number(slot);
    }

    if (!pump_enabled)
    {
        dispense_test_start();
    }
    else
    {
        dispense_test_end(true);
    }
}

void page_maintenance_dispenser::on_pushButton_auto_dispense_large_clicked()
{
    // if (pump_enabled)
    // {
    // dispense_test_end(true);
    // usleep(1000);
    autoDispenseStart(SIZE_LARGE_INDEX);
    // }
}

void page_maintenance_dispenser::on_pushButton_auto_dispense_medium_clicked()
{
    autoDispenseStart(SIZE_MEDIUM_INDEX);
}

void page_maintenance_dispenser::on_pushButton_auto_dispense_small_clicked()
{
    autoDispenseStart(SIZE_SMALL_INDEX);
}

void page_maintenance_dispenser::autoDispenseStart(int size)
{
    QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);

    if (!pump_enabled)
    {

        // ui->pushButton_enable_pump->setText("DISABLE PUMP");
        p_page_idle->setTemplateTextWithIdentifierToObject(ui->pushButton_enable_pump, "disable_pump");

        ui->pushButton_enable_pump->setProperty("class", "pump_disable");
        ui->pushButton_enable_pump->setStyleSheet(styleSheet);
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

        pump_enabled = true;
    }
    else
    {
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

    pump_enabled = true;

    //ui->label_enabled_status->setText("Manual Pump ready. Press dispense button.");
     p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_enabled_status, "pump_ready");
     p_page_idle->setTemplateTextWithIdentifierToObject(ui->pushButton_enable_pump, "disable_pump");
    //p_page_idle->setTemplateTextToObject(ui->pushButton_enable_pump);

    //ui->pushButton_enable_pump->setText("DISABLE PUMP");

    QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);
    ui->pushButton_enable_pump->setProperty("class", "pump_disable");
    ui->pushButton_enable_pump->setStyleSheet(styleSheet);
}

void page_maintenance_dispenser::dispense_test_end(bool sendStopToController)
{
    QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);

    if (pump_enabled)
    {
        dispenseTimer->stop();
        pump_enabled = false;
        // ui->label_enabled_status->setText("Pump manual mode OFF.");
        p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_enabled_status, "pump_off");
        //ui->pushButton_enable_pump->setText("ENABLE PUMP");
        p_page_idle->setTemplateTextWithIdentifierToObject(ui->pushButton_enable_pump,"enable_pump");

        ui->pushButton_enable_pump->setProperty("class", "pump_enable");
        ui->pushButton_enable_pump->setStyleSheet(styleSheet);

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
    }
}

void page_maintenance_dispenser::reset_all_dispense_stats()
{
    dispenserEnabledSecs = 0.0;
    dispenserPumpingSecs = 0.0; // reset pump_enabled timer
    ui->label_status_button_press_time->setText(QString::number(dispenserPumpingSecs, 'f', 1) + "s / " + QString::number(dispenserEnabledSecs, 'f', 1) + "s");

    ui->label_status_flow_rate->setText("0");

    ui->label_status_volume_dispensed->setText("");
    ui->label_calibration_result->setText("-"); // calibration constant

    setButtonPressCountLabel(true);
    QString slotStatus = p_page_idle->selectedProduct->getStatusText();
    ui->label_status_dispenser->setText(slotStatus);
}

void page_maintenance_dispenser::update_volume_received_dispense_stats(double dispensed)
{
    double vol_dispensed = dispensed;

    if (this->units_selected_product == "oz")
    {
        // ui->label_status_volume_dispensed->setText("fdvbc x (" + QString::number(df_util::convertMlToOz(volume_per_tick_buffer), 'f', 2) + "oz/tick: " + QString::number(vol_dispensed / volume_per_tick_buffer));
        ui->label_status_volume_dispensed->setText(QString::number(vol_dispensed / volume_per_tick_buffer) + "ticks x " + QString::number(df_util::convertMlToOz(volume_per_tick_buffer), 'f', 2) + "oz/tick = " + QString::number(vol_dispensed) + df_util::getConvertedStringVolumeFromMl(vol_dispensed, "oz", false, true));
    }
    else
    {
        ui->label_status_volume_dispensed->setText(QString::number(vol_dispensed / volume_per_tick_buffer) + "ticks  x " + QString::number(volume_per_tick_buffer) + "ml/tick = " + QString::number(vol_dispensed) + "ml");
    }

    // update calibration field, ticks per ml if 1000ml would have been dispensed in reality at this point.
    if (vol_dispensed > 0)
    {
        QString vol_per_tick_for_1000ml = QString::number(1000 / (vol_dispensed / volume_per_tick_buffer), 'f', 2);
        ui->label_calibration_result->setText(vol_per_tick_for_1000ml + "ml/tick"); // calibration constant if 1000ml were dispensed.
    }
}

void page_maintenance_dispenser::fsmReceivedVolumeDispensed(double dispensed, bool isFull)
{

    if (pump_enabled)
    {
        update_volume_received_dispense_stats(dispensed);
    }
}

void page_maintenance_dispenser::fsmReceiveDispenseRate(double flowrate)
{
    // qDebug() << "Dispense flow rate received from FSM: " << QString::number(flowrate, 'f', 2);
    ui->label_status_flow_rate->setText(QString::number(flowrate, 'f', 2) + "ml/s");
};

void page_maintenance_dispenser::fsmReceiveDispenserStatus(QString status)
{
    QString dispenseStatus = status;
    qDebug() << "Dispense status received from FSM: " << dispenseStatus;
    ui->label_status_dispenser->setText(dispenseStatus);
};

void page_maintenance_dispenser::setButtonPressCountLabel(bool init)
{
    if (init)
    {
        this->button_press_count = 0;
    }
    ui->label_status_dispense_button_press_count->setText(QString::number(this->button_press_count));
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

// ****************************************************************
// ****************** PRODUCT STOCK ACTIONS ***********************
// ****************************************************************

void page_maintenance_dispenser::on_pushButton_restock_clicked()
{
    QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);

    qDebug() << "refill clicked. slot: " << QString::number(this->p_page_idle->selectedProduct->getSlot());
    qDebug() << "refill clicked. size: " << QString::number(this->p_page_idle->selectedProduct->getRestockVolume());

    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

    // ARE YOU SURE YOU WANT TO COMPLETE?
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    msgBox.setText("<p align=center>Are you sure you want to refill?</p>");

    // p_page_idle->addCssClassToObject(&msgBox, "msgBoxbutton msgBox", PAGE_MAINTENANCE_DISPENSER_CSS);

    msgBox.setProperty("class", "msgBoxbutton msgBox"); // set property goes first!!
    msgBox.setStyleSheet(styleSheet);

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int ret = msgBox.exec();
    bool success;
    switch (ret)
    {
    case QMessageBox::Yes:
    {
    }
    break;
    case QMessageBox::No:
    {
        msgBox.hide();
        return;
    }
    break;
    }

    success = p_page_idle->selectedProduct->restock();

    if (success)
    {
        sendRestockToCloud();

        // ui->label_action_feedback->setText("Refill Succesfull");
    //p_page_idle->setTemplateTextToObject(ui->pushButton_enable_pump);
    p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_action_feedback, "success");
        p_page_idle->selectedProduct->setStatusText("SLOT_STATE_AVAILABLE");
    }
    else
    {
        // ui->label_action_feedback->setText("Refill ERROR");
    p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_action_feedback, "error");
    }

    updateProductLabelValues(true);
}

void page_maintenance_dispenser::on_pushButton_soldOut_clicked()
{
    qDebug() << "soldout clicked. slot: " << QString::number(this->p_page_idle->selectedProduct->getSlot());

    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

    QString slotStatus = p_page_idle->selectedProduct->getStatusText();
    bool isEnabled = p_page_idle->selectedProduct->getSlotEnabled();

    if (isEnabled)
    {

        // // ARE YOU SURE YOU WANT TO COMPLETE?
        // QMessageBox msgBox;
        // msgBox.setWindowFlags(Qt::FramelessWindowHint);
        // msgBox.setText("<p align=center>Are you sure you want to Disable product?</p>");

        // QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);
        // msgBox.setProperty("class", "msgBoxbutton msgBox"); // set property goes first!!
        // msgBox.setStyleSheet(styleSheet);

        // msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        // int ret = msgBox.exec();

        // switch (ret)
        // {
        // case QMessageBox::Yes:
        // {
        QMessageBox msgBox_set_availabilty;
        msgBox_set_availabilty.setWindowFlags(Qt::FramelessWindowHint);

        msgBox_set_availabilty.setText("<p align=center>Label product as 'coming soon' ?</p>");
        QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);
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
        isEnabled = false;
        // break;
        // }
        // case QMessageBox::No:
        // {

        //     msgBox.hide();
        //     return;
        // }
        // break;
        // }
        qDebug() << "---will make UN available";
    }
    else
    {
        // // ARE YOU SURE YOU WANT TO COMPLETE?
        // QMessageBox msgBox;
        // msgBox.setWindowFlags(Qt::FramelessWindowHint);
        // msgBox.setText("<p align=center>Are you sure you want to Enable Product? (This will reset technical problems messages too)</p>");

        // QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);
        // msgBox.setProperty("class", "msgBoxbutton msgBox"); // set property goes first!!
        // msgBox.setStyleSheet(styleSheet);

        // msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        // int ret = msgBox.exec();

        // switch (ret)
        // {
        // case QMessageBox::Yes:
        // {

        isEnabled = true;
        slotStatus = "SLOT_STATE_AVAILABLE";
        qDebug() << "---will make available";
        // break;
        // }
        // case QMessageBox::No:
        // {
        //     msgBox.hide();
        //     return;
        //     break;
        // }
        // }
    }

    // set to database
    p_page_idle->selectedProduct->setSlotEnabled(isEnabled);
    qDebug() << "feiwjefwe ";
    p_page_idle->selectedProduct->setStatusText(slotStatus);
    qDebug() << "after ste status ";

    ui->label_action_feedback->setText("Slot Status set to\n" + slotStatus);
    qDebug() << "before updatelabels ";
    updateProductLabelValues(true);
    qDebug() << "after updatelabels ";
}

// ****************************************************************
// ****************** KEYPAD ACTIONS ******************************
// ****************************************************************

void page_maintenance_dispenser::buttonGroup_keypad_Pressed(int buttonId)
{
    qDebug() << "keypad group enter ";
    QAbstractButton *buttonpressed = ui->buttonGroup_keypad->button(buttonId);
    QString buttonText = buttonpressed->text();
    qDebug() << "keypad group enter button id: " << buttonText;
    if (ui->textEntry->hasSelectedText())
    {
        ui->textEntry->setText("");
    }

    ui->textEntry->setText(ui->textEntry->text() + buttonText);
    qDebug() << "keypad group exit ";
}

void page_maintenance_dispenser::on_buttonBack_clicked()
{
    ui->textEntry->backspace();
}

void page_maintenance_dispenser::on_buttonPeriod_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + ".");
}

void page_maintenance_dispenser::on_pushButton_done_clicked()
{
    QString text_entered = ui->textEntry->text();
    text_entered.trimmed();
    qDebug() << "button done. Text entered (trimmed of whitespace): >>" << text_entered << "<<";
    ui->numberEntry->hide();

    bool isAdmin = p_page_idle->thisMachine.isAllowedAsAdmin();

    if (text_entered != "" && isAdmin)
    {
        if (activeEditField == "pushButton_price_small")
        {
            p_page_idle->selectedProduct->setPrice(SIZE_SMALL_INDEX, text_entered.toDouble());
            ui->pushButton_price_small->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_SMALL_INDEX)));
            //p_page_idle->setTemplateTextToObject(ui->pushButton_enable_pump);
            p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_title,"small");
            // ui->label_title->setText("Price Small:");
        }
        else if (activeEditField == "pushButton_price_medium")
        {
            //ui->label_title->setText("Price Medium:");
            p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_title,"medium");
            p_page_idle->selectedProduct->setPrice(SIZE_MEDIUM_INDEX, text_entered.toDouble());
            ui->pushButton_price_medium->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_MEDIUM_INDEX)));
        }
        else if (activeEditField == "pushButton_price_large")
        {
            p_page_idle->selectedProduct->setPrice(SIZE_LARGE_INDEX, text_entered.toDouble());
            ui->pushButton_price_large->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_LARGE_INDEX)));
        }
        else if (activeEditField == "pushButton_price_custom")
        {
            p_page_idle->selectedProduct->setPrice(SIZE_CUSTOM_INDEX, text_entered.toDouble());
            ui->pushButton_price_custom->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_CUSTOM_INDEX)));
        }
        else if (activeEditField == "pushButton_target_volume_small")
        {
            p_page_idle->selectedProduct->setSizeToVolumeForSlot(text_entered, SIZE_SMALL_INDEX);
        }
        else if (activeEditField == "pushButton_target_volume_medium")
        {
            p_page_idle->selectedProduct->setSizeToVolumeForSlot(text_entered, SIZE_MEDIUM_INDEX);
        }
        else if (activeEditField == "pushButton_target_volume_large")
        {
            p_page_idle->selectedProduct->setSizeToVolumeForSlot(text_entered, SIZE_LARGE_INDEX);
        }
        else if (activeEditField == "pushButton_target_volume_custom")
        {
            p_page_idle->selectedProduct->setSizeToVolumeForSlot(text_entered, SIZE_CUSTOM_INDEX);
        }
        else if (activeEditField == "pushButton_plu_small")
        {
            p_page_idle->selectedProduct->setPlu(SIZE_SMALL_INDEX, text_entered);
        }
        else if (activeEditField == "pushButton_plu_medium")
        {
            p_page_idle->selectedProduct->setPlu(SIZE_MEDIUM_INDEX, text_entered);
        }
        else if (activeEditField == "pushButton_plu_large")
        {
            p_page_idle->selectedProduct->setPlu(SIZE_LARGE_INDEX, text_entered);
        }
        else if (activeEditField == "pushButton_plu_custom")
        {
            p_page_idle->selectedProduct->setPlu(SIZE_CUSTOM_INDEX, text_entered);
        }
        else if (activeEditField == "pushButton_volume_per_tick")
        {
            p_page_idle->selectedProduct->setVolumePerTickForSlot(text_entered);
        }
        else if (activeEditField == "pushButton_set_restock_volume")
        {
            p_page_idle->selectedProduct->setFullVolumeCorrectUnits(text_entered);
        }
        else if (activeEditField == "pushButton_set_volume_remaining")
        {
            p_page_idle->selectedProduct->setVolumeRemainingUserInput(text_entered);
        }
        else if (activeEditField == "pushButton_setting_speed_pwm")
        {
            p_page_idle->selectedProduct->setDispenseSpeedPercentage(text_entered.toInt());
        }
        else
        {
            qDebug() << "Active edit field not found: " << activeEditField;
        }
        activeEditField = "";

        updateProductLabelValues(true);
    }

    ui->textEntry->setText("");
    ui->label_title->setText("");
    // ui->errorLabel->setText("");
}

void page_maintenance_dispenser::on_pushButton_cancel_clicked()
{

    ui->buttonPeriod->show();
    ui->numberEntry->hide();
    ui->textEntry->setText("");
    // ui->errorLabel->setText("");
    // text_entered = "";
    activeEditField = "";
    p_page_idle->setTemplateTextToObject(ui->pushButton_cancel);
}

// ****************************************************************
// ****************** PRODUCT PROPERTY ACTIONS ********************
// ****************************************************************

void page_maintenance_dispenser::buttonGroup_edit_product_Pressed(int buttonId)
{
    QAbstractButton *buttonpressed = ui->buttonGroup_edit_product->button(buttonId);
    QString buttonText = buttonpressed->text();
    qDebug() << "accessible name: " << buttonpressed->objectName();
    QString buttonTitle = buttonpressed->accessibleName();
    activeEditField = buttonpressed->objectName();
    ui->label_title->setText(buttonTitle);
    ui->numberEntry->show();
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

    ui->textEntry->selectAll();

    if (!p_page_idle->thisMachine.isAllowedAsAdmin())
    {
        QMessageBox::information(this, "Admininstrator role required", "You do not have the rights to change these values. Please enter maintenance mode with the admin password.", QMessageBox::Ok);
        ui->numberEntry->hide();
    }
}

void page_maintenance_dispenser::on_pushButton_to_previous_page_clicked()
{
    hideCurrentPageAndShowProvided(p_page_maintenance);
}

void page_maintenance_dispenser::on_pushButton_price_small_clicked()
{
    ui->textEntry->setText(QString::number(p_page_idle->selectedProduct->getPrice(SIZE_SMALL_INDEX)));
}

void page_maintenance_dispenser::on_pushButton_price_medium_clicked()
{
    ui->textEntry->setText(QString::number(p_page_idle->selectedProduct->getPrice(SIZE_MEDIUM_INDEX)));
}

void page_maintenance_dispenser::on_pushButton_price_large_clicked()
{
    ui->textEntry->setText(QString::number(p_page_idle->selectedProduct->getPrice(SIZE_LARGE_INDEX)));
}

void page_maintenance_dispenser::on_pushButton_price_custom_clicked()
{
    ui->textEntry->setText(QString::number(p_page_idle->selectedProduct->getPrice(SIZE_CUSTOM_INDEX)));
}

void page_maintenance_dispenser::on_pushButton_target_volume_small_clicked()
{
    ui->textEntry->setText(p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(SIZE_SMALL_INDEX, false, false));
}

void page_maintenance_dispenser::on_pushButton_target_volume_medium_clicked()
{
    ui->textEntry->setText(p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(SIZE_MEDIUM_INDEX, false, false));
}

void page_maintenance_dispenser::on_pushButton_target_volume_large_clicked()
{
    ui->textEntry->setText(p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(SIZE_LARGE_INDEX, false, false));
}

void page_maintenance_dispenser::on_pushButton_target_volume_custom_clicked()
{
    ui->textEntry->setText(p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(SIZE_CUSTOM_INDEX, false, false));
}

void page_maintenance_dispenser::on_pushButton_plu_small_clicked()
{
    ui->textEntry->setText(p_page_idle->selectedProduct->getPlu(SIZE_SMALL_INDEX));
}

void page_maintenance_dispenser::on_pushButton_plu_medium_clicked()
{
    ui->textEntry->setText(p_page_idle->selectedProduct->getPlu(SIZE_MEDIUM_INDEX));
}

void page_maintenance_dispenser::on_pushButton_plu_large_clicked()
{
    ui->textEntry->setText(p_page_idle->selectedProduct->getPlu(SIZE_LARGE_INDEX));
}

void page_maintenance_dispenser::on_pushButton_plu_custom_clicked()
{
    ui->textEntry->setText(p_page_idle->selectedProduct->getPlu(SIZE_CUSTOM_INDEX));
}

void page_maintenance_dispenser::on_pushButton_volume_per_tick_clicked()
{
    ui->textEntry->setText(QString::number(p_page_idle->selectedProduct->getVolumePerTickForSlot()));
}

void page_maintenance_dispenser::on_pushButton_setting_speed_pwm_clicked()
{
    ui->textEntry->setText(QString::number(p_page_idle->selectedProduct->getDispenseSpeedPercentage()));
}

void page_maintenance_dispenser::on_pushButton_set_volume_remaining_clicked()
{
    //    qDebug() << "Remaining button clicked" ;
    // modify_stock = true;
    // _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
    // ui->label_title->setText("Adjust the remaining volume");
    p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_title,"adjust_volume");
    ui->textEntry->setText(p_page_idle->selectedProduct->getVolumeRemainingCorrectUnits());
}

void page_maintenance_dispenser::on_pushButton_set_restock_volume_clicked()
{
    ui->textEntry->setText(p_page_idle->selectedProduct->getFullVolumeCorrectUnits(false));
    // ui->label_title->setText("Full Volume button clicked");
    p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_title,"full_button");
    //    qDebug() << "Full Volume button clicked" ;
    // full = true;
    // _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

// ****************************************************************
// ****************** BACKEND CLOUD ACTIONS ***********************
// ****************************************************************

size_t WriteCallback3(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void page_maintenance_dispenser::sendRestockToCloud()
{
    QString curl_param = "pid=" + p_page_idle->selectedProduct->getAwsProductId() + "&volume_full=" + p_page_idle->selectedProduct->getFullVolumeCorrectUnits(false);

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

void page_maintenance_dispenser::on_pushButton_update_portal_clicked()
{
    qDebug() << "update portal clicked ";
    QString curl_params = "productId=" + p_page_idle->selectedProduct->getAwsProductId() + "&source=soapstandStation" +
                          "&price_small=" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_SMALL_INDEX)) +
                          "&price_medium=" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_MEDIUM_INDEX)) +
                          "&price_large=" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_LARGE_INDEX)) +
                          "&price_custom=" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_CUSTOM_INDEX)) +
                          "&size_small=" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_SMALL_INDEX)) +
                          "&size_medium=" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_MEDIUM_INDEX)) +
                          "&size_large=" + QString::number(p_page_idle->selectedProduct->getPrice(SIZE_LARGE_INDEX));
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
        //ui->label_action_feedback->setText("Portal Update Succesfull");
        p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_action_feedback, "portal_success");

        // readbuffer is a string. "true" or "false"
        if (readBuffer == "true")
        {
            // return data
        }
    }
    curl_easy_cleanup(curl2);
    readBuffer = "";
}
