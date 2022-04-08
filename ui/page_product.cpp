
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

#include "pagepayment.h"
#include "page_select_product.h"
#include "page_idle.h"
#include <curl/curl.h>
#include <json.hpp>

using json = nlohmann::json;
double promoPercent = 0.0;

uint16_t orderSizeButtons_xywh_generic_product_page[4][4] = {
    {560, 990, 135, 100}, // S
    {706, 990, 135, 100}, // M
    {852, 990, 135, 100}, // L
    {560, 1100, 430, 115} // custom
};

uint16_t orderSizeButtons_xywh_static_product_page[4][4] = {{564, 1088, 209, 126}, {1, 1, 1, 1}, {790, 1087, 198, 126}, {1, 1, 1, 1}};
// uint16_t orderSizeButtons_xywh_static_product_page[4][4] = {{560, 1155, 211, 41}, {1, 1, 1, 1}, {790, 1155, 201, 41}, {1, 1, 1, 1}};

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

    ui->promoInputButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->promoCode->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->promoButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

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

    QString css_description = "QLabel{"
                              "position: absolute;"
                              "width: 894px;"
                              "height: 252px;"
                              "left: 95px;"
                              "top: 474px;"
                              "font-family: 'Montserrat';"
                              "font-style: normal;"
                              "font-weight: 400;"
                              "font-size: 28px;"
                              "line-height: 36px;"
                              "color: #58595B;"
                              "}";
    ui->label_product_description->setStyleSheet(css_description);
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

    QString css_discount_name = "QLabel{"
                                "font-family: 'Montserrat';"
                                "font-style: normal;"
                                "font-weight: 400;"
                                "font-size: 24px;"
                                "line-height: 29px;"
                                "color: #58595B;"
                                "}";
    ui->label_invoice_discount_name->setStyleSheet(css_discount_name);

    ui->label_invoice_discount_amount->hide();
    ui->label_invoice_discount_name->hide();

    promoPercent = 0.0;
    ui->promoCode->clear();
    ui->promoCode->hide();
    ui->promoKeyboard->hide();
    ui->promoInputButton->show();

    couponHandler();

    {
        selectIdleTimer = new QTimer(this);
        selectIdleTimer->setInterval(40);
        connect(ui->promoButton, SIGNAL(clicked()), this, SLOT(on_applyPromo_Button_clicked()));
        connect(ui->promoInputButton, SIGNAL(clicked()), this, SLOT(on_promoCodeInput_clicked()));
        connect(ui->buttonGroup, SIGNAL(buttonPressed(int)), this, SLOT(keyboardButtonPressed(int)));
        connect(selectIdleTimer, SIGNAL(timeout()), this, SLOT(onSelectTimeoutTick()));
    }
}

/*
 * Page Tracking reference to Select Drink, Payment Page and Idle page
 */
void pageProduct::setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, pagePayment *pagePayment, page_help *pageHelp)
{
    this->p_page_select_product = pageSelect;
    this->paymentPage = pagePayment;
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->helpPage = pageHelp;
    this->wifiError = pageWifiError;

    ui->promoCode->clear();
    ui->promoCode->hide();
    ui->label_invoice_discount_amount->hide();
    ui->label_invoice_discount_name->hide();

    selectedProductOrder = p_page_idle->currentProductOrder;
    selectedProductOrder->setSelectedSize(SIZE_LARGE_INDEX); // default size

    couponHandler();
}

// DTOR
pageProduct::~pageProduct()
{
    delete ui;
}

void pageProduct::cancelTimers()
{
    selectIdleTimer->stop();
}

/* GUI */

void pageProduct::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Product >>>>>>>>>";
    selectedProductOrder->setSelectedSize(SIZE_LARGE_INDEX);
    QWidget::showEvent(event);

    loadOrderSize(SIZE_LARGE_INDEX);

    reset_and_show_page_elements();
}

// void pageProduct::paintEvent(QPaintEvent *event){
//     qDebug() << "Page_product: PaintEvent";
//     QWidget::paintEvent(event);
// }

