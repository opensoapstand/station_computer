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
    selectProductButtons[0] = ui->selection1_Button;
    selectProductButtons[1] = ui->selection2_Button;
    selectProductButtons[2] = ui->selection3_Button;
    selectProductButtons[3] = ui->selection4_Button;

    selectProductPhotoLabels[0] = ui->product_1_photo_label;
    selectProductPhotoLabels[1] = ui->product_2_photo_label;
    selectProductPhotoLabels[2] = ui->product_3_photo_label;
    selectProductPhotoLabels[3] = ui->product_4_photo_label;

    selectProductOverlayLabels[0] = ui->product_1_overlay_label;
    selectProductOverlayLabels[1] = ui->product_2_overlay_label;
    selectProductOverlayLabels[2] = ui->product_3_overlay_label;
    selectProductOverlayLabels[3] = ui->product_4_overlay_label;

    selectProductPhotoLabelsText[0] = ui->product_1_photo_label_text;
    selectProductPhotoLabelsText[1] = ui->product_2_photo_label_text;
    selectProductPhotoLabelsText[2] = ui->product_3_photo_label_text;
    selectProductPhotoLabelsText[3] = ui->product_4_photo_label_text;

    selectProductNameLabels[0] = ui->product_1_name_label;
    selectProductNameLabels[1] = ui->product_2_name_label;
    selectProductNameLabels[2] = ui->product_3_name_label;
    selectProductNameLabels[3] = ui->product_4_name_label;

    selectProductIconLabels[0] = ui->product_1_icon_label;
    selectProductIconLabels[1] = ui->product_2_icon_label;
    selectProductIconLabels[2] = ui->product_3_icon_label;
    selectProductIconLabels[3] = ui->product_4_icon_label;

    selectProductTypeLabels[0] = ui->product_1_type_label;
    selectProductTypeLabels[1] = ui->product_2_type_label;
    selectProductTypeLabels[2] = ui->product_3_type_label;
    selectProductTypeLabels[3] = ui->product_4_type_label;

    ui->helpPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    ui->mainPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    ui->label_pick_soap->setStyleSheet(
        "QLabel {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 64px;"
        "line-height: 99px;"
        "letter-spacing: px;"
        "color: #003840;"
        "text-align: center;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");
    ui->label_pick_soap->setText("Pick your soap");

    ui->mainPage_Button->setStyleSheet(
        "QPushButton {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 32px;"
        "background-color: transparent;"
        "border: 0px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "color: #003840;"
        "text-align: center;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");

    ui->mainPage_Button->setText("<-back");

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
    // this->selection_PageTwo = pageTwoProducts;
    this->p_page_product = pageSizeSelect;
    this->p_page_idle = pageIdle;
    this->p_page_maintenance = pageMaintenance;
    this->p_page_help = pageHelp;

    selectedProductOrder = p_page_idle->currentProductOrder;

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_IDLE_BACKGROUND_PATH);
    QString full_path = p_page_idle->getTemplatePathFromName(IMAGE_BUTTON_HELP);
    qDebug() << full_path;
    p_page_idle->addPictureToLabel(ui->label_notify_us, full_path);
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
}
void page_idle_products::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void page_idle_products::displayProducts()
{
#ifdef ENABLE_DYNAMIC_UI
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
        selectProductPhotoLabels[i]->setStyleSheet("border: 1px solid black;");
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

        selectProductNameLabels[i]->setText(product_name);
        selectProductNameLabels[i]->setStyleSheet("QLabel{font-family: 'Montserrat';font-style: normal;font-weight: 400;font-size: 28px;line-height: 36px;qproperty-alignment: AlignCenter;color: #003840;}");

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

        p_page_idle->addPictureToLabel(selectProductIconLabels[i], icon_path_with_template);
        selectProductIconLabels[i]->setText(""); // icon should not display text.

        selectProductButtons[i]->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
        // selectProductButtons[i]->setStyleSheet("QPushButton{ background-color: 0x44881188; border: 2px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
        selectProductOverlayLabels[i]->raise();
        selectProductIconLabels[i]->raise();
        selectProductPhotoLabelsText[i]->raise();
        selectProductButtons[i]->raise();

        selectProductOverlayLabels[i]->setText("");

        // overlay product status
        if (!product_slot_enabled)
        {
            selectProductPhotoLabelsText[i]->setText(product_status_text);
            selectProductOverlayLabels[i]->setStyleSheet("background-color: rgba(255,255,255,170);");
            // selectProductPhotoLabels[i]->setStyleSheet("Qlabel {background-color: rgba(255,255,255,127);}");
        }
        else if (product_sold_out)
        {
            selectProductPhotoLabelsText[i]->setText("Sold out");
            selectProductOverlayLabels[i]->setStyleSheet("background-color: transparent;");
            // selectProductPhotoLabels[i]->setStyleSheet("Qlabel {background-color: rgba(255,255,255,127);}");
        }
        else
        {
            selectProductPhotoLabelsText[i]->setText("");
            selectProductOverlayLabels[i]->setStyleSheet("background-color: transparent;");
            // selectProductPhotoLabels[i]->setStyleSheet("Qlabel {background-color: rgba(255,255,255,0);}");
            selectProductButtons[i]->setStyleSheet("QPushButton {background-color: transparent; border: 0px }");
        }

        selectProductTypeLabels[i]->setText(type_text);
        selectProductTypeLabels[i]->setStyleSheet("QLabel{font-family: 'Brevia';font-style: normal;font-weight: 700;font-size: 30px;line-height: 41px;qproperty-alignment: AlignCenter;text-transform: uppercase;color: #5E8580;}");
    }
#else
    for (uint8_t i = 0; i < SLOT_COUNT; i++)
    {
        selectProductButtons[i]->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
        selectProductButtons[i]->raise();
        selectProductPhotoLabels[i]->hide();
        selectProductNameLabels[i]->hide();
        selectProductIconLabels[i]->hide();
        selectProductTypeLabels[i]->hide();
    }

#endif
}

void page_idle_products::select_product(int slot)
{
        qDebug() << "selected slot: " << slot;
        p_page_idle->currentProductOrder->setSelectedSlot(slot);
        hideCurrentPageAndShowProvided(p_page_product);
}

// FIXME: This is terrible...no time to make array reference to hold button press functions
void page_idle_products::on_selection1_Button_clicked()
{
    select_product(1);
}
void page_idle_products::on_selection2_Button_clicked()
{
    select_product(2);
}
void page_idle_products::on_selection3_Button_clicked()
{
    select_product(3);
}
void page_idle_products::on_selection4_Button_clicked()
{
    select_product(4);
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

void page_idle_products::on_mainPage_Button_clicked()
{
    qDebug() << "Back to Idle Page Button pressed";
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_idle_products::on_helpPage_Button_clicked()
{
    qDebug() << "Help_Button pressed";
    hideCurrentPageAndShowProvided(p_page_help);
}