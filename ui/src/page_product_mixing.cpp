
//***************************************
//
// page_product_mixing.cpp
// GUI class for user to select size and
// payment for drink.
//
// Allows for product mixing + additivies
//
//
// created: 05-04-2022
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#include "page_product_mixing.h"
#include "ui_page_product_mixing.h"
#include "pnumberproduct.h"

using json = nlohmann::json;

// CTOR
page_product_mixing::page_product_mixing(QWidget *parent) : QWidget(parent),
                                              ui(new Ui::page_product_mixing)
{
    ui->setupUi(this);

    additiveBackgroundRows[0] = ui->label_additive_background_row_1;
    additiveBackgroundRows[1] = ui->label_additive_background_row_2;
    additiveBackgroundRows[2] = ui->label_additive_background_row_3;
    additiveBackgroundRows[3] = ui->label_additive_background_row_4;
    additiveBackgroundRows[4] = ui->label_additive_background_row_5;

    additiveTitles[0] = ui->label_additive_title_1;
    additiveTitles[1] = ui->label_additive_title_2;
    additiveTitles[2] = ui->label_additive_title_3;
    additiveTitles[3] = ui->label_additive_title_4;
    additiveTitles[4] = ui->label_additive_title_5;

    additiveMinusButtonBackgrounds[0] = ui->label_additive_minus_background_1;
    additiveMinusButtonBackgrounds[1] = ui->label_additive_minus_background_2;
    additiveMinusButtonBackgrounds[2] = ui->label_additive_minus_background_3;
    additiveMinusButtonBackgrounds[3] = ui->label_additive_minus_background_4;
    additiveMinusButtonBackgrounds[4] = ui->label_additive_minus_background_5;

    additiveMinusButtons[0] = ui->pushButton_additive_minus_1;
    additiveMinusButtons[1] = ui->pushButton_additive_minus_2;
    additiveMinusButtons[2] = ui->pushButton_additive_minus_3;
    additiveMinusButtons[3] = ui->pushButton_additive_minus_4;
    additiveMinusButtons[4] = ui->pushButton_additive_minus_5;

    QSignalMapper *minusButtonsMapper = new QSignalMapper(this);
    connect(minusButtonsMapper, SIGNAL(mapped(int)), this, SLOT(additiveMinusButtonsPressed(int)));
    for(int i = 0; i < 5; i++){
        minusButtonsMapper->setMapping(additiveMinusButtons[i], i + 1);
        connect(additiveMinusButtons[i], SIGNAL(clicked()), minusButtonsMapper, SLOT(map()));
    }

    additivePlusButtonBackgrounds[0] = ui->label_additive_plus_background_1;
    additivePlusButtonBackgrounds[1] = ui->label_additive_plus_background_2;
    additivePlusButtonBackgrounds[2] = ui->label_additive_plus_background_3;
    additivePlusButtonBackgrounds[3] = ui->label_additive_plus_background_4;
    additivePlusButtonBackgrounds[4] = ui->label_additive_plus_background_5;

    additivePlusButtons[0] = ui->pushButton_additive_plus_1;
    additivePlusButtons[1] = ui->pushButton_additive_plus_2;
    additivePlusButtons[2] = ui->pushButton_additive_plus_3;
    additivePlusButtons[3] = ui->pushButton_additive_plus_4;
    additivePlusButtons[4] = ui->pushButton_additive_plus_5;

    QSignalMapper *plusButtonsMapper = new QSignalMapper(this);
    connect(plusButtonsMapper, SIGNAL(mapped(int)), this, SLOT(additivePlusButtonsPressed(int)));
    for(int i = 0; i < 5; i++){
        plusButtonsMapper->setMapping(additivePlusButtons[i], i + 1);
        connect(additivePlusButtons[i], SIGNAL(clicked()), plusButtonsMapper, SLOT(map()));
    }

    // additivePercentageLabels[0] = ui->label_additive_percentage_1;
    // additivePercentageLabels[1] = ui->label_additive_percentage_2;
    // additivePercentageLabels[2] = ui->label_additive_percentage_3;
    // additivePercentageLabels[3] = ui->label_additive_percentage_4;
    // additivePercentageLabels[4] = ui->label_additive_percentage_5;

    additiveBarsLow[0] = ui->label_additive_bar_low_1;
    additiveBarsLow[1] = ui->label_additive_bar_low_2;
    additiveBarsLow[2] = ui->label_additive_bar_low_3;
    additiveBarsLow[3] = ui->label_additive_bar_low_4;
    additiveBarsLow[4] = ui->label_additive_bar_low_5;

    additiveBarsDefault[0] = ui->label_additive_bar_default_1;
    additiveBarsDefault[1] = ui->label_additive_bar_default_2;
    additiveBarsDefault[2] = ui->label_additive_bar_default_3;
    additiveBarsDefault[3] = ui->label_additive_bar_default_4;
    additiveBarsDefault[4] = ui->label_additive_bar_default_5;

    additiveBarsHigh[0] = ui->label_additive_bar_high_1;
    additiveBarsHigh[1] = ui->label_additive_bar_high_2;
    additiveBarsHigh[2] = ui->label_additive_bar_high_3;
    additiveBarsHigh[3] = ui->label_additive_bar_high_4;
    additiveBarsHigh[4] = ui->label_additive_bar_high_5;

    orderSizeButtons[0] = ui->pushButton_order_small;
    orderSizeButtons[1] = ui->pushButton_order_medium;
    orderSizeButtons[2] = ui->pushButton_order_big;
    orderSizeButtons[3] = ui->pushButton_order_custom;

    orderSizeLabelsPrice[0] = ui->label_price_small;
    orderSizeLabelsPrice[1] = ui->label_price_medium;
    orderSizeLabelsPrice[2] = ui->label_price_large;
    orderSizeLabelsPrice[3] = ui->label_price_custom;

    orderSizeLabelsVolume[0] = ui->label_size_small;
    orderSizeLabelsVolume[1] = ui->label_size_medium;
    orderSizeLabelsVolume[2] = ui->label_size_large;
    orderSizeLabelsVolume[3] = ui->label_size_custom;

    orderSizeBackgroundLabels[0] = ui->label_background_small;
    orderSizeBackgroundLabels[1] = ui->label_background_medium;
    orderSizeBackgroundLabels[2] = ui->label_background_large;
    orderSizeBackgroundLabels[3] = ui->label_background_custom;
    ui->pushButton_order_sample->hide();
    selectIdleTimer = new QTimer(this);
    selectIdleTimer->setInterval(40);
    connect(selectIdleTimer, SIGNAL(timeout()), this, SLOT(onSelectTimeoutTick()));

    statusbarLayout = new QVBoxLayout(this);
}

