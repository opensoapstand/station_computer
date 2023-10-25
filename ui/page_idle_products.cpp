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
// #include "page_maintenance.h"
// #include "page_maintenance_general.h"
#include "page_idle.h"
#include "page_select_product.h"
// #include "page_product.h"

// #include <QMediaPlayer>
// #include <QGraphicsVideoItem>
// #include <QMainWindow>
#include <QtWidgets>
// #include <QtMultimediaWidgets>
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
}

/*
 * Page Tracking reference
 */
void page_idle_products::setPage(page_idle *pageIdle, page_select_product *page_select_product)
{
    this->p_page_idle = pageIdle;
    this->p_page_select_product = page_select_product;
}

// DTOR
page_idle_products::~page_idle_products()
{
    delete ui;
}

void page_idle_products::showEvent(QShowEvent *event)
{
    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget

    _backgroundChangeTimeLeftTenthsOfSec = PAGE_IDLE_PRODUCTS_MAIN_PAGE_DISPLAY_TIME_SECONDS * 10;
    backgroundChangeTimer->start();
    active_background_index = 0;

    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_title);

    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_IDLE_PRODUCTS_CSS);
    ui->pushButton_to_select_product_page->setStyleSheet(styleSheet);
    ui->label_title->setStyleSheet(styleSheet);

    for (int slot_index = 0; slot_index < p_page_idle->thisMachine->getSlotCount(); slot_index++)
    {
        labels_product_picture[slot_index]->setProperty("class", "labels_product_picture");
        labels_product_type[slot_index]->setProperty("class", "labels_product_type");
        labels_selectProductOverlay[slot_index]->setProperty("class", "labels_selectProductOverlay"); // seems to do nothing
        labels_product_picture[slot_index]->setStyleSheet(styleSheet);
        labels_selectProductOverlay[slot_index]->setStyleSheet(styleSheet);
        labels_product_type[slot_index]->setStyleSheet(styleSheet);
    }

    ui->label_printer_status->setStyleSheet(styleSheet);

    // we already checked this in p_page_idle the results from there.
    // if (p_page_idle->thisMachine->hasReceiptPrinter())
    // {
    //     p_page_idle->checkReceiptPrinterStatus();
    // }

    p_page_idle->thisMachine->addClientLogoToLabel(ui->label_client_logo);

    changeBackground();
    displayProducts();
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

    for (uint8_t slot_index = 0; slot_index < p_page_idle->thisMachine->getSlotCount(); slot_index++)
    {
        // display product picture
        p_page_idle->thisMachine->addPictureToLabel(labels_product_picture[slot_index], p_page_idle->thisMachine->getProduct(slot_index + 1)->getProductPicturePath());
        product_slot_enabled = p_page_idle->thisMachine->getSlotEnabled(slot_index + 1);
        product_status_text = p_page_idle->thisMachine->getStatusText(slot_index + 1);

        product_type = p_page_idle->thisMachine->getProduct(slot_index + 1)->getProductType();
        product_name = p_page_idle->thisMachine->getProduct(slot_index + 1)->getProductName();

        labels_selectProductOverlay[slot_index]->raise();
        labels_product_overlay_text[slot_index]->raise();

        labels_selectProductOverlay[slot_index]->setText("");
        labels_product_overlay_text[slot_index]->setText("");
        labels_product_type[slot_index]->setText(product_type);
    }

    // overlay product status
    ui->pushButton_to_select_product_page->show();

    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_IDLE_PRODUCTS_CSS);
    ui->label_printer_status->setStyleSheet(styleSheet);
}

void page_idle_products::hideCurrentPageAndShowProvided(QWidget *pageToShow, bool createNewSessionId)
{
    if (createNewSessionId)
    {
        // the moment there is a user interaction to go to select product , a new session ID is created.
        p_page_idle->thisMachine->createSessionId();
    }

    backgroundChangeTimer->stop();

    p_page_idle->thisMachine->pageTransition(this, pageToShow);
}

void page_idle_products::showAllLabelsAndButtons()
{
    ui->label_title->show();
    ui->label_client_logo->show();

    displayPrinterStatus();
    ui->pushButton_to_select_product_page->show();

    for (int slot_index = 0; slot_index < p_page_idle->thisMachine->getSlotCount(); slot_index++)
    {
        labels_selectProductOverlay[slot_index]->show(); // seems to do nothing
        labels_product_picture[slot_index]->show();
        labels_product_type[slot_index]->show();
    }
}

