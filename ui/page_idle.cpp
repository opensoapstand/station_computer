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
#include "page_maintenance.h"
#include "page_maintenance_general.h"

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
    // dfUtility->m_IsSendingFSM = false;

    // Background Set here; Inheritance on forms places image on all elements otherwise.
    ui->setupUi(this);

    // QPixmap background(PAGE_IDLE_BACKGROUND_PATH);
    // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);

    ui->testButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    ui->testButton->raise();
    ui->toSelectProductPageButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    ui->toSelectProductPageButton->raise();

    // QPixmap image_logo(logo_path);
    // df_util::fileExists(logo_path);

    // int w = ui->logo_label->width();
    // int h = ui->logo_label->height();

    // // // set a scaled pixmap to a w x h window keeping its aspect ratio
    // ui->logo_label->setPixmap(image_logo.scaled(w,h,Qt::KeepAspectRatio));
    // // ui->logo_label->show();
    // // ui->logo_label->raise();

    // TODO: Hold and pass DrinkOrder Object
    currentProductOrder = new DrinkOrder();
    currentProductOrder->setSelectedSlot(OPTION_SLOT_INVALID);
}

// bool page_idle::isSlotAvailable(int slot){
//     return this->slotIndexAvailable[slot - 1];
// }
// void page_idle::setSlotAvailability(int slot, bool isEnabled){
//     this->slotIndexAvailable[slot - 1] = isEnabled;
// }

/*
 * Navigation to Product item
 */
