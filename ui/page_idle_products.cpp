//***************************************
//
// page_idle_products.cpp
// GUI class while machine is page_idle_products.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Product Page1
//
// created: 01-05-2023
// by: Lode Ameije & Ash Singla & Udbhav Kansal
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "page_idle_products.h"
#include "ui_page_idle_products.h"
#include "page_maintenance.h"
#include "page_maintenance_general.h"
#include "page_idle.h"
#include "page_select_product.h"
#include "page_product.h"


#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#include <QMainWindow>
#include <QtWidgets>
#include <QtMultimediaWidgets>

//    #define PLAY_VIDEO
// CTOR
page_idle_products::page_idle_products(QWidget *parent) : QWidget(parent),
                                                            ui(new Ui::page_idle_products)
{
    ui->setupUi(this);

    ui->p_page_maintenanceButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
 

    selectProductPhotoLabels[0] = ui->label_product_1_photo;
    selectProductPhotoLabels[1] = ui->label_product_2_photo;
    selectProductPhotoLabels[2] = ui->label_product_3_photo;
    selectProductPhotoLabels[3] = ui->label_product_4_photo;

    selectProductOverlayLabels[0] = ui->label_product_1_overlay;
    selectProductOverlayLabels[1] = ui->label_product_2_overlay;
    selectProductOverlayLabels[2] = ui->label_product_3_overlay;
    selectProductOverlayLabels[3] = ui->label_product_4_overlay;

    selectProductPhotoLabelsText[0] = ui->label_product_1_photo_text;
    selectProductPhotoLabelsText[1] = ui->label_product_2_photo_text;
    selectProductPhotoLabelsText[2] = ui->label_product_3_photo_text;
    selectProductPhotoLabelsText[3] = ui->label_product_4_photo_text;



    selectProductTypeLabels[0] = ui->label_product_1_type;
    selectProductTypeLabels[1] = ui->label_product_2_type;
    selectProductTypeLabels[2] = ui->label_product_3_type;
    selectProductTypeLabels[3] = ui->label_product_4_type;

    //ui->pushButton_help_page->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    // ui->pushButton_to_idle->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    ui->label_pick_soap->setStyleSheet(
        "QLabel {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 54px;"
        "line-height: 99px;"
        "letter-spacing: px;"
        "color: #003840;"
        "text-align: center;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");
    ui->label_pick_soap->setText("Discover how to<br> refill soap here");

   

    QFont font;
    font.setFamily(QStringLiteral("Brevia"));
    font.setPointSize(20);
    font.setBold(true);
    font.setWeight(75);

    productPageEndTimer = new QTimer(this);
    productPageEndTimer->setInterval(1000);
    connect(productPageEndTimer, SIGNAL(timeout()), this, SLOT(onProductPageTimeoutTick()));
}

/*
 * Page Tracking reference
 */
void page_idle_products::setPage(pageProduct *pageSizeSelect, page_idle_products *p_page_idle_products, page_idle *pageIdle, page_maintenance *pageMaintenance, page_help *pageHelp)
{
    this->p_page_product = pageSizeSelect;
    this->p_page_idle = pageIdle;
    this->p_page_maintenance = pageMaintenance;
    this->p_page_help = pageHelp;

    selectedProductOrder = p_page_idle->currentProductOrder;

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_IDLE_PRODUCTS_BACKGROUND_PATH);
    QString full_path = p_page_idle->getTemplatePathFromName(IMAGE_BUTTON_HELP);
    qDebug() << full_path;
}

// DTOR
page_idle_products::~page_idle_products()
{
    delete ui;
}

