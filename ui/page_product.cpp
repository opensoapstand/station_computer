
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

#include "page_payment.h"
#include "page_select_product.h"
#include "page_productOverview.h"

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
    {605, 1110}, // S vol
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
    {605, 1150}, // S price
    {1, 1},      // M price
    {1, 1},      // L price
    {1, 1}       // custom price
};

// CTOR
pageProduct::pageProduct(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::pageProduct)
{
    ui->setupUi(this);

    orderSizeButtons[0] = ui->orderSmall_Button;
    orderSizeButtons[1] = ui->orderMedium_Button;
    orderSizeButtons[2] = ui->orderBig_Button;
    orderSizeButtons[3] = ui->orderCustom_Button;

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

    QString css_title = "QLabel{"
                        "position: absolute;"
                        "width: 877px;"
                        "height: 104px;"
                        "left: 94px;"
                        "top: 336px;"
                        "font-family: 'Montserrat';"
                        "font-style: normal;"
                        "font-weight: 600;"
                        "font-size: 48px;"
                        "line-height: 52px;"
                        "text-transform: capitalize;"
                        "color: #5E8580;"
                        "}";
    ui->label_product_title->setStyleSheet(css_title);

    QFont font;
    font.setFamily(QStringLiteral("Brevia"));
    font.setPointSize(20);
    font.setBold(true);
    font.setWeight(200);

    ui->back_Button->setStyleSheet(
        "QPushButton {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 32px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "text-transform: lowercase;"
        "color: #003840;"
        "text-align: center;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");
    ui->back_Button->setText("<- Products");

    QString css_description = "QLabel{"
                              "position: absolute;"
                              //   "width: 894px;"
                              //   "height: 252px;"
                              //   "left: 95px;"
                              //   "top: 474px;"
                              "font-family: 'Montserrat';"
                              "font-style: normal;"
                              "font-weight: 400;"
                              "font-size: 24px;"
                              "line-height: 36px;"
                              "color: #58595B;"
                              "}";
    ui->label_product_description->setStyleSheet(css_description);

    ui->label_select_quantity->setStyleSheet(
        "QLabel {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 200;"
        "font-size: 36px;"
        "line-height: auto;"
        "letter-spacing: 0px;"
        "color: #58595B;"
        "border: none;"
        "}");
    ui->label_select_quantity->setText("Select Quantity");

    ui->label_product_description->setWordWrap(true);

    QString css_ingredients = "QLabel{"
                              "position: absolute;"
                              "width: 425px;"
                              "height: 319px;"
                              "left: 564px;"
                              "top: 813px;"
                              "font-family: 'Montserrat';"
                              "font-style: normal;"
                              "font-weight: 400;"
                              "font-size: 18px;"
                              "line-height: 28px;"
                              "color: #58595B;"
                              "}";
    ui->label_product_ingredients->setStyleSheet(css_ingredients);
    ui->label_product_ingredients->setWordWrap(true);

    QString css_ingredients_title = "QLabel{"
                                    "font-weight: bold;"
                                    "position: absolute;"
                                    "width: 425px;"
                                    "height: 319px;"
                                    "left: 564px;"
                                    "top: 763px;"
                                    "font-family: 'Montserrat';"
                                    "font-style: normal;"
                                    "font-size: 18px;"
                                    "line-height: 28px;"
                                    "color: #58595B;"
                                    "}";
    ui->label_product_ingredients_title->setStyleSheet(css_ingredients_title);
    ui->label_notify_us->setStyleSheet(
        "QLabel {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 20px;"
        "line-height: 99px;"
        "letter-spacing: px;"
        "color: #FFFFFF;"
        "text-align: center;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");
    ui->label_notify_us->setText("Notify Us");

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
void pageProduct::setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_payment *page_payment, page_help *pageHelp, pageProductOverview *page_Overview)
{
    this->p_page_select_product = pageSelect;
    this->paymentPage = page_payment;
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->p_page_help = pageHelp;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_overview = page_Overview;

    selectedProductOrder = p_page_idle->currentProductOrder;
    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_PRODUCT_BACKGROUND_PATH);
}

