
//***************************************
//
// page_product.cpp
// GUI class for user to select size and
// payment for drink.
//
// Allows navigation to First product page
// payment page and page_idle page
//
// created: 05-04-2022
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "page_product.h"
#include "ui_page_product.h"


using json = nlohmann::json;
QString transactionLogging = "";
extern bool promoApplied;
// button positions
uint16_t orderSizeButtons_xywh_dynamic_ui_all_sizes_available[4][4] = {
    {560, 990, 135, 100}, // S
    {706, 990, 135, 100}, // M
    {852, 990, 135, 100}, // L
    {560, 1100, 430, 115} // custom
};

uint16_t orderSizeButtons_xywh_dynamic_ui_small_available[4][4] = {
    {564, 1088, 209, 126}, // S
    {1, 1, 1, 1},          // M
    {1, 1, 1, 1},          // L
    {1, 1, 1, 1}           // custom
};

uint16_t orderSizeButtons_xywh_dynamic_ui_small_large_custom_available[4][4] = {
    {564, 990, 209, 100}, // S
    {1, 1, 1, 1},
    {790, 990, 198, 100}, // L
    {564, 1100, 424, 113} // custom
};

uint16_t orderSizeButtons_xywh_dynamic_ui_small_and_large_available[4][4] = {
    {567, 1024, 198, 126}, // S
    {1, 1, 1, 1},          // M
    {788, 1024, 198, 126}, // L
    {1, 1, 1, 1}           // custom
};
uint16_t orderSizeButtons_xywh_dynamic_ui_custom_available[4][4] = {
    {1, 1, 1, 1},
    {1, 1, 1, 1}, // M
    {1, 1, 1, 1},
    {564, 1037, 424, 113} // custom
    // {564, 1037, 424, 113} // custom
};
uint16_t orderSizeButtons_xywh_dynamic_ui_small_custom_available[4][4] = {
    {560, 990, 430, 100}, // S
    {1, 1, 1, 1},         // M
    {1, 1, 1, 1},
    {560, 1100, 430, 115} // custom
    // {564, 1037, 424, 113} // custom
};

uint16_t orderSizeButtons_xywh_dynamic_ui_large_custom_available[4][4] = {
    {1, 1, 1, 1},         // S
    {1, 1, 1, 1},         // M
    {560, 990, 430, 100}, // L
    {560, 1100, 430, 115} // custom
    // {564, 1037, 424, 113} // custom
};

// labels of volume and price are different (and annoying)
uint16_t orderSizeButtons_xywh_static_product_page[4][4] = {
    {564, 1088, 209, 126},
    {1, 1, 1, 1},
    {790, 1087, 198, 126},
    {1, 1, 1, 1}};

uint16_t orderSizeVolumeLabels_xy_dynamic_ui_all_sizes_available[4][2] = {
    {560, 1000}, // S vol
    {710, 1000}, // M vol
    {860, 1000}, // L vol
    {570, 1110}  // custom col
};
uint16_t orderSizePriceLabels_xy_dynamic_ui_all_sizes_available[8][2] = {
    {560, 1040}, // S price
    {710, 1040}, // M price
    {860, 1040}, // L price
    {560, 1160}  // custom price
};

uint16_t orderSizeVolumeLabels_xy_dynamic_ui_small_large_custom_available[8][2] = {
    {605, 1000}, // S vol
    {1, 1},      // M vol
    {825, 1000}, // L vol
    {570, 1110}  // custom col
};
uint16_t orderSizePriceLabels_xy_dynamic_ui_small_large_custom_available[8][2] = {
    {605, 1040}, // S price
    {1, 1},      // M price
    {825, 1040}, // L price
    {560, 1160}  // custom price
};

uint16_t orderSizeVolumeLabels_xy_dynamic_ui_custom_available[8][2] = {
    {1, 1},     // S vol
    {1, 1},     // M vol
    {1, 1},     // L vol
    {570, 1047} // custom col
};
uint16_t orderSizePriceLabels_xy_dynamic_ui_custom_available[8][2] = {
    {1, 1},     // S price
    {1, 1},     // M price
    {1, 1},     // L price
    {560, 1097} // custom price
};

