

//***************************************
//
// page_product_menu.cpp
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

#include "page_product_menu.h"
#include "ui_page_product_menu.h"

// #include "page_product.h"
#include "page_product_mixing.h"
#include "page_idle.h"
#include "df_util.h"

// CTOR
page_product_menu::page_product_menu(QWidget *parent) : QWidget(parent),
                                                        ui(new Ui::page_product_menu)
{
    ui->setupUi(this); // "this" is the page, the centralWidget, the main widget of which all other widgets are children.

    labels_base_product_bg[0] = ui->label_base_product_bg_1;
    labels_base_product_bg[1] = ui->label_base_product_bg_2;
    labels_base_product_bg[2] = ui->label_base_product_bg_3;
    labels_base_product_bg[3] = ui->label_base_product_bg_4;

    pushButtons_base_product[0] = ui->pushButton_base_product_1;
    pushButtons_base_product[1] = ui->pushButton_base_product_2;
    pushButtons_base_product[2] = ui->pushButton_base_product_3;
    pushButtons_base_product[3] = ui->pushButton_base_product_4;

    labels_base_product_picture[0] = ui->label_base_product_picture_1;
    labels_base_product_picture[1] = ui->label_base_product_picture_2;
    labels_base_product_picture[2] = ui->label_base_product_picture_3;
    labels_base_product_picture[3] = ui->label_base_product_picture_4;

    labels_base_product_name[0] = ui->label_base_product_name_1;
    labels_base_product_name[1] = ui->label_base_product_name_2;
    labels_base_product_name[2] = ui->label_base_product_name_3;
    labels_base_product_name[3] = ui->label_base_product_name_4;

    pushButtons_dispense_product[0] = ui->pushButton_dispense_product_1;
    pushButtons_dispense_product[1] = ui->pushButton_dispense_product_2;
    pushButtons_dispense_product[2] = ui->pushButton_dispense_product_3;
    pushButtons_dispense_product[3] = ui->pushButton_dispense_product_4;
    pushButtons_dispense_product[4] = ui->pushButton_dispense_product_5;
    pushButtons_dispense_product[5] = ui->pushButton_dispense_product_6;

    labels_dispense_product_picture[0] = ui->label_dispense_product_picture_1;
    labels_dispense_product_picture[1] = ui->label_dispense_product_picture_2;
    labels_dispense_product_picture[2] = ui->label_dispense_product_picture_3;
    labels_dispense_product_picture[3] = ui->label_dispense_product_picture_4;
    labels_dispense_product_picture[4] = ui->label_dispense_product_picture_5;
    labels_dispense_product_picture[5] = ui->label_dispense_product_picture_6;

    labels_dispense_product_name[0] = ui->label_dispense_product_name_1;
    labels_dispense_product_name[1] = ui->label_dispense_product_name_2;
    labels_dispense_product_name[2] = ui->label_dispense_product_name_3;
    labels_dispense_product_name[3] = ui->label_dispense_product_name_4;
    labels_dispense_product_name[4] = ui->label_dispense_product_name_5;
    labels_dispense_product_name[5] = ui->label_dispense_product_name_6;

    labels_product_overlay_text[0] = ui->label_dispense_product_overlay_text_1;
    labels_product_overlay_text[1] = ui->label_dispense_product_overlay_text_2;
    labels_product_overlay_text[2] = ui->label_dispense_product_overlay_text_3;
    labels_product_overlay_text[3] = ui->label_dispense_product_overlay_text_4;
    labels_product_overlay_text[4] = ui->label_dispense_product_overlay_text_5;
    labels_product_overlay_text[5] = ui->label_dispense_product_overlay_text_6;

    productPageEndTimer = new QTimer(this);
    productPageEndTimer->setInterval(1000);
    connect(productPageEndTimer, SIGNAL(timeout()), this, SLOT(onProductPageTimeoutTick()));

    statusbarLayout = new QVBoxLayout(this);
}

/*
 * Page Tracking reference
 */
void page_product_menu::setPage(page_product *p_page_product, page_product_mixing *p_page_product_mixing, page_idle_products *p_page_idle_products, page_idle *pageIdle, page_maintenance *pageMaintenance, page_help *pageHelp, statusbar *p_statusbar)
{
    this->p_page_product = p_page_product;
    this->p_page_product_mixing = p_page_product_mixing;
    this->p_page_idle = pageIdle;
    this->p_page_maintenance = pageMaintenance;
    this->p_page_help = pageHelp;
    this->p_statusbar = p_statusbar;
}
// DTOR
page_product_menu::~page_product_menu()
{
    delete ui;
}

