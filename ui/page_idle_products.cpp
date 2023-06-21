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
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado & Udbhav Kansal & Daniel C.
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
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
#include <QTimer>
#include <QObject>

//    #define PLAY_VIDEO
// CTOR
page_idle_products::page_idle_products(QWidget *parent) : QWidget(parent),
                                                          ui(new Ui::page_idle_products)
{
    ui->setupUi(this);

    backgroundChangeTimer = new QTimer(this);
    backgroundChangeTimer->setInterval(100); // interval 10th's of seconds.

    connect(backgroundChangeTimer, SIGNAL(timeout()), this, SLOT(onBackgroundChangeTimerTimeout()));
    connect(backgroundChangeTimer, SIGNAL(timeout()), this, SLOT(onBackgroundChangeTimerTick()));

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
    p_page_idle->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    _backgroundChangeTimeLeftTenthsOfSec = PAGE_IDLE_PRODUCTS_MAIN_PAGE_DISPLAY_TIME_SECONDS;
    backgroundChangeTimer->start();
    active_background_index = 0;

    p_page_idle->setTemplateTextToObject(ui->label_pick_soap);

    QString styleSheet = p_page_idle->getCSS(PAGE_IDLE_PRODUCTS_CSS);
    ui->pushButton_to_select_product_page->setStyleSheet(styleSheet);
    ui->label_pick_soap->setStyleSheet(styleSheet);

    for (int slot_index = 0; slot_index < SLOT_COUNT; slot_index++)
    {

        labels_product_picture[slot_index]->setProperty("class", "labels_product_picture");
        labels_product_type[slot_index]->setProperty("class", "labels_product_type");
        labels_selectProductOverlay[slot_index]->setProperty("class", "labels_selectProductOverlay"); // seems to do nothing
        labels_product_picture[slot_index]->setStyleSheet(styleSheet);
        labels_selectProductOverlay[slot_index]->setStyleSheet(styleSheet);
        labels_product_type[slot_index]->setStyleSheet(styleSheet);
    }

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

    p_page_idle->addCompanyLogoToLabel(ui->logo_label);

    ui->pushButton_to_select_product_page->raise();
    // method called to change backgrounds (if they exist)
    changeBackground();
    showAllLabelsAndButtons();
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
        // labels_product_type[slot_index]->setStyleSheet(styleSheet);
    }
}

void page_idle_products::select_product(int slot)
{
    qDebug() << "selected slot: " << slot;
    hideCurrentPageAndShowProvided(p_page_product);
}

void page_idle_products::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    // productPageEndTimer->stop();
    qDebug() << "Exit select product page.";
    // this->raise();
    p_page_idle->pageTransition(this, pageToShow);
    backgroundChangeTimer->stop();
}

void page_idle_products::showAllLabelsAndButtons()
{
    ui->label_pick_soap->show();
    ui->logo_label->show();
    ui->printer_status_label->show();
    ui->pushButton_to_select_product_page->show();

    for (int slot_index = 0; slot_index < SLOT_COUNT; slot_index++)
    {
        labels_product_picture[slot_index]->show();
        labels_selectProductOverlay[slot_index]->show(); // seems to do nothing
        labels_product_picture[slot_index]->show();
        labels_selectProductOverlay[slot_index]->show();
        labels_product_type[slot_index]->show();
    }
}

void page_idle_products::hideAllLabelAndButtons()
{
    ui->label_pick_soap->hide();
    ui->logo_label->hide();
    ui->printer_status_label->hide();
    // ui->pushButton_to_select_product_page->hide();

    for (int slot_index = 0; slot_index < SLOT_COUNT; slot_index++)
    {
        labels_product_picture[slot_index]->hide();
        labels_selectProductOverlay[slot_index]->hide(); // seems to do nothing
        labels_product_picture[slot_index]->hide();
        labels_selectProductOverlay[slot_index]->hide();
        labels_product_type[slot_index]->hide();
    }
}

