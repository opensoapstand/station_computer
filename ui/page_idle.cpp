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
#include <map>
#include "page_idle.h"
#include "ui_page_idle.h"
#include "page_idle_products.h"
#include "page_maintenance.h"
#include "page_maintenance_general.h"
#include "product.h"

#include <QMediaPlayer>
#include <QGraphicsVideoItem>
// #include <QMainWindow>
#include <QtWidgets>
#include <QtMultimediaWidgets>

//    #define PLAY_VIDEO
// CTOR

page_idle::page_idle(QWidget *parent) : QWidget(parent),
                                        ui(new Ui::page_idle)
{
    // IPC Networking
    dfUtility = new df_util();

    // for products.cpp
    for (int slot_index = 0; slot_index < SLOT_COUNT; slot_index++)
    {
        products[slot_index].setSlot(slot_index + 1);
        products[slot_index].load();
    }

    setSelectedProduct(0);
    // Background Set here; Inheritance on forms places image on all elements otherwise.
    ui->setupUi(this);

    ui->pushButton_test->raise();
    // ui->pushButton_to_select_product_page->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    ui->pushButton_to_select_product_page->raise();

    // TODO: Hold and pass Product Object
    selectedProduct = new product();
    // product *selectedProduct;

    idlePageTypeSelectorTimer = new QTimer(this);
    idlePageTypeSelectorTimer->setInterval(1000);
    connect(idlePageTypeSelectorTimer, SIGNAL(timeout()), this, SLOT(onIdlePageTypeSelectorTimerTick()));

}

/*
 * Navigation to Product item
 */
void page_idle::setPage(page_select_product *p_page_select_product, page_maintenance *pageMaintenance, page_maintenance_general *pageMaintenanceGeneral, page_idle_products *p_page_idle_products)
{
    // Chained to KB Listener
    this->p_pageSelectProduct = p_page_select_product;
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
    qDebug() << "<<<<<<< Page Enter: idle >>>>>>>>>";
    QWidget::showEvent(event);

    qDebug() << "TEST TEXTS LOADING";
    loadTextsFromCsv();
    QString result = getText("lode");
    qDebug() << result;
    result = getText("brecht");
    qDebug() << result;

    QString styleSheet = getCSS(PAGE_IDLE_CSS);
    ui->pushButton_to_select_product_page->setStyleSheet(styleSheet);
    ui->label_welcome_message->setStyleSheet(styleSheet);
    ui->pushButton_test->setStyleSheet(styleSheet);
    ui->label_printer_status->setStyleSheet(styleSheet);

    qDebug() << "open db: payment method";
    bool needsReceiptPrinter = false;
    for (int slot = 1; slot <= SLOT_COUNT; slot++)
    {
        QString paymentMethod = products[slot - 1].getPaymentMethod();
        // products[slot - 1].setDiscountPercentageFraction(0.0);
        // products[slot - 1].setPromoCode("");
        if (paymentMethod == "plu" || paymentMethod == "barcode" || paymentMethod == "barcode_EAN-2 " || paymentMethod == "barcode_EAN-13")
        {
            needsReceiptPrinter = true;
            qDebug() << "Needs receipt printer: " << paymentMethod;
            break;
        }

        // reset promovalue
        // currentProductOrder->setDiscountPercentageFraction(0.0);
        // currentProductOrder->setPromoCode("");
        
    }

    DbManager db(DB_PATH);
    // call db check if idle or idle_products
    idle_page_type = db.getIdlePageType();
    db.closeDB();

    if (idle_page_type == "static_products")
    {
        hideCurrentPageAndShowProvided(this->p_page_idle_products);
    }

    this->lower();
    qDebug() << "<<<<<<< Page Enter: idle >>>>>>>>>";
    QWidget::showEvent(event);

    ui->label_welcome_message->setText("refill soap here! <br>tap screen to start");

    

    addCompanyLogoToLabel(ui->logo_label);

    ui->label_printer_status->hide(); // always hide here, will show if enabled and has problems.
    if (needsReceiptPrinter)
    {
        checkReceiptPrinterStatus();
    }

    QString machine_logo_full_path = getTemplatePathFromName(MACHINE_LOGO_PATH);
    addPictureToLabel(ui->drinkfill_logo_label, machine_logo_full_path);
    ui->drinkfill_logo_label->setStyleSheet(styleSheet);

    idlePageTypeSelectorTimer->start(100);
    _idlePageTypeSelectorTimerTimeoutSec = 2;

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
}

void page_idle::changeToIdleProductsIfSet()
{
    DbManager db(DB_PATH);
    // call db check if idle or idle_products
    idle_page_type = db.getIdlePageType();
    db.closeDB();

    if (idle_page_type == "static_products")
    {
        hideCurrentPageAndShowProvided(this->p_page_idle_products);
    }
}

