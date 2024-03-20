#include "page_maintenance_dispenser.h"
#include "ui_page_maintenance_dispenser.h"
#include "page_idle.h"
// #include "/home/df-admin/station_computer/fsm/components/pcb.cpp"
//  #include "/home/df-admin/station_computer/fsm/components/pcb.h"

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

    maintainProductPageEndTimer = new QTimer(this);
    maintainProductPageEndTimer->setInterval(1000);
    connect(maintainProductPageEndTimer, SIGNAL(timeout()), this, SLOT(onMaintainProductPageTimeoutTick()));
    statusbarLayout = new QVBoxLayout(this);

    dispenseTimer = new QTimer(this);
    dispenseTimer->setInterval(100);
    connect(dispenseTimer, SIGNAL(timeout()), this, SLOT(onDispenseTimerTick()));

    connect(ui->buttonGroup_edit_product, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroup_edit_product_Pressed(int)));
    connect(ui->buttonGroup_keypad, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroup_keypad_Pressed(int)));

    buttons_select_additive[0] = ui->pushButton_active_pnumber_additive_1;
    buttons_select_additive[1] = ui->pushButton_active_pnumber_additive_2;
    buttons_select_additive[2] = ui->pushButton_active_pnumber_additive_3;
    buttons_select_additive[3] = ui->pushButton_active_pnumber_additive_4;
    buttons_select_additive[4] = ui->pushButton_active_pnumber_additive_5;

    buttons_select_mix[0] = ui->pushButton_dispense_pnumber_1;
    buttons_select_mix[1] = ui->pushButton_dispense_pnumber_2;
    buttons_select_mix[2] = ui->pushButton_dispense_pnumber_3;
    buttons_select_mix[3] = ui->pushButton_dispense_pnumber_4;
    buttons_select_mix[4] = ui->pushButton_dispense_pnumber_5;
    buttons_select_mix[5] = ui->pushButton_dispense_pnumber_6;

    buttons_select_size[0] = ui->pushButton_set_quantity_small;
    buttons_select_size[1] = ui->pushButton_set_quantity_medium;
    buttons_select_size[2] = ui->pushButton_set_quantity_large;
    buttons_select_size[3] = ui->pushButton_set_quantity_custom;
    buttons_select_size[4] = ui->pushButton_set_quantity_test;
    buttons_select_size[5] = ui->pushButton_set_quantity_sample;

    buttons_slot_shortcuts[0] = ui->pushButton_activate_slot_1;
    buttons_slot_shortcuts[1] = ui->pushButton_activate_slot_2;
    buttons_slot_shortcuts[2] = ui->pushButton_activate_slot_3;
    buttons_slot_shortcuts[3] = ui->pushButton_activate_slot_4;
    buttons_slot_shortcuts[4] = ui->pushButton_activate_slot_5;
}

// DTOR
page_maintenance_dispenser::~page_maintenance_dispenser()
{
    delete ui;
}

void page_maintenance_dispenser::setPage(page_maintenance *pageMaintenance, page_idle *pageIdle, page_idle_products *p_page_idle_products, statusbar *p_statusbar)
{

    this->p_page_maintenance = pageMaintenance;
    this->p_page_idle = pageIdle;
    this->p_statusbar = p_statusbar;
}

void page_maintenance_dispenser::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    qDebug() << "Exit Maintenance dispense page";
    dispense_test_end(true);
    maintainProductPageEndTimer->stop();
    statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.
    p_page_idle->thisMachine->pageTransition(this, pageToShow);
}

