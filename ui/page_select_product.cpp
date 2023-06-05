

//***************************************
//
// page_select_product.cpp
// GUI class for user to browse up to
// 6 drinks.
//
// Allows navigation to First product page
// payment selection page
//
// created: 05-04-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "page_select_product.h"
#include "ui_page_select_product.h"

#include "page_product.h"
#include "page_idle.h"

// CTOR
page_select_product::page_select_product(QWidget *parent) : QWidget(parent),
                                                            ui(new Ui::page_select_product)
{
    ui->setupUi(this);

    pushButtons_product_select[0] = ui->pushButton_selection1;
    pushButtons_product_select[1] = ui->pushButton_selection2;
    pushButtons_product_select[2] = ui->pushButton_selection3;
    pushButtons_product_select[3] = ui->pushButton_selection4;

    labels_product_picture[0] = ui->label_product_1_photo;
    labels_product_picture[1] = ui->label_product_2_photo;
    labels_product_picture[2] = ui->label_product_3_photo;
    labels_product_picture[3] = ui->label_product_4_photo;

    labels_product_overlay_text[0] = ui->label_product_1_photo_text;
    labels_product_overlay_text[1] = ui->label_product_2_photo_text;
    labels_product_overlay_text[2] = ui->label_product_3_photo_text;
    labels_product_overlay_text[3] = ui->label_product_4_photo_text;

    labels_product_name[0] = ui->label_product_1_name;
    labels_product_name[1] = ui->label_product_2_name;
    labels_product_name[2] = ui->label_product_3_name;
    labels_product_name[3] = ui->label_product_4_name;

    labels_product_icon[0] = ui->label_product_1_icon;
    labels_product_icon[1] = ui->label_product_2_icon;
    labels_product_icon[2] = ui->label_product_3_icon;
    labels_product_icon[3] = ui->label_product_4_icon;

    labels_product_type[0] = ui->label_product_1_type;
    labels_product_type[1] = ui->label_product_2_type;
    labels_product_type[2] = ui->label_product_3_type;
    labels_product_type[3] = ui->label_product_4_type;

    // ui->pushButton_to_idle->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget

    productPageEndTimer = new QTimer(this);
    productPageEndTimer->setInterval(1000);
    connect(productPageEndTimer, SIGNAL(timeout()), this, SLOT(onProductPageTimeoutTick()));
}

/*
 * Page Tracking reference
 */
void page_select_product::setPage(page_product *p_page_product, page_idle_products *p_page_idle_products, page_idle *pageIdle, page_maintenance *pageMaintenance, page_help *pageHelp)
{
    // this->selection_PageTwo = pageTwoProducts;
    this->p_page_product = p_page_product;
    this->p_page_idle = pageIdle;
    this->p_page_maintenance = pageMaintenance;
    this->p_page_help = pageHelp;

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_SELECT_PRODUCT_BACKGROUND_PATH);
    QString full_path = p_page_idle->getTemplatePathFromName(IMAGE_BUTTON_HELP);
    qDebug() << full_path;
    p_page_idle->addPictureToLabel(ui->label_notify_us, full_path);
}

// DTOR
page_select_product::~page_select_product()
{
    delete ui;
}

