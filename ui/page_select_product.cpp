

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
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "page_select_product.h"
#include "ui_page_select_product.h"

#include "page_product.h"
#include "page_idle.h"
#include "df_util.h"

// CTOR
page_select_product::page_select_product(QWidget *parent) : QWidget(parent),
                                                            ui(new Ui::page_select_product)
{
    ui->setupUi(this); // "this" is the page, the centralWidget, the main widget of which all other widgets are children.

    pushButtons_product_select[0] = ui->pushButton_selection1;
    pushButtons_product_select[1] = ui->pushButton_selection2;
    pushButtons_product_select[2] = ui->pushButton_selection3;
    pushButtons_product_select[3] = ui->pushButton_selection4;

    labels_product_picture[0] = ui->label_product_1_photo;
    labels_product_picture[1] = ui->label_product_2_photo;
    labels_product_picture[2] = ui->label_product_3_photo;
    labels_product_picture[3] = ui->label_product_4_photo;

    labels_product_overlay_text[0] = ui->label_product_1_overlay_text;
    labels_product_overlay_text[1] = ui->label_product_2_overlay_text;
    labels_product_overlay_text[2] = ui->label_product_3_overlay_text;
    labels_product_overlay_text[3] = ui->label_product_4_overlay_text;

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

    productPageEndTimer = new QTimer(this);
    productPageEndTimer->setInterval(1000);
    connect(productPageEndTimer, SIGNAL(timeout()), this, SLOT(onProductPageTimeoutTick()));
}

/*
 * Page Tracking reference
 */
void page_select_product::setPage(page_product *p_page_product, page_idle_products *p_page_idle_products, page_idle *pageIdle, page_maintenance *pageMaintenance, page_help *pageHelp)
{
    this->p_page_product = p_page_product;
    this->p_page_idle = pageIdle;
    this->p_page_maintenance = pageMaintenance;
    this->p_page_help = pageHelp;
}

// DTOR
page_select_product::~page_select_product()
{
    delete ui;
}