// DTOR
pageProduct::~pageProduct()
{
    delete ui;
}

void pageProduct::setDefaultSize()
{

    bool default_size_set = false;

    for (uint8_t i = 0; i < 4; i++)
    {
        int size = product_sizes[i];
        if (selectedProductOrder->getLoadedProductSizeEnabled(size))
        {
            selectedProductOrder->setSelectedSize(product_sizes[i]);
        }
    }
}

void pageProduct::cancelTimers()
{
    selectIdleTimer->stop();
}

/* GUI */

void pageProduct::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Product >>>>>>>>>";
    QWidget::showEvent(event);

    selectedProductOrder->loadSelectedProductProperties();
    setDefaultSize();
    loadProdSpecs();
    reset_and_show_page_elements();
}

// void pageProduct::paintEvent(QPaintEvent *event){
//     qDebug() << "Page_product: PaintEvent";
//     QWidget::paintEvent(event);
// }

void pageProduct::resizeEvent(QResizeEvent *event)
{
    qDebug() << "\n---Page_product: resizeEvent";
    // QWidget::resizeEvent(event);
    // selectedProductOrder->loadSelectedProductProperties();
    // reset_and_show_page_elements();
}

void pageProduct::onSelectTimeoutTick()
{
    if (--_selectIdleTimeoutSec >= 0)
    {
        //        qDebug() << "pageProduct: Tick Down - " << _selectIdleTimeoutSec << endl;
    }
    else
    {
        // qDebug() << "Timer Done!" << _selectIdleTimeoutSec << endl;
        selectIdleTimer->stop();

        mainPage();
    }
}

void pageProduct::reset_and_show_page_elements()
{

#ifdef ENABLE_DYNAMIC_UI

    ui->label_product_photo->setStyleSheet("QLabel{border: 1px solid #5E8680;}");
    p_page_idle->addPictureToLabel(ui->label_product_photo, p_page_idle->currentProductOrder->getSelectedProductPicturePath());

    ui->label_product_title->setText(selectedProductOrder->getLoadedProductName());
    ui->label_product_ingredients->setText(selectedProductOrder->getLoadedProductIngredients());
    ui->label_product_description->setText(selectedProductOrder->getLoadedProductDescription());

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

    // create signature by sizes availability
    for (uint8_t i = 0; i < 4; i++)
    {
        available_sizes_signature |= selectedProductOrder->getLoadedProductSizeEnabled(product_sizes[i]) << i;
    }

    if (available_sizes_signature == 1)
    {
        // only small available
        xywh_size_buttons = orderSizeButtons_xywh_dynamic_ui_small_available;
        xy_size_labels_volume = orderSizeVolumeLabels_xy_dynamic_ui_small_available;
        xy_size_labels_price = orderSizePriceLabels_xy_dynamic_ui_small_and_large_available;
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
        if (selectedProductOrder->getLoadedProductSizeEnabled(product_sizes[i]))
        {
            orderSizeButtons[i]->show();

            orderSizeButtons[i]->setFixedSize(QSize(xywh_size_buttons[i][2], xywh_size_buttons[i][3]));
            orderSizeButtons[i]->move(xywh_size_buttons[i][0], xywh_size_buttons[i][1]);

            orderSizeBackgroundLabels[i]->setFixedSize(QSize(xywh_size_buttons[i][2], xywh_size_buttons[i][3]));
            orderSizeBackgroundLabels[i]->move(xywh_size_buttons[i][0], xywh_size_buttons[i][1]);
            orderSizeBackgroundLabels[i]->lower();
            orderSizeBackgroundLabels[i]->setStyleSheet("QLabel { background-color: transparent; border: 0px }");

            orderSizeLabelsPrice[i]->move(xy_size_labels_price[i][0], xy_size_labels_price[i][1]);
            orderSizeLabelsVolume[i]->move(xy_size_labels_volume[i][0], xy_size_labels_volume[i][1]);

            qDebug() << "Product size index enabled: " << i;
        }
        else
        {
            qDebug() << "Product size index NOT enabled: " << i;
            orderSizeButtons[i]->hide();
        }
    }

#else
    QString bitmap_location;
    ui->label_product_ingredients->hide();
    ui->label_product_ingredients_title->hide();
    ui->label_product_title->hide();
    ui->label_product_description->hide();
    ui->label_product_photo->hide();
    ui->label_product_photo->setStyleSheet("QLabel{border: 0px solid black;}");
    int product_slot___ = selectedProductOrder->getSelectedSlot();
    // uint16_t orderSizeButtons_xywh[4][4] = {{564, 1088, 209, 126}, {1, 1, 1, 1}, {790, 1087, 198, 126}, {1, 1, 1, 1}};
    if (product_slot___ > 0 && product_slot___ <= SLOT_COUNT)
    {
        bitmap_location.append("/home/df-admin/production/references/4_pay_select_page_l_");
        bitmap_location.append(QString::number(selectedProductOrder->getSelectedSlot()));
        bitmap_location.append(".png");
    }
    else
    {
        bitmap_location = "/home/df-admin/production/references/4_pay_select_page_l_1.png";
    }
    for (uint8_t i = 0; i < SLOT_COUNT; i++)
    {
        orderSizeButtons[i]->setFixedSize(QSize(orderSizeButtons_xywh_static_product_page[i][2], orderSizeButtons_xywh_static_product_page[i][3]));
        orderSizeButtons[i]->move(orderSizeButtons_xywh_static_product_page[i][0], orderSizeButtons_xywh_static_product_page[i][1]);
    }
    qDebug() << bitmap_location << endl;
    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);