void page_idle_products::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Select Product >>>>>>>>>";
    this->lower();
    QWidget::showEvent(event);
    maintenanceCounter = 0;

    displayProducts();

    if (productPageEndTimer == nullptr)
    {
        productPageEndTimer = new QTimer(this);
        productPageEndTimer->setInterval(1000);
        connect(productPageEndTimer, SIGNAL(timeout()), this, SLOT(onProductPageTimeoutTick()));
    }

    productPageEndTimer->start(1000);
    _productPageTimeoutSec = 15;

    this->raise();
    addCompanyLogoToLabel(ui->logo_label);
}
void page_idle_products::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void page_idle_products::displayProducts()
{
    QString product_type_icons[5] = {ICON_TYPE_CONCENTRATE_PATH, ICON_TYPE_ALL_PURPOSE_PATH, ICON_TYPE_DISH_PATH, ICON_TYPE_HAND_PATH, ICON_TYPE_LAUNDRY_PATH};

    bool product_slot_enabled;
    bool product_sold_out;
    QString product_type;
    QString product_name;
    QString product_status_text;

    for (uint8_t i = 0; i < SLOT_COUNT; i++)
    {
        uint8_t slot = i + 1;

        // display product picture
        selectProductPhotoLabels[i]->setStyleSheet("border: none;");
        p_page_idle->addPictureToLabel(selectProductPhotoLabels[i], p_page_idle->currentProductOrder->getProductPicturePath(slot));

        qDebug() << "db (re)load product details:";
        DbManager db(DB_PATH);
        
        product_slot_enabled = db.getSlotEnabled(slot);

        product_sold_out = !(db.isProductVolumeInContainer(slot));
        product_status_text = db.getStatusText(slot);
        db.closeDB();

        product_type = p_page_idle->currentProductOrder->getProductType(slot);
        product_name = p_page_idle->currentProductOrder->getProductName(slot);
        
        qDebug() << "Product: " << product_type << "At slot: " << slot << ", enabled: " << product_slot_enabled << ", product set as not available?: " << product_sold_out << " Status text: " << product_status_text;

        // selectProductNameLabels[i]->setText(product_name);
        // selectProductNameLabels[i]->setStyleSheet("QLabel{font-family: 'Montserrat';font-style: normal;font-weight: 400;font-size: 28px;line-height: 36px;qproperty-alignment: AlignCenter;color: #003840;}");

        // display product type icon  picture
        QString icon_path = "not found";
        QString type_text;
        if (product_type == "Dish")
        {
            icon_path = ICON_TYPE_DISH_PATH;
            type_text = "DISH SOAP";
        }
        else if (product_type == "Hand")
        {
            icon_path = ICON_TYPE_HAND_PATH;
            type_text = "HAND SOAP";
        }
        else if (product_type == "Laundry")
        {
            icon_path = ICON_TYPE_LAUNDRY_PATH;
            type_text = "LAUNDRY";
        }
        else if (product_type == "All Purpose")
        {
            icon_path = ICON_TYPE_ALL_PURPOSE_PATH;
            type_text = "CLEANER ";
        }
        else if (product_type == "Contentrate")
        {
            icon_path = ICON_TYPE_CONCENTRATE_PATH;
            type_text = "CONCENTRATE";
        }
        else
        {
            icon_path = "NOT A VALID PRODUCT TYPE";
            type_text = product_type;
            qDebug() << "Product type not found for UI text and icon. Is the slot set correctly in database?";
        }
        QString icon_path_with_template = p_page_idle->getTemplatePathFromName(icon_path);

        selectProductOverlayLabels[i]->raise();
        selectProductPhotoLabelsText[i]->raise();
      
        selectProductOverlayLabels[i]->setText("");

        // overlay product status
        if (!product_slot_enabled)
        {
            selectProductPhotoLabelsText[i]->setText(product_status_text);
            selectProductOverlayLabels[i]->setStyleSheet("background-color: rgba(255,255,255,170);");
        }
        else if (product_sold_out)
        {
            selectProductPhotoLabelsText[i]->setText("Sold out");
            selectProductOverlayLabels[i]->setStyleSheet("background-color: transparent;");
        }
        else
        {
            selectProductPhotoLabelsText[i]->setText("");
            selectProductOverlayLabels[i]->setStyleSheet("background-color: transparent;");
      
        }

        selectProductTypeLabels[i]->setText(type_text);
        selectProductTypeLabels[i]->setStyleSheet("QLabel{font-family: 'Brevia';font-style: normal;font-weight: 700;font-size: 30px;line-height: 41px;qproperty-alignment: AlignCenter;text-transform: uppercase;color: #003840;}");
    }

   

}

void page_idle_products::select_product(int slot)
{
        qDebug() << "selected slot: " << slot;
        p_page_idle->currentProductOrder->setSelectedSlot(slot);
        hideCurrentPageAndShowProvided(p_page_product);
}

