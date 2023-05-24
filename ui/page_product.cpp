
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
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "page_product.h"
#include "ui_page_product.h"
#include <iostream>
#include <string>
#include <cmath>

#include "page_qr_payment.h"
#include "page_select_product.h"
#include "page_productOverview.h"
#include "product.h"

#include "page_idle.h"
#include <curl/curl.h>
#include <json.hpp>

using json = nlohmann::json;
QString transactionLogging = "";
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
uint16_t orderSizePriceLabels_xy_dynamic_ui_custom_available[8][2] = {
    {1, 1},     // S price
    {1, 1},     // M price
    {1, 1},     // L price
    {560, 1097} // custom price
};

uint16_t orderSizeVolumeLabels_xy_dynamic_ui_custom_available[8][2] = {
    {1, 1},     // S vol
    {1, 1},     // M vol
    {1, 1},     // L vol
    {570, 1047} // custom col
};

uint16_t orderSizeVolumeLabels_xy_dynamic_ui_small_available[8][2] = {
    {605, 1150}, // S vol
    {1, 1},      // M vol
    {1, 1},      // L vol
    {1, 1}       // custom col
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
uint16_t orderSizePriceLabels_xy_dynamic_ui_small_available[8][2] = {
    {605, 1110}, // S price
    {1, 1},      // M price
    {1, 1},      // L price
    {1, 1}       // custom price
};

// CTOR
pageProduct::pageProduct(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::pageProduct)
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



    {
        selectIdleTimer = new QTimer(this);
        selectIdleTimer->setInterval(40);
        connect(selectIdleTimer, SIGNAL(timeout()), this, SLOT(onSelectTimeoutTick()));
    }



    transactionLogging = "";
}

/*
 * Page Tracking reference to Select Drink, Payment Page and Idle page
 */
void pageProduct::setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_help *pageHelp, page_product_overview *page_Overview)
{
    this->p_page_select_product = pageSelect;
    this->paymentPage = page_qr_payment;
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->p_page_help = pageHelp;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_overview = page_Overview;

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_PRODUCT_BACKGROUND_PATH);
}

// DTOR
pageProduct::~pageProduct()
{
    delete ui;
}

/* GUI */
void pageProduct::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    qDebug() << "<<<<<<< Page Enter: Product >>>>>>>>>";

    QString styleSheet = p_page_idle->getCSS(PAGE_PRODUCT_CSS);

    ui->label_product_title->setProperty("class", "css_title");
    ui->label_product_title->setStyleSheet(styleSheet);
    ui->pushButton_back->setStyleSheet(styleSheet);
    ui->label_product_description->setStyleSheet(styleSheet);
    ui->label_product_photo->setStyleSheet(styleSheet);
    ui->label_select_quantity->setStyleSheet(styleSheet);
    ui->label_product_ingredients->setStyleSheet(styleSheet);
    ui->label_product_ingredients_title->setStyleSheet(styleSheet);
    ui->label_notify_us->setStyleSheet(styleSheet);
    ui->pushButton_continue->setStyleSheet(styleSheet);
    ui->pushButton_previous_page->setStyleSheet(styleSheet);
    ui->pushButton_to_idle->setStyleSheet(styleSheet);

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

    p_page_idle->selectedProduct->loadProductProperties();
    loadProdSpecs();
    
    reset_and_show_page_elements();
}

void pageProduct::resizeEvent(QResizeEvent *event)
{
    qDebug() << "\n---Page_product: resizeEvent";
}

void pageProduct::onSelectTimeoutTick()
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