void page_idle_products::hideAllLabelAndButtons()
{
    ui->label_title->hide();
    ui->label_client_logo->hide();
    ui->label_printer_status->hide();

    for (int slot_index = 0; slot_index < p_page_idle->thisMachine->getSlotCount(); slot_index++)
    {
        labels_selectProductOverlay[slot_index]->hide(); // seems to do nothing
        labels_product_picture[slot_index]->hide();
        labels_product_type[slot_index]->hide();
    }
}

// void page_idle_products::printerStatusFeedback(bool isOnline, bool hasPaper)
// {
//     qDebug() << "Printer feedback received from fsm";
//     m_printer_isOnline = isOnline;
//     m_printer_hasPaper = hasPaper;
// }

void page_idle_products::displayPrinterStatus()
{
    bool isOnline;
    bool hasPaper;
    p_page_idle->thisMachine->getPrinterStatusFromDb(&isOnline, &hasPaper);

    ui->label_printer_status->hide();
    if (p_page_idle->thisMachine->hasReceiptPrinter())
    {
        if (!isOnline)
        {
            ui->label_printer_status->raise();
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_printer_status, "assistance_printer_offline");
            ui->label_printer_status->show();
        }
        else if (!hasPaper)
        {
            ui->label_printer_status->raise();
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_printer_status, "empty_improperly_loaded");
            ui->label_printer_status->show();
        }
    }
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
        // qDebug() << "The string contains the sequence.";
    }
    else
    {
        // qDebug() << "The string does not contain the sequence.";
        _backgroundChangeTimeLeftTenthsOfSec = defaultTimeMillis / 100; // PAGE_IDLE_PRODUCTS_STEP_DISPLAY_TIME_SECONDS;
    }
}

void page_idle_products::changeBackground()
{
    QStringList all_files_in_template_folder = df_util::getFileList(p_page_idle->thisMachine->getTemplateFolder());

    QString template_name = "background_idle_products_%1_"; // background_idle_products_X_Yms.png  --> where X is a sequence number and Y is a display time in ms.
    QString filterPattern = template_name.arg(active_background_index);
    QStringList filteredList = all_files_in_template_folder.filter(filterPattern, Qt::CaseSensitive);

    if (filteredList.count() == 0)
    {
        // at end of sequence, there is no image found with name. (if there is no image at all, this will also be triggered)
        active_background_index = 0;
        filterPattern = template_name.arg(active_background_index);
        filteredList = all_files_in_template_folder.filter(filterPattern, Qt::CaseSensitive);
    }

    if (filteredList.count() == 0)
    {
        // If there is no matching image at all (not even for position zero), set to default.
        p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_IDLE_PRODUCTS_BACKGROUND_PATH);
        showAllLabelsAndButtons();
        _backgroundChangeTimeLeftTenthsOfSec = PAGE_IDLE_PRODUCTS_MAIN_PAGE_DISPLAY_TIME_SECONDS * 10; // PAGE_IDLE_PRODUCTS_STEP_DISPLAY_TIME_SECONDS;
    }
    else if (filteredList.count() >= 1)
    {
        QString background_name = filteredList[0];
        if (active_background_index == 0)
        {
            setStepTimerFromFileName(background_name, PAGE_IDLE_PRODUCTS_MAIN_PAGE_DISPLAY_TIME_SECONDS * 10);
            showAllLabelsAndButtons();
        }
        else
        {
            setStepTimerFromFileName(background_name, PAGE_IDLE_PRODUCTS_STEP_DISPLAY_TIME_SECONDS * 10);
            hideAllLabelAndButtons();
        }
        p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, background_name);
    }
    else
    {
        qDebug() << "ASSERT ERROR invalid list count: " << filteredList.count();
    }
}

void page_idle_products::onBackgroundChangeTimerTick()
{
    if (--_backgroundChangeTimeLeftTenthsOfSec >= 0)
    {
    }
    else
    {
        active_background_index++;
        changeBackground();
    }
}
void page_idle_products::on_pushButton_to_select_product_page_clicked()
{
    qDebug() << "To idle page press";
    this->hideCurrentPageAndShowProvided(p_page_select_product, true);
}