void page_select_product::showEvent(QShowEvent *event)
{

    p_page_idle->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    p_page_idle->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget
    
    p_page_idle->thisMachine.dispenseButtonLightsAnimateState(false);
    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_SELECT_PRODUCT_BACKGROUND_PATH);
    
    QString full_path = p_page_idle->thisMachine.getTemplatePathFromName(IMAGE_BUTTON_HELP);
    p_page_idle->addPictureToLabel(ui->label_help, full_path);
    


    QString styleSheet = p_page_idle->getCSS(PAGE_SELECT_PRODUCT_CSS);
    ui->pushButton_to_maintenance->setStyleSheet(styleSheet);
    ui->pushButton_help_page->setStyleSheet(styleSheet);
    ui->pushButton_to_idle->setStyleSheet(styleSheet);
    ui->label_pick_soap->setStyleSheet(styleSheet);

    displayProducts();

    for (int slot_index = 0; slot_index < p_page_idle-> thisMachine.getSlotCount(); slot_index++)
    {
        labels_product_overlay_text[slot_index]->setStyleSheet(styleSheet);
        labels_product_overlay_text[slot_index]->setProperty("class", "label_product_overlay_available"); // apply class BEFORE setStyleSheet!!

        labels_product_type[slot_index]->setProperty("class", "label_product_type");
        labels_product_type[slot_index]->setStyleSheet(styleSheet);
        labels_product_picture[slot_index]->setProperty("class", "label_product_photo");
        labels_product_picture[slot_index]->setStyleSheet(styleSheet);
        // p_page_idle->addCssClassToObject(labels_product_picture[slot_index], "label_product_overlay_unavailable", PAGE_SELECT_PRODUCT_CSS);
        labels_product_name[slot_index]->setProperty("class", "label_product_name");
        labels_product_name[slot_index]->setStyleSheet(styleSheet);
        pushButtons_product_select[slot_index]->setProperty("class", "PushButton_selection");
        pushButtons_product_select[slot_index]->setStyleSheet(styleSheet);
    }

    p_page_idle->setTemplateTextToObject(ui->label_pick_soap);
    p_page_idle->setTemplateTextToObject(ui->pushButton_to_idle);

    this->lower();

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
    QString product_type_icons[9] = {ICON_TYPE_CONCENTRATE_PATH, ICON_TYPE_ALL_PURPOSE_PATH, ICON_TYPE_DISH_PATH, ICON_TYPE_HAND_PATH, ICON_TYPE_LAUNDRY_PATH, ICON_TYPE_PROBIOTIC_PATH, ICON_TYPE_KOMBUCHA_PATH, ICON_TYPE_JUICE_PATH, ICON_TYPE_TEA_PATH};

    QString product_type;
    QString product_name;
    QString product_status_text;

    for (uint8_t slot_index = 0; slot_index < p_page_idle-> thisMachine.getSlotCount(); slot_index++)
    {
        QString styleSheet = p_page_idle->getCSS(PAGE_SELECT_PRODUCT_CSS);

        // display product picture

        p_page_idle->addPictureToLabel(labels_product_picture[slot_index], p_page_idle->products[slot_index].getProductPicturePath());
        product_type = p_page_idle->products[slot_index].getProductType();
        product_name = p_page_idle->products[slot_index].getProductName();

        if (!p_page_idle->products[slot_index].getSlotEnabled())
        {
            p_page_idle->addCssClassToObject(labels_product_overlay_text[slot_index], "label_product_overlay_unavailable", PAGE_SELECT_PRODUCT_CSS);

        labels_product_overlay_text[slot_index]->setStyleSheet(styleSheet);
              QString styleSheet = p_page_idle->getCSS(PAGE_SELECT_PRODUCT_CSS);
        labels_product_overlay_text[slot_index]->setProperty("class", "label_product_overlay_unavailable"); // apply class BEFORE setStyleSheet!!

            // qDebug() << labels_product_picture[slot_index]->styleSheet();
        }
        else
        {
            p_page_idle->addCssClassToObject(labels_product_overlay_text[slot_index], "label_product_overlay_available", PAGE_SELECT_PRODUCT_CSS);
        }

        product_status_text = p_page_idle->products[slot_index].getStatusText();

        qDebug() << "Product: " << product_type << "At slot: " << (slot_index + 1) << ", enabled: " << p_page_idle->products[slot_index].getSlotEnabled() << " Status text: " << product_status_text;

        labels_product_name[slot_index]->setText(product_name);

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
            type_text = "CLEANER";
        }
        else if (product_type == "Concentrate")
        {
            icon_path = ICON_TYPE_CONCENTRATE_PATH;
            type_text = "CONCENTRATE";
        }
        else if (product_type == "Kombucha")
        {
            icon_path = ICON_TYPE_KOMBUCHA_PATH;
            type_text = "KOMBUCHA";
        }
        else if (product_type == "Probiotic Soda")
        {
            icon_path = ICON_TYPE_PROBIOTIC_PATH;
            type_text = "PROBIOTIC SODA";
        }
        else if (product_type == "Juice")
        {
            icon_path = ICON_TYPE_JUICE_PATH;
            type_text = "JUICE";
        }
        else if (product_type == "Tea")
        {
            icon_path = ICON_TYPE_JUICE_PATH;
            type_text = "TEA";
        }
        else
        {
            icon_path = ICON_TYPE_DEFAULT;
            type_text = product_type;
            qDebug() << "Icon for product type not found : " << type_text << " Set to default. ";
        }
        QString product_type_icon_path = p_page_idle->thisMachine.getTemplatePathFromName(icon_path);
        p_page_idle->addPictureToLabel(labels_product_icon[slot_index], product_type_icon_path);

        // labels_product_icon[slot_index]->setText(p_page_idle->getTemplateTextByPage(this, "no_text"));

        //  labels_selectProductOverlay[slot_index]->raise();
        labels_product_overlay_text[slot_index]->raise();
        pushButtons_product_select[slot_index]->raise();
        labels_product_icon[slot_index]->setText("");
        labels_product_icon[slot_index]->raise();

        if (product_status_text.compare("SLOT_STATE_DISABLED_COMING_SOON") == 0)
        {
            labels_product_overlay_text[slot_index]->setText(p_page_idle->getTemplateTextByPage(this, "status_text->coming_soon"));
        }
        else if (product_status_text.compare("SLOT_STATE_DISABLED") == 0)
        {
            labels_product_overlay_text[slot_index]->setText(p_page_idle->getTemplateTextByPage(this, "status_text->not_enabled"));
        }
        else if (!p_page_idle->products[slot_index].getSlotEnabled())
        {
            labels_product_overlay_text[slot_index]->setText(p_page_idle->getTemplateTextByPage(this, "status_text->not_enabled"));
        }
        else if (!(p_page_idle->products[slot_index].isProductVolumeInContainer()))
        {
            labels_product_overlay_text[slot_index]->setText(p_page_idle->getTemplateTextByPage(this, "status_text->empty"));
        }
        else if (product_status_text.compare("SLOT_STATE_AVAILABLE") == 0)
        {
            labels_product_overlay_text[slot_index]->setText(p_page_idle->getTemplateTextByPage(this, "status_text->available"));
        }
        else if (product_status_text.compare("SLOT_STATE_AVAILABLE_LOW_STOCK") == 0)
        {
            labels_product_overlay_text[slot_index]->setText(p_page_idle->getTemplateTextByPage(this, "status_text->almost_empty"));
        }
        else if (product_status_text.compare("SLOT_STATE_PROBLEM_EMPTY") == 0)
        {
            labels_product_overlay_text[slot_index]->setText(p_page_idle->getTemplateTextByPage(this, "status_text->empty"));
        }
        else if (product_status_text.compare("SLOT_STATE_PROBLEM_NEEDS_ATTENTION") == 0)
        {
            labels_product_overlay_text[slot_index]->setText(p_page_idle->getTemplateTextByPage(this, "status_text->assistance"));
        }
        else
        {
            labels_product_overlay_text[slot_index]->setText(p_page_idle->getTemplateTextByPage(this, "status_text->default"));
        }

        labels_product_type[slot_index]->setText(type_text);
    }
}

void page_select_product::select_product(int slot)
{

    if (p_page_idle->products[slot - 1].getSlotEnabled())
    {
        qDebug() << "Selected slot: " << slot;
        p_page_idle->setSelectedProduct(slot);
        hideCurrentPageAndShowProvided(p_page_product);
    }
    else
    {
        qDebug() << "Invalid choice. Slot not enabled: " << slot;
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

void page_select_product::on_pushButton_to_maintenance_pressed()
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
    // p_page_idle->setDiscountPercentage(0.0);
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_select_product::on_pushButton_help_page_clicked()
{
    qDebug() << "Help_Button pressed";
    // p_page_idle->setDiscountPercentage(0.0);
    hideCurrentPageAndShowProvided(p_page_help);
}
