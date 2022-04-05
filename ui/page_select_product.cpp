

//***************************************
//
// page_select_product.cpp
// GUI class for user to browse up to
// 6 drinks.
//
// Allows navigation to First product page
// payment selection page
//
// created: 16-07-2020
// by: Jason Wang
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
    // qDebug()<< "Constructor callk"<< endl;
    QPixmap background(PAGE_SELECT_PRODUCT_BACKGROUND_PATH);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    ui->p_page_maintenanceButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    selectProductButtons[0] = ui->selection1_Button;
    selectProductButtons[1] = ui->selection2_Button;
    selectProductButtons[2] = ui->selection3_Button;
    selectProductButtons[3] = ui->selection4_Button;

    selectProductPhotoLabels[0] = ui->product_1_photo_label;
    selectProductPhotoLabels[1] = ui->product_2_photo_label;
    selectProductPhotoLabels[2] = ui->product_3_photo_label;
    selectProductPhotoLabels[3] = ui->product_4_photo_label;

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

    ui->mainPage_Button->setStyleSheet("QPushButton { color:#003840;background-color: #FFFFFF; box-sizing: border-box;border-radius: 20px;}"); // border: 1px solid #3D6675;

    // font-family: 'Brevia';
    // font-style: normal;
    // font-weight: 700;
    // font-size: 24px;
    // line-height: 32px;
    // text-transform: uppercase;

    // color: #003840;

    QFont font;
    font.setFamily(QStringLiteral("Brevia"));
    font.setPointSize(20);
    font.setBold(true);
    font.setWeight(75);
    ui->mainPage_Button->setFont(font);
    ui->mainPage_Button->setText("<- HOME");

    productPageEndTimer = new QTimer(this);
    productPageEndTimer->setInterval(1000);
    connect(productPageEndTimer, SIGNAL(timeout()), this, SLOT(onProductPageTimeoutTick()));
}

/*
 * Page Tracking reference
 */
void page_select_product::setPage(pageProduct *pageSizeSelect, page_idle *pageIdle, page_maintenance *pageMaintenance, page_help *pageHelp)
{
    // this->selection_PageTwo = pageTwoProducts;
    this->p_page_product = pageSizeSelect;
    this->p_page_idle = pageIdle;
    this->p_page_maintenance = pageMaintenance;
    this->helpPage = pageHelp;
}

// DTOR
page_select_product::~page_select_product()
{
    delete ui;
}

// void page_select_product::on_nextPageButton_clicked()
//{
//     selection_PageTwo->showFullScreen();
//     this->hide();
// }

void page_select_product::displayProducts()
{

    QString product_type_icons[5] = {ICON_TYPE_CONCENTRATE_PATH, ICON_TYPE_ALL_PURPOSE_PATH, ICON_TYPE_DISH_PATH, ICON_TYPE_HAND_PATH, ICON_TYPE_LAUNDRY_PATH};
   
    
    for (uint8_t i = 0; i < SLOT_COUNT; i++)
    {
       
        selectProductButtons[i]->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
        selectProductButtons[i]->raise();
        p_page_idle->addPictureToLabel(selectProductPhotoLabels[i], p_page_idle->currentProductOrder->getProductPicturePath(i+1));
        selectProductPhotoLabels[i]->setStyleSheet("QLabel{border: 1px solid black;}");

        qDebug() << "db product details:";
        DbManager db(DB_PATH);
        QString product_type = db.getProductType(i + 1);
        db.closeDB();

        QString name = p_page_idle->currentProductOrder->getProductName(i + 1);


        selectProductNameLabels[i]->setText(name);
        selectProductNameLabels[i]->setStyleSheet("QLabel{font-family: 'Montserrat';font-style: normal;font-weight: 400;font-size: 28px;line-height: 36px;qproperty-alignment: AlignCenter;color: #003840;}");

        QString icon_path = "not found";
        if (product_type == "Dish")
        {
            icon_path = ICON_TYPE_DISH_PATH;
        }
        else if (product_type == "Hand")
        {
            icon_path = ICON_TYPE_HAND_PATH;
        }
        else if (product_type == "Laundry")
        {
            icon_path = ICON_TYPE_LAUNDRY_PATH;
        }
        else if (product_type == "All Purpose")
        {
            icon_path = ICON_TYPE_ALL_PURPOSE_PATH;
        }
        else if (product_type == "Contentrate")
        {
            icon_path = ICON_TYPE_CONCENTRATE_PATH;
        }
        else
        {
            icon_path = "Product/type/for/icon/not/found.aiaiai";
        }
        // switch (product_type){
        //     case (QString("dish")):{
        //         icon_path = ICON_TYPE_DISH_PATH;
        //         break;
        //     }
        //     default:
        //     {
        //         icon_path = ICON_TYPE_DISH_PATH;
        //         break;
        //     }
        // }

        p_page_idle->addPictureToLabel(selectProductIconLabels[i], icon_path);

        selectProductTypeLabels[i]->setText(product_type);
        selectProductTypeLabels[i]->setStyleSheet("QLabel{font-family: 'Brevia';font-style: normal;font-weight: 700;font-size: 30px;line-height: 41px;qproperty-alignment: AlignCenter;text-transform: uppercase;color: #5E8580;}");

        selectProductIconLabels[i]->setText(""); // icon should not display text.
    }
}