void page_idle::setPage(page_select_product *p_pageProduct, page_maintenance *pageMaintenance, page_maintenance_general *pageMaintenanceGeneral)
{
    // Chained to KB Listener
    this->p_pageSelectProduct = p_pageProduct;
    this->p_page_maintenance = pageMaintenance;
    this->p_page_maintenance_general = pageMaintenanceGeneral;
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

    this->lower();
    qDebug() << "<<<<<<< Page Enter: idle >>>>>>>>>";
    QWidget::showEvent(event);

    ui->welcome_message_label->setText("refill soap here! <br>tap screen to start");
    ui->welcome_message_label->setStyleSheet(
        "QLabel {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 700;"
        "font-size: 85px;"
        "line-height: 99px;"
        "text-align: center;"
        "letter-spacing: 1.5px;"
        "text-transform: lowercase;"
        "color: #FFFFFF;"
        "qproperty-alignment: AlignCenter;"
        "}");

    ui->printer_status_label->setStyleSheet(
        "QLabel {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 100;"
        "background-color: #5E8580;"
        "font-size: 42px;"
        "text-align: centre;"
        "line-height: auto;"
        "letter-spacing: 0px;"
        "qproperty-alignment: AlignCenter;"
        "border-radius: 20px;"
        "color: white;"
        "border: none;"
        "}");

    // reset promovalue
    currentProductOrder->setDiscountPercentageFraction(0.0);
    currentProductOrder->setPromoCode("");

    addCompanyLogoToLabel(ui->logo_label);
    DbManager db(DB_PATH);
    QString paymentMethod = db.getPaymentMethod(1);
    qDebug() << paymentMethod;

    if (paymentMethod == "plu" || paymentMethod == "barcode")
    {
        checkReceiptPrinterStatus();
    }

    db.closeDB();
    qDebug() << "db closed";

    addPictureToLabel(ui->drinkfill_logo_label, DRINKFILL_LOGO_VERTICAL_PATH);

    // m_transitioning = false;

    // player = new QMediaPlayer(this);

    // QGraphicsVideoItem *item = new QGraphicsVideoItem;
    // player->setVideoOutput(item);
    // graphicsView->scene()->addItem(item);
    // graphicsView->show();

    // player->setMedia(QUrl("http://example.com/myclip4.ogv"));
    // player->play();

    // QMainWindow w;
// #define PLAY_VIDEO
#ifdef PLAY_VIDEO
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

    player->setMedia(QUrl::fromLocalFile("/home/df-admin/production/references/media/ttt.mp4"));
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

void page_idle::checkReceiptPrinterStatus()
{
    qDebug() << "db idle check printer";
    DbManager db(DB_PATH);
    bool isPrinterOnline = false;
    bool hasPrinterPaper = false;
    bool hasReceiptPrinter = db.hasReceiptPrinter();
    db.printerStatus(&isPrinterOnline, &hasPrinterPaper);
    db.closeDB();

    ui->printer_status_label->hide();
    if (hasReceiptPrinter)
    {
        this->p_page_maintenance_general->send_check_printer_status_command();
    }
   
    // this->p_page_maintenance_general->on_printer_check_status_clicked();
    // usleep(50000);

    // if (hasReceiptPrinter)
    // {
    //     ui->printer_status_label->raise();
    //     if (!isPrinterOnline)
    //     {
    //         ui->printer_status_label->setText("Error: Receipt Printer offline.");
    //         ui->printer_status_label->show();
    //     }
    //     else if (!hasPrinterPaper)
    //     {
    //         ui->printer_status_label->setText("Error: Receipt printer empty or improperly loaded.");
    //         ui->printer_status_label->show();
    //     }
    //     else
    //     {
    //         ui->printer_status_label->hide();
    //     }
    // }
    // else
    // {
    //     ui->printer_status_label->hide();
    // }
}
/*
 * Screen click shows product page as full screen and hides page_idle screen
 */

// void page_idle::checkReceiptPrinter()
// {
//     p_page_idle->dfUtility->send_command_to_FSM("p");
//     usleep(50000);
//     p_page_idle->dfUtility->send_command_to_FSM("1");
//     usleep(50000);
//     p_page_idle->dfUtility->send_command_to_FSM("q");
// }

void page_idle::printerStatusFeedback(bool isOnline, bool hasPaper)
{
    qDebug() << "Printer feedback received from fsm";

    if (!isOnline)
    {
        ui->printer_status_label->raise();
        ui->printer_status_label->setText("Assistance needed\nReceipt Printer offline.");
        ui->printer_status_label->show();
    }
    else if (!hasPaper)
    {
        ui->printer_status_label->raise();
        ui->printer_status_label->setText("Assistance needed\nReceipt printer empty or improperly loaded.");
        ui->printer_status_label->show();
    }
    else
    {
        ui->printer_status_label->hide();
    }
}

void page_idle::on_toSelectProductPageButton_clicked()
{
    qDebug() << "Proceed to next page button clicked. ";

    this->pageTransition(this, p_pageSelectProduct);

    // p_pageSelectProduct->showFullScreen();

    // // if (!m_transitioning)
    // // {
    // //     m_transitioning = true;
    //     this->raise();
    //     p_pageSelectProduct->showFullScreen();
    // // this->lower();

    //     // DO NOT HIDE IDLE PAGE
    //     // it's staying in the background to counter a hack UBC students found (when changing screens and tapping during the swap, they could get a hold of the machine)
    //     // Tapping on on the desktop wallpaper minimizes the application.
    //     // If the idle page is not hidden, and always on the background, there is never a wall paper showing. Effectively preventing this vulnerability to be exploited.
    //     this->hide();

    //     // m_transitioning = false;
    // }
}

void page_idle::on_testButton_clicked()
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

void page_idle::MMSlot()
{
    qDebug() << "Signal: Enter maintenance mode";
    // p_page_maintenance->showFullScreen();
    // this->hide();
    this->p_pageSelectProduct->hide();
    this->pageTransition(this, p_page_maintenance);
}

void page_idle::addCompanyLogoToLabel(QLabel *label)
{
#ifdef ENABLE_DYNAMIC_UI
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
#endif
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
QString page_idle::getTemplatePathFromName(QString backgroundPictureName)
{
    QString image_path = m_templatePath + backgroundPictureName;

    if (!df_util::fileExists(image_path))
    {
        QString image_default_path = getDefaultTemplatePathFromName(backgroundPictureName);
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

QString page_idle::getDefaultTemplatePathFromName(QString backgroundPictureName)
{
    QString template_root_path = TEMPLATES_ROOT_PATH;
    return template_root_path + TEMPLATES_DEFAULT_NAME + "/" + backgroundPictureName;
}

void page_idle::pageTransition(QWidget *pageToHide, QWidget *pageToShow)
{
    // page transition effects are not part of QT but of the operating system!
    // search for ubuntu tweaks program to set animations to "off"
    qDebug() << "---------page transition";
    // pageToHide->raise();
    pageToShow->showFullScreen();
    // usleep(200000);
    pageToHide->hide();
}

void page_idle::setBackgroundPictureFromTemplateToPage(QWidget *p_widget, QString imageName)
{
    // on Page: if called from setPage: will not scale
    // on Page: if called from showEvent: will scale to screen

    QString image_path = imageName;
#ifdef ENABLE_DYNAMIC_UI
    image_path = getTemplatePathFromName(imageName);
    // if (! df_util::fileExists(image_path)){

    //     image_path = getDefaultTemplatePathFromName(imageName);
    //     qDebug() << "File not found in template folder, will revert to default template: " + image_path;
    // }
#endif
#define USE_PIXMAP
#ifdef USE_PIXMAP
#ifdef ENABLE_DYNAMIC_UI

    QPixmap background(image_path);
#else
    QPixmap background(imageName);
#endif

    // background = background.scaled(p_widget->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    p_widget->setPalette(palette);

    // QPixmap background(PAGE_DISPENSE_INSTRUCTIONS_BACKGROUND_PATH);
    // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    // QPalette palette;
    // palette.setBrush(QPalette::Background, background);
    // this->setPalette(palette);

#else

    p_widget->setStyleSheet("QWidget { border-image: url(" + image_path + "); }");
    // p_widget->setStyleSheet("background-image: url("+ image_path +")");
#endif
}