#endif

    // ordersize buttons
    /* Hacky transparent button */
    ui->previousPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->page_payment_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    // ui->page_payment_Button->setStyleSheet("QPushButton { background-color: red; border: 0px };QPushButton:pressed { background-color: green; border: 10px }");

    ui->mainPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    loadProdSpecs();
    selectIdleTimer->start(1000);
    _selectIdleTimeoutSec = 400;
}

void pageProduct::loadProductBySize(int sizeIndex)
{
    // DF_QT_SIZES tmp [6] = {INVALID_DRINK, SIZE_SMALL_INDEX, MEDIUM_DRINK, SIZE_LARGE_INDEX, CUSTOM_DRINK, TEST_DRINK};

    selectedProductOrder->setSelectedSize(sizeIndex);
    loadProdSpecs();
}

#ifdef ENABLE_DYNAMIC_UI
void pageProduct::loadProdSpecs()
{

    qDebug() << "-------------------------- LOAD PRODUCT ----------------";
    _selectIdleTimeoutSec = 140;

    ui->mainPage_Button->setEnabled(true);
    ui->previousPage_Button->setEnabled(true);

    for (uint8_t i = 0; i < 4; i++)
    {
        orderSizeLabelsPrice[i]->hide();
        orderSizeLabelsVolume[i]->hide();
        orderSizeBackgroundLabels[i]->hide();
        orderSizeButtons[i]->hide();

        if (selectedProductOrder->getLoadedProductSizeEnabled(product_sizes[i]))
        {

            orderSizeLabelsPrice[i]->show();
            orderSizeLabelsVolume[i]->show();
            orderSizeBackgroundLabels[i]->show();
            orderSizeButtons[i]->show();
            orderSizeLabelsVolume[i]->setStyleSheet("QLabel {font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: normal; font-size: 24px; line-height: 44px; color: #5E8580;}");
            orderSizeLabelsPrice[i]->setStyleSheet("QLabel {font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;}");
            orderSizeBackgroundLabels[i]->setStyleSheet("QLabel { background-color: #FFFFFF; border: 0px }");
            orderSizeButtons[i]->setStyleSheet("QPushButton { background-color: transparent; border: 1px  solid #3D6675; }");

            double price = selectedProductOrder->getPrice(product_sizes[i]);
            QString transparent_path = FULL_TRANSPARENT_IMAGE_PATH;

            if (product_sizes[i] == SIZE_CUSTOM_INDEX)
            {
                orderSizeLabelsVolume[i]->setText("Custom Volume");
                QString units = selectedProductOrder->getUnitsForSelectedSlot();
                if (units == "ml")
                {
                    units = "L";
                    price = price * 1000;
                }

                else if (units == "g")
                {
                    if (selectedProductOrder->getVolume(SIZE_CUSTOM_INDEX) == VOLUME_TO_TREAT_CUSTOM_DISPENSE_AS_PER_100G)
                    {
                        units = "100g";
                        price = price * 100;
                    }
                    else
                    {
                        units = "kg";
                        price = price * 1000;
                    }
                }
                else if (units == "oz")
                {
                    units = "oz";
                    price = price * OZ_TO_ML;
                }
                orderSizeLabelsPrice[i]->setText("$" + QString::number(price, 'f', 2) + "/" + units);
            }
            else
            {
                orderSizeLabelsPrice[i]->setText("$" + QString::number(price, 'f', 2));
                orderSizeLabelsVolume[i]->setText(selectedProductOrder->getSizeToVolumeWithCorrectUnitsForSelectedSlot(product_sizes[i], true, true));
            }

            // selected size styling
            if (selectedProductOrder->getSelectedSize() == product_sizes[i])
            {

                orderSizeButtons[i]->setStyleSheet("QPushButton { background-color: transparent; border: 1px  solid #3D6675; }");
                orderSizeLabelsVolume[i]->setStyleSheet("QLabel { font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: normal; font-size: 24px; line-height: 44px; color: #5E8580; }");
                orderSizeLabelsPrice[i]->setStyleSheet("QLabel {font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;}");
                orderSizeBackgroundLabels[i]->setStyleSheet("QLabel { background-color: transparent; border: 0px }");
            }
            orderSizeButtons[i]->raise();
        }
    }

    double selectedPrice = selectedProductOrder->getSelectedPrice();
    double discount = selectedProductOrder->getDiscount();
    double selectedPriceCorrected = selectedProductOrder->getSelectedPriceCorrected();
    //double selectedPrice;
    // double discount;
    // double selectedPriceCorrected;

    if (selectedProductOrder->getSelectedSize() == SIZE_CUSTOM_INDEX)
    {
        // QString unitsInvoice;
        // selectedProductOrder->getCustomPriceDetails(&unitsInvoice, &selectedPrice, &discount, &selectedPriceCorrected);
    //     QString unitsInvoice = selectedProductOrder->getUnitsForSelectedSlot();

    //     if (unitsInvoice == "ml")
    //     {
    //         unitsInvoice = "L";
    //         selectedPrice = selectedPrice * 1000;
    //         selectedPriceCorrected = selectedPriceCorrected * 1000;
    //         discount = discount * 1000;
    //     }
    //     else if (unitsInvoice == "g")
    //     {
    //         if (selectedProductOrder->getVolume(SIZE_CUSTOM_INDEX) == VOLUME_TO_TREAT_CUSTOM_DISPENSE_AS_PER_100G)
    //         {
    //             unitsInvoice = "100g";
    //             selectedPrice = selectedPrice * 100;
    //             selectedPriceCorrected = selectedPriceCorrected * 100;
    //             discount = discount * 100;
    //         }
    //         else
    //         {
    //             unitsInvoice = "kg";
    //             selectedPrice = selectedPrice * 1000;
    //             selectedPriceCorrected = selectedPriceCorrected * 1000;
    //             discount = discount * 1000;
    //         }
    //     }
    //     else if (unitsInvoice == "oz")
    //     {
    //         unitsInvoice = "oz";
    //         selectedPrice = selectedPrice * OZ_TO_ML;
    //         selectedPriceCorrected = selectedPriceCorrected * OZ_TO_ML;
    //         discount = discount * OZ_TO_ML;
    //     }

    // ui->label_invoice_name->setText(selectedProductOrder->getSelectedProductName());
    // ui->label_invoice_price->setText("$" + QString::number(selectedPrice, 'f', 2) + "/" + unitsInvoice);

    // ui->label_invoice_price_total->setText("$" + QString::number(selectedPriceCorrected, 'f', 2) + "/" + unitsInvoice); // how to handle promo ?! todo!
}
else
{

    // ui->label_invoice_name->setText(selectedProductOrder->getSelectedProductName() + " " + selectedProductOrder->getSelectedSizeToVolumeWithCorrectUnits(true, true));
    // ui->label_invoice_price->setText("$" + QString::number(selectedPrice, 'f', 2));

    // ui->label_invoice_price_total->setText("$" + QString::number(selectedPriceCorrected, 'f', 2)); // how to handle promo ?! todo!
}

qDebug() << "-------------------------- END LOAD PRODUCT ----------------";
}
#else
void pageProduct::loadProdSpecs()
{

    qDebug() << "-------------------------- LOAD ORDER ----------------";

    _selectIdleTimeoutSec = 140;

    ui->mainPage_Button->setEnabled(true);
    ui->previousPage_Button->setEnabled(true);

    int product_sizes[4] = {SIZE_SMALL_INDEX, SIZE_MEDIUM_INDEX, SIZE_LARGE_INDEX, SIZE_CUSTOM_INDEX};

    QString bitmap_location;
    if (selectedProductOrder->getSelectedSize() == SIZE_SMALL_INDEX)
    {
        bitmap_location.append("/home/df-admin/production/references/4_pay_select_page_s_");
    }
    else
    {
        bitmap_location.append("/home/df-admin/production/references/4_pay_select_page_l_");
    }

    bitmap_location.append(QString::number(selectedProductOrder->getSelectedSlot()));
    bitmap_location.append(".png");

    qDebug() << bitmap_location << endl;
    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    QString transparent_path = FULL_TRANSPARENT_IMAGE_PATH;
    for (uint8_t i = 0; i < SLOT_COUNT; i++)
    {
        qDebug() << "*****load size: " << i;
        QString price = QString::number(selectedProductOrder->getPrice(product_sizes[i]), 'f', 2);
        orderSizeLabelsPrice[i]->setText("$" + price);
        orderSizeLabelsPrice[i]->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
        orderSizeLabelsVolume[i]->setText(selectedProductOrder->getSizeToVolumeWithCorrectUnitsForSelectedSlot(product_sizes[i], true, true));

        if (selectedProductOrder->getSelectedSize() == product_sizes[i])
        {
            orderSizeButtons[i]->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
        }
        else
        {
            orderSizeButtons[i]->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
        }
        orderSizeButtons[i]->raise();
    }

    if (selectedProductOrder->getSelectedSize() == SIZE_SMALL_INDEX)
    {
        ui->label_price_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #FFFFFF;");
        ui->label_price_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
        ui->label_size_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #5E8500;");
        ui->label_size_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #D2E4CD;");
    }
    else
    {

        ui->label_price_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
        ui->label_price_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #FFFFFF;");
        ui->label_size_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #D2E4CD;");
        ui->label_size_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #5E8500;");
    }

    ui->label_price_small->move(600, 1155);
    ui->label_price_large->move(830, 1155);

    ui->label_size_small->move(600, 1106);
    ui->label_size_large->move(830, 1106);
    // ui->label_price_small->move(560, 1155);
    // ui->label_price_large->move(790, 1155);
    // ui->label_size_small->move(570, 1106);
    // ui->label_size_large->move(790, 1106);
    ui->label_size_medium->hide();
    ui->label_price_medium->hide();
    ui->label_price_custom->hide();
    ui->label_size_custom->hide();

    // ui->label_price_large->setStyleSheet("color: #FFFFFF;");
    // ui->label_price_small->setStyleSheet("color: #FFFFFF;");
    // ui->label_price_large->setStyleSheet(QLatin1String("color: #FFFFFF;\n"
    //     "font-family: Montserrat;\n"
    //     "font-style: light;\n"
    //     "font-weight: normal;\n"
    //     "font-size: 20px;\n"
    //     "line-height: 24px;\n"
    //     "text-align: center;\n"
    //     ""));
    // ui->label_price_small->setStyleSheet(QLatin1String("color: #FFFFFF;\n"
    //     "font-family: Montserrat;\n"
    //     "font-style: light;\n"
    //     "font-weight: normal;\n"
    //     "font-size: 20px;\n"
    //     "line-height: 24px;\n"
    //     "text-align: center;\n"
    //     ""));

    ui->label_invoice_name->setText(selectedProductOrder->getSelectedProductName() + " " + selectedProductOrder->getSelectedSizeToVolumeWithCorrectUnits(true, true));
    double selectedPrice = selectedProductOrder->getSelectedPriceCorrected();
    ui->label_invoice_price->setText("$" + QString::number(selectedPrice, 'f', 2));
    ui->label_invoice_price_total->setText("$" + QString::number(selectedPrice)); // how to handle promo ?! todo!

    qDebug() << "-------------------------- END LOAD ORDER ----------------";
}

