//***************************************
//
// page_idle.cpp
// GUI class while machine is page_idle.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Product Page1
//
// created: 05-04-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "page_idle.h"
#include "ui_page_idle.h"
#include "page_idle_products.h"
#include "page_maintenance.h"
#include "page_maintenance_general.h"
#include "product.h"

#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#include <QMainWindow>
#include <QtWidgets>
#include <QtMultimediaWidgets>

//    #define PLAY_VIDEO
// CTOR

page_idle::page_idle(QWidget *parent) : QWidget(parent),
                                        ui(new Ui::page_idle)
{
    // IPC Networking
    dfUtility = new df_util();
    // product products[SLOT_COUNT]; // create an array of Product objects with size SLOT_COUNT
// for products.cpp
    // for (int slot_index = 0; slot_index <= SLOT_COUNT; slot_index++)
    // {
    //     products[slot_index].setSlot(slot_index);
    //     products[slot_index].loadFromDb(slot_index);
    // }


    // Background Set here; Inheritance on forms places image on all elements otherwise.
    ui->setupUi(this);

    ui->pushButton_test->raise();
    //ui->pushButton_to_select_product_page->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    ui->pushButton_to_select_product_page->raise();

    // TODO: Hold and pass DrinkOrder Object
    currentProductOrder = new DrinkOrder();
    currentProductOrder->setSelectedSlot(OPTION_SLOT_INVALID);
    // product *selectedProduct;
}

/*
 * Navigation to Product item
 */
void page_idle::setPage(page_select_product *p_pageProduct, page_maintenance *pageMaintenance, page_maintenance_general *pageMaintenanceGeneral, page_idle_products *p_page_idle_products)
{
    // Chained to KB Listener
    this->p_pageSelectProduct = p_pageProduct;
    this->p_page_maintenance = pageMaintenance;
    this->p_page_maintenance_general = pageMaintenanceGeneral;
    this->p_page_idle_products = p_page_idle_products;
#ifndef PLAY_VIDEO
    setBackgroundPictureFromTemplateToPage(this, PAGE_IDLE_BACKGROUND_PATH);
#endif

}

// DTOR
page_idle::~page_idle()
{
    delete ui;
}