void page_maintenance_dispenser::showEvent(QShowEvent *event)
{
    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    statusbarLayout->addWidget(p_statusbar);                 // Only one instance can be shown. So, has to be added/removed per page.
    statusbarLayout->setContentsMargins(0, 1874, 0, 0);      // int left, int top, int right, int bottom);
    qDebug() << "Active Slot: " << QString::number(this->p_page_idle->thisMachine->getSelectedSlot()->getSlotId());
    QWidget::showEvent(event);

    m_activePNumber = this->p_page_idle->thisMachine->getSelectedSlot()->getBasePNumber();

    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget

    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);

    this->p_page_idle->thisMachine->setSelectedProduct(m_activePNumber);

    // shortcuts to other slots
    for (int slot_index = 0; slot_index < BASE_LINE_COUNT_MAX; slot_index++)
    {
        if (this->p_page_idle->thisMachine->isSlotExisting(slot_index + 1) && this->p_page_idle->thisMachine->isAllowedAsAdmin() // while transitioning, it's easy to tap the hidden page and get out of the UI this is a security risk.
        )
        {
            QString button_text = this->p_page_idle->thisMachine->getTemplateTextByPage(this, "slot_shortcut");
            buttons_slot_shortcuts[slot_index]->setText(button_text.arg(slot_index + 1));

            buttons_slot_shortcuts[slot_index]->show();
            if ((slot_index + 1) == p_page_idle->thisMachine->getSelectedSlot()->getSlotId())
            {
                buttons_slot_shortcuts[slot_index]->setProperty("class", "active_slot");
                buttons_slot_shortcuts[slot_index]->setStyleSheet(styleSheet);
            }
            else
            {
                buttons_slot_shortcuts[slot_index]->setProperty("class", "inactive_slot");
                buttons_slot_shortcuts[slot_index]->setStyleSheet(styleSheet);
            }
        }
        else
        {
            buttons_slot_shortcuts[slot_index]->hide();
        }
    }
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_to_previous_page);

    p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_pump_enabled_status, "pump_off");
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_calibration_instructions);
    p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_enable_pump, "enable_pump");

    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_set_restock_volume);
    ui->pushButton_done->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "pushButton_keypad_done"));
    ui->pushButton_cancel->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "pushButton_keypad_cancel"));
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_clear_problem);

    ui->label_product_name->setStyleSheet(styleSheet);
    ui->label_product_picture->setStyleSheet(styleSheet);
    ui->pushButton_to_previous_page->setStyleSheet(styleSheet);
    ui->pushButton_restock->setStyleSheet(styleSheet);
    ui->pushButton_clear_problem->setStyleSheet(styleSheet);
    ui->pushButton_enable_pump->setProperty("class", "pump_enable");
    ui->pushButton_enable_pump->setStyleSheet(styleSheet);
    ui->label_action_feedback->setStyleSheet(styleSheet);
    ui->label_status_dispenser->setStyleSheet(styleSheet);
    ui->pushButton_set_volume_remaining->setStyleSheet(styleSheet);

    ui->pushButton_active_pnumber_base->setProperty("class", "product_active");
    ui->pushButton_active_pnumber_base->setStyleSheet(styleSheet);
    ui->pushButton_active_pnumber_base->setText("Base\nP-" + QString::number(this->p_page_idle->thisMachine->getSelectedSlot()->getBasePNumber()));

    setSizeIndex(SIZE_SMALL_INDEX); // default selected size for dispensing

    // first, pretend like there is no selected size
    for (int size_index = 1; size_index < SIZES_COUNT; size_index++)
    {
        buttons_select_size[size_index - 1]->setProperty("class", "product_not_available");
        buttons_select_size[size_index - 1]->setStyleSheet(styleSheet);
        buttons_select_size[size_index - 1]->setEnabled(false);
    }

    // first, pretend like there are no dispense products
    for (int dispense_product_position = 1; dispense_product_position <= DISPENSE_PRODUCTS_PER_BASE_LINE_MAX; dispense_product_position++)
    {
        buttons_select_mix[dispense_product_position - 1]->setProperty("class", "product_not_available");
        buttons_select_mix[dispense_product_position - 1]->setStyleSheet(styleSheet);
        buttons_select_mix[dispense_product_position - 1]->setText("Dispense Product " + QString::number(dispense_product_position) + "\nN/A");
        buttons_select_mix[dispense_product_position - 1]->setEnabled(false);
        // buttons_select_mix[dispense_product_position - 1]->hide();
    }
    // set up for all available dispense products
    int dispense_product_position = 1;
    while (dispense_product_position <= this->p_page_idle->thisMachine->getSelectedSlot()->getDispenseProductsCount())
    {
        buttons_select_mix[dispense_product_position - 1]->setEnabled(true);
        int pnumber = this->p_page_idle->thisMachine->getSelectedSlot()->getDispensePNumber(dispense_product_position);
        buttons_select_mix[dispense_product_position - 1]->setText("Dispense Product " + QString::number(dispense_product_position) + "\nP-" + QString::number(pnumber));
        buttons_select_mix[dispense_product_position - 1]->show();
        dispense_product_position++;
    }

    // first, pretend like there are no additives
    for (int additive_position = 1; additive_position <= ADDITIVES_PER_SLOT_COUNT_MAX; additive_position++)
    {
        buttons_select_additive[additive_position - 1]->setProperty("class", "product_not_available");
        buttons_select_additive[additive_position - 1]->setStyleSheet(styleSheet);
        buttons_select_additive[additive_position - 1]->setText("Additive " + QString::number(additive_position) + "\nN/A");
        buttons_select_additive[additive_position - 1]->setEnabled(false);
        // buttons_select_additive[additive_position - 1]->hide();
    }
    // set up for all available additives
    int additive_position = 1;
    while (additive_position <= this->p_page_idle->thisMachine->getSelectedSlot()->getAdditiveCount())
    {
        buttons_select_additive[additive_position - 1]->setEnabled(true);
        int pnumber = this->p_page_idle->thisMachine->getSelectedSlot()->getAdditivePNumber(additive_position);
        buttons_select_additive[additive_position - 1]->setText("Additive " + QString::number(additive_position) + "\nP-" + QString::number(pnumber));
        buttons_select_additive[additive_position - 1]->show();
        additive_position++;
    }

    ui->label_action_feedback->setText("");
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

    ui->numberEntry->hide();
    ui->label_title->setText("");

    ui->pushButton_plu_small->setEnabled(true);
    ui->pushButton_plu_medium->setEnabled(true);
    ui->pushButton_plu_large->setEnabled(true);
    ui->pushButton_plu_custom->setEnabled(true);

    QString path = p_page_idle->thisMachine->getSelectedProduct()->getProductPicturePath();
    p_page_idle->thisMachine->addPictureToLabel(ui->label_product_picture, path);

    ui->pushButton_setting_speed_pwm->hide();
    ui->label_setting_speed_pwm->hide();
    ui->pushButton_set_max_temperature->setVisible(false);
    ui->label_setting_temperature->hide();
    ui->label_setting_temperature_2->hide();

    isDispenseButtonPressed = false;
    activeEditField = "";

    maintainProductPageEndTimer->start();
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
        p_page_idle->thisMachine->loadDynamicContent();
    }

    // product size selection buttons
    for (int size_index = 1; size_index < SIZES_COUNT; size_index++)
    {
        buttons_select_size[size_index - 1]->setEnabled(true);

        if (size_index == m_selected_size_index)
        {
            p_page_idle->thisMachine->addCssClassToObject(buttons_select_size[size_index - 1], "product_active", PAGE_MAINTENANCE_DISPENSER_CSS);
        }
        else
        {
            p_page_idle->thisMachine->addCssClassToObject(buttons_select_size[size_index - 1], "product_not_active", PAGE_MAINTENANCE_DISPENSER_CSS);
        }
    }

    // BASE PNUMBER
    if (this->p_page_idle->thisMachine->getSelectedSlot()->getBasePNumber() == this->p_page_idle->thisMachine->getSelectedProduct()->getPNumber())
    {
        p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_active_pnumber_base, "product_active", PAGE_MAINTENANCE_DISPENSER_CSS);
    }
    else
    {
        p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_active_pnumber_base, "product_not_active", PAGE_MAINTENANCE_DISPENSER_CSS);
    }

    // available additives
    int additive_position = 1;
    while (additive_position <= this->p_page_idle->thisMachine->getSelectedSlot()->getAdditiveCount())
    {
        int pnumber = this->p_page_idle->thisMachine->getSelectedSlot()->getAdditivePNumber(additive_position);

        if (pnumber == this->p_page_idle->thisMachine->getSelectedProduct()->getPNumber())
        {
            p_page_idle->thisMachine->addCssClassToObject(buttons_select_additive[additive_position - 1], "product_active", PAGE_MAINTENANCE_DISPENSER_CSS);
        }
        else
        {
            p_page_idle->thisMachine->addCssClassToObject(buttons_select_additive[additive_position - 1], "product_not_active", PAGE_MAINTENANCE_DISPENSER_CSS);
        }
        additive_position++;
    }

    // available dispense products
    int dispense_product_position = 1;
    while (dispense_product_position <= this->p_page_idle->thisMachine->getSelectedSlot()->getDispenseProductsCount())
    {
        int pnumber = this->p_page_idle->thisMachine->getSelectedSlot()->getDispensePNumber(dispense_product_position);

        if (pnumber == this->p_page_idle->thisMachine->getSelectedProduct()->getPNumber())
        {
            p_page_idle->thisMachine->addCssClassToObject(buttons_select_mix[dispense_product_position - 1], "product_active", PAGE_MAINTENANCE_DISPENSER_CSS);
        }
        else
        {
            p_page_idle->thisMachine->addCssClassToObject(buttons_select_mix[dispense_product_position - 1], "product_not_active", PAGE_MAINTENANCE_DISPENSER_CSS);
        }
        dispense_product_position++;
    }

    this->units_selected_product = this->p_page_idle->thisMachine->getSizeUnit();
    ui->label_volume_per_tick->setText(p_page_idle->thisMachine->getSelectedProduct()->getVolumePerTickAsStringForSlot() + "/tick");
    ui->label_product_name->setText(p_page_idle->thisMachine->getSelectedProduct()->getProductName());

    ui->pushButton_price_small->setText("$" + QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_SMALL_INDEX)));
    ui->pushButton_price_medium->setText("$" + QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_MEDIUM_INDEX)));
    ui->pushButton_price_large->setText("$" + QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_LARGE_INDEX)));
    ui->pushButton_price_custom->setText("$" + QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_CUSTOM_INDEX)));
    ui->pushButton_price_sample->setText("$" + QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_SAMPLE_INDEX)));

    ui->pushButton_target_volume_small->setText(p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(SIZE_SMALL_INDEX, false, true));
    ui->pushButton_target_volume_medium->setText(p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(SIZE_MEDIUM_INDEX, false, true));
    ui->pushButton_target_volume_large->setText(p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(SIZE_LARGE_INDEX, false, true));
    ui->pushButton_target_volume_custom->setText(p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(SIZE_CUSTOM_INDEX, false, true));
    ui->pushButton_target_volume_sample->setText(p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(SIZE_SAMPLE_INDEX, false, true));

    ui->checkBox_enable_small->setChecked(p_page_idle->thisMachine->getSelectedProduct()->getSizeEnabled(SIZE_SMALL_INDEX));
    ui->checkBox_enable_medium->setChecked(p_page_idle->thisMachine->getSelectedProduct()->getSizeEnabled(SIZE_MEDIUM_INDEX));
    ui->checkBox_enable_large->setChecked(p_page_idle->thisMachine->getSelectedProduct()->getSizeEnabled(SIZE_LARGE_INDEX));
    ui->checkBox_enable_custom->setChecked(p_page_idle->thisMachine->getSelectedProduct()->getSizeEnabled(SIZE_CUSTOM_INDEX));
    ui->checkBox_enable_sample->setChecked(p_page_idle->thisMachine->getSelectedProduct()->getSizeEnabled(SIZE_SAMPLE_INDEX));

    ui->label_restock_volume->setText(p_page_idle->thisMachine->getSelectedProduct()->getFullVolumeCorrectUnits(true));

    ui->label_volume_dispensed_total->setText(p_page_idle->thisMachine->getSelectedProduct()->getTotalDispensedCorrectUnits());
    ui->label_volume_dispensed_since_restock->setText(p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedSinceRestockCorrectUnits());
    ui->label_volume_remaining->setText(p_page_idle->thisMachine->getSelectedProduct()->getVolumeRemainingCorrectUnits(true));

    ui->label_setting_speed_pwm->setText(QString::number(p_page_idle->thisMachine->getSelectedProduct()->getDispenseSpeedPercentage()) + "%");

    ui->label_restock_timestamp->setText(p_page_idle->thisMachine->getSelectedProduct()->getLastRestockDate());

    ui->pushButton_plu_small->setText(p_page_idle->thisMachine->getSelectedProduct()->getPlu(SIZE_SMALL_INDEX));
    ui->pushButton_plu_medium->setText(p_page_idle->thisMachine->getSelectedProduct()->getPlu(SIZE_MEDIUM_INDEX));
    ui->pushButton_plu_large->setText(p_page_idle->thisMachine->getSelectedProduct()->getPlu(SIZE_LARGE_INDEX));
    ui->pushButton_plu_custom->setText(p_page_idle->thisMachine->getSelectedProduct()->getPlu(SIZE_CUSTOM_INDEX));
    ui->pushButton_plu_sample->setText(p_page_idle->thisMachine->getSelectedProduct()->getPlu(SIZE_SAMPLE_INDEX));
    QString statusText = p_page_idle->thisMachine->getSelectedSlot()->getSlotStatusAsString();
    setStatusTextLabel(ui->label_status_dispenser_elaborated, statusText, true);
    setStatusTextLabel(ui->label_status_dispenser, statusText, true);
    
    QString statusTextProduct = p_page_idle->thisMachine->getSelectedProduct()->getProductStateAsString();
    // setStatusTextLabel(ui->label_status_selected_product, statusTextProduct, true);
    setStatusTextLabel(ui->label_status_selected_product, statusTextProduct, true);

    if (p_page_idle->thisMachine->getSelectedSlot()->getIsSlotEnabled())
    {
        // if slot is enabled, set button text to "make unavailable"
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_set_status_slot, "unavailable");
        // p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_set_status_slot, "pushButton_set_status_unavailable", PAGE_MAINTENANCE_DISPENSER_CSS);
    }
    else
    {
        // if slot is disabled
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_set_status_slot, "available");
        //p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_set_status_slot, "pushButton_set_status_available", PAGE_MAINTENANCE_DISPENSER_CSS);
    }


     if (p_page_idle->thisMachine->getSelectedProduct()->getIsProductEnabled())
    {
        // if slot is enabled, set button text to "make unavailable"
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_set_status_product, "unavailable");
        p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_set_status_product, "pushButton_set_status_unavailable", PAGE_MAINTENANCE_DISPENSER_CSS);
    }
    else
    {
        // if slot is disabled
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_set_status_product, "available");
        p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_set_status_product, "pushButton_set_status_available", PAGE_MAINTENANCE_DISPENSER_CSS);
    }


    if (statusText.compare("SLOT_STATE_PROBLEM_NEEDS_ATTENTION") == 0)
    {
        // only show button if there is an issue.
        ui->pushButton_clear_problem->show();
    }
    else
    {
        ui->pushButton_clear_problem->hide();
    }
}