#endif

bool pageProduct::stopSelectTimers()
{
    //    qDebug() << "Stop pageProduct Timers" << endl;
    if (selectIdleTimer != nullptr)
    {
        // qDebug() << "Enter Stop" << endl;
        selectIdleTimer->stop();
        return true;
    }
    else
    {
        return false;
    }
}

void pageProduct::mainPage()
{
    //    qDebug() << "pageProduct: mainPage button" << endl;
    this->stopSelectTimers();
    selectIdleTimer->stop();
    // qDebug() << "product to idle";
    // p_page_idle->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_idle);
}

void pageProduct::on_mainPage_Button_clicked()
{
    //    qDebug() << "pageProduct: helpPage button" << endl;
    this->stopSelectTimers();
    selectIdleTimer->stop();
    // p_page_help->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_help);
}

void pageProduct::on_orderCustom_Button_clicked()
{
    qDebug() << "button custom";
    this->loadProductBySize(SIZE_CUSTOM_INDEX);
    on_page_payment_Button_clicked();
}

void pageProduct::on_orderMedium_Button_clicked()
{
    qDebug() << "button medium";
    this->loadProductBySize(SIZE_MEDIUM_INDEX);
    on_page_payment_Button_clicked();
}

// on_Small_Order button listener
void pageProduct::on_orderSmall_Button_clicked()
{
    qDebug() << "button small";
    this->loadProductBySize(SIZE_SMALL_INDEX);
    on_page_payment_Button_clicked();
}

// on_Large_Order button listener
void pageProduct::on_orderBig_Button_clicked()
{
    qDebug() << "button big";
    this->loadProductBySize(SIZE_LARGE_INDEX);
    on_page_payment_Button_clicked();
}

size_t WriteCallback_coupon(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void pageProduct::on_previousPage_Button_clicked()
{

    //    qDebug() << "pageProduct: Previous button" << endl;
    while (!stopSelectTimers())
    {
    };
    selectIdleTimer->stop();
    p_page_idle->pageTransition(this, p_page_select_product);
}

void pageProduct::on_page_payment_Button_clicked()
{
    qDebug() << "pageProduct: Pay button";

    ui->mainPage_Button->setEnabled(false);
    ui->previousPage_Button->setEnabled(false);

    this->stopSelectTimers();
    p_page_idle->pageTransition(this, p_page_overview);
}

void pageProduct::on_back_Button_clicked()
{
    //    qDebug() << "pageProduct: Previous button" << endl;
    while (!stopSelectTimers())
    {
    };
    selectIdleTimer->stop();
    p_page_idle->pageTransition(this, p_page_select_product);
}