void page_idle::showEvent(QShowEvent *event)
{
    QString styleSheet = getCSS(PAGE_IDLE_CSS);
    ui->pushButton_to_select_product_page->setStyleSheet(styleSheet); 
    ui->label_welcome_message->setStyleSheet(styleSheet);
    ui->pushButton_test->setStyleSheet(styleSheet);
    ui->label_printer_status->setStyleSheet(styleSheet);



    qDebug() << "open db: payment method";
    DbManager db(DB_PATH);
    bool needsReceiptPrinter = false;
    for (int slot = 1; slot <= SLOT_COUNT; slot++)
    {
        QString paymentMethod = db.getPaymentMethod(slot);
        if (paymentMethod == "plu" || paymentMethod == "barcode" || paymentMethod == "barcode_EAN-2 " || paymentMethod == "barcode_EAN-13")
        {
            needsReceiptPrinter = true;
            qDebug() << "Needs receipt printer: " << paymentMethod;
            break;
        }
    }
    // call db check if idle or idle_products
    idle_page_type = db.getIdlePageType();
    db.closeDB();

    if (idle_page_type == "static_products")
    {
        hideCurrentPageAndShowProvided(p_page_idle_products);
    }

    // DbManager db(DB_PATH);

    // else if (idlePageType == "dynamic_products")
    // {
    //     hideCurrentPageAndShowProvided(p_page_idle_products);
    // }

    // db.closeDB();

    this->lower();
    qDebug() << "<<<<<<< Page Enter: idle >>>>>>>>>";
    QWidget::showEvent(event);

    ui->label_welcome_message->setText("refill soap here! <br>tap screen to start");
    

   
    // reset promovalue
    currentProductOrder->setDiscountPercentageFraction(0.0);
    currentProductOrder->setPromoCode("");

    addCompanyLogoToLabel(ui->logo_label);

    ui->label_printer_status->hide(); // always hide here, will show if enabled and has problems.
    if (needsReceiptPrinter)
    {
        checkReceiptPrinterStatus();
    }

    addPictureToLabel(ui->drinkfill_logo_label, DRINKFILL_LOGO_VERTICAL_PATH);

// #define PLAY_VIDEO
#ifdef PLAY_VIDEO
    // player = new QMediaPlayer(this);

    // QGraphicsVideoItem *item = new QGraphicsVideoItem;
    // player->setVideoOutput(item);
    // graphicsView->scene()->addItem(item);
    // graphicsView->show();

    // player->setMedia(QUrl("http://example.com/myclip4.ogv"));
    // player->play();

    // QMainWindow w;
    QVideoWidget *videoWidget = new QVideoWidget(ui->video_player);
    QMediaPlayer *player = new QMediaPlayer(this);

    videoWidget->setGeometry(QRect(0, 0, 1000, 1000));

    // test text label
    video_label = new QLabel(this);
    // video_label = new QLabel(ui->video_player);
    video_label->setObjectName(QStringLiteral("video testset"));
    video_label->setGeometry(QRect(0, 0, 100, 31));
    video_label->setText("lode was here. And Ash too");
    video_label->raise();
    video_label->show();

    // ui->video_player
    // w.setCentralWidget(&videoWidget);

    ui->media_player->show();
    ui->media_player->raise();

    player->setMedia(QUrl::fromLocalFile("/home/df-admin/production/references/media/ttFILEREMOVEDITSTOOBIGt.mp4"));
    player->setVideoOutput(videoWidget);

    // #define VIDEO_IN_WIDGET
    player->play();

#ifdef VIDEO_IN_WIDGET
    // helpful?! https://stackoverflow.com/questions/65910004/qvideowidget-doesnt-display-frames

    videoWidget->show(); // needed to display in non full screen.
    // ui->video_player->raise();
    // ui->video_player->show();

#else
    qDebug() << "Video player is fullscreen? : " << videoWidget->isFullScreen();
    videoWidget->setFullScreen(true);

#endif
    qDebug() << "Video player. Is fullscreen? : " << videoWidget->isFullScreen();
#endif
    this->raise();
}
//for products.cpp
// product* page_idle::getSelectedProduct(){
//     return selectedProduct;
// }

// void page_idle::setSelectedProduct(uint8_t slot)
// {
//     product *selectedProduct = &products[slot - 1];
// }

void page_idle::checkReceiptPrinterStatus()
{
    qDebug() << "db idle check printer";
    DbManager db(DB_PATH);
    bool isPrinterOnline = false;
    bool hasPrinterPaper = false;
    bool hasReceiptPrinter = db.hasReceiptPrinter();
    db.printerStatus(&isPrinterOnline, &hasPrinterPaper);
    db.closeDB();

    if (hasReceiptPrinter)
    {
        qDebug() << "Check receipt printer functionality disabled.";
        this->p_page_maintenance_general->send_check_printer_status_command();
        ui->pushButton_to_select_product_page->hide(); // when printer needs to be restarted, it can take some time. Make sure nobody presses the button in that interval (to prevent crashes)
    }
    else
    {
        qDebug() << "Can't check receipt printer. Not enabled in db->machine table";
    }
}

void page_idle::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    this->pageTransition(this, pageToShow);
}
/*
 * Screen click shows product page as full screen and hides page_idle screen
 */

void page_idle::printerStatusFeedback(bool isOnline, bool hasPaper)
{
    qDebug() << "Printer feedback received from fsm";

    if (!isOnline)
    {
        ui->label_printer_status->raise();
        ui->label_printer_status->setText("Assistance needed\nReceipt Printer offline.");
        ui->label_printer_status->show();
    }
    else if (!hasPaper)
    {
        ui->label_printer_status->raise();
        ui->label_printer_status->setText("Assistance needed\nReceipt printer empty or improperly loaded.");
        ui->label_printer_status->show();
    }
    else
    {
        ui->label_printer_status->hide();
    }
    ui->pushButton_to_select_product_page->show();
}

