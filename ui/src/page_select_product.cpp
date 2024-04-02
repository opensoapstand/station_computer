

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
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "page_select_product.h"
#include "ui_page_select_product.h"
#include "page_buybottle.h"
#include "page_product.h"
#include "page_product_mixing.h"
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

    statusbarLayout = new QVBoxLayout(this);
}

/*
 * Page Tracking reference
 */
void page_select_product::setPage(page_product *p_page_product, page_buyBottle *p_page_buyBottle, page_idle_products *p_page_idle_products, page_idle *pageIdle, page_maintenance *pageMaintenance, page_help *pageHelp, statusbar *p_statusbar)
{
    this->p_page_product = p_page_product;
    this->p_page_buyBottle = p_page_buyBottle;
    this->p_page_idle = pageIdle;
    this->p_page_maintenance = pageMaintenance;
    this->p_page_help = pageHelp;
    this->p_statusbar = p_statusbar;
}
// DTOR
page_select_product::~page_select_product()
{
    delete ui;
}

void page_select_product::showEvent(QShowEvent *event)
{
    statusbarLayout->addWidget(p_statusbar); // with an index of 0.

    //  statusbarLayout->setAlignment(Qt::AlignBottom); // Align at the bottom of the layout
    // statusbarLayout->setContentsMargins(50, 50, 50, 50); // int left, int top, int right, int bottom);
    statusbarLayout->setContentsMargins(0, 1874, 0, 0); // int left, int top, int right, int bottom);

    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);
    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget

    p_page_idle->thisMachine->dispenseButtonLightsAnimateState(false);
    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_SELECT_PRODUCT_BACKGROUND_PATH);

    QString full_path = p_page_idle->thisMachine->getTemplatePathFromName(IMAGE_BUTTON_HELP);

    p_page_idle->thisMachine->addPictureToLabel(ui->label_help, full_path);
    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_SELECT_PRODUCT_CSS);
    ui->pushButton_to_maintenance->setStyleSheet(styleSheet);
    ui->pushButton_help_page->setStyleSheet(styleSheet);
    ui->pushButton_to_idle->setStyleSheet(styleSheet);
    ui->label_pick_soap->setStyleSheet(styleSheet);

    displayProducts();

    for (uint8_t slot_index = 0; slot_index < SELECT_PRODUCT_PAGE_SLOT_COUNT_MAX; slot_index++)
    // for (uint8_t option_index = 0; option_index < p_page_idle->thisMachine->getOptionCount(); option_index++)
    {
        labels_product_overlay_text[slot_index]->setStyleSheet(styleSheet);
        labels_product_overlay_text[slot_index]->setProperty("class", "label_product_overlay_available"); // apply class BEFORE setStyleSheet!!

        labels_product_type[slot_index]->setProperty("class", "label_product_type");
        labels_product_type[slot_index]->setStyleSheet(styleSheet);
        labels_product_picture[slot_index]->setProperty("class", "label_product_photo");
        labels_product_picture[slot_index]->setStyleSheet(styleSheet);
        // p_page_idle->thisMachine->addCssClassToObject(labels_product_picture[slot_index], "label_product_overlay_unavailable", PAGE_SELECT_PRODUCT_CSS);
        labels_product_name[slot_index]->setProperty("class", "label_product_name");
        labels_product_name[slot_index]->setStyleSheet(styleSheet);
        pushButtons_product_select[slot_index]->setProperty("class", "PushButton_selection");
        pushButtons_product_select[slot_index]->setStyleSheet(styleSheet);
    }

    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_pick_soap);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_to_idle);

    maintenanceCounter = 0;

    productPageEndTimer->start(1000);
    _productPageTimeoutSec = 15;

    ui->pushButton_to_idle->raise();
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
    // QString product_status_text;

    for (uint8_t slot_index = 0; slot_index < SELECT_PRODUCT_PAGE_SLOT_COUNT_MAX; slot_index++)
    // for (uint8_t option_index = 0; option_index < p_page_idle->thisMachine->getOptionCount(); option_index++)
    {

        int option_index = (DISPENSE_PRODUCTS_PER_BASE_LINE_MAX * slot_index); // option menu has more products per slot, we need to take that into account for this 1 product per slot select product page.
        qDebug() << "Page select. Set up option: " << option_index + 1;
        QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_SELECT_PRODUCT_CSS);

        if (p_page_idle->thisMachine->getSlotByPosition(slot_index + 1)->getIsSlotEnabled())
        {
            pushButtons_product_select[slot_index]->show();
            labels_product_picture[slot_index]->show();
            labels_product_icon[slot_index]->show();
            labels_product_type[slot_index]->show();
            labels_product_name[slot_index]->show();
            labels_product_overlay_text[slot_index]->show();

            // display product picture
            p_page_idle->thisMachine->addPictureToLabel(labels_product_picture[slot_index], p_page_idle->thisMachine->getProductFromMenuOption(option_index + 1)->getProductPicturePath());
            product_type = p_page_idle->thisMachine->getProductFromMenuOption(option_index + 1)->getProductType();
            product_name = p_page_idle->thisMachine->getProductFromMenuOption(option_index + 1)->getProductName();

            QString base_product_state_as_string = p_page_idle->thisMachine->getSlotBaseProduct(slot_index + 1)->getProductStateAsString();

            qDebug() << "Product: " << product_type << "At Option: " << (option_index + 1) << ", enabled: " << p_page_idle->thisMachine->getSlotByPosition(slot_index + 1)->getIsSlotEnabled() << " Base product status: " << base_product_state_as_string;

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
            labels_product_type[slot_index]->setText(type_text);
            QString product_type_icon_path = p_page_idle->thisMachine->getTemplatePathFromName(icon_path);
            p_page_idle->thisMachine->addPictureToLabel(labels_product_icon[slot_index], product_type_icon_path);

            labels_product_overlay_text[slot_index]->raise();
            pushButtons_product_select[slot_index]->raise();
            labels_product_icon[slot_index]->setText("");
            labels_product_icon[slot_index]->raise();

            ProductState base_product_state = p_page_idle->thisMachine->getSlotBaseProduct(slot_index + 1)->getProductState();
            switch (base_product_state)
            {
            case PRODUCT_STATE_AVAILABLE:
            {

                p_page_idle->thisMachine->addCssClassToObject(labels_product_overlay_text[slot_index], "label_product_overlay_available", PAGE_SELECT_PRODUCT_CSS);

                labels_product_overlay_text[slot_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->available"));
                break;
            }
            case PRODUCT_STATE_AVAILABLE_LOW_STOCK:
            {

                p_page_idle->thisMachine->addCssClassToObject(labels_product_overlay_text[slot_index], "label_product_overlay_unavailable", PAGE_SELECT_PRODUCT_CSS);
                labels_product_overlay_text[slot_index]->setProperty("class", "label_product_overlay_unavailable"); // apply class BEFORE setStyleSheet!!
                labels_product_overlay_text[slot_index]->setStyleSheet(styleSheet);

                labels_product_overlay_text[slot_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->almost_empty"));
                break;
            }
            case PRODUCT_STATE_PROBLEM_EMPTY:
            {
                p_page_idle->thisMachine->addCssClassToObject(labels_product_overlay_text[slot_index], "label_product_overlay_unavailable", PAGE_SELECT_PRODUCT_CSS);
                labels_product_overlay_text[slot_index]->setProperty("class", "label_product_overlay_unavailable"); // apply class BEFORE setStyleSheet!!
                labels_product_overlay_text[slot_index]->setStyleSheet(styleSheet);

                labels_product_overlay_text[slot_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->empty"));
                pushButtons_product_select[slot_index]->hide();
                break;
            }
            case PRODUCT_STATE_DISABLED:
            {
                p_page_idle->thisMachine->addCssClassToObject(labels_product_overlay_text[slot_index], "label_product_overlay_unavailable", PAGE_SELECT_PRODUCT_CSS);
                labels_product_overlay_text[slot_index]->setProperty("class", "label_product_overlay_unavailable"); // apply class BEFORE setStyleSheet!!
                labels_product_overlay_text[slot_index]->setStyleSheet(styleSheet);

                labels_product_overlay_text[slot_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->not_enabled"));
                pushButtons_product_select[slot_index]->hide();

                break;
            }
            case PRODUCT_STATE_INVALID:
            {
                p_page_idle->thisMachine->addCssClassToObject(labels_product_overlay_text[slot_index], "label_product_overlay_unavailable", PAGE_SELECT_PRODUCT_CSS);
                labels_product_overlay_text[slot_index]->setProperty("class", "label_product_overlay_unavailable"); // apply class BEFORE setStyleSheet!!
                labels_product_overlay_text[slot_index]->setStyleSheet(styleSheet);

                labels_product_overlay_text[slot_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->not_enabled"));
                pushButtons_product_select[slot_index]->hide();
                break;
            }
            default:
            {
                p_page_idle->thisMachine->addCssClassToObject(labels_product_overlay_text[slot_index], "label_product_overlay_unavailable", PAGE_SELECT_PRODUCT_CSS);
                labels_product_overlay_text[slot_index]->setProperty("class", "label_product_overlay_unavailable"); // apply class BEFORE setStyleSheet!!
                labels_product_overlay_text[slot_index]->setStyleSheet(styleSheet);
                labels_product_overlay_text[slot_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->default"));
                pushButtons_product_select[slot_index]->hide();
                break;
            }
            }

            // if (p_page_idle->thisMachine->getSlotByPosition(slot_index + 1)->getIsSlotEnabled())
            // {
            //     p_page_idle->thisMachine->addCssClassToObject(labels_product_overlay_text[slot_index], "label_product_overlay_available", PAGE_SELECT_PRODUCT_CSS);
            // }
            // else
            // {
            //     p_page_idle->thisMachine->addCssClassToObject(labels_product_overlay_text[slot_index], "label_product_overlay_unavailable", PAGE_SELECT_PRODUCT_CSS);
            //     labels_product_overlay_text[slot_index]->setProperty("class", "label_product_overlay_unavailable"); // apply class BEFORE setStyleSheet!!
            //     labels_product_overlay_text[slot_index]->setStyleSheet(styleSheet);
            // }

            // ProductState base_product_state = p_page_idle->thisMachine->getSlotBaseProduct(slot_index + 1)->getProductState();
            // switch (base_product_state)
            // {
            // case (PRODUCT_STATE_AVAILABLE):
            // {
            //     labels_product_overlay_text[slot_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->available"));
            //     break;
            // }
            // case (PRODUCT_STATE_AVAILABLE_LOW_STOCK):
            // {
            //     labels_product_overlay_text[slot_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->almost_empty"));
            //     break;
            // }
            // case (PRODUCT_STATE_PROBLEM_EMPTY):
            // {
            //     labels_product_overlay_text[slot_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->empty"));
            //     break;
            // }
            // case (PRODUCT_STATE_DISABLED):
            // {
            //     labels_product_overlay_text[slot_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->coming_soon"));
            //     break;
            // }
            // case (PRODUCT_STATE_INVALID):
            // {
            //     labels_product_overlay_text[slot_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->not_enabled"));
            //     break;
            // }
            // default:
            // {
            //     labels_product_overlay_text[slot_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->default"));
            //     break;
            // }
            // }
        }
        else
        {
            qDebug() << "Slot not existing: " << (slot_index + 1);
            pushButtons_product_select[slot_index]->hide();
            labels_product_picture[slot_index]->hide();
            labels_product_icon[slot_index]->hide();
            labels_product_type[slot_index]->hide();
            labels_product_name[slot_index]->hide();
            labels_product_overlay_text[slot_index]->hide();
        }
    }
}

void page_select_product::select_product(int option)
{
    if (p_page_idle->thisMachine->getSlotFromOption(option)->getIsSlotEnabled())
    {
        p_page_idle->thisMachine->setSelectedProductByOption(option);
        p_page_idle->thisMachine->setSelectedSlotFromSelectedProduct();

        hideCurrentPageAndShowProvided(p_page_product);
    }
    else
    {
        qDebug() << "Invalid choice. Option not available: " << option;
    }
}

// FIXME: This is terrible...no time to make array reference to hold button press functions
void page_select_product::on_pushButton_selection1_clicked()
{
    select_product(DISPENSE_PRODUCTS_PER_BASE_LINE_MAX * 0 + 1);
}
void page_select_product::on_pushButton_selection2_clicked()
{
    select_product(DISPENSE_PRODUCTS_PER_BASE_LINE_MAX * 1 + 1);
}
void page_select_product::on_pushButton_selection3_clicked()
{
    select_product(DISPENSE_PRODUCTS_PER_BASE_LINE_MAX * 2 + 1);
}
void page_select_product::on_pushButton_selection4_clicked()
{
    select_product(DISPENSE_PRODUCTS_PER_BASE_LINE_MAX * 3 + 1);
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
    statusbarLayout->removeWidget(p_statusbar);
    p_page_idle->thisMachine->pageTransition(this, pageToShow);
}

void page_select_product::on_pushButton_to_idle_clicked()
{
    qDebug() << "Back to Idle Page Button pressed";
    // p_page_idle->setDiscountPercentage(0.0);
    if (p_page_idle->thisMachine->hasBuyBottleOption())
    {
        hideCurrentPageAndShowProvided(p_page_buyBottle);
    }
    else
    {
        hideCurrentPageAndShowProvided(p_page_idle);
    }
}

void page_select_product::on_pushButton_help_page_clicked()
{
    qDebug() << "Help_Button pressed";
    // p_page_idle->setDiscountPercentage(0.0);
    hideCurrentPageAndShowProvided(p_page_help);
}
