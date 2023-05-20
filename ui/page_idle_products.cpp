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

    labels_product_picture[0] = ui->label_product_1_photo;
    labels_product_picture[1] = ui->label_product_2_photo;
    labels_product_picture[2] = ui->label_product_3_photo;
    labels_product_picture[3] = ui->label_product_4_photo;

    selectProductOverlayLabels[0] = ui->label_product_1_overlay;
    selectProductOverlayLabels[1] = ui->label_product_2_overlay;
    selectProductOverlayLabels[2] = ui->label_product_3_overlay;
    selectProductOverlayLabels[3] = ui->label_product_4_overlay;

    labels_product_overlay_text[0] = ui->label_product_1_photo_text;
    labels_product_overlay_text[1] = ui->label_product_2_photo_text;
    labels_product_overlay_text[2] = ui->label_product_3_photo_text;
    labels_product_overlay_text[3] = ui->label_product_4_photo_text;



    labels_product_type[0] = ui->label_product_1_type;
    labels_product_type[1] = ui->label_product_2_type;
    labels_product_type[2] = ui->label_product_3_type;
    labels_product_type[3] = ui->label_product_4_type;

    ui->pushButton_to_select_product_page->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    ui->pushButton_to_select_product_page->raise();

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

    // productPageEndTimer = new QTimer(this);
    // productPageEndTimer->setInterval(1000);
    // connect(productPageEndTimer, SIGNAL(timeout()), this, SLOT(onProductPageTimeoutTick()));
}

/*
 * Page Tracking reference
 */
void page_idle_products::setPage(page_idle *pageIdle, page_select_product *p_pageProduct, page_maintenance *pageMaintenance, page_maintenance_general *pageMaintenanceGeneral)
{
    // Chained to KB Listener
    this->p_pageSelectProduct = p_pageProduct;
    this->p_page_maintenance = pageMaintenance;
    this->p_page_maintenance_general = pageMaintenanceGeneral;
    this->p_page_idle = pageIdle;

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_IDLE_PRODUCTS_BACKGROUND_PATH);
}

// DTOR
page_idle_products::~page_idle_products()
{
    delete ui;
}

void page_idle_products::showEvent(QShowEvent *event)
{
    qDebug() << "open db: payment method";
    bool needsReceiptPrinter = false;
    for (int slot = 1; slot <= SLOT_COUNT; slot++)
    {
        QString paymentMethod = p_page_idle->products[slot - 1].getPaymentMethod();
        if (paymentMethod == "plu" || paymentMethod == "barcode" || paymentMethod == "barcode_EAN-2 " || paymentMethod == "barcode_EAN-13")
        {
            needsReceiptPrinter = true;
            qDebug() << "Needs receipt printer: " << paymentMethod;
            break;
        }

        p_page_idle->products[slot - 1].setDiscountPercentageFraction(0.0);
        p_page_idle->products[slot - 1].setPromoCode("");
    }
    qDebug() << "<<<<<<< Page Enter: Idle page (show products) >>>>>>>>>";

    QWidget::showEvent(event);
    maintenanceCounter = 0;

    displayProducts();

    // productPageEndTimer->start(1000);
    // _productPageTimeoutSec = 15;

    addCompanyLogoToLabel(ui->logo_label);

    // ui->printer_status_label->hide(); // always hide here, will show if enabled and has problems.
    // if (needsReceiptPrinter)
    // {
    //     checkReceiptPrinterStatus();
    // }
    ui->toSelectProductPageButton->raise();
}
void page_idle_products::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void page_idle_products::displayProducts()
{
    QString product_type_icons[5] = {ICON_TYPE_CONCENTRATE_PATH, ICON_TYPE_ALL_PURPOSE_PATH, ICON_TYPE_DISH_PATH, ICON_TYPE_HAND_PATH, ICON_TYPE_LAUNDRY_PATH};

    bool product_slot_enabled;
    // bool product_sold_out;
    QString product_type;
    QString product_name;
    QString product_status_text;

    for (uint8_t i = 0; i < SLOT_COUNT; i++)
    {
        uint8_t slot = i + 1;

        // display product picture
        labels_product_picture[i]->setStyleSheet("border: none;");
        p_page_idle->addPictureToLabel(labels_product_picture[i], p_page_idle->products[i].getProductPicturePath());
        product_slot_enabled = p_page_idle->products[i].getSlotEnabled();

        qDebug() << "db (re)load product details:";
        DbManager db(DB_PATH);
        product_status_text = db.getStatusText(slot);
        db.closeDB();

        product_type = p_page_idle->products[i].getProductType();
        product_name = p_page_idle->products[i].getProductName();

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
        QString icon_path_with_template = p_page_idle->getTemplatePathFromName(icon_path);

        selectProductOverlayLabels[i]->raise();
        labels_product_overlay_text[i]->raise();
      
        selectProductOverlayLabels[i]->setText("");

        // overlay product status
            labels_product_overlay_text[i]->setText("");
            selectProductOverlayLabels[i]->setStyleSheet("background-color: transparent;");

        labels_product_type[i]->setText(type_text);
        labels_product_type[i]->setStyleSheet("QLabel{font-family: 'Brevia';font-style: normal;font-weight: 700;font-size: 30px;line-height: 41px;qproperty-alignment: AlignCenter;text-transform: uppercase;color: #003840;}");
    }
}

