

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

    selectProductButtons[0] = ui->pushButton_selection1;
    selectProductButtons[1] = ui->pushButton_selection2;
    selectProductButtons[2] = ui->pushButton_selection3;
    selectProductButtons[3] = ui->pushButton_selection4;

    selectProductPhotoLabels[0] = ui->label_product_1_photo;
    selectProductPhotoLabels[1] = ui->label_product_2_photo;
    selectProductPhotoLabels[2] = ui->label_product_3_photo;
    selectProductPhotoLabels[3] = ui->label_product_4_photo;

    // selectProductOverlayLabels[0] = ui->label_product_1_overlay;
    // selectProductOverlayLabels[1] = ui->label_product_2_overlay;
    // selectProductOverlayLabels[2] = ui->label_product_3_overlay;
    // selectProductOverlayLabels[3] = ui->label_product_4_overlay;

    selectProductPhotoLabelsText[0] = ui->label_product_1_photo_text;
    selectProductPhotoLabelsText[1] = ui->label_product_2_photo_text;
    selectProductPhotoLabelsText[2] = ui->label_product_3_photo_text;
    selectProductPhotoLabelsText[3] = ui->label_product_4_photo_text;

    selectProductNameLabels[0] = ui->label_product_1_name;
    selectProductNameLabels[1] = ui->label_product_2_name;
    selectProductNameLabels[2] = ui->label_product_3_name;
    selectProductNameLabels[3] = ui->label_product_4_name;

    selectProductIconLabels[0] = ui->label_product_1_icon;
    selectProductIconLabels[1] = ui->label_product_2_icon;
    selectProductIconLabels[2] = ui->label_product_3_icon;
    selectProductIconLabels[3] = ui->label_product_4_icon;

    selectProductTypeLabels[0] = ui->label_product_1_type;
    selectProductTypeLabels[1] = ui->label_product_2_type;
    selectProductTypeLabels[2] = ui->label_product_3_type;
    selectProductTypeLabels[3] = ui->label_product_4_type;

    // ui->pushButton_to_idle->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
       ui->label_pick_soap->setText("Pick your soap");

    // ui->pushButton_to_idle->setStyleSheet(
    //     "QPushButton {"

    // "background-color: #3498db;"
    // "border: 2px solid #2980b9;"
    // "border-radius: 5px;"
    // "color: #fff;"
    // "font-size: 16px;"
    // "padding: 10px 20px;"
    //     "}");
    
    ui->pushButton_to_idle->setText("<-back");

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
void page_select_product::setPage(pageProduct *pageSizeSelect, page_idle_products *p_page_idle_products, page_idle *pageIdle, page_maintenance *pageMaintenance, page_help *pageHelp)
{
    // this->selection_PageTwo = pageTwoProducts;
    this->p_page_product = pageSizeSelect;
    this->p_page_idle = pageIdle;
    this->p_page_maintenance = pageMaintenance;
    this->p_page_help = pageHelp;

    selectedProductOrder = p_page_idle->currentProductOrder;

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
    QString styleSheet = p_page_idle->getCSS(PAGE_SELECT_PRODUCT_CSS);
    ui->p_page_maintenanceButton->setStyleSheet(styleSheet); 
    ui->pushButton_help_page->setStyleSheet(styleSheet); 
    ui->pushButton_to_idle->setStyleSheet(styleSheet);
    ui->label_pick_soap->setStyleSheet(styleSheet);

    ui->label_product_1_name->setProperty("class", "label_product_name");
    ui->label_product_2_name->setProperty("class", "label_product_name");
    ui->label_product_3_name->setProperty("class", "label_product_name");
    ui->label_product_4_name->setProperty("class", "label_product_name");
    ui->label_product_1_name->setStyleSheet(styleSheet);
    ui->label_product_2_name->setStyleSheet(styleSheet);
    ui->label_product_3_name->setStyleSheet(styleSheet);
    ui->label_product_4_name->setStyleSheet(styleSheet);

    ui->pushButton_selection1->setProperty("class", "PushButton_selection");
    ui->pushButton_selection2->setProperty("class", "PushButton_selection");
    ui->pushButton_selection3->setProperty("class", "PushButton_selection");
    ui->pushButton_selection4->setProperty("class", "PushButton_selection");
    ui->pushButton_selection1->setStyleSheet(styleSheet);
    ui->pushButton_selection2->setStyleSheet(styleSheet);
    ui->pushButton_selection3->setStyleSheet(styleSheet);
    ui->pushButton_selection4->setStyleSheet(styleSheet);

    // ui->label_product_1_overlay->setStyleSheet(styleSheet);
    // ui->label_product_2_overlay->setStyleSheet(styleSheet);
    // ui->label_product_3_overlay->setStyleSheet(styleSheet);
    // ui->label_product_4_overlay->setStyleSheet(styleSheet);
    
    
    ui->label_product_1_type->setProperty("class", "label_product_type");
    ui->label_product_2_type->setProperty("class", "label_product_type");
    ui->label_product_3_type->setProperty("class", "label_product_type");
    ui->label_product_4_type->setProperty("class", "label_product_type");
    ui->label_product_1_type->setStyleSheet(styleSheet);
    ui->label_product_2_type->setStyleSheet(styleSheet);
    ui->label_product_3_type->setStyleSheet(styleSheet);
    ui->label_product_4_type->setStyleSheet(styleSheet);

    ui->label_product_1_photo->setProperty("class", "label_product_photo");
    ui->label_product_2_photo->setProperty("class", "label_product_photo");
    ui->label_product_3_photo->setProperty("class", "label_product_photo");
    ui->label_product_4_photo->setProperty("class", "label_product_photo");
    ui->label_product_1_photo->setStyleSheet(styleSheet);
    ui->label_product_2_photo->setStyleSheet(styleSheet);
    ui->label_product_3_photo->setStyleSheet(styleSheet);
    ui->label_product_4_photo->setStyleSheet(styleSheet);

////
//
//
//
//
//
//
//
//
//
//
//
//
//

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
void page_select_product::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void page_select_product::displayProducts()
{
    QString product_type_icons[5] = {ICON_TYPE_CONCENTRATE_PATH, ICON_TYPE_ALL_PURPOSE_PATH, ICON_TYPE_DISH_PATH, ICON_TYPE_HAND_PATH, ICON_TYPE_LAUNDRY_PATH};

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
        
        //selectProductPhotoLabels[i]->setStyleSheet("border: 1px solid black;");
        //selectProductPhotoLabels[i]->setStyleSheet(styleSheet);
        p_page_idle->addPictureToLabel(selectProductPhotoLabels[i], p_page_idle->currentProductOrder->getProductPicturePath(slot));

        qDebug() << "db (re)load product details:";
        DbManager db(DB_PATH);

        product_slot_enabled = db.getSlotEnabled(slot);

        product_status_text = db.getStatusText(slot);
        double remaining_volume = db.getVolumeRemaining(slot);

        bool set_to_sold_out_below_threshold = db.getEmptyContainerDetectionEnabled();

        db.closeDB();

        product_type = p_page_idle->currentProductOrder->getProductType(slot);
        product_name = p_page_idle->currentProductOrder->getProductName(slot);

        qDebug() << "Product: " << product_type << "At slot: " << slot << ", enabled: " << product_slot_enabled << " Status text: " << product_status_text;

        selectProductNameLabels[i]->setText(product_name);
        //selectProductNameLabels[i]->setStyleSheet("QLabel{font-family: 'Montserrat';font-style: normal;font-weight: 400;font-size: 28px;line-height: 36px;qproperty-alignment: AlignCenter;color: #003840;}");
        //selectProductNameLabels[i]->setStyleSheet(styleSheet);


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
            qDebug() << "Product type not found for UI text and icon. Is the slot type set correctly in the products file? : " << type_text;
        }
        QString icon_path_with_template = p_page_idle->getTemplatePathFromName(icon_path);

        p_page_idle->addPictureToLabel(selectProductIconLabels[i], icon_path_with_template);
        selectProductIconLabels[i]->setText(""); // icon should not display text.

        //selectProductButtons[i]->setStyleSheet(styleSheet); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
        //selectProductButtons[i]->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
        // selectProductButtons[i]->setStyleSheet("QPushButton{ background-color: 0x44881188; border: 2px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
        // selectProductOverlayLabels[i]->raise();
        selectProductIconLabels[i]->raise();
        selectProductPhotoLabelsText[i]->raise();
        selectProductButtons[i]->raise();

        // selectProductOverlayLabels[i]->setText("");

        // overlay product status

        if (!product_slot_enabled)
        {
             selectProductPhotoLabelsText[i]->setProperty("class", "label_product_oberlay_sold_out");
        }
        else
        {

            selectProductPhotoLabelsText[i]->setProperty("class", "label_product_oberlay_available");
        }

        

        if (product_status_text.compare("DISPENSER_STATE_AVAILABLE") == 0)
        {
            selectProductPhotoLabelsText[i]->setText("");
// <<<<<<< HEAD
//             ui->label_product_1_overlay->setProperty("class", "label_product_oberlay_sold_out");
//             ui->label_product_2_overlay->setProperty("class", "label_product_oberlay_sold_out");
//             ui->label_product_3_overlay->setProperty("class", "label_product_oberlay_sold_out");
//             ui->label_product_4_overlay->setProperty("class", "label_product_oberlay_sold_out");
//             //selectProductOverlayLabels[i]->setStyleSheet("background-color: transparent;");
//             //// selectProductPhotoLabels[i]->setStyleSheet("Qlabel {background-color: rgba(255,255,255,0);}");
//             // selectProductButtons[i]->setStyleSheet("QPushButton {background-color: transparent; border: 0px }");
// =======
// >>>>>>> origin/SS1
        }
        else if (product_status_text.compare("DISPENSER_STATE_AVAILABLE_LOW_STOCK") == 0)
        {
            selectProductPhotoLabelsText[i]->setText("Almost Empty");
        }
        else if (product_status_text.compare("DISPENSER_STATE_PROBLEM_EMPTY") == 0)
        {
            selectProductPhotoLabelsText[i]->setText("Sold Out");
        }
        else if (product_status_text.compare("DISPENSER_STATE_DISABLED_COMING_SOON") == 0)
        {
            selectProductPhotoLabelsText[i]->setText("Coming Soon");
        }
        else if (product_status_text.compare("DISPENSER_STATE_PROBLEM_NEEDS_ATTENTION") == 0)

        {
            selectProductPhotoLabelsText[i]->setText("Assistance Needed");
        }
        else
        {
            selectProductPhotoLabelsText[i]->setText("Assistance Needed");
        }

        if (!(p_page_idle->currentProductOrder->isProductVolumeInContainer(slot))){
            selectProductPhotoLabelsText[i]->setText("Sold Out");
        }

        selectProductTypeLabels[i]->setText(type_text);
        //selectProductTypeLabels[i]->setStyleSheet("QLabel{font-family: 'Brevia';font-style: normal;font-weight: 700;font-size: 30px;line-height: 41px;qproperty-alignment: AlignCenter;text-transform: uppercase;color: #5E8580;}");
    }
}

void page_select_product::select_product(int slot)
{
    DbManager db(DB_PATH);

    bool product_slot_enabled = db.getSlotEnabled(slot);
    db.closeDB();

    if (product_slot_enabled)
    {
        qDebug() << "selected slot: " << slot;
        p_page_idle->currentProductOrder->setSelectedSlot(slot);
        hideCurrentPageAndShowProvided(p_page_product);
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
    selectedProductOrder->setDiscountPercentageFraction(0.0);
    this->raise();
    p_page_idle->pageTransition(this, pageToShow);
}

void page_select_product::on_pushButton_to_idle_clicked()
{
    qDebug() << "Back to Idle Page Button pressed";
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_select_product::on_pushButton_help_page_clicked()
{
    qDebug() << "Help_Button pressed";
    hideCurrentPageAndShowProvided(p_page_help);
}