void page_maintenance_dispenser::setStatusTextLabel(QLabel *label, QString statusText, bool displayRawStatus)
{
    QString status_display_text = "";

    QString element_name = label->objectName();

    if (statusText.compare("SLOT_STATE_DISABLED_COMING_SOON") == 0)
    {
        status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, element_name + "->coming_soon");
    }
    else if (statusText.compare("SLOT_STATE_DISABLED") == 0)
    {
        status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, element_name + "->not_enabled");
    }
    else if (statusText.compare("SLOT_STATE_AVAILABLE") == 0)
    {
        status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, element_name + "->available");
    }
    else if (statusText.compare("SLOT_STATE_AVAILABLE_LOW_STOCK") == 0)
    {
        status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, element_name + "->almost_empty");
    }
    else if (statusText.compare("SLOT_STATE_PROBLEM_NEEDS_ATTENTION") == 0)
    {
        status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, element_name + "->assistance");
    }
    else if (statusText.compare("SLOT_STATE_PROBLEM_EMPTY") == 0)
    {
        status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, element_name + "->empty");
    }
    else if (statusText.compare("SLOT_STATE_DISABLED") == 0)
    {
        status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, element_name + "->disabled");
    }
    else if (statusText.compare("SLOT_STATE_WARNING_PRIMING") == 0)
    {
        status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, element_name + "->priming");
    }
    else
    {
        status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->default");
    }
    if (displayRawStatus)
    {
        //label->setText(statusText + ": " + status_display_text);
        label->setText(statusText);
    }
    else
    {
        label->setText(status_display_text);
    }
}