void page_idle_products::select_product(int slot)
{
    qDebug() << "selected slot: " << slot;
    hideCurrentPageAndShowProvided(p_page_product);
}

void page_idle_products::checkReceiptPrinterStatus()
{
    qDebug() << "db idle_products check printer";
    DbManager db(DB_PATH);
    bool isPrinterOnline = false;
    bool hasPrinterPaper = false;
    bool hasReceiptPrinter = db.hasReceiptPrinter();
    db.printerStatus(&isPrinterOnline, &hasPrinterPaper);
    db.closeDB();

    if (hasReceiptPrinter)
    {
        qDebug() << "Check receipt printer functionality disabled.";
        this->p_page_maintenance_general->send_check_printer_status_command();
        ui->toSelectProductPageButton->hide(); // when printer needs to be restarted, it can take some time. Make sure nobody presses the button in that interval (to prevent crashes)
    }
    else
    {
        qDebug() << "Can't check receipt printer. Not enabled in db->machine table";
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

// void page_idle_products::onProductPageTimeoutTick()
// {
//     if (--_productPageTimeoutSec >= 0)
//     {
//     }
//     else
//     {
//         // hideCurrentPageAndShowProvided(p_page_idle);
//     }
// }

void page_idle_products::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    // productPageEndTimer->stop();
    qDebug() << "Exit select product page.";
    this->raise();
    p_page_idle->pageTransition(this, pageToShow);
}

void page_idle_products::printerStatusFeedback(bool isOnline, bool hasPaper)
{
    qDebug() << "Printer feedback received from fsm";

    if (!isOnline)
    {
        ui->printer_status_label->raise();
        ui->printer_status_label->setText("Assistance needed\nReceipt Printer offline.");
        ui->printer_status_label->show();
    }
    else if (!hasPaper)
    {
        ui->printer_status_label->raise();
        ui->printer_status_label->setText("Assistance needed\nReceipt printer empty or improperly loaded.");
        ui->printer_status_label->show();
    }
    else
    {
        ui->printer_status_label->hide();
    }
    ui->toSelectProductPageButton->show();

    ui->printer_status_label->setStyleSheet(
        "QLabel {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 100;"
        "background-color: #5E8580;"
        "font-size: 42px;"
        "text-align: centre;"
        "line-height: auto;"
        "letter-spacing: 0px;"
        "qproperty-alignment: AlignCenter;"
        "border-radius: 20px;"
        "color: white;"
        "border: none;"
        "}");
}

void page_idle_products::on_pushButton_to_select_product_page_clicked()
{
    this->hideCurrentPageAndShowProvided(p_pageSelectProduct);
}