uint16_t orderSizeVolumeLabels_xy_dynamic_ui_small_custom_available[8][2] = {
    {710, 1000}, // S vol
    {1, 1},      // M vol
    {1, 1},      // L vol
    {570, 1110}  // custom col
};

uint16_t orderSizePriceLabels_xy_dynamic_ui_small_custom_available[8][2] = {
    {710, 1040}, // S price
    {1, 1},      // M price
    {1, 1},      // L price
    {560, 1160}  // custom price
};

uint16_t orderSizeVolumeLabels_xy_dynamic_ui_large_custom_available[8][2] = {
    {1, 1}, // S vol
    {1, 1},      // M vol
    {710, 1000},      // L vol
    {570, 1110}  // custom col
};

uint16_t orderSizePriceLabels_xy_dynamic_ui_large_custom_available[8][2] = {
    {1, 1}, // S price
    {1, 1},      // M price
    {710, 1040},      // L price
    {560, 1160}  // custom price
};


uint16_t orderSizeVolumeLabels_xy_dynamic_ui_small_available[8][2] = {
    {605, 1150}, // S vol
    {1, 1},      // M vol
    {1, 1},      // L vol
    {1, 1}       // custom col
};
uint16_t orderSizePriceLabels_xy_dynamic_ui_small_available[8][2] = {
    {605, 1110}, // S price
    {1, 1},      // M price
    {1, 1},      // L price
    {1, 1}       // custom price
};

uint16_t orderSizeVolumeLabels_xy_dynamic_ui_small_and_large_available[8][2] = {
    {605, 1050}, // S vol
    {1, 1},      // M vol
    {825, 1050}, // L vol
    {1, 1}       // custom col
};
uint16_t orderSizePriceLabels_xy_dynamic_ui_small_and_large_available[8][2] = {
    {605, 1090}, // S price
    {1, 1},      // M price
    {825, 1090}, // L price
    {1, 1}       // custom price
};

// CTOR
page_product::page_product(QWidget *parent) : QWidget(parent),
                                              ui(new Ui::page_product)
{
    ui->setupUi(this);

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

    selectIdleTimer = new QTimer(this);
    selectIdleTimer->setInterval(40);
    connect(selectIdleTimer, SIGNAL(timeout()), this, SLOT(onSelectTimeoutTick()));

    transactionLogging = "";
}

/*
 * Page Tracking reference to Select Drink, Payment Page and Idle page
 */
void page_product::setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_qr_payment *page_qr_payment,page_payment_tap_serial *page_payment_tap_serial,page_payment_tap_tcp *page_payment_tap_tcp, page_help *pageHelp, page_product_overview *page_Overview)
{
    this->p_page_select_product = pageSelect;
    this->paymentPage = page_qr_payment;
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->p_page_help = pageHelp;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_overview = page_Overview;
    this->p_page_payment_tap_serial = page_payment_tap_serial;
    this->p_page_payment_tap_tcp = page_payment_tap_tcp;

    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_PRODUCT_BACKGROUND_PATH);
}

// DTOR
page_product::~page_product()
{
    delete ui;
}