void pageProduct::resizeEvent(QResizeEvent *event)
{
    qDebug() << "\n---Page_product: resizeEvent";
    QWidget::resizeEvent(event);
    reset_and_show_page_elements();
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

    selectedProductOrder->loadSelectedProductProperties();

    QString bitmap_location;

#ifdef ENABLE_DYNAMIC_UI

    ui->label_product_photo->setStyleSheet("QLabel{border: 1px solid black;}");
    ui->label_product_title->setText(selectedProductOrder->getLoadedProductName());
    ui->label_product_ingredients->setText(selectedProductOrder->getLoadedProductIngredients());
    ui->label_product_description->setText(selectedProductOrder->getLoadedProductDescription());

    p_page_idle->addPictureToLabel(ui->label_product_photo, p_page_idle->currentProductOrder->getSelectedProductPicturePath());

    bitmap_location = PAGE_PRODUCT_BACKGROUND_PATH;
    // uint16_t orderSizeButtons_xywh[4][4] = {
    //     {560, 990, 135, 110},  // S
    //     {706, 990, 135, 105},  // M
    //     {852, 990, 135, 100},  // L
    //     {560, 1100, 430, 115}   // custom
    //     };
    for (uint8_t i = 0; i < SLOT_COUNT; i++)
    {
        if (selectedProductOrder->getLoadedProductSizeEnabled(i))
        {
            orderSizeButtons[i]->show();

            orderSizeButtons[i]->setFixedSize(QSize(orderSizeButtons_xywh_generic_product_page[i][2], orderSizeButtons_xywh_generic_product_page[i][3]));
            orderSizeButtons[i]->move(orderSizeButtons_xywh_generic_product_page[i][0], orderSizeButtons_xywh_generic_product_page[i][1]);

            orderSizeBackgroundLabels[i]->setFixedSize(QSize(orderSizeButtons_xywh_generic_product_page[i][2], orderSizeButtons_xywh_generic_product_page[i][3]));
            orderSizeBackgroundLabels[i]->move(orderSizeButtons_xywh_generic_product_page[i][0], orderSizeButtons_xywh_generic_product_page[i][1]);
            orderSizeBackgroundLabels[i]->lower();
            orderSizeBackgroundLabels[i]->setStyleSheet("QLabel { background-color: red; border: 0px }");
            qDebug() << "Product size index enabled: " << i;
        }
        else
        {
            qDebug() << "Product size index NOT enabled: " << i;
            orderSizeButtons[i]->hide();
        }
    }

#else
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

#endif

    qDebug() << bitmap_location << endl;
    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    // ordersize buttons
    /* Hacky transparent button */
    ui->previousPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->pagePayment_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->mainPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    QString keyboard = KEYBOARD_IMAGE_PATH;
    QString keyboard_style_sheet = " background-image: url(" + keyboard + "); }";
    ui->promoKeyboard->setStyleSheet(keyboard_style_sheet);

    loadOrderSelectedSize();

    selectIdleTimer->start(1000);
    _selectIdleTimeoutSec = 400;
}

void pageProduct::loadOrderSize(int sizeIndex)
{
    // DF_QT_SIZES tmp [6] = {INVALID_DRINK, SIZE_SMALL_INDEX, MEDIUM_DRINK, SIZE_LARGE_INDEX, CUSTOM_DRINK, TEST_DRINK};

    selectedProductOrder->setSelectedSize(sizeIndex);
    loadOrderSelectedSize();
}