void page_maintenance_dispenser::on_image_clicked()
{
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_pushButton_set_max_temperature_clicked()
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

void page_maintenance_dispenser::fsmReceiveTemperature(double temperature_1, double temperature_2)
{
    if (p_page_idle->thisMachine->isAelenPillarElseSoapStand())
    {
        p_page_idle->thisMachine->fsmReceiveTemperature(temperature_1, temperature_2);
        ui->label_setting_temperature->show();
        ui->label_setting_temperature_2->show();
        ui->label_setting_temperature->setText(QString::number(temperature_1, 'f', 2));
        ui->label_setting_temperature_2->setText(QString::number(temperature_2, 'f', 2));
    }
    else
    {
        qDebug() << "Received temperature. Discarded because this is not a Pillar machine.";
    }
}

void page_maintenance_dispenser::onMaintainProductPageTimeoutTick()
{

    if (--_maintainProductPageTimeoutSec >= 0)
    {
        // qDebug() << "Maintain Product Tick Down: " << _maintainProductPageTimeoutSec ;
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
    int slot = p_page_idle->thisMachine->getSelectedSlot()->getSlotId();
    if (slot < 1 || slot > p_page_idle->thisMachine->getSlotCount())
    {
        qDebug() << "ASSERT ERROR: Invalid slot : " << QString::number(slot);
    }

    if (!is_pump_enabled_for_dispense)
    {
        dispense_test_start();
    }
    else
    {
        dispense_test_end(true);
    }
    usleep(1000000); // give the controller a chance to update the db with the dispenser state.
    updateProductLabelValues(true);
}

void page_maintenance_dispenser::on_pushButton_auto_dispense_large_clicked()
{
    // autoDispenseStart(SIZE_LARGE_INDEX);
}

void page_maintenance_dispenser::on_pushButton_auto_dispense_medium_clicked()
{
    qDebug() << "Button not available!!!";
    // autoDispenseStart(SIZE_MEDIUM_INDEX);
}

void page_maintenance_dispenser::on_pushButton_auto_dispense_selected_quantity_clicked()
{
    qDebug() << "Button not available!";
    autoDispenseStart(m_selected_size_index);
}

void page_maintenance_dispenser::autoDispenseStart(int size)
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_enable_pump, "disable_pump");
        p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_enable_pump, "pump_disable", PAGE_MAINTENANCE_DISPENSER_CSS);
        qDebug() << "Autofill quantity pressed.";
        QString dispenseCommand = QString::number(this->p_page_idle->thisMachine->getSelectedSlot()->getSlotId());
        dispenseCommand.append(df_util::sizeIndexToChar(m_selected_size_index));

        dispenseCommand.append(SEND_DISPENSE_AUTOFILL);

        reset_all_dispense_stats();
        dispenseTimer->start(100);
        update_volume_received_dispense_stats(0);

        // p_page_idle->thisMachine->dfUtility->send_command_to_FSM(command, true);

        int pNumberSelectedProduct = this->p_page_idle->thisMachine->getSelectedProduct()->getPNumber();

        QString command = "dispensePNumber|" + dispenseCommand + "|" + QString::number(pNumberSelectedProduct) + "|"; // dispensePNumber|slot|dispensePNumber

        p_page_idle->thisMachine->dfUtility->send_command_to_FSM(command, true);

        is_pump_enabled_for_dispense = true;
    }
    else
    {
    }
}

void page_maintenance_dispenser::dispense_test_start()
{
    qDebug() << "Start dispense in maintenance mode. (FYI: if app crashes, it's probably about the update volume interrupts caused by the controller sending data.)";
    QString dispenseCommand = QString::number(p_page_idle->thisMachine->getSelectedSlot()->getSlotId());

    qDebug() << "Autofill quantity pressed.";

    dispenseCommand.append(df_util::sizeIndexToChar(m_selected_size_index));

    // dispenseCommand.append("t");
    // dispenseCommand.append("t");
    dispenseCommand.append(SEND_DISPENSE_START);

    // #define DISPENSE_CUSTOM_MIX

#ifdef DISPENSE_CUSTOM_MIX
    // check if pnumber is a mix.
    QString pNumbersAsCsvString = this->p_page_idle->thisMachine->getSelectedProduct()->getMixPNumbersAsCsv();
    QString pNumberRatiosAsCsvString = this->p_page_idle->thisMachine->getSelectedProduct()->getMixRatiosAsCsv(this->p_page_idle->thisMachine->getSelectedProduct()->getCustomMixRatios());
    int pNumberSelectedProduct = this->p_page_idle->thisMachine->getSelectedProduct()->getPNumber();

    QString command = "dispenseCustomMix|" + dispenseCommand + "|" + QString::number(pNumberSelectedProduct) + "|" + pNumbersAsCsvString + "|" + pNumberRatiosAsCsvString + "|"; // dipenseMix|slot|dispensePNumber|pnumberscsv|ratioscsv
#else

    int pNumberSelectedProduct = this->p_page_idle->thisMachine->getSelectedProduct()->getPNumber();

    QString command = "dispensePNumber|" + dispenseCommand + "|" + QString::number(pNumberSelectedProduct) + "|"; // dispensePNumber|slot|dispensePNumber

#endif

    reset_all_dispense_stats();
    dispenseTimer->start(100);
    update_volume_received_dispense_stats(0);

    p_page_idle->thisMachine->dfUtility->send_command_to_FSM(command, true);

    is_pump_enabled_for_dispense = true;

    p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_pump_enabled_status, "pump_ready");
    p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_enable_pump, "disable_pump");
    p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_enable_pump, "pump_disable", PAGE_MAINTENANCE_DISPENSER_CSS);
}