void page_idle_products::addCompanyLogoToLabel(QLabel *label)
{
    qDebug() << "db init company logo";
    DbManager db(DB_PATH);
    QString id = db.getCustomerId();
    QString size_small;
    QString size_medium;
    QString size_large;
    QString size_units;
    QString price_small;
    QString price_medium;
    QString price_large;
    for (uint8_t i = 0; i < SLOT_COUNT; i++) {
    size_units = db.getUnits(i);
    size_small = db.getSizeSmall(i);
    size_medium = db.getSizeMedium(i);
    size_large = db.getSizeLarge(i);
    price_small = db.getPriceSmall(i);
    price_medium = db.getPriceMedium(i);
    price_large = db.getPriceLarge(i);

    // do something with size_units
}
    db.closeDB();
    qDebug() << "db closed";
    if (id.at(0) == 'C')
    {
        QString logo_path = QString(COMPANY_LOGO_PATH).arg(id);
        p_page_idle->addPictureToLabel(label, logo_path);
    }
    else
    {
        qDebug() << "WARNING: invalid customer ID. Should like C-1, C-374, ... . Provided id: " << id;
    }
        // ui->label_product1_price->setStyleSheet(
        // "QLabel {"

        // "font-family: 'Brevia';"
        // "font-style: normal;"
        // "font-weight: 50;"
        // "font-size: 23px;"
        // "line-height: 99px;"
        // "text-align: center;"
        // "letter-spacing: 1.5px;"
        // "text-transform: lowercase;"
        // "color: #003840;"
        // "qproperty-alignment: AlignCenter;"
        // "}");

        // ui->label_product2_price->setStyleSheet(
        // "QLabel {"

        // "font-family: 'Brevia';"
        // "font-style: normal;"
        // "font-weight: 50;"
        // "font-size: 23px;"
        // "line-height: 99px;"
        // "text-align: center;"
        // "letter-spacing: 1.5px;"
        // "text-transform: lowercase;"
        // "color: #003840;"
        // "qproperty-alignment: AlignCenter;"
        // "}");

        // ui->label_product3_price->setStyleSheet(
        // "QLabel {"

        // "font-family: 'Brevia';"
        // "font-style: normal;"
        // "font-weight: 50;"
        // "font-size: 23px;"
        // "line-height: 99px;"
        // "text-align: center;"
        // "letter-spacing: 1.5px;"
        // "text-transform: lowercase;"
        // "color: #003840;"
        // "qproperty-alignment: AlignCenter;"
        // "}");
        // ui->label_product4_price->setStyleSheet(
        // "QLabel {"

        // "font-family: 'Brevia';"
        // "font-style: normal;"
        // "font-weight: 50;"
        // "font-size: 23px;"
        // "line-height: 99px;"
        // "text-align: center;"
        // "letter-spacing: 1.5px;"
        // "text-transform: lowercase;"
        // "color: #003840;"
        // "qproperty-alignment: AlignCenter;"
        // "}");
    //     QString label_text = "%1 ml | %2 ml | %3 ml<br>%4....|....%5....|.... %6";

    //  if (size_units == "ml")
    // { 
    //     label_text = label_text.arg(size_small).arg(size_medium).arg(size_large).arg( price_small).arg(price_medium).arg(price_large);
    //     ui->label_product1_price->setText(label_text);
    //     ui->label_product2_price->setText(label_text);
    //     ui->label_product3_price->setText(label_text);
    //     ui->label_product4_price->setText(label_text);
    // }
    //  else if (size_units == "L")
    // {
    //     ui->label_product1_price->setText("L");
    //     ui->label_product2_price->setText("L");
    //     ui->label_product3_price->setText("L");
    //     ui->label_product4_price->setText("L");
    // }
    //  else if (size_units == "kg")
    // {
    //     ui->label_product1_price->setText("kg");
    //     ui->label_product2_price->setText("kg");
    //     ui->label_product3_price->setText("kg");
    //     ui->label_product4_price->setText("kg");
    // }
    // else if (size_units == "oz")
    // {
    //     ui->label_product1_price->setText("oz");
    //     ui->label_product2_price->setText("oz");
    //     ui->label_product3_price->setText("oz");
    //     ui->label_product4_price->setText("oz");
    // }

}


void page_idle_products::onProductPageTimeoutTick()
{
    if (--_productPageTimeoutSec >= 0)
    {
        // qDebug() << "Tick Down: " << _productPageTimeoutSec;
    }
    else
    {
        // qDebug() << "Timer Done!" << _productPageTimeoutSec;
        hideCurrentPageAndShowProvided(p_page_idle);
    }
}

void page_idle_products::on_p_page_maintenanceButton_pressed()
{
    maintenanceCounter++;
    if (maintenanceCounter > 50)
    {
        hideCurrentPageAndShowProvided(p_page_maintenance);
    }
}

void page_idle_products::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    productPageEndTimer->stop();
    qDebug() << "Exit select product page.";
    selectedProductOrder->setDiscountPercentageFraction(0.0);
    this->raise();
    p_page_idle->pageTransition(this, pageToShow);
}

// void page_idle_products::on_pushButton_to_idle_clicked()
// {
//     qDebug() << "Back to Idle Page Button pressed";
//     hideCurrentPageAndShowProvided(p_page_idle);
// }

void page_idle_products::on_pushButton_help_page_clicked()
{
    qDebug() << "Help_Button pressed";
    hideCurrentPageAndShowProvided(p_page_help);
}