/*
 * Page Tracking reference to Select Drink, Payment Page and Idle page
 */
void page_product_mixing::setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_offline_payment *page_offline_payment,page_payment_tap_serial *page_payment_tap_serial, page_payment_tap_tcp *page_payment_tap_tcp, page_help *pageHelp, page_product_overview *page_Overview, statusbar *p_statusbar, page_product_menu *page_product_menu,page_product_freeSample *page_freeSample)
{
    this->p_page_product_menu = page_product_menu;
    this->p_page_select_product = pageSelect;
    this->paymentPage = page_qr_payment;
    this->paymentOfflinePage = page_offline_payment;
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->p_page_help = pageHelp;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_overview = page_Overview;
    this->p_page_payment_tap_serial = page_payment_tap_serial;
    this->p_page_payment_tap_tcp = page_payment_tap_tcp;
    this->p_statusbar = p_statusbar;
    this->p_page_product_freeSample = page_freeSample;
    this->p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_PRODUCT_MENU_BACKGROUND_PATH);
}

// DTOR
page_product_mixing::~page_product_mixing()
{
    delete ui;
}

/* GUI */
void page_product_mixing::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;

    statusbarLayout->addWidget(p_statusbar);            // Only one instance can be shown. So, has to be added/removed per page.
    statusbarLayout->setContentsMargins(0, 1874, 0, 0); // int left, int top, int right, int bottom);

    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget

    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_PRODUCT_MIXING_CSS);

    QString picturePath = p_page_idle->thisMachine->getSelectedProduct()->getProductPicturePath();
    styleSheet.replace("%IMAGE_PATH%", picturePath);

    ui->label_product_title->setProperty("class", "title");
    ui->label_product_title->setStyleSheet(styleSheet);

    // ui->pushButton_order_sample->setProperty("class","sampleBackgroundLabel");
    // ui->pushButton_order_sample->setStyleSheet(styleSheet);

    ui->pushButton_back->setStyleSheet(styleSheet); // pushbutton
    // ui->label_product_description->setStyleSheet(styleSheet);
    ui->label_product_photo->setStyleSheet(styleSheet);
    ui->label_customize_drink->setStyleSheet(styleSheet);
    ui->label_select_quantity->setStyleSheet(styleSheet);
    ui->label_product_ingredients->setStyleSheet(styleSheet);
    ui->label_product_ingredients_title->setStyleSheet(styleSheet);
    ui->label_additives_background->setStyleSheet(styleSheet);
    ui->label_additives_background->setWordWrap(true);
    ui->label_help->setStyleSheet(styleSheet);
    ui->pushButton_continue->setStyleSheet(styleSheet);
    ui->pushButton_order_sample->setStyleSheet(styleSheet);
    ui->pushButton_recommended->setStyleSheet(styleSheet);
    // ui->pushButton_previous_page->setStyleSheet(styleSheet);
    ui->pushButton_to_help->setProperty("class", "button_transparent");
    ui->pushButton_to_help->setStyleSheet(styleSheet);

    for (int i = 0; i < 4; i++)
    {
        orderSizeLabelsVolume[i]->setProperty("class", "orderSizeLabelsVolume");
        orderSizeLabelsPrice[i]->setProperty("class", "orderSizeLabelsPrice");
        orderSizeBackgroundLabels[i]->setProperty("class", "orderSizeBackgroundLabels");
        orderSizeButtons[i]->setProperty("class", "orderSizeButtons");
        // orderSizeBackgroundLabels[i]->setProperty("class", "orderSizeBackgroundLabels1");
        orderSizeLabelsVolume[i]->setStyleSheet(styleSheet);
        orderSizeLabelsPrice[i]->setStyleSheet(styleSheet);
        orderSizeBackgroundLabels[i]->setStyleSheet(styleSheet);
        orderSizeButtons[i]->setStyleSheet(styleSheet);
    }
        
    if(p_page_idle->thisMachine->getSelectedProduct()->getMixPNumbers().size() > 1){
        ui->pushButton_recommended->show();
        ui->label_additives_background->setText("");
        ui->label_additives_background->hide();
        for (int j = 0; j < ADDITIVES_PER_SLOT_COUNT_MAX; j++){
            additiveTitles[j]->setProperty("class", "additiveTitles");
            additiveBackgroundRows[j]->setProperty("class", "additiveBackgroundRows");
            additiveMinusButtonBackgrounds[j]->setProperty("class", "additiveMinusButtonBackgrounds");
            additiveMinusButtons[j]->setProperty("class", "additiveMinusButtons");
            additivePlusButtonBackgrounds[j]->setProperty("class", "additivePlusButtonBackgrounds");
            additivePlusButtons[j]->setProperty("class", "additivePlusButtons");
            additiveBarsLow[j]->setProperty("class", "additiveBarsLow");
            additiveBarsDefault[j]->setProperty("class", "additiveBarsDefault");
            additiveBarsHigh[j]->setProperty("class", "additiveBarsHigh");

            additiveTitles[j]->setStyleSheet(styleSheet);
            additiveBackgroundRows[j]->setStyleSheet(styleSheet);
            additiveMinusButtonBackgrounds[j]->setStyleSheet(styleSheet);
            additiveMinusButtons[j]->setStyleSheet(styleSheet);
            additivePlusButtonBackgrounds[j]->setStyleSheet(styleSheet);
            additivePlusButtons[j]->setStyleSheet(styleSheet);
            additiveBarsLow[j]->setStyleSheet(styleSheet);
            additiveBarsDefault[j]->setStyleSheet(styleSheet);
            additiveBarsHigh[j]->setStyleSheet(styleSheet);
        }
        qDebug() << "DEFAULTTT" << p_page_idle->thisMachine->getSelectedProduct()->getMixRatios();
        qDebug() << "CUSTOM" << p_page_idle->thisMachine->getSelectedProduct()->getCustomMixRatios();
        checkMixRatiosLevel();
        updateMixRatiosLevel();
    }else{
        ui->pushButton_recommended->hide();
        ui->label_additives_background->show();
        p_page_idle->thisMachine->setTemplateTextToObject(ui->label_additives_background);
        resetMixRatiosLevel();
        updateMixRatiosLevel();
    }
    p_page_idle->thisMachine->resetTransactionLogging();
    // transactionLogging = "";
    reset_and_show_page_elements();
}

