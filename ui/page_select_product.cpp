

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
    QPixmap background("/home/df-admin/production/references/2_background_products.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    ui->p_page_maintenanceButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    selectProductButtons[0] = ui->selection1_Button;
    selectProductButtons[1] = ui->selection2_Button;
    selectProductButtons[2] = ui->selection3_Button;
    selectProductButtons[3] = ui->selection4_Button;

    for (uint8_t i = 0; i < 4; i++)
    {
        selectProductButtons[i]->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    }

    
    ui->helpPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");          // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    
    ui->mainPage_Button->setStyleSheet("QPushButton { color:#003840;background-color: #FFFFFF; box-sizing: border-box;border-radius: 20px;}"); //border: 1px solid #3D6675;


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
    this->paymentSelectPage = pageSizeSelect;
    this->idlePage = pageIdle;
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

void page_select_product::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    maintenanceCounter = 0;

    if (productPageEndTimer == nullptr)
    {
        productPageEndTimer = new QTimer(this);
        productPageEndTimer->setInterval(1000);
        connect(productPageEndTimer, SIGNAL(timeout()), this, SLOT(onProductPageTimeoutTick()));
    }

    productPageEndTimer->start(1000);
    _productPageTimeoutSec = 15;

    // qDebug() << "start s---- ======= "<< endl;
    // qDebug() << db.remainingVolumeIsBiggerThanLargestFixedSize(1) << "check levels" <<endl;

    // qDebug() << db.getPWM(1) << endl;
    // qDebug() << "tetseitsejt ======= "<< endl;

    // if (!db.remainingVolumeIsBiggerThanLargestFixedSize(1)  || !this->idlePage->isSlotAvailable(1) ){

    qDebug() << "ahoyy15";
    DbManager db(DB_PATH);
    for (uint8_t i = 0; i < 4; i++)
    {
        if (!db.remainingVolumeIsBiggerThanLargestFixedSize(i + 1) || !db.getSlotEnabled(i + 1))
        {
            ui->selection1_Button->setStyleSheet("QPushButton { border-image: url(/home/df-admin/production/references/soldOut.png); }");
        }
        else
        {
            ui->selection1_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
        }
    }
    db.closeDB();

    //     if (!db.remainingVolumeIsBiggerThanLargestFixedSize(1)  || !db.getSlotEnabled(1) ){
    //         // Change p1 to Sold Out
    // //        qDebug() << "Product 1 is Sold Out!" << endl;
    //         ui->selection1_Button->setStyleSheet("QPushButton { border-image: url(/home/df-admin/production/references/soldOut.png); }");
    //     }
    //     else{
    //         ui->selection1_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    //     }

    //     if (!db.remainingVolumeIsBiggerThanLargestFixedSize(2)   || !db.getSlotEnabled(2) ){
    //         // Change p2 to Sold Out
    // //        qDebug() << "Product 2 is Sold Out!" << endl;
    //         ui->selection2_Button->setStyleSheet("QPushButton { border-image: url(/home/df-admin/production/references/soldOut.png); }");
    //     }
    //     else{
    //         ui->selection2_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    //     }

    //     if (!db.remainingVolumeIsBiggerThanLargestFixedSize(3)   || !db.getSlotEnabled(3) ){
    //         // Change p3 to Sold Out
    // //        qDebug() << "Product 3 is Sold Out!" << endl;
    //         ui->selection3_Button->setStyleSheet("QPushButton { border-image: url(/home/df-admin/production/references/soldOut.png); }");
    //     }
    //     else{
    //         ui->selection3_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    //     }

    //     if (!db.remainingVolumeIsBiggerThanLargestFixedSize(4)   || !db.getSlotEnabled(4) ){
    //         // Change p4 to Sold Out
    // //        qDebug() << "Product 4 is Sold Out!" << endl;
    //         ui->selection4_Button->setStyleSheet("QPushButton { border-image: url(/home/df-admin/production/references/soldOut.png); }");
    //     }
    //     else{
    //         ui->selection4_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    //     }
}

void page_select_product::cancelTimers()
{
    productPageEndTimer->stop();
}

void page_select_product::select_product(int slot)
{
    qDebug() << "ahoyy16";
    DbManager db(DB_PATH);
    if (db.remainingVolumeIsBiggerThanLargestFixedSize(slot) && db.getSlotEnabled(slot))
    {
        db.closeDB();
        productPageEndTimer->stop();
        idlePage->currentProductOrder->setSelectedSlot(slot);
        idlePage->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);
        paymentSelectPage->resizeEvent(productResize);
        paymentSelectPage->showFullScreen();
        this->hide();
    }
    else
    {
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

// // FIXME: This is terrible...no time to make array reference to hold button press functions
// void page_select_product::on_selection1_Button_clicked()
// {
//     qDebug() << "ahoyy16" ;
//     DbManager db(DB_PATH);
//     if(db.remainingVolumeIsBiggerThanLargestFixedSize(1)  && db.getSlotEnabled(1)){
//         db.closeDB();
//         productPageEndTimer->stop();
//         idlePage->currentProductOrder->setSelectedSlot(1);
//         idlePage->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);
//         paymentSelectPage->resizeEvent(productResize);
//         paymentSelectPage->showFullScreen();
//         this->hide();
//     }else{
//         db.closeDB();
//     }
// }

// void page_select_product::on_selection2_Button_clicked()
// {
//     qDebug() << "ahoyy17" ;
//     DbManager db(DB_PATH);
//     if(db.remainingVolumeIsBiggerThanLargestFixedSize(2) && db.getSlotEnabled(2)){
//         db.closeDB();
//         productPageEndTimer->stop();
//         idlePage->currentProductOrder->setSelectedSlot(2);
//         idlePage->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);
//         paymentSelectPage->resizeEvent(productResize);
//         paymentSelectPage->showFullScreen();
//         this->hide();
//     }else{
//         db.closeDB();
//     }
// }

// void page_select_product::on_selection3_Button_clicked()
// {
//     qDebug() << "ahoyy18" ;
//     DbManager db(DB_PATH);
//     if(db.remainingVolumeIsBiggerThanLargestFixedSize(3)  && db.getSlotEnabled(3)){
//         db.closeDB();
//         productPageEndTimer->stop();
//         idlePage->currentProductOrder->setSelectedSlot(3);
//         idlePage->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);

//         paymentSelectPage->resizeEvent(productResize);
//         paymentSelectPage->showFullScreen();
//         this->hide();
//     }else{
//         db.closeDB();
//     }

// }

// void page_select_product::on_selection4_Button_clicked()
// {
//     qDebug() << "ahoyy19" ;
//     DbManager db(DB_PATH);
//     if(db.remainingVolumeIsBiggerThanLargestFixedSize(4)  && db.getSlotEnabled(4)){
//         db.closeDB();
//         productPageEndTimer->stop();
//         idlePage->currentProductOrder->setSelectedSlot(4);
//         idlePage->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);
//         paymentSelectPage->resizeEvent(productResize);
//         paymentSelectPage->showFullScreen();
//         this->hide();
//     }else{
//         db.closeDB();
//     }
// }

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
    idlePage->showFullScreen();
    this->hide();
}

void page_select_product::on_p_page_maintenanceButton_pressed()
{
    //    qDebug() << "Maintenance button pressed" << endl;
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
    qDebug() << "efaefasMain Page Button pressed" << endl;
    mainPage();
}

void page_select_product::on_helpPage_Button_clicked()
{
    productPageEndTimer->stop();
    helpPage->showFullScreen();
    this->hide();
}