#ifdef ENABLE_DYNAMIC_UI
void pageProduct::loadOrderSelectedSize()
{

    qDebug() << "-------------------------- LOAD ORDER ----------------";
    _selectIdleTimeoutSec = 140;

    ui->mainPage_Button->setEnabled(true);
    ui->previousPage_Button->setEnabled(true);

    int product_sizes[4] = {SIZE_SMALL_INDEX, SIZE_MEDIUM_INDEX, SIZE_LARGE_INDEX, SIZE_CUSTOM_INDEX};

    for (uint8_t i = 0; i < SLOT_COUNT; i++)
    {

        if (selectedProductOrder->getLoadedProductSizeEnabled(i))
        {
            qDebug() << "*****load size: " << i;

            orderSizeLabelsPrice[i]->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
            orderSizeBackgroundLabels[i]->setStyleSheet("QLabel { background-color: #FFFFFF; border: 0px }");
            orderSizeButtons[i]->setStyleSheet("QPushButton { background-color: transparent; border: 1px  solid #3D6675; }");
            // orderSizeButtons[i]->setStyleSheet("QPushButton { background-color: transparent; border: 1px  solid #3D6675; }");
            // orderSizeButtons[i]->setStyleSheet("QPushButton { background-color: rgba(0.25, 0.3, 0.5 , 0.2); border: 1px  solid #3D6675; }");

            QString price = QString::number(selectedProductOrder->getPrice(product_sizes[i]), 'f', 2);
            QString transparent_path = FULL_TRANSPARENT_IMAGE_PATH;

            if (product_sizes[i] == SIZE_CUSTOM_INDEX)
            {
                orderSizeLabelsVolume[i]->setText("Custom Volume.");
                QString units = selectedProductOrder->getUnitsForSelectedSlot();
                if (units == "ml")
                {
                    units = "L";
                }
                orderSizeLabelsPrice[i]->setText("$" + price + "/" + units);
            }
            else
            {
                orderSizeLabelsPrice[i]->setText("$" + price);
                orderSizeLabelsVolume[i]->setText(selectedProductOrder->getSizeToVolumeWithCorrectUnitsForSelectedSlot(product_sizes[i], true, true));
            }

            if (selectedProductOrder->getSelectedSize() == product_sizes[i])
            {
                orderSizeButtons[i]->setStyleSheet("QPushButton { background-color: transparent; border: 1px  solid #3D6675; }");
                // orderSizeButtons[i]->setStyleSheet("QPushButton { background-color: transparent; border: 2px  solid #3D6675; }");
                //  orderSizeButtons[i]->setStyleSheet("QPushButton { background-color: rgba(0.25, 0.3, 0.5 , 0.2); border: 8px  solid #3D6675; }");
                orderSizeBackgroundLabels[i]->setStyleSheet("QLabel { background-color: #5E8580; border: 0px }");
                orderSizeLabelsPrice[i]->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #FFFFFF;");
            }
            orderSizeButtons[i]->raise();
        }
    }

    ui->promoCode->clear();
    ui->promoCode->hide();
    promoPercent = 0.0;
    
    if (selectedProductOrder->getDiscountPercentageFraction() > 0.0){
        ui->label_invoice_discount_amount->show();
        ui->label_invoice_discount_name->show();

    }else{
        ui->label_invoice_discount_amount->hide();
        ui->label_invoice_discount_name->hide();
    }

    double selectedPrice = selectedProductOrder->getSelectedPrice();
    double discount = selectedProductOrder->getDiscount();
    double selectedPriceCorrected = selectedProductOrder->getSelectedPriceCorrected();

    if (selectedProductOrder->getSelectedSize() == SIZE_CUSTOM_INDEX)
    {
        QString unitsInvoice = selectedProductOrder->getUnitsForSelectedSlot();
        if (unitsInvoice == "ml")
        {
            unitsInvoice = "L";
        }
        ui->label_invoice_name->setText(selectedProductOrder->getSelectedProductName());
        ui->label_invoice_price->setText("$" + QString::number(selectedPrice, 'f', 2) + "/" + unitsInvoice);

        ui->label_invoice_discount_amount->setText("-$" + QString::number(discount, 'f', 2));

        ui->label_invoice_price_total->setText("$" + QString::number(selectedPriceCorrected, 'f', 2) + "/" + unitsInvoice); // how to handle promo ?! todo!
    }
    else
    {

        ui->label_invoice_name->setText(selectedProductOrder->getSelectedProductName() + " " + selectedProductOrder->getSelectedSizeToVolumeWithCorrectUnits(true, true));
        ui->label_invoice_price->setText("$" + QString::number(selectedPrice, 'f', 2));
        ui->label_invoice_discount_amount->setText("-$" + QString::number(discount, 'f', 2));

        ui->label_invoice_price_total->setText("$" + QString::number(selectedPriceCorrected, 'f', 2)); // how to handle promo ?! todo!
    }

    qDebug() << "-------------------------- END LOAD ORDER ----------------";
}
#else
void pageProduct::loadOrderSelectedSize()
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

    ui->promoCode->clear();
    ui->promoCode->hide();
    promoPercent = 0.0;

    ui->label_invoice_name->setText(selectedProductOrder->getSelectedProductName() + " " + selectedProductOrder->getSelectedSizeToVolumeWithCorrectUnits(true, true));
    double selectedPrice = selectedProductOrder->getSelectedPriceCorrected();
    ui->label_invoice_price->setText("$" + QString::number(selectedPrice, 'f', 2));
     ui->label_invoice_discount_amount->setText("-$" + QString::number(selectedProductOrder->getDiscount(), 'f', 2));
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
    p_page_idle->showFullScreen();
    //    usleep(100);
    this->hide();
}