void page_product_mixing::resizeEvent(QResizeEvent *event)
{
}

void page_product_mixing::onSelectTimeoutTick()
{
    if (--_selectIdleTimeoutSec >= 0)
    {
        // qDebug() << "Tick Down: " << _selectIdleTimeoutSec;
    }
    else
    {
        selectIdleTimer->stop();
        hideCurrentPageAndShowProvided(p_page_idle);
    }
}

void page_product_mixing::reset_and_show_page_elements()
{

    // general setup
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_back);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_customize_drink);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_select_quantity);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_product_ingredients_title);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_continue);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_recommended);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_order_sample);
    // ui->label_product_description->setWordWrap(true);
    ui->label_product_ingredients->setWordWrap(true);
    ui->pushButton_continue->hide();

    QString picturePath = p_page_idle->thisMachine->getSelectedProduct()->getProductPicturePath();
    ui->label_product_title->setText(p_page_idle->thisMachine->getSelectedProduct()->getProductName());
    ui->label_product_ingredients->setText(p_page_idle->thisMachine->getSelectedProduct()->getProductIngredients());
    // ui->label_product_description->setText(p_page_idle->thisMachine->getSelectedProduct()->getProductDescription());

    QString full_path = p_page_idle->thisMachine->getTemplatePathFromName(IMAGE_BUTTON_HELP);
    p_page_idle->thisMachine->addPictureToLabel(ui->label_help, full_path);

    selectIdleTimer->start(1000);
    _selectIdleTimeoutSec = 400;

    // dynamic page layout
    uint8_t available_sizes_signature = 0;
    uint16_t(*xywh_size_buttons)[4];
    uint16_t(*xy_size_labels_volume)[2];
    uint16_t(*xy_size_labels_price)[2];

    // create signature by sizes availability, use the bits
    for (uint8_t i = 0; i < 4; i++)
    {
        uint8_t enabled = p_page_idle->thisMachine->getSelectedProduct()->getSizeEnabled(product_sizes[i]);
        available_sizes_signature |= enabled << i;
        // S=1, M=2, L=4, C=8

        if (enabled)
        {
            orderSizeButtons[i]->show();
        }
        else
        {
            orderSizeButtons[i]->hide();
        }
    }

    // every combination
    if (available_sizes_signature == 1)
    {
        // only small available
        xywh_size_buttons = orderSizeButtons_xywh_dynamic_ui_small_available;
        xy_size_labels_volume = orderSizeVolumeLabels_xy_dynamic_ui_small_available;
        xy_size_labels_price = orderSizePriceLabels_xy_dynamic_ui_small_available;
    }
    else if (available_sizes_signature == 5)
    {
        // only small and large available
        xywh_size_buttons = orderSizeButtons_xywh_dynamic_ui_small_and_large_available;
        xy_size_labels_volume = orderSizeVolumeLabels_xy_dynamic_ui_small_and_large_available;
        xy_size_labels_price = orderSizePriceLabels_xy_dynamic_ui_small_and_large_available;
    }
    else if (available_sizes_signature == 8)
    {
        // only custom
        xywh_size_buttons = orderSizeButtons_xywh_dynamic_ui_custom_available;
        xy_size_labels_volume = orderSizeVolumeLabels_xy_dynamic_ui_custom_available;
        xy_size_labels_price = orderSizePriceLabels_xy_dynamic_ui_custom_available;
    }
    else if (available_sizes_signature == 9)
    {
        xywh_size_buttons = orderSizeButtons_xywh_dynamic_ui_small_custom_available;
        xy_size_labels_volume = orderSizeVolumeLabels_xy_dynamic_ui_small_custom_available;
        xy_size_labels_price = orderSizePriceLabels_xy_dynamic_ui_small_custom_available;
    }
    else if (available_sizes_signature == 12)
    {
        xywh_size_buttons = orderSizeButtons_xywh_dynamic_ui_large_custom_available;
        xy_size_labels_volume = orderSizeVolumeLabels_xy_dynamic_ui_large_custom_available;
        xy_size_labels_price = orderSizePriceLabels_xy_dynamic_ui_large_custom_available;
    }
    else if (available_sizes_signature == 13)
    {
        xywh_size_buttons = orderSizeButtons_xywh_dynamic_ui_small_large_custom_available;
        xy_size_labels_volume = orderSizeVolumeLabels_xy_dynamic_ui_small_large_custom_available;
        xy_size_labels_price = orderSizePriceLabels_xy_dynamic_ui_small_large_custom_available;
    }
    else if (available_sizes_signature == 15)
    {
        xywh_size_buttons = orderSizeButtons_xywh_dynamic_ui_all_sizes_available;
        xy_size_labels_volume = orderSizeVolumeLabels_xy_dynamic_ui_all_sizes_available;
        xy_size_labels_price = orderSizePriceLabels_xy_dynamic_ui_all_sizes_available;
    }
    else
    {
        qDebug() << "ERROR: Product signature SIZES not available (limited combinations of size buttons available to be set in database) signature: " << available_sizes_signature;
        // only small available
        xywh_size_buttons = orderSizeButtons_xywh_dynamic_ui_all_sizes_available;
        xy_size_labels_volume = orderSizeVolumeLabels_xy_dynamic_ui_all_sizes_available;
        xy_size_labels_price = orderSizePriceLabels_xy_dynamic_ui_all_sizes_available;
    }

    // set size specific details
    // run through all possible sizes
    int sizes_available_count = 0;
    for (uint8_t i = 0; i < 4; i++)
    {

        orderSizeLabelsPrice[i]->hide();
        orderSizeLabelsVolume[i]->hide();
        orderSizeBackgroundLabels[i]->hide();
        orderSizeButtons[i]->hide();

        // check if size is enabled
        if (p_page_idle->thisMachine->getSelectedProduct()->getSizeEnabled(product_sizes[i]))
        {
            sizes_available_count++;

            orderSizeLabelsPrice[i]->show();
            orderSizeLabelsVolume[i]->show();
            orderSizeBackgroundLabels[i]->show();
            orderSizeButtons[i]->show();
            p_page_idle->thisMachine->addCssClassToObject(orderSizeLabelsVolume[i], "orderSizeLabelsVolume", PAGE_PRODUCT_MIXING_CSS);
            p_page_idle->thisMachine->addCssClassToObject(orderSizeBackgroundLabels[i], "orderSizeBackgroundLabels", PAGE_PRODUCT_MIXING_CSS);
            p_page_idle->thisMachine->addCssClassToObject(orderSizeButtons[i], "orderSizeButtons", PAGE_PRODUCT_MIXING_CSS);
            p_page_idle->thisMachine->addCssClassToObject(orderSizeLabelsPrice[i], "orderSizeLabelsPrice", PAGE_PRODUCT_MIXING_CSS);

            double price = p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(product_sizes[i]);

            // if there is only one product size available, the continue button will show, it will then load this default size
            default_size = product_sizes[i];

            orderSizeButtons[i]->setFixedSize(QSize(xywh_size_buttons[i][2], xywh_size_buttons[i][3]));
            orderSizeButtons[i]->move(xywh_size_buttons[i][0], xywh_size_buttons[i][1]);

            orderSizeBackgroundLabels[i]->setFixedSize(QSize(xywh_size_buttons[i][2], xywh_size_buttons[i][3]));
            orderSizeBackgroundLabels[i]->move(xywh_size_buttons[i][0], xywh_size_buttons[i][1]);
            orderSizeBackgroundLabels[i]->lower();

            orderSizeLabelsPrice[i]->move(xy_size_labels_price[i][0], xy_size_labels_price[i][1]);
            orderSizeLabelsVolume[i]->move(xy_size_labels_volume[i][0], xy_size_labels_volume[i][1]);

            if (product_sizes[i] == SIZE_CUSTOM_INDEX)
            {
                bool large_volume_discount_is_enabled;
                double min_volume_for_discount;
                double discount_price_per_liter;
                // check if there is a price decline for large volumes

                p_page_idle->thisMachine->getSelectedProduct()->getCustomDiscountDetails(&large_volume_discount_is_enabled, &min_volume_for_discount, &discount_price_per_liter);
                orderSizeLabelsVolume[i]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "custom_volume"));

                QString units = p_page_idle->thisMachine->getSizeUnit();
                QString units_discount_indication = p_page_idle->thisMachine->getSizeUnit();
                if (units == "ml")
                {
                    units = "100ml";
                    units_discount_indication = "L";
                    price = price * 100;
                    discount_price_per_liter *= 1000;
                    min_volume_for_discount = min_volume_for_discount / 1000;
                }

                else if (units == "g")
                {
                    if (p_page_idle->thisMachine->getSelectedProduct()->getVolumeBySize(SIZE_CUSTOM_INDEX) == VOLUME_TO_TREAT_CUSTOM_DISPENSE_AS_PER_100G)
                    {
                        units = "100g";
                        units_discount_indication = "kg";
                        price = price * 100;
                        discount_price_per_liter *= 1000;
                        min_volume_for_discount = min_volume_for_discount / 1000;
                    }
                    else
                    {
                        units = "kg";
                        units_discount_indication = "kg";
                        price = price * 1000;
                        discount_price_per_liter *= 1000;
                        min_volume_for_discount = min_volume_for_discount / 1000.0;
                    }
                }
                else if (units == "oz")
                {
                    units = "oz";
                    units_discount_indication = "oz";
                    price = price * OZ_TO_ML;
                    discount_price_per_liter *= OZ_TO_ML;
                    min_volume_for_discount /= OZ_TO_ML;
                }

                orderSizeLabelsPrice[i]->setWordWrap(true);

                if (large_volume_discount_is_enabled)
                {
                    orderSizeLabelsPrice[i]->setText("$" + QString::number(price, 'f', 2) + "/" + "\n" + units + " \n>" + QString::number(min_volume_for_discount, 'f', 1) + units_discount_indication + " @ $" + QString::number(discount_price_per_liter, 'f', 2) + "/" + units);
                    orderSizeButtons[i]->setFixedSize(QSize(xywh_size_buttons[i][2], xywh_size_buttons[i][3] + 100));
                    orderSizeLabelsPrice[i]->setFixedSize(QSize(180, 26));
                }
                else
                {
                    orderSizeLabelsPrice[i]->setFixedSize(QSize(113, 72));
                    orderSizeLabelsPrice[i]->setText("$" + QString::number(price, 'f', 2) + "/" + "\n" + units);
                }
            }
            
            else
            {
                orderSizeLabelsPrice[i]->setText("$" + QString::number(price, 'f', 2));
                orderSizeLabelsVolume[i]->setText(p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(product_sizes[i], true, true));
            }
            orderSizeButtons[i]->raise();
        }

        else
        {
            qDebug() << "Product size index NOT enabled: " << i;
            orderSizeButtons[i]->hide();
        }
        
    }
    ui->label_price_custom->setAlignment(Qt::AlignCenter);

    // it was confusing for the people to chose a quantity if there was only one quantity available. So, add a continue button if they can't chose anyways.
    if (sizes_available_count == 1)
    {
        ui->pushButton_continue->show();
    }
    else
    {
        ui->pushButton_continue->hide();
    }
    if(p_page_idle->thisMachine->getSelectedProduct()->getSizeEnabled(SIZE_SAMPLE_INDEX))
    {
        ui->pushButton_order_sample->show();
        ui->pushButton_order_sample->raise();
    }else{
        ui->pushButton_order_sample->hide();
    }

    qDebug() << "-------------------------- END LOAD PRODUCTS ----------------";
}