void page_maintenance_dispenser::dispense_test_end(bool sendStopToController)
{
    if (is_pump_enabled_for_dispense)
    {
        qDebug() << "Active Dispense stop.";
        dispenseTimer->stop();
        is_pump_enabled_for_dispense = false;
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_pump_enabled_status, "pump_off");
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_enable_pump, "enable_pump");

        p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_enable_pump, "pump_enable", PAGE_MAINTENANCE_DISPENSER_CSS);

        if (sendStopToController)
        {
            qDebug() << "Manually finish dispense in maintenance mode.";
            // QString command = QString::number(p_page_idle->thisMachine->getSelectedSlot()->getSlotId());
            // command.append("t");
            // command.append(SEND_DISPENSE_STOP);
            QString command = "stopDispense";
            p_page_idle->thisMachine->dfUtility->send_command_to_FSM(command, true);
        }
        else
        {
            qDebug() << "controller sent stop dispensing signal in maintenance mode.";
        }
    }
    else
    {
        qDebug() << "Active Dispense was already stopped.";
    }
}

bool page_maintenance_dispenser::isDispenserPumpEnabledWarningBox()
{
    // ATTENTION: will return TRUE when ENABLED and provide a user warning msgbox

    if (is_pump_enabled_for_dispense)
    {
        qDebug() << "USER REQUESTED NON ALLOWED ACTION: Show Window saying this can't be done during 'pump enabled' --> ";
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog); // Qt::Dialog makes it 'modal' which means  you HAVE to take action and you can't click next to it
        msgBox.setText("<p align=center>Not possible<br>while dispense pump<br>is enabled!</p>");

        p_page_idle->thisMachine->addCssClassToObject(&msgBox, "msgBoxbutton msgBox", PAGE_MAINTENANCE_DISPENSER_CSS);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }
    return is_pump_enabled_for_dispense;
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
    QString statusText = p_page_idle->thisMachine->getSelectedSlot()->getSlotStatusAsString();
    setStatusTextLabel(ui->label_status_dispenser_elaborated, statusText, true);
}

void page_maintenance_dispenser::update_volume_received_dispense_stats(double dispensed)
{
    double vol_dispensed = dispensed;
    double volume_per_tick_buffer = p_page_idle->thisMachine->getSelectedProduct()->getVolumePerTickForSlot();

    if (this->units_selected_product == "oz")
    {
        ui->label_status_volume_dispensed->setText(QString::number(vol_dispensed / volume_per_tick_buffer) + "ticks x " + QString::number(df_util::convertMlToOz(volume_per_tick_buffer), 'f', 2) + "oz/tick = " + df_util::getConvertedStringVolumeFromMl(vol_dispensed, "oz", false, true));
    }
    else
    {
        ui->label_status_volume_dispensed->setText(QString::number(vol_dispensed / volume_per_tick_buffer) + "ticks  x " + QString::number(volume_per_tick_buffer) + "ml/tick = " + QString::number(vol_dispensed) + "ml");
    }

    // update calibration field, ticks per ml if 1000ml would have been dispensed in reality at this point.
    if (vol_dispensed > 0)
    {
        QString vol_per_tick_for_1000ml = QString::number(1000 / (vol_dispensed / volume_per_tick_buffer), 'f', 3);

        if (this->units_selected_product == "oz")
        {
            QString val = df_util::getConvertedStringVolumeFromMl(1000 / (vol_dispensed / volume_per_tick_buffer), "oz", false, false);
            ui->label_calibration_result->setText(val + "oz/tick"); // calibration constant if 1000ml were dispensed.
        }
        else
        {
            ui->label_calibration_result->setText(vol_per_tick_for_1000ml + "ml/tick"); // calibration constant if 1000ml were dispensed.
        }
    }
}

void page_maintenance_dispenser::fsmReceivedVolumeDispensed(double dispensed, bool isFull)
{
    if (is_pump_enabled_for_dispense)
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
    setStatusTextLabel(ui->label_status_dispenser_elaborated, dispenseStatus, true);
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
void page_maintenance_dispenser::on_pushButton_clear_problem_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        QString statusText = p_page_idle->thisMachine->getSelectedSlot()->getSlotStatusAsString();
        qDebug() << "Clear problem button clicked. Will set to available. Status was: " + statusText;
        p_page_idle->thisMachine->getSelectedSlot()->setSlotStatus(SLOT_STATE_AVAILABLE);
        updateProductLabelValues(true);
    }
}

void page_maintenance_dispenser::on_pushButton_restock_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        // QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_MAINTENANCE_DISPENSER_CSS);

        qDebug() << "refill clicked. slot: " << QString::number(this->p_page_idle->thisMachine->getSelectedSlot()->getSlotId());
        qDebug() << "refill clicked. size: " << QString::number(this->p_page_idle->thisMachine->getSelectedProduct()->getRestockVolume());

        _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

        // ARE YOU SURE YOU WANT TO COMPLETE?
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        msgBox.setText("<p align=center>Are you sure you want to restock the product?</p>");

        p_page_idle->thisMachine->addCssClassToObject(&msgBox, "msgBoxbutton msgBox", PAGE_MAINTENANCE_DISPENSER_CSS);

        // msgBox.setProperty("class", "msgBoxbutton msgBox"); // set property goes first!!
        // msgBox.setStyleSheet(styleSheet);

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

        success = p_page_idle->thisMachine->getSelectedProduct()->restock();

        if (success)
        {
            sendRestockToCloud();
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_action_feedback, "success");
            p_page_idle->thisMachine->getSelectedProduct()->setProductState(PRODUCT_STATE_AVAILABLE);
        }
        else
        {
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_action_feedback, "error");
        }

        updateProductLabelValues(true);
    }
}

void page_maintenance_dispenser::on_pushButton_set_status_product_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        qDebug() << "Toggle status button clicked. Product enabled: " << p_page_idle->thisMachine->getSelectedProduct()->getIsProductEnabled() << " Product status: " << p_page_idle->thisMachine->getSelectedProduct()->getProductStateAsString();

        _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

        bool isEnabled = p_page_idle->thisMachine->getSelectedProduct()->getIsProductEnabled();
        if (isEnabled)
        {
            p_page_idle->thisMachine->getSelectedProduct()->setProductState(PRODUCT_STATE_DISABLED);
            p_page_idle->thisMachine->getSelectedProduct()->setIsProductEnabled(false);
        }
        else
        {
            p_page_idle->thisMachine->getSelectedProduct()->setIsProductEnabled(true);
            p_page_idle->thisMachine->getSelectedProduct()->setProductState(PRODUCT_STATE_AVAILABLE);
        }
        // set to database

        //ui->label_action_feedback->setText("Slot Status set to " + slotStatus);
        updateProductLabelValues(true);
    }
}