void page_product_menu::showEvent(QShowEvent *event)
{
    statusbarLayout->addWidget(p_statusbar); // with an index of 0.

    statusbarLayout->setContentsMargins(0, 1874, 0, 0); // int left, int top, int right, int bottom);

    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);
    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget

    p_page_idle->thisMachine->dispenseButtonLightsAnimateState(false);
    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_PRODUCT_MENU_BACKGROUND_PATH);

    QString full_path = p_page_idle->thisMachine->getTemplatePathFromName(IMAGE_BUTTON_HELP);

    p_page_idle->thisMachine->addPictureToLabel(ui->label_help, full_path);
    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_PRODUCT_MENU_CSS);
    ui->pushButton_help_page->setStyleSheet(styleSheet);
    ui->pushButton_to_idle->setStyleSheet(styleSheet);
    ui->label_product_menu_title->setStyleSheet(styleSheet);

    ui->label_base_products_section_bg->setStyleSheet(styleSheet);
    ui->label_base_products_section_title->setStyleSheet(styleSheet);
    ui->label_dispense_products_section_title->setStyleSheet(styleSheet);
    ui->label_dispense_products_section_bg->setStyleSheet(styleSheet);

    for (int slot_index = 0; slot_index < p_page_idle->thisMachine->getSlotCount(); slot_index++)
    {
        qDebug() << "Set up slot: " << slot_index + 1 << " with base product: " << p_page_idle->thisMachine->getSlotBaseProduct(slot_index + 1)->getPNumber();
        labels_base_product_bg[slot_index]->setProperty("class", "label_base_product_bg");
        labels_base_product_bg[slot_index]->setStyleSheet(styleSheet);
        pushButtons_base_product[slot_index]->setProperty("class", "pushButton_base_product");
        pushButtons_base_product[slot_index]->setStyleSheet(styleSheet);

        p_page_idle->thisMachine->addPictureToLabelCircle(labels_base_product_picture[slot_index], p_page_idle->thisMachine->getSlotBaseProduct(slot_index + 1)->getProductPicturePath());
        labels_base_product_picture[slot_index]->setProperty("class", "label_base_product_picture");
        labels_base_product_picture[slot_index]->setStyleSheet(styleSheet);
        qDebug() << p_page_idle->thisMachine->getSlotBaseProduct(slot_index + 1)->getProductPicturePath();

        labels_base_product_name[slot_index]->setProperty("class", "label_base_product_name");
        labels_base_product_name[slot_index]->setStyleSheet(styleSheet);
        QString product_type = p_page_idle->thisMachine->getSlotBaseProduct(slot_index + 1)->getProductType();
        qDebug() << product_type;
        labels_base_product_name[slot_index]->setText(product_type);
    }

    select_base_product_in_menu(0);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_product_menu_title);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_to_idle);

    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_base_products_section_title);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_dispense_products_section_title);

    maintenanceCounter = 0;

    productPageEndTimer->start(1000);
    _productPageTimeoutSec = 60;

    ui->pushButton_to_idle->raise();
}