/* GUI */
void page_product::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;

    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget
    
    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_PRODUCT_CSS);

    ui->label_product_title->setProperty("class", "title");
    ui->label_product_title->setStyleSheet(styleSheet);
    ui->pushButton_back->setStyleSheet(styleSheet); // pushbutton
    ui->label_product_description->setStyleSheet(styleSheet);
    ui->label_product_photo->setStyleSheet(styleSheet);
    ui->label_select_quantity->setStyleSheet(styleSheet);
    ui->label_product_ingredients->setStyleSheet(styleSheet);
    ui->label_product_ingredients_title->setStyleSheet(styleSheet);
    ui->label_help->setStyleSheet(styleSheet);
    ui->pushButton_continue->setStyleSheet(styleSheet);
    ui->pushButton_previous_page->setStyleSheet(styleSheet);
    ui->pushButton_to_help->setProperty("class", "button_transparent");
    ui->pushButton_to_help->setStyleSheet(styleSheet);

    for (int i = 0; i < 4; i++)
    {
        orderSizeLabelsVolume[i]->setProperty("class", "orderSizeLabelsVolume");
        orderSizeLabelsPrice[i]->setProperty("class", "orderSizeLabelsPrice");
        orderSizeBackgroundLabels[i]->setProperty("class", "orderSizeBackgroundLabels");
        orderSizeButtons[i]->setProperty("class", "orderSizeButtons");
        orderSizeBackgroundLabels[i]->setProperty("class", "orderSizeBackgroundLabels1");
        orderSizeLabelsVolume[i]->setStyleSheet(styleSheet);
        orderSizeLabelsPrice[i]->setStyleSheet(styleSheet);
        orderSizeBackgroundLabels[i]->setStyleSheet(styleSheet);
        orderSizeButtons[i]->setStyleSheet(styleSheet);
        orderSizeBackgroundLabels[i]->setStyleSheet(styleSheet);
    }

    reset_and_show_page_elements();
}

void page_product::resizeEvent(QResizeEvent *event)
{
}

void page_product::onSelectTimeoutTick()
{
    if (--_selectIdleTimeoutSec >= 0)
    {
    }
    else
    {
        selectIdleTimer->stop();
        hideCurrentPageAndShowProvided(p_page_idle);
    }
}

void page_product::reset_and_show_page_elements()
{

    // general setup
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_back);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_select_quantity);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_continue);

    ui->label_product_description->setWordWrap(true);
    ui->label_product_ingredients->setWordWrap(true);
    ui->pushButton_continue->hide();

    p_page_idle->thisMachine->addPictureToLabel(ui->label_product_photo, p_page_idle->thisMachine->selectedProduct->getProductPicturePath());

    ui->label_product_title->setText(p_page_idle->thisMachine->selectedProduct->getProductName());
    ui->label_product_ingredients->setText(p_page_idle->thisMachine->selectedProduct->getProductIngredients());
    ui->label_product_description->setText(p_page_idle->thisMachine->selectedProduct->getProductDescription());

    QString full_path = p_page_idle->thisMachine->getTemplatePathFromName(IMAGE_BUTTON_HELP);
    qDebug() << full_path;
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
        uint8_t enabled = p_page_idle->thisMachine->selectedProduct->getSizeEnabled(product_sizes[i]);
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
        if (p_page_idle->thisMachine->selectedProduct->getSizeEnabled(product_sizes[i]))
        {
            sizes_available_count++;

            orderSizeLabelsPrice[i]->show();
            orderSizeLabelsVolume[i]->show();
            orderSizeBackgroundLabels[i]->show();
            orderSizeButtons[i]->show();
            p_page_idle->thisMachine->addCssClassToObject(orderSizeLabelsVolume[i], "orderSizeLabelsVolume", PAGE_PRODUCT_CSS);
            p_page_idle->thisMachine->addCssClassToObject(orderSizeBackgroundLabels[i], "orderSizeBackgroundLabels", PAGE_PRODUCT_CSS);
            p_page_idle->thisMachine->addCssClassToObject(orderSizeButtons[i], "orderSizeButtons", PAGE_PRODUCT_CSS);
            p_page_idle->thisMachine->addCssClassToObject(orderSizeLabelsPrice[i], "orderSizeLabelsPrice", PAGE_PRODUCT_CSS);

            double price = p_page_idle->thisMachine->selectedProduct->getBasePrice(product_sizes[i]);

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

                p_page_idle->thisMachine->selectedProduct->getCustomDiscountDetails(&large_volume_discount_is_enabled, &min_volume_for_discount, &discount_price_per_liter);
                orderSizeLabelsVolume[i]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "custom_volume"));

                QString units = p_page_idle->thisMachine->selectedProduct->getUnitsForSlot();
                QString units_discount_indication = p_page_idle->thisMachine->selectedProduct->getUnitsForSlot();
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
                    if (p_page_idle->thisMachine->selectedProduct->getVolumeBySize(SIZE_CUSTOM_INDEX) == VOLUME_TO_TREAT_CUSTOM_DISPENSE_AS_PER_100G)
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
                    orderSizeLabelsPrice[i]->setText("$" + QString::number(price, 'f', 2) + "/" + units + " \n>" + QString::number(min_volume_for_discount, 'f', 1) + units_discount_indication + " @ $" + QString::number(discount_price_per_liter, 'f', 2) + "/" + units);
                    orderSizeButtons[i]->setFixedSize(QSize(xywh_size_buttons[i][2], xywh_size_buttons[i][3] + 100));
                    orderSizeLabelsPrice[i]->setFixedSize(QSize(420, 101));
                }
                else
                {
                    orderSizeLabelsPrice[i]->setFixedSize(QSize(420, 50));
                    orderSizeLabelsPrice[i]->setText("$" + QString::number(price, 'f', 2) + "/" + units);
                }
            }
            else
            {
                orderSizeLabelsPrice[i]->setText("$" + QString::number(price, 'f', 2));
                orderSizeLabelsVolume[i]->setText(p_page_idle->thisMachine->selectedProduct->getSizeToVolumeWithCorrectUnits(product_sizes[i], true, true));
            }
            orderSizeButtons[i]->raise();
        }
        else
        {
            qDebug() << "Product size index NOT enabled: " << i;
            orderSizeButtons[i]->hide();
        }
    }

    // it was confusing for the people to chose a quantity if there was only one quantity available. So, add a continue button if they can't chose anyways.
    if (sizes_available_count == 1)
    {
        ui->pushButton_continue->show();
    }
    else
    {
        ui->pushButton_continue->hide();
    }

    qDebug() << "-------------------------- END LOAD PRODUCTS ----------------";
}