void pageProduct::on_mainPage_Button_clicked()
{
    //    qDebug() << "pageProduct: helpPage button" << endl;
    this->stopSelectTimers();
    selectIdleTimer->stop();
    helpPage->showFullScreen();
    this->hide();
}

void pageProduct::on_orderCustom_Button_clicked()
{
    qDebug() << "button custom";
    this->loadOrderSize(SIZE_CUSTOM_INDEX);
}

void pageProduct::on_orderMedium_Button_clicked()
{
    qDebug() << "button medium";
    this->loadOrderSize(SIZE_MEDIUM_INDEX);
}

// on_Small_Order button listener
void pageProduct::on_orderSmall_Button_clicked()
{
    qDebug() << "button small";
    this->loadOrderSize(SIZE_SMALL_INDEX);
}

// on_Large_Order button listener
void pageProduct::on_orderBig_Button_clicked()
{
    qDebug() << "button big";
    this->loadOrderSize(SIZE_LARGE_INDEX);
}

size_t WriteCallback_coupon(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void pageProduct::updatePriceAfterPromo(double discountPercent)
{
    // QString normal_price = (ui->label_invoice_price->text()).split("$")[1];
    // double price = normal_price.toDouble();
    selectedProductOrder->setDiscountPercentageFraction(discountPercent / 100);

    // ui->label_invoice_discount_amount->show();
    // ui->label_invoice_discount_name->show();

    // double discount;
    // discount = selectedProductOrder->getDiscountPercentageFraction() * selectedProductOrder->getPrice(selectedProductOrder->getSelectedSize());
    // ui->label_invoice_discount_amount->setText("-$" + QString::number(discount, 'f', 2));
    
    // ui->label_invoice_discount_amount->setText("-$" + QString::number(selectedProductOrder->getDiscount(), 'f', 2));
    // ui->label_invoice_price_total->setText("$" + QString::number(selectedProductOrder->getSelectedPriceCorrected(), 'f', 2));
}

void pageProduct::on_applyPromo_Button_clicked()
{

    QString promocode = ui->promoCode->text();
    ui->promoKeyboard->hide();

    CURL *curl;
    CURLcode res;
    long http_code = 0;
    if (promocode != "")
    {
        readBuffer.clear();
        curl = curl_easy_init();
        if (!curl)
        {
            qDebug() << "Pageproduct: apply promo cURL failed init";
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, ("https://soapstandportal.com/api/coupon/find/" + promocode).toUtf8().constData());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback_coupon);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (res != CURLE_OK)
        {
            ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #f44336;border-color:#f44336;");
            qDebug() << "Invalid Coupon curl problem. error code: " << res;
        }
        else
        {
            int new_percent;

            if (http_code == 200)
            {
                json coupon_obj = json::parse(readBuffer);
                if (coupon_obj["active"])
                {
                    new_percent = coupon_obj["discount_amount"];
                    
                    selectedProductOrder->setDiscountPercentageFraction((new_percent*1.0) / 100);
                    promoPercent = new_percent;
                    qDebug() << "Apply coupon percentage: " << promoPercent;
                    loadOrderSelectedSize();
                }
                else
                {
                    qDebug() << "Invalid Coupon";
                    ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #75756f;border-color:#f44336;");
                }
            }
            else
            {
                qDebug() << "Invalid Coupon http 200 response";
                ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #f44336;border-color:#f44336;");
            }
        }
    }
}