void page_maintenance_dispenser::on_pushButton_set_status_slot_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        qDebug() << "Toggle status button clicked.";

        _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

        SlotState slotStatus = p_page_idle->thisMachine->getSelectedSlot()->getSlotStatus();

        bool isEnabled = p_page_idle->thisMachine->getSelectedSlot()->getIsSlotEnabled();

        if (isEnabled)
        {
            // QMessageBox msgBox_set_availabilty;
            // msgBox_set_availabilty.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

            // msgBox_set_availabilty.setText("<p align=center>Label product as 'coming soon' <br>(if no, it will be set as 'sold out')?</p>");

            // p_page_idle->thisMachine->addCssClassToObject(&msgBox_set_availabilty, "msgBoxbutton msgBox", PAGE_MAINTENANCE_DISPENSER_CSS);

            // msgBox_set_availabilty.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            // int ret2 = msgBox_set_availabilty.exec();
            // switch (ret2)
            // {
            // case QMessageBox::Yes:
            // {
            //     slotStatus = SLOT_STATE_DISABLED_COMING_SOON;
            // }
            // break;
            // case QMessageBox::No:
            // {
            //     slotStatus = SLOT_STATE_DISABLED;
            // }
            // break;
            // }
            // isEnabled = false;

            // if (!p_page_idle->thisMachine->isAelenPillarElseSoapStand()){
            //     // set selected product too to available.

            // }
            isEnabled = false;
            slotStatus =  SLOT_STATE_DISABLED;
        }
        else
        {
            isEnabled = true;
            slotStatus = SLOT_STATE_AVAILABLE;
        }

        // set to database
        p_page_idle->thisMachine->getSelectedSlot()->setSlotEnabled(isEnabled, slotStatus);
        
        ui->label_action_feedback->setText("Slot Status set to: " + p_page_idle->thisMachine->getSelectedSlot()->getSlotStatusAsString());
        updateProductLabelValues(true);
    }
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
    if (!isDispenserPumpEnabledWarningBox())
    {
        QString text_entered = ui->textEntry->text();
        text_entered.trimmed();
        qDebug() << "button done. Text entered (trimmed of whitespace): >>" << text_entered << "<<";
        ui->numberEntry->hide();

        bool isAdmin = p_page_idle->thisMachine->isAllowedAsAdmin();

        if (text_entered != "" && isAdmin)
        {
            if (activeEditField == "pushButton_price_small")
            {
                p_page_idle->thisMachine->getSelectedProduct()->setPrice(SIZE_SMALL_INDEX, text_entered.toDouble());
                ui->pushButton_price_small->setText("$" + QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_SMALL_INDEX)));
                p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_title, "small");
            }
            else if (activeEditField == "pushButton_price_medium")
            {
                p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_title, "medium");
                p_page_idle->thisMachine->getSelectedProduct()->setPrice(SIZE_MEDIUM_INDEX, text_entered.toDouble());
                ui->pushButton_price_medium->setText("$" + QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_MEDIUM_INDEX)));
            }
            else if (activeEditField == "pushButton_price_large")
            {
                p_page_idle->thisMachine->getSelectedProduct()->setPrice(SIZE_LARGE_INDEX, text_entered.toDouble());
                ui->pushButton_price_large->setText("$" + QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_LARGE_INDEX)));
            }
            else if (activeEditField == "pushButton_price_custom")
            {
                p_page_idle->thisMachine->getSelectedProduct()->setPrice(SIZE_CUSTOM_INDEX, text_entered.toDouble());
                ui->pushButton_price_custom->setText("$" + QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_CUSTOM_INDEX)));
            }
            else if (activeEditField == "pushButton_target_volume_small")
            {
                p_page_idle->thisMachine->getSelectedProduct()->configureVolumeToSizeForSlot(text_entered, SIZE_SMALL_INDEX);
            }
            else if (activeEditField == "pushButton_target_volume_medium")
            {
                p_page_idle->thisMachine->getSelectedProduct()->configureVolumeToSizeForSlot(text_entered, SIZE_MEDIUM_INDEX);
            }
            else if (activeEditField == "pushButton_target_volume_large")
            {
                p_page_idle->thisMachine->getSelectedProduct()->configureVolumeToSizeForSlot(text_entered, SIZE_LARGE_INDEX);
            }
            else if (activeEditField == "pushButton_target_volume_custom")
            {
                p_page_idle->thisMachine->getSelectedProduct()->configureVolumeToSizeForSlot(text_entered, SIZE_CUSTOM_INDEX);
            }
            else if (activeEditField == "pushButton_plu_small")
            {
                p_page_idle->thisMachine->getSelectedProduct()->setPlu(SIZE_SMALL_INDEX, text_entered);
            }
            else if (activeEditField == "pushButton_plu_medium")
            {
                p_page_idle->thisMachine->getSelectedProduct()->setPlu(SIZE_MEDIUM_INDEX, text_entered);
            }
            else if (activeEditField == "pushButton_plu_large")
            {
                p_page_idle->thisMachine->getSelectedProduct()->setPlu(SIZE_LARGE_INDEX, text_entered);
            }
            else if (activeEditField == "pushButton_plu_custom")
            {
                p_page_idle->thisMachine->getSelectedProduct()->setPlu(SIZE_CUSTOM_INDEX, text_entered);
            }
            else if (activeEditField == "pushButton_volume_per_tick")
            {
                p_page_idle->thisMachine->getSelectedProduct()->setVolumePerTickForSlot(text_entered);
            }
            else if (activeEditField == "pushButton_set_restock_volume")
            {
                p_page_idle->thisMachine->getSelectedProduct()->setFullVolumeCorrectUnits(text_entered);
            }
            else if (activeEditField == "pushButton_set_volume_remaining")
            {
                p_page_idle->thisMachine->getSelectedProduct()->setVolumeRemainingUserInput(text_entered);
            }
            else if (activeEditField == "pushButton_setting_speed_pwm")
            {
                p_page_idle->thisMachine->getSelectedProduct()->setDispenseSpeedPercentage(text_entered.toInt());
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
}

void page_maintenance_dispenser::on_pushButton_cancel_clicked()
{
    ui->buttonPeriod->show();
    ui->numberEntry->hide();
    ui->textEntry->setText("");
    // ui->errorLabel->setText("");
    // text_entered = "";
    activeEditField = "";
    ui->pushButton_cancel->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "pushButton_keypad_cancel"));

    //    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_cancel);
}

// ****************************************************************
// ****************** PRODUCT PROPERTY ACTIONS ********************
// ****************************************************************

void page_maintenance_dispenser::buttonGroup_edit_product_Pressed(int buttonId)
{
    if (!isDispenserPumpEnabledWarningBox())
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

        if (!p_page_idle->thisMachine->isAllowedAsAdmin())
        {
            QMessageBox::information(this, "Admininstrator role required", "You do not have the rights to change these values. Please enter maintenance mode with the admin password.", QMessageBox::Ok);
            ui->numberEntry->hide();
        }
    }
}

