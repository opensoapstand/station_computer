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
// by: Lode Ameije & Ash Singla & Udbhav Kansal & Daniel C.
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
// #include <QMainWindow>
#include <QtWidgets>
#include <QtMultimediaWidgets>

//    #define PLAY_VIDEO
// CTOR
page_idle_products::page_idle_products(QWidget *parent) : QWidget(parent),
                                                          ui(new Ui::page_idle_products)
{
    ui->setupUi(this);

    labels_product_picture[0] = ui->label_product_1_photo;
    labels_product_picture[1] = ui->label_product_2_photo;
    labels_product_picture[2] = ui->label_product_3_photo;
    labels_product_picture[3] = ui->label_product_4_photo;

    labels_selectProductOverlay[0] = ui->label_product_1_overlay;
    labels_selectProductOverlay[1] = ui->label_product_2_overlay;
    labels_selectProductOverlay[2] = ui->label_product_3_overlay;
    labels_selectProductOverlay[3] = ui->label_product_4_overlay;

    labels_product_overlay_text[0] = ui->label_product_1_overlay_text;
    labels_product_overlay_text[1] = ui->label_product_2_overlay_text;
    labels_product_overlay_text[2] = ui->label_product_3_overlay_text;
    labels_product_overlay_text[3] = ui->label_product_4_overlay_text;

    labels_product_type[0] = ui->label_product_1_type;
    labels_product_type[1] = ui->label_product_2_type;
    labels_product_type[2] = ui->label_product_3_type;
    labels_product_type[3] = ui->label_product_4_type;

    ui->pushButton_to_select_product_page->raise();
    // ui->label_pick_soap->setText("Discover how to<br> refill soap here!");
    //setTemplateTextToObject(ui->label_pick_soap);


    QFont font;
    font.setFamily(QStringLiteral("Brevia"));
    font.setPointSize(20);
    font.setBold(true);
    font.setWeight(75);
}

/*
 * Page Tracking reference
 */
void page_idle_products::setPage(page_idle *pageIdle, page_select_product *p_page_select_product)
{
    this->p_pageSelectProduct = p_page_select_product;
    this->p_page_idle = pageIdle;
}

// DTOR
page_idle_products::~page_idle_products()
{
    delete ui;
}

void page_idle_products::showEvent(QShowEvent *event)
{

    p_page_idle->setTemplateTextToObject(ui->label_pick_soap);

    QString styleSheet = p_page_idle->getCSS(PAGE_IDLE_PRODUCTS_CSS);
    ui->pushButton_to_select_product_page->setStyleSheet(styleSheet); 
    ui->label_pick_soap->setStyleSheet(styleSheet);
    // ui->printer_status_label->setStyleSheet(styleSheet);
    
    for (int slot_index = 0; slot_index < SLOT_COUNT; slot_index++)
    {

        labels_product_picture[slot_index]->setProperty("class", "labels_product_picture");
        labels_product_type[slot_index]->setProperty("class", "labels_product_type");
        labels_selectProductOverlay[slot_index]->setProperty("class", "labels_selectProductOverlay");
        labels_product_picture[slot_index]->setStyleSheet(styleSheet);
        labels_selectProductOverlay[slot_index]->setStyleSheet(styleSheet);
        labels_product_type[slot_index]->setStyleSheet(styleSheet);

    }

    p_page_idle->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_IDLE_PRODUCTS_BACKGROUND_PATH);

    qDebug() << "open db: payment method";
    bool needsReceiptPrinter = false;
    for (int slot_index = 0; slot_index < SLOT_COUNT; slot_index++)
    {
        QString paymentMethod = p_page_idle->products[slot_index].getPaymentMethod();
        if (paymentMethod == "plu" || paymentMethod == "barcode" || paymentMethod == "barcode_EAN-2 " || paymentMethod == "barcode_EAN-13")
        {
            needsReceiptPrinter = true;
            qDebug() << "Needs receipt printer: " << paymentMethod;
            break;
        }

        p_page_idle->products[slot_index].setDiscountPercentageFraction(0.0);
        p_page_idle->products[slot_index].setPromoCode("");
    }
    maintenanceCounter = 0;

    displayProducts();

    addCompanyLogoToLabel(ui->logo_label);

    ui->pushButton_to_select_product_page->raise();
}
void page_idle_products::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void page_idle_products::displayProducts()
{
    QString product_type_icons[5] = {ICON_TYPE_CONCENTRATE_PATH, ICON_TYPE_ALL_PURPOSE_PATH, ICON_TYPE_DISH_PATH, ICON_TYPE_HAND_PATH, ICON_TYPE_LAUNDRY_PATH};

    bool product_slot_enabled;
    QString product_type;
    QString product_name;
    QString product_status_text;

    for (uint8_t slot_index = 0; slot_index < SLOT_COUNT; slot_index++)
    {
        // display product picture
      //  labels_product_picture[slot_index]->setStyleSheet("border: none;");
        p_page_idle->addPictureToLabel(labels_product_picture[slot_index], p_page_idle->products[slot_index].getProductPicturePath());
        product_slot_enabled = p_page_idle->products[slot_index].getSlotEnabled();
        product_status_text = p_page_idle->products[slot_index].getStatusText();
        product_type = p_page_idle->products[slot_index].getProductType();
        product_name = p_page_idle->products[slot_index].getProductName();

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
        else if (product_type == "Concentrate")
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
        QString icon_path_with_template = p_page_idle->thisMachine.getTemplatePathFromName(icon_path);

        labels_selectProductOverlay[slot_index]->raise();
        labels_product_overlay_text[slot_index]->raise();

        labels_selectProductOverlay[slot_index]->setText("");

        // overlay product status
            labels_product_overlay_text[slot_index]->setText("");

        labels_product_type[slot_index]->setText(type_text);
        //labels_product_type[slot_index]->setStyleSheet(styleSheet);
    }
}