void page_select_product::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Select Product >>>>>>>>>";

    QString styleSheet = p_page_idle->getCSS(PAGE_SELECT_PRODUCT_CSS);
    ui->p_page_maintenanceButton->setStyleSheet(styleSheet);
    ui->pushButton_help_page->setStyleSheet(styleSheet);
    ui->pushButton_to_idle->setStyleSheet(styleSheet);
    ui->label_pick_soap->setStyleSheet(styleSheet);
    displayProducts();

    for (int i = 0; i < 4; i++)
    {
        labels_product_overlay_text[i]->setProperty("class", "label_product_overlay_available"); // apply class BEFORE setStyleSheet!!
        labels_product_overlay_text[i]->setStyleSheet(styleSheet);
        labels_product_type[i]->setProperty("class", "label_product_type");
        labels_product_type[i]->setStyleSheet(styleSheet);
        labels_product_picture[i]->setProperty("class", "label_product_photo");
        labels_product_picture[i]->setStyleSheet(styleSheet);
        labels_product_name[i]->setProperty("class", "label_product_name");
        labels_product_name[i]->setStyleSheet(styleSheet);
        pushButtons_product_select[i]->setProperty("class", "PushButton_selection");
        pushButtons_product_select[i]->setStyleSheet(styleSheet);
    }

    p_page_idle->setTemplateTextToObject(ui->label_pick_soap);
    p_page_idle->setTemplateTextToObject(ui->pushButton_to_idle);

    this->lower();
    QWidget::showEvent(event);
    maintenanceCounter = 0;


   

    productPageEndTimer->start(1000);
    _productPageTimeoutSec = 15;

    this->raise();
}

void page_select_product::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void page_select_product::displayProducts()
{
    QString product_type_icons[8] = {ICON_TYPE_CONCENTRATE_PATH, ICON_TYPE_ALL_PURPOSE_PATH, ICON_TYPE_DISH_PATH, ICON_TYPE_HAND_PATH, ICON_TYPE_LAUNDRY_PATH, ICON_TYPE_MOCKTAIL_PATH, ICON_TYPE_KOMBUCHA_PATH, ICON_TYPE_JUICE_PATH};

    bool product_slot_enabled;
    // bool product_sold_out;
    QString product_type;
    QString product_name;
    QString product_status_text;

    for (uint8_t i = 0; i < SLOT_COUNT; i++)
    {
        QString styleSheet = p_page_idle->getCSS(PAGE_SELECT_PRODUCT_CSS);
        uint8_t slot = i + 1;

        // display product picture

        // labels_product_picture[i]->setStyleSheet("border: 1px solid black;");
        // labels_product_picture[i]->setStyleSheet(styleSheet);
        p_page_idle->addPictureToLabel(labels_product_picture[i], p_page_idle->products[i].getProductPicturePath());
        product_type = p_page_idle->products[i].getProductType();
        product_name = p_page_idle->products[i].getProductName();

        qDebug() << "db (re)load product details:";
        product_slot_enabled = p_page_idle->products[i].getSlotEnabled();

        DbManager db(DB_PATH);
        product_status_text = db.getStatusText(slot);
        db.closeDB();

        qDebug() << "Product: " << product_type << "At slot: " << slot << ", enabled: " << product_slot_enabled << " Status text: " << product_status_text;

        labels_product_name[i]->setText(product_name);
        
        // labels_product_name[i]->setStyleSheet("QLabel{font-family: 'Montserrat';font-style: normal;font-weight: 400;font-size: 28px;line-height: 36px;qproperty-alignment: AlignCenter;color: #003840;}");
        // labels_product_name[i]->setStyleSheet(styleSheet);

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
        else if(product_type== "Kombucha"){
            icon_path = ICON_TYPE_KOMBUCHA_PATH;
            type_text = "KOMBUCHA";
        }
        else if(product_type == "Mocktail"){
            icon_path = ICON_TYPE_MOCKTAIL_PATH;
            type_text = "MOCKTAIL";
        }
        else if(product_type == "Juice"){
            icon_path = ICON_TYPE_JUICE_PATH;
            type_text = "JUICE";
        }
        else
        {
            icon_path = "NOT A VALID PRODUCT TYPE";
            type_text = product_type;
            qDebug() << "Product type not found for UI text and icon. Is the slot type set correctly in the products file? : " << type_text;
        }
        QString icon_path_with_template = p_page_idle->getTemplatePathFromName(icon_path);

        p_page_idle->addPictureToLabel(labels_product_icon[i], icon_path_with_template);
        // labels_product_icon[i]->setText(""); // icon should not display text.
        labels_product_icon[i]->setText(p_page_idle->getTemplateTextByPage(this, "no_text"));

        // pushButtons_product_select[i]->setStyleSheet(styleSheet); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
        // pushButtons_product_select[i]->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
        //  pushButtons_product_select[i]->setStyleSheet("QPushButton{ background-color: 0x44881188; border: 2px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
        //  selectProductOverlayLabels[i]->raise();
        labels_product_icon[i]->raise();
        labels_product_overlay_text[i]->raise();

        pushButtons_product_select[i]->raise();


        if (product_status_text.compare("SLOT_STATE_AVAILABLE") == 0)
        {
            labels_product_overlay_text[i]->setText(p_page_idle->getTemplateTextByPage(this, "available"));
        }
        else if (product_status_text.compare("SLOT_STATE_AVAILABLE_LOW_STOCK") == 0)
        {
            labels_product_overlay_text[i]->setText(p_page_idle->getTemplateTextByPage(this, "almost_empty"));
        }
        else if (product_status_text.compare("SLOT_STATE_PROBLEM_EMPTY") == 0)
        {
            labels_product_overlay_text[i]->setText(p_page_idle->getTemplateTextByPage(this, "sold_out"));
        }
        else if (product_status_text.compare("SLOT_STATE_DISABLED_COMING_SOON") == 0)
        {
            labels_product_overlay_text[i]->setText(p_page_idle->getTemplateTextByPage(this, "coming_soon"));
        }
        else if (product_status_text.compare("SLOT_STATE_PROBLEM_NEEDS_ATTENTION") == 0)
        {
            labels_product_overlay_text[i]->setText(p_page_idle->getTemplateTextByPage(this, "assistance"));
        }
        else
        {
            labels_product_overlay_text[i]->setText(p_page_idle->getTemplateTextByPage(this, "default"));
        }

        if (!(p_page_idle->products[i].isProductVolumeInContainer()))
        {
            labels_product_overlay_text[i]->setText(p_page_idle->getTemplateTextByPage(this, "sold_out"));
        }

        labels_product_type[i]->setText(type_text);
        // labels_product_type[i]->setStyleSheet("QLabel{font-family: 'Brevia';font-style: normal;font-weight: 700;font-size: 30px;line-height: 41px;qproperty-alignment: AlignCenter;text-transform: uppercase;color: #5E8580;}");
    }
}