void page_maintenance_dispenser::on_pushButton_to_previous_page_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        update_changes_to_portal();
        hideCurrentPageAndShowProvided(p_page_maintenance);
    }
}

void page_maintenance_dispenser::on_pushButton_price_small_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_SMALL_INDEX)));
    }
}

void page_maintenance_dispenser::on_pushButton_price_medium_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_MEDIUM_INDEX)));
    }
}

void page_maintenance_dispenser::on_pushButton_price_large_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_LARGE_INDEX)));
    }
}

void page_maintenance_dispenser::on_pushButton_price_custom_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_CUSTOM_INDEX)));
    }
}

void page_maintenance_dispenser::on_pushButton_price_sample_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_SAMPLE_INDEX)));
    }
}

void page_maintenance_dispenser::on_pushButton_target_volume_small_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(SIZE_SMALL_INDEX, false, false));
    }
}

void page_maintenance_dispenser::on_pushButton_target_volume_medium_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(SIZE_MEDIUM_INDEX, false, false));
    }
}

void page_maintenance_dispenser::on_pushButton_target_volume_large_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(SIZE_LARGE_INDEX, false, false));
    }
}

void page_maintenance_dispenser::on_pushButton_target_volume_custom_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(SIZE_CUSTOM_INDEX, false, false));
    }
}

void page_maintenance_dispenser::on_pushButton_target_volume_sample_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(SIZE_SAMPLE_INDEX, false, false));
    }
}

void page_maintenance_dispenser::on_pushButton_plu_small_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(p_page_idle->thisMachine->getSelectedProduct()->getPlu(SIZE_SMALL_INDEX));
    }
}

void page_maintenance_dispenser::on_pushButton_plu_medium_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(p_page_idle->thisMachine->getSelectedProduct()->getPlu(SIZE_MEDIUM_INDEX));
    }
}

void page_maintenance_dispenser::on_pushButton_plu_large_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(p_page_idle->thisMachine->getSelectedProduct()->getPlu(SIZE_LARGE_INDEX));
    }
}

void page_maintenance_dispenser::on_pushButton_plu_custom_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(p_page_idle->thisMachine->getSelectedProduct()->getPlu(SIZE_CUSTOM_INDEX));
    }
}

void page_maintenance_dispenser::on_pushButton_plu_sample_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(p_page_idle->thisMachine->getSelectedProduct()->getPlu(SIZE_SAMPLE_INDEX));
    }
}

void page_maintenance_dispenser::on_pushButton_volume_per_tick_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(QString::number(p_page_idle->thisMachine->getSelectedProduct()->getVolumePerTickForSlot()));
    }
}

void page_maintenance_dispenser::on_pushButton_setting_speed_pwm_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(QString::number(p_page_idle->thisMachine->getSelectedProduct()->getDispenseSpeedPercentage()));
    }
}

void page_maintenance_dispenser::on_pushButton_set_volume_remaining_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_title, "adjust_volume");
        ui->textEntry->setText(p_page_idle->thisMachine->getSelectedProduct()->getVolumeRemainingCorrectUnits(false));
    }
}

void page_maintenance_dispenser::on_pushButton_set_restock_volume_clicked()
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        ui->textEntry->setText(p_page_idle->thisMachine->getSelectedProduct()->getFullVolumeCorrectUnits(false));
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_title, "full_button");
    }
}

// ****************************************************************
// ****************** BACKEND CLOUD ACTIONS ***********************
// ****************************************************************

void page_maintenance_dispenser::sendRestockToCloud()
{
    QString curl_params = "pid=" + p_page_idle->thisMachine->getSelectedProductAwsProductId() + "&volume_full=" + p_page_idle->thisMachine->getSelectedProduct()->getFullVolumeCorrectUnits(false);

    std::tie(res, readBuffer, http_code) = p_page_idle->thisMachine->sendRequestToPortal(PORTAL_RESET_STOCK, "POST", curl_params, "PAGE_MAINTENANCE_DISPENSER");
    // error code 6 (cannot resolve host) showed up when not connected to wifi. Make distinct!
    if (res != CURLE_OK)
    {
        qDebug() << "pagemaintenancedispenser. cURL fail. (6=could not resolve host (no internet)) Error code: " + QString::number(res);
        restockTransactionToFile(curl_params);
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
}

void page_maintenance_dispenser::restockTransactionToFile(QString curl_params)
{
    qDebug() << "Write Restock transaction to file ";
    p_page_idle->thisMachine->dfUtility->write_to_file(TRANSACTIONS_RESTOCK_OFFLINE_PATH, curl_params);
}

void page_maintenance_dispenser::update_changes_to_portal()
{
    qDebug() << "update portal clicked ";
    QString curl_params = "productId=" + p_page_idle->thisMachine->getSelectedProductAwsProductId() + "&source=soapstandStation" +
                          "&price_small=" + QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_SMALL_INDEX)) +
                          "&price_medium=" + QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_MEDIUM_INDEX)) +
                          "&price_large=" + QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_LARGE_INDEX)) +
                          "&price_custom=" + QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_CUSTOM_INDEX)) +
                          "&size_small=" + p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(SIZE_SMALL_INDEX, false, false) +
                          "&size_medium=" + p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(SIZE_MEDIUM_INDEX, false, false) +
                          "&size_large=" + p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(SIZE_LARGE_INDEX, false, false);

    std::tie(res, readBuffer, http_code) = p_page_idle->thisMachine->sendRequestToPortal(PORTAL_UPDATE_PRODUCT_FROM_STATION, "POST", curl_params, "PAGE_MAINTENANCE_DISPENSER");

    // error code 6 (cannot resolve host) showed up when not connected to wifi. Make distinct!
    if (res != CURLE_OK)
    {
        qDebug() << "pagemaintenancedispenser. cURL fail. (6=could not resolve host (no internet)) Error code: " + QString::number(res);
        restockTransactionToFile(curl_params);
    }
    else
    {
        QString feedback = QString::fromUtf8(readBuffer.c_str());
        qDebug() << "Pagemaintenancedispenser cURL success. Server feedback readbuffer: " << feedback;
        // ui->label_action_feedback->setText("Portal Update Succesfull");
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_action_feedback, "portal_success");

        // readbuffer is a string. "true" or "false"
        if (readBuffer == "true")
        {
            // return data
        }
    }
}

void page_maintenance_dispenser::on_checkBox_enable_small_clicked()
{
    p_page_idle->thisMachine->getSelectedProduct()->toggleSizeEnabled(SIZE_SMALL_INDEX);
    updateProductLabelValues(true);
}