// for products.cpp
product *page_idle::getSelectedProduct()
{
    return selectedProduct;
}

void page_idle::setSelectedProduct(uint8_t slot)
{
    selectedProduct = &products[slot - 1];
}

void page_idle::setDiscountPercentage(double percentageFraction)
{
    // ratio = percentage / 100;
    qDebug() << "Set discount percentage fraction in idle page: " << QString::number(percentageFraction, 'f', 3);
    m_discount_percentage_fraction = percentageFraction;
}


double page_idle::getDiscountPercentage()
{
    qDebug() << "Get Discount percentange" << m_discount_percentage_fraction;
    return m_discount_percentage_fraction;
}

bool page_idle::isPromoApplied(){
    if(m_discount_percentage_fraction != 0.0){
        return true;
    }
    return false;
}

QString page_idle::getPromoCode()
{
    return m_promoCode;
}

void page_idle::setPromoCode(QString promoCode)
{
    // ratio = percentage / 100;
    qDebug() << "Set Promo Code: " << promoCode;
    m_promoCode = promoCode;
}

double page_idle::getPriceCorrectedAfterDiscount(double price){
    return price*( 1 - m_discount_percentage_fraction);
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
        idlePageTypeSelectorTimer->stop();
}
/*
 * Screen click shows product page as full screen and hides page_idle screen
 */
void page_idle::onIdlePageTypeSelectorTimerTick()
{
    if (--_idlePageTypeSelectorTimerTimeoutSec >= 0)
    {
    }
    else
    {
       changeToIdleProductsIfSet();
    idlePageTypeSelectorTimer->stop();
    }
}
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
    // ui->pushButton_to_select_product_page->show();
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

void page_idle::addCssStyleToObject(QWidget *qtType, QString classname, QString css_name)
{
    QString styleSheet = getCSS(css_name);
    qtType->setProperty("class", classname);
    qtType->setStyleSheet(styleSheet);
}

void page_idle::addCompanyLogoToLabel(QLabel *label)
{
    qDebug() << "db init company logo";
    DbManager db(DB_PATH);
    QString id = db.getCustomerId();
    db.closeDB();
    qDebug() << "db closed";
    if (id.at(0) == 'C')
    {
        QString logo_path = QString(CLIENT_LOGO_PATH).arg(id);
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
    setBackgroundPictureToQWidget(p_widget, image_path);
}

void page_idle::setBackgroundPictureToQWidget(QWidget *p_widget, QString image_path)
{
    QPixmap background(image_path);

    // background = background.scaled(p_widget->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    p_widget->setPalette(palette);
    p_widget->repaint();
    p_widget->update();
}

void page_idle::setTemplateTextToObject(QWidget *p_element)
{
   QWidget *parentWidget = p_element->parentWidget();
    QString pageName = parentWidget->objectName();
    QString elementName = p_element->objectName();

    QString searchString = pageName + "->" + elementName;

    QString text = getText(searchString);

    if (QLabel *label = qobject_cast<QLabel *>(p_element))
    {
        label->setText(text);
    }
    else if (QPushButton *button = qobject_cast<QPushButton *>(p_element))
    {
        button->setText(text);
    }
    else
    {
        // Handle other types of elements if needed
    }
}

QString page_idle::getText(QString textName_to_find)
{

    std::string key = textName_to_find.toStdString();
    auto it = textNameToTextMap.find(QString::fromStdString(key));
    QString retval;
    if (it != textNameToTextMap.end())
    {
        // std::cout << "Word found! Sentence: " << it->second ;
        retval = it->second;
    }
    else
    {
        qDebug() << "no template text value found for: " + textName_to_find;
        retval = textName_to_find;
    }
    return retval;
}

void page_idle::loadTextsFromCsv()
{
    // Create an input filestream

    QString name = UI_TEXTS_CSV_PATH;
    QString csv_path = getTemplatePathFromName(name);

    std::ifstream file(csv_path.toStdString());
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            if (!line.empty() && line[0] != '#')  // Skip empty lines and lines starting with '#'
            {
                qDebug() << QString::fromStdString(line);

                std::size_t space_pos = line.find(',');
                if (space_pos != std::string::npos)
                {
                    std::string word = line.substr(0, space_pos);
                    std::string sentence = line.substr(space_pos + 1);
                    QString qword = QString::fromStdString(word);
                    QString qsentence = QString::fromStdString(sentence);
                    textNameToTextMap[qword] = qsentence;
                }
            }
        }
        file.close();

        // Print the word-sentence mapping
        for (const auto &pair : textNameToTextMap)
        {
            qDebug() << pair.first << ": " << pair.second;
        }
    }
    else
    {
        qDebug() << "Texts file path could not be opened: " + csv_path;
    }
}