void pageProduct::reset_and_show_page_elements()
{

    // ui->pushButton_back->setText("<- Products");
    // ui->label_select_quantity->setText("Select Quantity");
    // ui->pushButton_continue->setText("Continue");
    p_page_idle->setTemplateTextToObject(ui->pushButton_back);
    p_page_idle->setTemplateTextToObject(ui->label_select_quantity);
    p_page_idle->setTemplateTextToObject(ui->pushButton_continue);
   
    ui->label_product_description->setWordWrap(true);
    ui->label_product_ingredients->setWordWrap(true);
    ui->pushButton_continue->hide();


    p_page_idle->addPictureToLabel(ui->label_product_photo, p_page_idle->selectedProduct->getProductPicturePath());

    ui->label_product_title->setText(p_page_idle->selectedProduct->getProductName());
    ui->label_product_ingredients->setText(p_page_idle->selectedProduct->getProductIngredients());
    ui->label_product_description->setText(p_page_idle->selectedProduct->getProductDescription());
    QString full_path = p_page_idle->getTemplatePathFromName(IMAGE_BUTTON_HELP);
    qDebug() << full_path;
    p_page_idle->addPictureToLabel(ui->label_notify_us, full_path);

    // bitmap_location = PAGE_PRODUCT_BACKGROUND_PATH;
    // uint16_t orderSizeButtons_xywh[4][4] = {
    //     {560, 990, 135, 110},  // S
    //     {706, 990, 135, 105},  // M
    //     {852, 990, 135, 100},  // L
    //     {560, 1100, 430, 115}   // custom
    //     };

    uint8_t available_sizes_signature = 0;

    uint16_t(*xywh_size_buttons)[4];
    uint16_t(*xy_size_labels_volume)[2];
    uint16_t(*xy_size_labels_price)[2];

    // create signature by sizes availability, use the bits
    for (uint8_t i = 0; i < 4; i++)
    {
        available_sizes_signature |= p_page_idle->selectedProduct->getSizeEnabled(product_sizes[i]) << i;
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
    else if (available_sizes_signature == 13)
    {
        xywh_size_buttons = orderSizeButtons_xywh_dynamic_ui_small_large_custom_available;
        xy_size_labels_volume = orderSizeVolumeLabels_xy_dynamic_ui_small_large_custom_available;
        xy_size_labels_price = orderSizePriceLabels_xy_dynamic_ui_small_large_custom_available;
    }
    else if (available_sizes_signature == 8)
    {
        // only custom
        xywh_size_buttons = orderSizeButtons_xywh_dynamic_ui_custom_available;
        xy_size_labels_volume = orderSizeVolumeLabels_xy_dynamic_ui_custom_available;
        xy_size_labels_price = orderSizePriceLabels_xy_dynamic_ui_custom_available;
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
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        if (p_page_idle->selectedProduct->getSizeEnabled(product_sizes[i]))
        {
            orderSizeButtons[i]->show();

            orderSizeButtons[i]->setFixedSize(QSize(xywh_size_buttons[i][2], xywh_size_buttons[i][3]));
            orderSizeButtons[i]->move(xywh_size_buttons[i][0], xywh_size_buttons[i][1]);

            orderSizeBackgroundLabels[i]->setFixedSize(QSize(xywh_size_buttons[i][2], xywh_size_buttons[i][3]));
            orderSizeBackgroundLabels[i]->move(xywh_size_buttons[i][0], xywh_size_buttons[i][1]);
            orderSizeBackgroundLabels[i]->lower();
            orderSizeBackgroundLabels[i]->setProperty("class", "orderSizeBackgroundLabels1");

            //       labels_product_overlay_text[i]->setProperty("class", "label_product_oberlay_sold_out");

            orderSizeLabelsPrice[i]->move(xy_size_labels_price[i][0], xy_size_labels_price[i][1]);
            orderSizeLabelsVolume[i]->move(xy_size_labels_volume[i][0], xy_size_labels_volume[i][1]);

            qDebug() << "Product size index enabled: " << i;

            if (product_sizes[i] == SIZE_CUSTOM_INDEX)
            {

                bool large_volume_discount_is_enabled;
                double min_volume_for_discount;
                double discount_price_per_liter;
                // check if there is a price decline for large volumes

                p_page_idle->selectedProduct->getCustomDiscountDetails(&large_volume_discount_is_enabled, &min_volume_for_discount, &discount_price_per_liter);
                if (large_volume_discount_is_enabled)
                {
                    orderSizeButtons[i]->setFixedSize(QSize(xywh_size_buttons[i][2], xywh_size_buttons[i][3] + 100));
                    orderSizeLabelsPrice[i]->setFixedSize(QSize(420, 101));
                }
                else
                {
                    orderSizeLabelsPrice[i]->setFixedSize(QSize(420, 50));
                }
            }
        }
        else
        {
            qDebug() << "Product size index NOT enabled: " << i;
            orderSizeButtons[i]->hide();
        }
    }

    // ordersize buttons
    /* Hacky transparent button */
    loadProdSpecs();
    selectIdleTimer->start(1000);
    _selectIdleTimeoutSec = 400;
}

void pageProduct::loadProductBySize(int sizeIndex)
{
    p_page_idle->selectedProduct->setSize(sizeIndex);
    loadProdSpecs();
}

void pageProduct::loadProdSpecs()
{

    qDebug() << "-------------------------- LOAD PRODUCTS ----------------";
    _selectIdleTimeoutSec = 140;

    ui->pushButton_to_idle->setEnabled(true);
    ui->pushButton_previous_page->setEnabled(true);

    int sizes_available_count = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        orderSizeLabelsPrice[i]->hide();
        orderSizeLabelsVolume[i]->hide();
        orderSizeBackgroundLabels[i]->hide();
        orderSizeButtons[i]->hide();
        if (p_page_idle->selectedProduct->getSizeEnabled(product_sizes[i]))
        {
            sizes_available_count++;

            orderSizeLabelsPrice[i]->show();
            orderSizeLabelsVolume[i]->show();
            orderSizeBackgroundLabels[i]->show();
            orderSizeButtons[i]->show();
            orderSizeLabelsVolume[i]->setProperty("class", "orderSizeLabelsVolume");
            orderSizeLabelsPrice[i]->setProperty("class", "orderSizeLabelsPrice");
            orderSizeBackgroundLabels[i]->setProperty("class", "orderSizeBackgroundLabels");
            orderSizeButtons[i]->setProperty("class", "orderSizeButtons");

            double price = p_page_idle->selectedProduct->getPrice(product_sizes[i]);
            // QString transparent_path = FULL_TRANSPARENT_IMAGE_PATH;

            if (product_sizes[i] == SIZE_CUSTOM_INDEX)
            {
                bool large_volume_discount_is_enabled;
                double min_volume_for_discount;
                double discount_price_per_liter;
                // check if there is a price decline for large volumes
                p_page_idle->selectedProduct->getCustomDiscountDetails(&large_volume_discount_is_enabled, &min_volume_for_discount, &discount_price_per_liter);

                orderSizeLabelsVolume[i]->setText("Custom Volume");
                QString units = p_page_idle->selectedProduct->getUnitsForSlot();
                QString units_discount_indication = p_page_idle->selectedProduct->getUnitsForSlot();
                if (units == "ml")
                {
                    units = "L";
                    units_discount_indication = "L";
                    price = price * 1000;
                    discount_price_per_liter *= 1000;
                    min_volume_for_discount = min_volume_for_discount / 1000;
                }

                else if (units == "g")
                {
                    if (p_page_idle->selectedProduct->getVolume(SIZE_CUSTOM_INDEX) == VOLUME_TO_TREAT_CUSTOM_DISPENSE_AS_PER_100G)
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
                }
                else
                {
                    orderSizeLabelsPrice[i]->setText("$" + QString::number(price, 'f', 2) + "/" + units);
                }
            }
            else
            {
                orderSizeLabelsPrice[i]->setText("$" + QString::number(price, 'f', 2));
                orderSizeLabelsVolume[i]->setText(p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(product_sizes[i], true, true));
            }

            // selected size styling
            if (p_page_idle->selectedProduct->getSize() == product_sizes[i])
            {
                orderSizeLabelsVolume[i]->setProperty("class", "orderSizeLabelsVolume");
                // orderSizeLabelsPrice[i]->setStyleSheet("QLabel {font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;}");
                orderSizeLabelsPrice[i]->setProperty("class", "orderSizeLabelsPrice");
                // orderSizeBackgroundLabels[i]->setStyleSheet("QLabel { background-color: #FFFFFF; border: 0px }");
                orderSizeBackgroundLabels[i]->setProperty("class", "orderSizeBackgroundLabels");
                // orderSizeButtons[i]->setStyleSheet("QPushButton { background-color: transparent; border: 1px  solid #3D6675; }");
                orderSizeButtons[i]->setProperty("class", "orderSizeButtons");
            }
            orderSizeButtons[i]->raise();
        }
    }

    // double selectedPrice = p_page_idle->selectedProduct->getPrice();
    // double discount = p_page_idle->selectedProduct->getDiscount();
    // double selectedPriceCorrected = p_page_idle->selectedProduct->getPriceCorrected();

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

bool pageProduct::stopSelectTimers()
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

void pageProduct::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{

    // ui->pushButton_to_idle->setEnabled(false);
    // ui->pushButton_previous_page->setEnabled(false);
    selectIdleTimer->stop();
    this->stopSelectTimers();
    p_page_idle->pageTransition(this, pageToShow);
}

void pageProduct::on_pushButton_to_idle_clicked()
{
    hideCurrentPageAndShowProvided(p_page_help);
}

void pageProduct::on_pushButton_order_custom_clicked()
{
    qDebug() << "button custom clicked ";
    this->loadProductBySize(SIZE_CUSTOM_INDEX);
    on_pushButton_continue_clicked();
}

void pageProduct::on_pushButton_order_medium_clicked()
{
    qDebug() << "button medium";
    this->loadProductBySize(SIZE_MEDIUM_INDEX);
    on_pushButton_continue_clicked();
}

// on_Small_Order button listener
void pageProduct::on_pushButton_order_small_clicked()
{
    qDebug() << "button small";
    this->loadProductBySize(SIZE_SMALL_INDEX);
    on_pushButton_continue_clicked();
}

// on_Large_Order button listener
void pageProduct::on_pushButton_order_big_clicked()
{
    qDebug() << "button big";
    this->loadProductBySize(SIZE_LARGE_INDEX);
    on_pushButton_continue_clicked();
}

size_t WriteCallback_coupon(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void pageProduct::on_pushButton_previous_page_clicked()
{
    hideCurrentPageAndShowProvided(p_page_select_product);
}

void pageProduct::on_pushButton_continue_clicked()
{
    hideCurrentPageAndShowProvided(p_page_overview);
}

void pageProduct::on_pushButton_back_clicked()
{
    hideCurrentPageAndShowProvided(p_page_select_product);
}