bool page_product::stopSelectTimers()
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

void page_product::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{

    selectIdleTimer->stop();
    this->stopSelectTimers();
    p_page_idle->thisMachine->pageTransition(this, pageToShow);
}

void page_product::on_pushButton_to_help_clicked()
{
    hideCurrentPageAndShowProvided(p_page_help);
}

void page_product::on_pushButton_order_custom_clicked()
{
    qDebug() << "Button custom clicked ";
    p_page_idle->thisMachine->selectedProduct->setSize(SIZE_CUSTOM_INDEX);
    hideCurrentPageAndShowProvided(p_page_overview);
}

void page_product::on_pushButton_order_medium_clicked()
{
    qDebug() << "Button medium clicked";
    p_page_idle->thisMachine->selectedProduct->setSize(SIZE_MEDIUM_INDEX);
    hideCurrentPageAndShowProvided(p_page_overview);
}

// on_Small_Order button listener
void page_product::on_pushButton_order_small_clicked()
{
    qDebug() << "Button small clicked";
    p_page_idle->thisMachine->selectedProduct->setSize(SIZE_SMALL_INDEX);
    hideCurrentPageAndShowProvided(p_page_overview);
}

// on_Large_Order button listener
void page_product::on_pushButton_order_big_clicked()
{
    qDebug() << "Button big clicked";
    p_page_idle->thisMachine->selectedProduct->setSize(SIZE_LARGE_INDEX);
    hideCurrentPageAndShowProvided(p_page_overview);
}

size_t WriteCallback_coupon(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void page_product::on_pushButton_previous_page_clicked()
{
    hideCurrentPageAndShowProvided(p_page_select_product);
}

void page_product::on_pushButton_continue_clicked()
{
    // which size is enabled? select that size
    p_page_idle->thisMachine->selectedProduct->setSize(default_size);
    hideCurrentPageAndShowProvided(p_page_overview);
}

void page_product::on_pushButton_back_clicked()
{
    hideCurrentPageAndShowProvided(p_page_select_product);
}