bool page_product_mixing::stopSelectTimers()
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

void page_product_mixing::hideCurrentPageAndShowProductMenu()
{
    if (p_page_idle->thisMachine->getHardwareMajorVersion().startsWith("AP2"))
    {
        this->hideCurrentPageAndShowProvided(p_page_product_menu);
    }
    else
    {
        this->hideCurrentPageAndShowProvided(p_page_select_product);
    }
}
    
void page_product_mixing::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{

    selectIdleTimer->stop();
    this->stopSelectTimers();

    statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.

    p_page_idle->thisMachine->pageTransition(this, pageToShow);
}

void page_product_mixing::on_pushButton_to_help_clicked()
{
    hideCurrentPageAndShowProvided(p_page_help);
}

void page_product_mixing::on_pushButton_order_custom_clicked()
{
    qDebug() << "Button custom clicked ";
    p_page_idle->thisMachine->getSelectedProduct()->setSelectedSize(SIZE_CUSTOM_INDEX);
    hideCurrentPageAndShowProvided(p_page_overview);
}

void page_product_mixing::on_pushButton_order_medium_clicked()
{
    qDebug() << "Button medium clicked";
    p_page_idle->thisMachine->getSelectedProduct()->setSelectedSize(SIZE_MEDIUM_INDEX);
    hideCurrentPageAndShowProvided(p_page_overview);
}