void page_idle_products::select_product(int slot)
{
    qDebug() << "selected slot: " << slot;
    hideCurrentPageAndShowProvided(p_page_product);
}

void page_idle_products::checkReceiptPrinterStatus()
{
    if (p_page_idle->thisMachine.hasReceiptPrinter())
    {
        qDebug() << "Check receipt printer functionality.";
        this->p_page_maintenance_general->send_check_printer_status_command();
        ui->pushButton_to_select_product_page->hide(); // when printer needs to be restarted, it can take some time. Make sure nobody presses the button in that interval (to prevent crashes)
    }
    else
    {
        qDebug() << "Receipt printer not enabled in db->machine table";
    }
}

void page_idle_products::addCompanyLogoToLabel(QLabel *label)
{
    qDebug() << "db init company logo";
    DbManager db(DB_PATH);
    QString id = db.getCustomerId();

    db.closeDB();
    qDebug() << "db closed";
    if (id.at(0) == 'C')
    {
        QString logo_path = QString(CLIENT_LOGO_PATH).arg(id);
        p_page_idle->addPictureToLabel(label, logo_path);
    }
    else
    {
        qDebug() << "WARNING: invalid customer ID. Should like C-1, C-374, ... . Provided id: " << id;
    }
}

void page_idle_products::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    // productPageEndTimer->stop();
    qDebug() << "Exit select product page.";
    // this->raise();
    p_page_idle->pageTransition(this, pageToShow);
}

void page_idle_products::printerStatusFeedback(bool isOnline, bool hasPaper)
{
    qDebug() << "Printer feedback received from fsm";

    if (!isOnline)
    {
        ui->printer_status_label->raise();

        p_page_idle->setTemplateTextWithIdentifierToObject(ui->printer_status_label, "assistance_printer_offline");
        // ui->printer_status_label->setText("Assistance needed\nReceipt Printer offline.");
        ui->printer_status_label->show();
    }
    else if (!hasPaper)
    {
        ui->printer_status_label->raise();
       // ui->printer_status_label->setText("Assistance needed\nReceipt printer empty or improperly loaded.");
        p_page_idle->setTemplateTextWithIdentifierToObject(ui->printer_status_label, "empty_improperly_loaded");
        ui->printer_status_label->show();
    }
    else
    {
        ui->printer_status_label->hide();
    }
    ui->pushButton_to_select_product_page->show();

    QString styleSheet = p_page_idle->getCSS(PAGE_IDLE_PRODUCTS_CSS);

    ui->printer_status_label->setStyleSheet(styleSheet);

    // ui->printer_status_label->setStyleSheet(
    //     "QLabel {"

    //     "font-family: 'Brevia';"
    //     "font-style: normal;"
    //     "font-weight: 100;"
    //     "background-color: #5E8580;"
    //     "font-size: 42px;"
    //     "text-align: centre;"
    //     "line-height: auto;"
    //     "letter-spacing: 0px;"
    //     "qproperty-alignment: AlignCenter;"
    //     "border-radius: 20px;"
    //     "color: white;"
    //     "border: none;"
    //     "}");

}

void page_idle_products::on_pushButton_to_select_product_page_clicked()
{
    this->hideCurrentPageAndShowProvided(p_pageSelectProduct);
}

// void page_idle_products::setTemplateTextToObject(QWidget *p_element)
// {
//     QString searchString = getTemplateTextByElementNameAndPage(p_element);
//     setTextToOjbect(p_element, searchString);
// }