void page_idle::on_pushButton_to_select_product_page_clicked()
{
    this->hideCurrentPageAndShowProvided(p_pageSelectProduct);
}

void page_idle::on_pushButton_test_clicked()
{
    qDebug() << "test buttonproceeed clicked.. ";
}

bool page_idle::isEnough(int p)
{
    switch (p)
    {
    case (1):
        return p1;
        break;
    case (2):
        return p2;
        break;
    case (3):
        return p3;
        break;
    case (4):
        return p4;
        break;
    }
    return false;
}

// void page_idle::MMSlot()
// {
//     qDebug() << "Signal: Enter maintenance mode";
//     this->p_pageSelectProduct->hide(); // if pressed from another page. This is not good. It could be on any page!

//     hideCurrentPageAndShowProvided(p_page_maintenance);
// }

void page_idle::addCompanyLogoToLabel(QLabel *label)
{
    qDebug() << "db init company logo";
    DbManager db(DB_PATH);
    QString id = db.getCustomerId();
    db.closeDB();
    qDebug() << "db closed";
    if (id.at(0) == 'C')
    {
        QString logo_path = QString(COMPANY_LOGO_PATH).arg(id);
        addPictureToLabel(label, logo_path);
    }
    else
    {
        qDebug() << "WARNING: invalid customer ID. Should like C-1, C-374, ... . Provided id: " << id;
    }
}

void page_idle::addPictureToLabel(QLabel *label, QString picturePath)

{
    df_util::fileExists(picturePath);
    QPixmap picture(picturePath);

    int w = label->width();
    int h = label->height();

    // // set a scaled pixmap to a w x h window keeping its aspect ratio
    label->setPixmap(picture.scaled(w, h, Qt::KeepAspectRatio));
}
QString page_idle::getTemplateFolder()
{
    return m_templatePath;
}

QString page_idle::getCSS(QString cssName)
{
    QString cssFilePath = getTemplatePathFromName(cssName);

    QFile cssFile(cssFilePath);
    QString styleSheet = "";
    if (cssFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        styleSheet = QString::fromUtf8(cssFile.readAll());
    }
    else
    {
        qDebug() << "CSS file could not be opened." << cssFilePath;
    }
    return styleSheet;
}

QString page_idle::getTemplatePathFromName(QString fileName)
{
    QString image_path = getTemplateFolder() + fileName;

    if (!df_util::fileExists(image_path))
    {
        QString image_default_path = getDefaultTemplatePathFromName(fileName);
        qDebug() << "File not found in template folder: " + image_path + ". Default template path: " + image_default_path;
        if (!df_util::fileExists(image_default_path))
        {
            qDebug() << "File not found in default template folder (will use path anyways...): " + image_default_path;
        }
        image_path = image_default_path;
    }

    return image_path;
}
void page_idle::setTemplateFolder(QString rootPath, QString templateFolder)
{
    m_templatePath = rootPath + templateFolder + "/";
    qDebug() << "Template path set to: " + m_templatePath;
}

QString page_idle::getDefaultTemplatePathFromName(QString fileName)
{
    QString template_root_path = TEMPLATES_ROOT_PATH;
    return template_root_path + TEMPLATES_DEFAULT_NAME + "/" + fileName;
}

void page_idle::pageTransition(QWidget *pageToHide, QWidget *pageToShow)
{
    // page transition effects are not part of QT but of the operating system! // search for ubuntu settings program to set transition animations to "off"
    qDebug() << "---------page transition";
    pageToShow->showFullScreen();
    pageToHide->hide();
}

void page_idle::setBackgroundPictureFromTemplateToPage(QWidget *p_widget, QString imageName)
{
    // on Page: if called from setPage: will not scale
    // on Page: if called from showEvent: will scale to screen

    QString image_path = imageName;
    image_path = getTemplatePathFromName(imageName);
    QPixmap background(image_path);

    // background = background.scaled(p_widget->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    p_widget->setPalette(palette);
    p_widget->repaint();
    p_widget->update();
}