// on_Small_Order button listener
void page_product_mixing::on_pushButton_order_small_clicked()
{
    qDebug() << "Button small clicked";
    p_page_idle->thisMachine->getSelectedProduct()->setSelectedSize(SIZE_SMALL_INDEX);
    hideCurrentPageAndShowProvided(p_page_overview);
}

// on_Large_Order button listener
void page_product_mixing::on_pushButton_order_big_clicked()
{
    qDebug() << "Button big clicked";
    p_page_idle->thisMachine->getSelectedProduct()->setSelectedSize(SIZE_LARGE_INDEX);
    hideCurrentPageAndShowProvided(p_page_overview);
}

void page_product_mixing::on_pushButton_order_sample_clicked()
{
    qDebug() << "Button sample clicked";
    p_page_idle->thisMachine->getSelectedProduct()->setSelectedSize(SIZE_SAMPLE_INDEX);
    hideCurrentPageAndShowProvided(p_page_product_freeSample);
}

size_t page_product_mixing::WriteCallback_coupon(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

// void page_product_mixing::on_pushButton_previous_page_clicked()
// {
//     hideCurrentPageAndShowProductMenu();
// }

void page_product_mixing::on_pushButton_continue_clicked()
{
    // which size is enabled? select that size
    p_page_idle->thisMachine->getSelectedProduct()->setSelectedSize(default_size);
    hideCurrentPageAndShowProvided(p_page_overview);
}

void page_product_mixing::on_pushButton_back_clicked()
{
    hideCurrentPageAndShowProductMenu();
}

double page_product_mixing::convertAdditivePRatioToPercentage(double additivePRatio)
{
    return additivePRatio * 100;
} 

bool page_product_mixing::isAdditiveEnabled(int index){
    // 1st value in mix_pnumbers is the base pnumber which we do not care about for additives ratio
    // if within boundry return true
    return index+1 <= p_page_idle->thisMachine->getSelectedProduct()->getMixPNumbers().size() - 1;
}

void page_product_mixing::additiveMinusButtonsPressed(int index){
    p_page_idle->thisMachine->getSelectedProduct()->setCustomMixRatios(index, "-"); //starts off index = 1; perfect for this function because index 0 is base, which is not an additive
    if(m_mixRatios_level[index-1] > 0){ // m_mixRatios_level does not include base, index starts off at 1; need to start of from zero
        if(m_mixRatios_level[index-1] == 1){
            // dont do anything because lowest level
        }else{
            m_mixRatios_level[index-1]--;
        }
    }
    updateMixRatiosLevel();

    // !!!!!!!!! For displaying percentages - will probably need tweaking !!!!!!!!!!!!!!
    // double additiveRatioToPercentage = convertAdditivePRatioToPercentage(additivePRatio) - ADDITIVES_RATIO_INCREMENT;
    // if( additiveRatioToPercentage >= 0){
    //     additivePercentageLabels[index - 1]->setText(QString::number(additiveRatioToPercentage) + "%");
    //     p_page_idle->thisMachine->getSelectedProduct()->adjustAdditivesRatioModifier(index - 1, additiveRatioToPercentage / 100.0);
    // }else{
    //     additiveRatioToPercentage = 0;
    //     additivePercentageLabels[index - 1]->setText(QString::number(additiveRatioToPercentage) + "%");
    //     p_page_idle->thisMachine->getSelectedProduct()->adjustAdditivesRatioModifier(index - 1, additiveRatioToPercentage / 100.0);
    // }
}

void page_product_mixing::additivePlusButtonsPressed(int index){
    p_page_idle->thisMachine->getSelectedProduct()->setCustomMixRatios(index, "+"); //starts off index = 1; perfect for this function because index 0 is base, which is not an additive
    if(m_mixRatios_level[index-1] < 3){ // m_mixRatios_level does not include base, index starts off at 1; need to start of from zero
        if(m_mixRatios_level[index-1] == 0){
            // dont do anything because additive unadjustable
        }else{
            m_mixRatios_level[index-1]++;
        }
    }
    updateMixRatiosLevel();

    // !!!!!!!!! For displaying percentages - will probably need tweaking !!!!!!!!!
    // double additivePRatio = p_page_idle->thisMachine->getSelectedProduct()->getAdditivesRatioModifier(index - 1);
    // double additiveRatioToPercentage = convertAdditivePRatioToPercentage(additivePRatio) + ADDITIVES_RATIO_INCREMENT;
    // if( additiveRatioToPercentage <= 999){
    //     additivePercentageLabels[index - 1]->setText(QString::number(additiveRatioToPercentage) + "%");
    //     p_page_idle->thisMachine->getSelectedProduct()->adjustAdditivesRatioModifier(index - 1, additiveRatioToPercentage / 100.0);
    // }else{
    //     additiveRatioToPercentage = 990;
    //     additivePercentageLabels[index - 1]->setText(QString::number(additiveRatioToPercentage) + "%");
    //     p_page_idle->thisMachine->getSelectedProduct()->adjustAdditivesRatioModifier(index - 1, additiveRatioToPercentage / 100.0);
    // }
}

void page_product_mixing::on_pushButton_recommended_clicked(){
    resetMixRatiosLevel();
    p_page_idle->thisMachine->getSelectedProduct()->resetCustomMixRatioParameters();
    updateMixRatiosLevel();

    // For displaying percentages - will probably need tweaking
    // p_page_idle->thisMachine->getSelectedProduct()->resetCustomMixRatioParameters();
    // for (int j = 0; j < 5; j++){
    //     if(isAdditiveEnabled(j)){
    //         double additivePRatio = p_page_idle->thisMachine->getSelectedProduct()->getAdditivesRatioModifier(j);
    //         QString additivePRatio_string = QString::number(convertAdditivePRatioToPercentage(additivePRatio));
    //         additivePercentageLabels[j]->setText(additivePRatio_string + "%");
    //     }
    // }
}

void page_product_mixing::checkMixRatiosLevel(){ 
    //check if customer has already adjust additives
    // if adjusted; don't do anything
    // otherwise reset mix ratios level
    // qDebug() << "DEFAULTTTTT" << p_page_idle->thisMachine->getSelectedProduct()->getMixRatios();
    // qDebug() << "CUSTOMMMMM" << p_page_idle->thisMachine->getSelectedProduct()->getCustomMixRatios();
    bool resetCustomMixRatios = true;
    for(int i = 1; i < p_page_idle->thisMachine->getSelectedProduct()->getMixRatios().size(); i++){
        if(p_page_idle->thisMachine->getSelectedProduct()->getMixRatios()[i] != p_page_idle->thisMachine->getSelectedProduct()->getCustomMixRatios()[i]){
            resetCustomMixRatios = false;
        }
    }
    if(resetCustomMixRatios){
        resetMixRatiosLevel();
    }
}

void page_product_mixing::resetMixRatiosLevel(){
    m_mixRatios_level.clear();
    if(p_page_idle->thisMachine->getSelectedProduct()->getMixPNumbers().size() > 1){
        for (int j = 1; j <= ADDITIVES_PER_SLOT_COUNT_MAX; j++){ // 1, 2, 3, 4, 5 -> additives
            if(j <= p_page_idle->thisMachine->getSelectedProduct()->getMixPNumbers().size() - 1){ // max Mix PNumbers = 6; dont need the first because base product
                double low = p_page_idle->thisMachine->getSelectedProduct()->getMixRatiosLow()[j];
                double def = p_page_idle->thisMachine->getSelectedProduct()->getMixRatios()[j];
                double high = p_page_idle->thisMachine->getSelectedProduct()->getMixRatiosHigh()[j];
                if(low == def && def == high){ 
                    // if additive getMixRatiosLow()[j] == getMixRatiosDefault()[j] == getMixRatiosHigh()[j]; hide the additive adjustment row
                    // if additive level are the same for low, default, and high; it means additivie is not adjustable
                    // set to Zero Level
                    m_mixRatios_level.insert(j-1, 0);
                }else{
                    // set to Two Level for default
                    m_mixRatios_level.insert(j-1, 2);
                }
            }else{
                // set to Zero Level
                m_mixRatios_level.insert(j-1, 0);
            }
        }
    }else{
        for (int j = 1; j <= ADDITIVES_PER_SLOT_COUNT_MAX; j++){ // 1, 2, 3, 4, 5 -> additives
            // set to Zero Level for all Mix PNumbers because no additives for the dispense product
            m_mixRatios_level.insert(j-1, 0);
        }
    }
}

void page_product_mixing::updateMixRatiosLevel(){
    qDebug() << "DEFAULTTT" << p_page_idle->thisMachine->getSelectedProduct()->getMixRatios();
    qDebug() << "CUSTOM" << p_page_idle->thisMachine->getSelectedProduct()->getCustomMixRatios();
    qDebug() << "LEVEL" << m_mixRatios_level;
    for (int i = 0; i < m_mixRatios_level.size(); i++){
        if(m_mixRatios_level[i] > 0){ 
            additiveTitles[i]->show();
            additiveBackgroundRows[i]->show();
            additiveMinusButtonBackgrounds[i]->show();
            additiveMinusButtons[i]->show();
            additivePlusButtonBackgrounds[i]->show();
            additivePlusButtons[i]->show();
            additiveBarsLow[i]->show();
            additiveBarsDefault[i]->show();
            additiveBarsHigh[i]->show();
            int additivePNumber = p_page_idle->thisMachine->getSelectedProduct()->getMixPNumbers()[i+1];
            additiveTitles[i]->setText(p_page_idle->thisMachine->getProductByPNumber(additivePNumber)->getProductName());
            if(m_mixRatios_level[i] == 3){ // shade in all 3 bars = high
                p_page_idle->thisMachine->addCssClassToObject(additiveBarsLow[i], "fill", PAGE_PRODUCT_MIXING_CSS);
                p_page_idle->thisMachine->addCssClassToObject(additiveBarsDefault[i], "fill", PAGE_PRODUCT_MIXING_CSS);
                p_page_idle->thisMachine->addCssClassToObject(additiveBarsHigh[i], "fill", PAGE_PRODUCT_MIXING_CSS);
            }else if(m_mixRatios_level[i] == 2){ // shade in 2 bars = default
                p_page_idle->thisMachine->addCssClassToObject(additiveBarsLow[i], "fill", PAGE_PRODUCT_MIXING_CSS);
                p_page_idle->thisMachine->addCssClassToObject(additiveBarsDefault[i], "fill", PAGE_PRODUCT_MIXING_CSS);
                p_page_idle->thisMachine->addCssClassToObject(additiveBarsHigh[i], "empty", PAGE_PRODUCT_MIXING_CSS);
            }else{ // shade in 1 bar = low
                p_page_idle->thisMachine->addCssClassToObject(additiveBarsLow[i], "fill", PAGE_PRODUCT_MIXING_CSS);
                p_page_idle->thisMachine->addCssClassToObject(additiveBarsDefault[i], "empty", PAGE_PRODUCT_MIXING_CSS);
                p_page_idle->thisMachine->addCssClassToObject(additiveBarsHigh[i], "empty", PAGE_PRODUCT_MIXING_CSS);
            }
        }else{ // if zero bars; hide the row
            additiveTitles[i]->hide();
            additiveBackgroundRows[i]->hide();
            additiveMinusButtonBackgrounds[i]->hide();
            additiveMinusButtons[i]->hide();
            additivePlusButtonBackgrounds[i]->hide();
            additivePlusButtons[i]->hide();
            additiveBarsLow[i]->hide();
            additiveBarsDefault[i]->hide();
            additiveBarsHigh[i]->hide();
        }
    }
}