void page_select_product::select_product(int slot)
{
    bool product_slot_enabled = p_page_idle->products[slot - 1].getSlotEnabled();

    if (product_slot_enabled)
    {
        qDebug() << "selected slot: " << slot;
        p_page_idle->setSelectedProduct(slot);
        hideCurrentPageAndShowProvided(p_page_product);
    }
    else
    {
        qDebug() << "Slot not enabled : " << slot;
    }
}

// FIXME: This is terrible...no time to make array reference to hold button press functions
void page_select_product::on_pushButton_selection1_clicked()
{
    select_product(1);
}
void page_select_product::on_pushButton_selection2_clicked()
{
    select_product(2);
}
void page_select_product::on_pushButton_selection3_clicked()
{
    select_product(3);
}
void page_select_product::on_pushButton_selection4_clicked()
{
    select_product(4);
}

void page_select_product::onProductPageTimeoutTick()
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

void page_select_product::on_p_page_maintenanceButton_pressed()
{
    maintenanceCounter++;
    if (maintenanceCounter > 50)
    {
        hideCurrentPageAndShowProvided(p_page_maintenance);
    }
}

void page_select_product::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    productPageEndTimer->stop();
    qDebug() << "Exit select product page.";
    this->raise();
    p_page_idle->pageTransition(this, pageToShow);
}

void page_select_product::on_pushButton_to_idle_clicked()
{
    qDebug() << "Back to Idle Page Button pressed";
    p_page_idle->setDiscountPercentage(0.0);
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_select_product::on_pushButton_help_page_clicked()
{
    qDebug() << "Help_Button pressed";
    p_page_idle->setDiscountPercentage(0.0);
    hideCurrentPageAndShowProvided(p_page_help);
}