void page_select_product::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<<Page Enter: Select Product >>>>>>>>>";
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

    qDebug() << "db check if product is enabled";
    DbManager db(DB_PATH);
    for (uint8_t i = 0; i < SLOT_COUNT; i++)
    {
        if (!db.remainingVolumeIsBiggerThanLargestFixedSize(i + 1) || !db.getSlotEnabled(i + 1))
        {
            QString path = SOLD_OUT_IMAGE_PATH;
            selectProductButtons[i]->setStyleSheet("QPushButton { border-image: url(" + path + "); }");
        }
        else
        {
            selectProductButtons[i]->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
        }
    }
    db.closeDB();
}

void page_select_product::cancelTimers()
{
    productPageEndTimer->stop();
}

void page_select_product::select_product(int slot)
{
    qDebug() << "ahoyy16 (last call before freeze up?!)";

    DbManager db(DB_PATH);
    if (db.remainingVolumeIsBiggerThanLargestFixedSize(slot) && db.getSlotEnabled(slot))
    {
        qDebug() << "select product: " << slot;
        db.closeDB();
        productPageEndTimer->stop();
        p_page_idle->currentProductOrder->setSelectedSlot(slot);
        p_page_idle->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);
        // p_page_product->resizeEvent(productResize);
        p_page_product->showFullScreen();

        this->hide();
    }
    else
    {
        qDebug() << "not enabled product: " << slot;
        db.closeDB();
    }
}

// FIXME: This is terrible...no time to make array reference to hold button press functions
void page_select_product::on_selection1_Button_clicked()
{
    select_product(1);
}
void page_select_product::on_selection2_Button_clicked()
{
    select_product(2);
}
void page_select_product::on_selection3_Button_clicked()
{
    select_product(3);
}
void page_select_product::on_selection4_Button_clicked()
{
    select_product(4);
}

void page_select_product::onProductPageTimeoutTick()
{
    if (--_productPageTimeoutSec >= 0)
    {
        // qDebug() << "Tick Down: " << _productPageTimeoutSec << endl;
    }
    else
    {
        // qDebug() << "Timer Done!" << _productPageTimeoutSec << endl;
        mainPage();
    }
}

void page_select_product::mainPage()
{
    productPageEndTimer->stop();
    p_page_idle->showFullScreen();
    this->hide();
}

void page_select_product::on_p_page_maintenanceButton_pressed()
{
    maintenanceCounter++;
    if (maintenanceCounter > 15)
    {
        productPageEndTimer->stop();
        p_page_maintenance->showFullScreen();
        this->hide();
    }
}

void page_select_product::on_mainPage_Button_clicked()
{
    qDebug() << "Back to Main Page Button pressed" << endl;
    mainPage();
}

void page_select_product::on_helpPage_Button_clicked()
{
    productPageEndTimer->stop();
    helpPage->showFullScreen();
    this->hide();
}