void page_idle_products::printerStatusFeedback(bool isOnline, bool hasPaper)
{
    qDebug() << "Printer feedback received from fsm";

    if (!isOnline)
    {
        ui->printer_status_label->raise();

        p_page_idle->setTemplateTextWithIdentifierToObject(ui->printer_status_label, "assistance_printer_offline");
        ui->printer_status_label->show();
    }
    else if (!hasPaper)
    {
        ui->printer_status_label->raise();
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
}

int page_idle_products::setStepTimerFromFileName(QString fileName, int defaultTimeMillis)
{
    // there should be only one file withheld. But, if more than one, we'll take the first element.
    QString background_name = fileName;
    QStringList name_split = background_name.split("_");
    QString lastPart = name_split[name_split.size() - 1];
    QStringList parts = lastPart.split(".");
    QString timePart = parts[0];

    QString timeIdentifier = "ms";
    if (timePart.contains(timeIdentifier))
    {
        int time_millis = timePart.replace(timeIdentifier, "").toInt();
        _backgroundChangeTimeLeftTenthsOfSec = time_millis / 100;
        qDebug() << "The string contains the sequence.";
    }
    else
    {
        qDebug() << "The string does not contain the sequence.";
        _backgroundChangeTimeLeftTenthsOfSec = defaultTimeMillis / 100; // PAGE_IDLE_PRODUCTS_STEP_DISPLAY_TIME_SECONDS;
    }
}

void page_idle_products::changeBackground()
{
    // QString base_path = "/home/df-admin/production/references/templates/default/";
    QStringList all_files_in_template_folder = df_util::getFileList(p_page_idle->thisMachine.getTemplateFolder());

    QString template_name = "background_idle_products_%1";
    QString filterPattern = template_name.arg(active_background_index);
    QStringList filteredList = all_files_in_template_folder.filter(filterPattern, Qt::CaseSensitive);

    if (filteredList.count() == 0)
    {
        qDebug() << "active_background_index set to 0. was: " << active_background_index;
        // automatically revert to first image if non found.
        active_background_index = 0;
        filterPattern = template_name.arg(active_background_index);
        filteredList = all_files_in_template_folder.filter(filterPattern, Qt::CaseSensitive);
    }

    if (filteredList.count() == 0)
    {
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_IDLE_PRODUCTS_BACKGROUND_PATH);
        showAllLabelsAndButtons();
        _backgroundChangeTimeLeftTenthsOfSec = PAGE_IDLE_PRODUCTS_MAIN_PAGE_DISPLAY_TIME_SECONDS * 10; // PAGE_IDLE_PRODUCTS_STEP_DISPLAY_TIME_SECONDS;
    }
    else if (filteredList.count() >= 1)
    {
        qDebug() << "background  " << filterPattern << "found.";
        QString background_name = filteredList[0];
        if (active_background_index == 0)
        {
            qDebug() << "Main page (index 0 )";

            setStepTimerFromFileName(background_name, PAGE_IDLE_PRODUCTS_MAIN_PAGE_DISPLAY_TIME_SECONDS * 10);
            showAllLabelsAndButtons();
        }
        else
        {
            qDebug() << "normal page. index: " << active_background_index;

            setStepTimerFromFileName(background_name, PAGE_IDLE_PRODUCTS_STEP_DISPLAY_TIME_SECONDS * 10);
            hideAllLabelAndButtons();
        }
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, background_name);
    }
    else
    {
        qDebug() << "ASSERT ERROR.aeijies" << filteredList.count();
    }
}

// void page_idle_products::changeBackground()
// {

//     if (active_background_index == 0)
//     {

//         p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_IDLE_PRODUCTS_BACKGROUND_PATH);
//         return;
//     }
//     QString fileName = "background%1.png";
//     QString background_name = fileName.arg(active_background_index); // "background" + active_background_index + ".png"
//     QString background_path = p_page_idle->thisMachine.getTemplatePathFromName(background_name);

//     if (df_util::pathExists(background_path))
//     {
//         hideAllLabelAndButtons();
//         p_page_idle->setBackgroundPictureFromTemplateToPage(this, background_name);
//         _backgroundChangeTimeLeftTenthsOfSec = PAGE_IDLE_PRODUCTS_STEP_DISPLAY_TIME_SECONDS;
//     }
//     else
//     {
//         _backgroundChangeTimeLeftTenthsOfSec = PAGE_IDLE_PRODUCTS_MAIN_PAGE_DISPLAY_TIME_SECONDS;
//         active_background_index = 0;
//         p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_IDLE_PRODUCTS_BACKGROUND_PATH);
//         showAllLabelsAndButtons();
//     }
//
// }

void page_idle_products::onBackgroundChangeTimerTick()
{
    if (--_backgroundChangeTimeLeftTenthsOfSec >= 0)
    {
        // qDebug() << "one sec";
    }
    else
    {
        // qDebug() << "timer elapsed";
        active_background_index++;
        changeBackground();
    }
}

void page_idle_products::onBackgroundChangeTimerTimeout()
{
    // Stop the timer
    // backgroundChangeTimer->stop();
    // qDebug() << "Background change timer";
    // changeBackground();
}

void page_idle_products::on_pushButton_to_select_product_page_clicked()
{
    this->hideCurrentPageAndShowProvided(p_pageSelectProduct);
}