void page_maintenance_dispenser::on_checkBox_enable_medium_clicked()
{
    p_page_idle->thisMachine->getSelectedProduct()->toggleSizeEnabled(SIZE_MEDIUM_INDEX);
    updateProductLabelValues(true);
}

void page_maintenance_dispenser::on_checkBox_enable_large_clicked()
{
    p_page_idle->thisMachine->getSelectedProduct()->toggleSizeEnabled(SIZE_LARGE_INDEX);
    updateProductLabelValues(true);
}

void page_maintenance_dispenser::on_checkBox_enable_custom_clicked()
{
    p_page_idle->thisMachine->getSelectedProduct()->toggleSizeEnabled(SIZE_CUSTOM_INDEX);
    updateProductLabelValues(true);
}

void page_maintenance_dispenser::on_checkBox_enable_sample_clicked()
{
    p_page_idle->thisMachine->getSelectedProduct()->toggleSizeEnabled(SIZE_SAMPLE_INDEX);
    updateProductLabelValues(true);
}

void page_maintenance_dispenser::setSelectedProduct(int pnumber)
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        this->p_page_idle->thisMachine->setSelectedProduct(pnumber);
        reset_all_dispense_stats();
        updateProductLabelValues(false);
    }
}

void page_maintenance_dispenser::on_pushButton_active_pnumber_base_clicked()
{
    setSelectedProduct(this->p_page_idle->thisMachine->getSelectedSlot()->getBasePNumber());
}

void page_maintenance_dispenser::on_pushButton_active_pnumber_additive_1_clicked()
{
    setSelectedProduct(this->p_page_idle->thisMachine->getSelectedSlot()->getAdditivePNumber(1));
}

void page_maintenance_dispenser::on_pushButton_active_pnumber_additive_2_clicked()
{
    setSelectedProduct(this->p_page_idle->thisMachine->getSelectedSlot()->getAdditivePNumber(2));
}

void page_maintenance_dispenser::on_pushButton_active_pnumber_additive_3_clicked()
{
    setSelectedProduct(this->p_page_idle->thisMachine->getSelectedSlot()->getAdditivePNumber(3));
}

void page_maintenance_dispenser::on_pushButton_active_pnumber_additive_4_clicked()
{
    setSelectedProduct(this->p_page_idle->thisMachine->getSelectedSlot()->getAdditivePNumber(4));
}

void page_maintenance_dispenser::on_pushButton_active_pnumber_additive_5_clicked()
{
    setSelectedProduct(this->p_page_idle->thisMachine->getSelectedSlot()->getAdditivePNumber(5));
}

void page_maintenance_dispenser::on_pushButton_dispense_pnumber_1_clicked()
{
    setSelectedProduct(this->p_page_idle->thisMachine->getSelectedSlot()->getDispensePNumber(1));
}

void page_maintenance_dispenser::on_pushButton_dispense_pnumber_2_clicked()
{
    setSelectedProduct(this->p_page_idle->thisMachine->getSelectedSlot()->getDispensePNumber(2));
}

void page_maintenance_dispenser::on_pushButton_dispense_pnumber_3_clicked()
{
    setSelectedProduct(this->p_page_idle->thisMachine->getSelectedSlot()->getDispensePNumber(3));
}

void page_maintenance_dispenser::on_pushButton_dispense_pnumber_4_clicked()
{
    setSelectedProduct(this->p_page_idle->thisMachine->getSelectedSlot()->getDispensePNumber(4));
}

void page_maintenance_dispenser::on_pushButton_dispense_pnumber_5_clicked()
{
    setSelectedProduct(this->p_page_idle->thisMachine->getSelectedSlot()->getDispensePNumber(5));
}

void page_maintenance_dispenser::on_pushButton_dispense_pnumber_6_clicked()
{
    setSelectedProduct(this->p_page_idle->thisMachine->getSelectedSlot()->getDispensePNumber(6));
}

void page_maintenance_dispenser::showSlotShortcut(int slot)
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        qDebug() << "Select shortcut for slot : " << QString::number(slot);
        dispense_test_end(true);
        maintainProductPageEndTimer->stop();
        statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.
        this->hide();
        usleep(1000);
        this->showFullScreen();
    }
}

void page_maintenance_dispenser::on_pushButton_activate_slot_1_clicked()
{
    this->p_page_idle->thisMachine->setSelectedSlot(1);
    showSlotShortcut(this->p_page_idle->thisMachine->getSelectedSlot()->getSlotId());
}
void page_maintenance_dispenser::on_pushButton_activate_slot_2_clicked()
{
    this->p_page_idle->thisMachine->setSelectedSlot(2);
    showSlotShortcut(this->p_page_idle->thisMachine->getSelectedSlot()->getSlotId());
}
void page_maintenance_dispenser::on_pushButton_activate_slot_3_clicked()
{
    this->p_page_idle->thisMachine->setSelectedSlot(3);
    showSlotShortcut(this->p_page_idle->thisMachine->getSelectedSlot()->getSlotId());
}
void page_maintenance_dispenser::on_pushButton_activate_slot_4_clicked()
{
    this->p_page_idle->thisMachine->setSelectedSlot(4);
    showSlotShortcut(this->p_page_idle->thisMachine->getSelectedSlot()->getSlotId());
}
void page_maintenance_dispenser::on_pushButton_activate_slot_5_clicked()
{
    this->p_page_idle->thisMachine->setSelectedSlot(5);
    showSlotShortcut(this->p_page_idle->thisMachine->getSelectedSlot()->getSlotId());
}

void page_maintenance_dispenser::setSizeIndex(int size_index)
{
    if (!isDispenserPumpEnabledWarningBox())
    {
        qDebug() << "Select size index: " << QString::number(size_index);
        m_selected_size_index = size_index;
        updateProductLabelValues(false);
    }
}

void page_maintenance_dispenser::on_pushButton_set_quantity_small_clicked()
{
    setSizeIndex(SIZE_SMALL_INDEX);
}
void page_maintenance_dispenser::on_pushButton_set_quantity_medium_clicked()
{
    setSizeIndex(SIZE_MEDIUM_INDEX);
}
void page_maintenance_dispenser::on_pushButton_set_quantity_large_clicked()
{
    setSizeIndex(SIZE_LARGE_INDEX);
}
void page_maintenance_dispenser::on_pushButton_set_quantity_custom_clicked()
{
    setSizeIndex(SIZE_CUSTOM_INDEX);
}
void page_maintenance_dispenser::on_pushButton_set_quantity_test_clicked()
{
    setSizeIndex(SIZE_TEST_INDEX);
}
void page_maintenance_dispenser::on_pushButton_set_quantity_sample_clicked()
{
    setSizeIndex(SIZE_SAMPLE_INDEX);
}