void pageProduct::keyboardButtonPressed(int buttonID)
{

    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    QString buttonText = buttonpressed->objectName();

    if (buttonText == "backspace")
    {
        ui->promoCode->backspace();
    }
    else if (buttonText == "done")
    {
        if (ui->promoCode->text() == "")
        {
            ui->promoCode->hide();
        }
        ui->promoKeyboard->hide();
    }
    else if (buttonText.mid(0, 3) == "num")
    {
        ui->promoCode->setText(ui->promoCode->text() + buttonText.mid(3, 1));
    }
    else
    {
        ui->promoCode->setText(ui->promoCode->text() + buttonText);
    }
}
void pageProduct::on_previousPage_Button_clicked()
{

    //    qDebug() << "pageProduct: Previous button" << endl;
    while (!stopSelectTimers())
    {
    };
    selectIdleTimer->stop();
    p_page_select_product->showFullScreen();

    ui->label_invoice_discount_amount->hide();
    ui->label_invoice_discount_name->hide();

    ui->promoInputButton->show();

    couponHandler();

    usleep(100);
    this->hide();
}

void pageProduct::coupon_disable()
{
    ui->promoCode->hide();
    ui->promoKeyboard->hide();
    ui->promoInputButton->hide();
    ui->label_invoice_discount_amount->hide();
    ui->label_invoice_discount_name->hide();
    ui->promoButton->hide();
}
void pageProduct::coupon_input_show()
{
}
void pageProduct::coupon_input_hide()
{
    ui->promoKeyboard->hide();
}
void pageProduct::coupon_input_reset()
{
}
void pageProduct::on_promoCodeInput_clicked()
{
    QObject *button = QObject::sender();
    ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #5E8580;border-color:#5E8580;");
    // ui->promoInputButton->hide();
    ui->promoKeyboard->show();
    qDebug() << "show promo keyboard.";
    ui->promoCode->show();
}
void pageProduct::couponHandler()
{
    qDebug() << "db for coupons";

    DbManager db(DB_PATH);
    bool coupons_enabled = db.getCouponsEnabled();

    db.closeDB();

    if (coupons_enabled)
    {
        qDebug() << "Coupons are enabled for this machine.";
        ui->promoInputButton->show();
    }
    else
    {
        qDebug() << "Coupons are disabled for this machine.";
        coupon_disable();
    }
}

void pageProduct::on_pagePayment_Button_clicked()
{
    qDebug() << "pageProduct: Pay button";

    ui->mainPage_Button->setEnabled(false);
    ui->previousPage_Button->setEnabled(false);

    this->stopSelectTimers();
    selectIdleTimer->stop();
    QString paymentMethod = selectedProductOrder->getSelectedPaymentMethod();

    if (paymentMethod == "qr" || paymentMethod == "tap")
    {
        CURL *curl;
        CURLcode res;
        curl = curl_easy_init();

        if (!curl)
        {
            qDebug() << "Pageproduct: cURL failed init";
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/ping");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            qDebug() << "ERROR: Failed to reach soapstandportal. error code: " + QString::number(res);

            // wifiError->showEvent(wifiErrorEvent);
            wifiError->showFullScreen();
            this->hide();
        }
        else
        {
            QString feedback = QString::fromUtf8(readBuffer.c_str());
            qDebug() << "Server feedback readbuffer: " << feedback;

            ui->label_invoice_price_total->text();

            paymentPage->showFullScreen();
            this->hide();
        }
        curl_easy_cleanup(curl);
        readBuffer = "";
    }
    else if (paymentMethod == "barcode" || paymentMethod == "plu")
    {
        // p_page_dispense->showEvent(dispenseEvent); // todo Lode: this enabled together with showfullscreen calls the showEvent twice. only showevent, does not display the dispense page though.
        p_page_dispense->showFullScreen();
        this->hide();
    }
}