void page_product_menu::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void page_product_menu::displayDispenseProductsMenu()
{
    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_PRODUCT_MENU_CSS);
    // QString product_status_text;
    for (int sub_menu_index = 0; sub_menu_index < DISPENSE_PRODUCTS_PER_BASE_LINE_MAX; sub_menu_index++)
    {
        int option_index = m_selectedBaseProductIndex * DISPENSE_PRODUCTS_PER_BASE_LINE_MAX + sub_menu_index;

        if (p_page_idle->thisMachine->isOptionExisting(option_index + 1))
        {
            pnumberproduct *dispenseProduct = p_page_idle->thisMachine->getProductFromMenuOption(option_index + 1);

            QString picturePath = dispenseProduct->getProductPicturePath();
            QString increment_text = "%IMAGE_PATH%1%";
            QString image_path_for_position = increment_text.arg(sub_menu_index);
            styleSheet.replace(image_path_for_position, picturePath);

            qDebug() << "Set up sub menu for item: " << sub_menu_index + 1 << " which is option: " << option_index + 1 << " which has pnumber; " << p_page_idle->thisMachine->getProductFromMenuOption(option_index + 1)->getPNumber();

            pushButtons_dispense_product[sub_menu_index]->setProperty("class", "pushButton_base_product");
            pushButtons_dispense_product[sub_menu_index]->setStyleSheet(styleSheet);
            labels_dispense_product_picture[sub_menu_index]->setProperty("class", "label_base_product_picture");
            labels_dispense_product_picture[sub_menu_index]->setStyleSheet(styleSheet);

            QString dispense_product_name = p_page_idle->thisMachine->getProductFromMenuOption(option_index + 1)->getProductName();
            labels_dispense_product_name[sub_menu_index]->setProperty("class", "label_dispense_product_name");
            labels_dispense_product_name[sub_menu_index]->setStyleSheet(styleSheet);
            labels_dispense_product_name[sub_menu_index]->setText(dispense_product_name);

            pushButtons_dispense_product[sub_menu_index]->show();
            labels_dispense_product_picture[sub_menu_index]->show();
            labels_dispense_product_name[sub_menu_index]->show();
            labels_product_overlay_text[sub_menu_index]->show();

            QString unavailable_increment_text = "label_overlay_unavailable_increment%1";
            QString label_overlay_unavailable_increment = unavailable_increment_text.arg(sub_menu_index);

            p_page_idle->thisMachine->setSelectedProductByOption(option_index+1);
            if (!p_page_idle->thisMachine->getSelectedSlot()->getIsSlotEnabled()){ //slot disabled = all products disabled as well
                p_page_idle->thisMachine->addCssClassToObject(labels_product_overlay_text[sub_menu_index], label_overlay_unavailable_increment, PAGE_PRODUCT_MENU_CSS);
            }else{
                p_page_idle->thisMachine->addCssClassToObject(labels_product_overlay_text[sub_menu_index], "label_product_overlay_available", PAGE_PRODUCT_MENU_CSS);
                if (!p_page_idle->thisMachine->getIsOptionAvailable(option_index + 1)) // if slot is available, check if product option is available 
                {
                    p_page_idle->thisMachine->addCssClassToObject(labels_product_overlay_text[sub_menu_index], label_overlay_unavailable_increment, PAGE_PRODUCT_MENU_CSS);
                }
            }
        }
        else
        {
            qDebug() << "Set up sub menu for item: " << sub_menu_index + 1 << "Invalid pnumber product. Will hide option. ";
            pushButtons_dispense_product[sub_menu_index]->hide();
            labels_dispense_product_picture[sub_menu_index]->hide();
            labels_dispense_product_name[sub_menu_index]->hide();
            labels_product_overlay_text[sub_menu_index]->hide();
        }
        labels_product_overlay_text[sub_menu_index]->raise();
        pushButtons_dispense_product[sub_menu_index]->raise();

        // QString available_increment_text = "label_product_overlay_available_with_status%1";
        // QString label_overlay_available_with_status_increment = available_increment_text.arg(sub_menu_index);
        // product_status_text = p_page_idle->thisMachine->getProductFromMenuOption(option_index + 1)->getProductStateAsString();

        ProductState base_product_state = p_page_idle->thisMachine->getProductFromMenuOption(option_index + 1)->getProductState();
        switch (base_product_state)
        {
        case (PRODUCT_STATE_AVAILABLE):
        {
            labels_product_overlay_text[sub_menu_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->available"));
            break;
        }
        case (PRODUCT_STATE_AVAILABLE_LOW_STOCK):
        {
             labels_product_overlay_text[sub_menu_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->almost_empty"));
            break;
        }
        case (PRODUCT_STATE_PROBLEM_EMPTY):
        {
            labels_product_overlay_text[sub_menu_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->assistance"));
            break;
        }
        case (PRODUCT_STATE_DISABLED):
        {
            labels_product_overlay_text[sub_menu_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->not_enabled"));
            // p_page_idle->thisMachine->addCssClassToObject(labels_product_overlay_text[sub_menu_index], label_overlay_available_with_status_increment, PAGE_PRODUCT_MENU_CSS);
            break;
        }
        case (PRODUCT_STATE_INVALID):
        {
            labels_product_overlay_text[sub_menu_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->default"));
            break;
        }
        default:
        {
            labels_product_overlay_text[sub_menu_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->default"));
            break;
        }
        }
        if(!p_page_idle->thisMachine->getSelectedSlot()->getIsSlotEnabled()) {
            labels_product_overlay_text[sub_menu_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->not_enabled"));
        }
        // if (product_status_text.compare("SLOT_STATE_DISABLED_COMING_SOON") == 0)
        // {
        // }
        // else if (product_status_text.compare("SLOT_STATE_PROBLEM_NEEDS_ATTENTION") == 0)
        // {
        //     labels_product_overlay_text[sub_menu_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->assistance"));
        //     p_page_idle->thisMachine->addCssClassToObject(labels_product_overlay_text[sub_menu_index], label_overlay_available_with_status_increment, PAGE_PRODUCT_MENU_CSS);
        // }
        // else if (product_status_text.compare("SLOT_STATE_DISABLED") == 0)
        // {
        //     labels_product_overlay_text[sub_menu_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->not_enabled"));
        // }
        // else if (!p_page_idle->thisMachine->getIsOptionAvailable(option_index + 1))
        // {
        //     labels_product_overlay_text[sub_menu_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->not_enabled"));
        // }
        // else if (!(p_page_idle->thisMachine->isProductVolumeInContainer(p_page_idle->thisMachine->getProductFromMenuOption(option_index + 1)->getPNumber())))
        // {
        // }
        // else if (product_status_text.compare("SLOT_STATE_AVAILABLE") == 0)
        // {
        // }
        // else if (product_status_text.compare("SLOT_STATE_AVAILABLE_LOW_STOCK") == 0)
        // {
           
        // }
        // else if (product_status_text.compare("SLOT_STATE_PROBLEM_EMPTY") == 0)
        // {
        //     labels_product_overlay_text[sub_menu_index]->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->empty"));
        // }
        // else
        // {
        // }
    }
}

void page_product_menu::select_base_product_in_menu(int base_product_index)
{
    _productPageTimeoutSec = 60;
    m_selectedBaseProductIndex = base_product_index;
    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_PRODUCT_MENU_CSS);

    for (int slot_index = 0; slot_index < p_page_idle->thisMachine->getSlotCount(); slot_index++)
    {
        QString activity;
        if (slot_index == m_selectedBaseProductIndex)
        {
            activity = "active";
        }
        else
        {
            activity = "inactive";
        }
        p_page_idle->thisMachine->addCssClassToObject(labels_base_product_bg[slot_index], activity, PAGE_PRODUCT_MENU_CSS);
    }
    p_page_idle->thisMachine->setSelectedSlot(base_product_index+1);
    displayDispenseProductsMenu();
}

void page_product_menu::select_submenu_option(int position)
{
    int option = m_selectedBaseProductIndex * DISPENSE_PRODUCTS_PER_BASE_LINE_MAX + position;
    if(p_page_idle->thisMachine->getSelectedSlot()->getIsSlotEnabled()){
        if (p_page_idle->thisMachine->getIsOptionAvailable(option)) // if slot is available, check if product option is available 
        {
            p_page_idle->thisMachine->setSelectedProductByOption(option);
            p_page_idle->thisMachine->setSelectedSlotFromSelectedProduct();
            p_page_idle->thisMachine->getSelectedProduct()->resetCustomMixRatioParameters();
            p_page_idle->thisMachine->hasMixing() ? hideCurrentPageAndShowProvided(p_page_product_mixing) : hideCurrentPageAndShowProvided(p_page_product);
        }else{
            qDebug() << "Invalid choice. Option not available: " << option;
        }
    }else{
        qDebug() << "Invalid choice. Option not available: " << option;
    }
    // if (p_page_idle->thisMachine->getIsOptionAvailable(option))
    // {
    //     p_page_idle->thisMachine->setSelectedProductByOption(option);
    //     p_page_idle->thisMachine->setSelectedSlotFromSelectedProduct();
    //     p_page_idle->thisMachine->getSelectedProduct()->resetCustomMixRatioParameters();
    //     p_page_idle->thisMachine->hasMixing() ? hideCurrentPageAndShowProvided(p_page_product_mixing) : hideCurrentPageAndShowProvided(p_page_product);
    // }
    // else
    // {
    //     qDebug() << "Invalid choice. Option not available: " << option;
    // }
}

void page_product_menu::on_pushButton_base_product_1_clicked()
{
    select_base_product_in_menu(0);
}
void page_product_menu::on_pushButton_base_product_2_clicked()
{
    select_base_product_in_menu(1);
}
void page_product_menu::on_pushButton_base_product_3_clicked()
{
    select_base_product_in_menu(2);
}
void page_product_menu::on_pushButton_base_product_4_clicked()
{
    select_base_product_in_menu(3);
}

void page_product_menu::onProductPageTimeoutTick()
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

void page_product_menu::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    productPageEndTimer->stop();
    qDebug() << "Exit select product page.";
    this->raise();
    statusbarLayout->removeWidget(p_statusbar);
    p_page_idle->thisMachine->pageTransition(this, pageToShow);
}

void page_product_menu::on_pushButton_to_idle_clicked()
{
    qDebug() << "Back to Idle Page Button pressed";
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_product_menu::on_pushButton_help_page_clicked()
{
    qDebug() << "Help_Button pressed";
    hideCurrentPageAndShowProvided(p_page_help);
}

void page_product_menu::on_pushButton_dispense_product_1_clicked()
{
    select_submenu_option(1);
}

void page_product_menu::on_pushButton_dispense_product_2_clicked()
{

    select_submenu_option(2);
}

void page_product_menu::on_pushButton_dispense_product_3_clicked()
{

    select_submenu_option(3);
}

void page_product_menu::on_pushButton_dispense_product_4_clicked()
{

    select_submenu_option(4);
}

void page_product_menu::on_pushButton_dispense_product_5_clicked()
{
    select_submenu_option(5);
}

void page_product_menu::on_pushButton_dispense_product_6_clicked()
{
    select_submenu_option(6);
}
