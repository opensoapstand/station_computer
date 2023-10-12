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
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "page_idle.h"
#include "ui_page_idle.h"
#include "page_idle_products.h"
#include "page_maintenance.h"
#include "page_maintenance_general.h"
#include "product.h"
#include "dbmanager.h"

#include <cstdlib> // For rand() function

#include <QStringList>
#include <QMediaPlayer>
#include <QGraphicsVideoItem>
// #include <QMainWindow>
#include <QtWidgets>
#include <QtMultimediaWidgets>

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>

//    #define PLAY_VIDEO
// CTOR

page_idle::page_idle(QWidget *parent) : QWidget(parent),
                                        ui(new Ui::page_idle)
{

    // Background Set here; Inheritance on forms places image on all elements otherwise.
    ui->setupUi(this);

    ui->pushButton_test->raise();
    ui->pushButton_to_select_product_page->raise();

    idlePageTypeSelectorTimer = new QTimer(this);
    idlePageTypeSelectorTimer->setInterval(1000);
    connect(idlePageTypeSelectorTimer, SIGNAL(timeout()), this, SLOT(onIdlePageTypeSelectorTimerTick()));

    pollTemperatureTimer = new QTimer(this);
    pollTemperatureTimer->setInterval(1000);
    connect(pollTemperatureTimer, SIGNAL(timeout()), this, SLOT(onPollTemperatureTimerTick()));

    testForFrozenScreenTimer = new QTimer(this);
    testForFrozenScreenTimer->setInterval(1000);
    connect(testForFrozenScreenTimer, SIGNAL(timeout()), this, SLOT(onTestForFrozenScreenTick()));

    // qDebug() << "TESTTTTTTT" << thisMachine.getSlotCount();
    // for (int slot_index = 0; slot_index < thisMachine.getSlotCount(); slot_index++)
    // {
    //     qDebug() << "chckckckckck" << slot_index;
    //     products[slot_index].setSlot(slot_index + 1);
    //     products[slot_index].setMachine(&thisMachine);
    //     products[slot_index].setDb(g_database);
    // }

    // thisMachine.setProducts(products);
    stateScreenCheck = state_screen_check_not_initiated;
}

/*
 * Navigation to Product item
 */
void page_idle::setPage(page_select_product *p_page_select_product, page_maintenance *pageMaintenance, page_maintenance_general *pageMaintenanceGeneral, page_idle_products *p_page_idle_products, page_error_wifi *p_page_error_wifi)
{
    // Chained to KB Listener
    this->p_pageSelectProduct = p_page_select_product;
    this->p_page_maintenance = pageMaintenance;
    this->p_page_maintenance_general = pageMaintenanceGeneral;
    this->p_page_idle_products = p_page_idle_products;
    this->p_page_error_wifi = p_page_error_wifi;
}

// DTOR
page_idle::~page_idle()
{
    delete ui;
}

void page_idle::displayTemperature()
{
    //  QString base_text = "Current Temperature: %1 °C"; //Assuming you have the base_text defined somewhere else or you can define it here.
    QString base_text = thisMachine.getTemplateTextByElementNameAndPage(ui->label_show_temperature);
    float temperature = thisMachine.getTemperature_1();
    ui->label_show_temperature->setText(base_text.arg(QString::number(temperature, 'f', 1)));
}

void page_idle::setMachine(machine* pmachine){
    thisMachine = pmachine;
}

void page_idle::showEvent(QShowEvent *event)
{

    registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);
    thisMachine.loadDynamicContent();
    thisMachine.getSlotCount();
    thisMachine.resetSessionId();
    thisMachine.dispenseButtonLightsAnimateState(true);
    thisMachine.setRole(UserRole::user);
    // thisMachine.setSlotCount();

    // everything coupon is reset when idle page is reached.
    thisMachine.initCouponState();
    thisMachine.setSelectedProduct(1); // default selected product is necessary to deal with things if no product is chosen yet e.g. show transaction history

#ifndef PLAY_VIDEO
    thisMachine.setBackgroundPictureFromTemplateToPage(this, PAGE_IDLE_BACKGROUND_PATH);
#endif
    thisMachine.applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget

    QString styleSheet = thisMachine.getCSS(PAGE_IDLE_CSS);
    ui->pushButton_to_select_product_page->setStyleSheet(styleSheet);
    ui->label_welcome_message->setStyleSheet(styleSheet);
    ui->pushButton_test->setStyleSheet(styleSheet);
    ui->label_printer_status->setStyleSheet(styleSheet);
    ui->label_temperature_status->setStyleSheet(styleSheet);

    ui->label_show_temperature->setStyleSheet(styleSheet);

    displayTemperature();

    if (thisMachine.isAelenPillarElseSoapStand() == false)
    {
        ui->label_show_temperature->hide();
        ui->label_temperature_status->hide();
    }
    else
    {
    }

    ui->label_temperature_status->hide();

    // template text with argument example
    // QString base_text = getTemplateTextByElementNameAndPageAndIdentifier(ui->label_welcome_message, "testargument" );
    // ui->label_welcome_message->setText(base_text.arg("SoAp")); // will replace %1 character in string by the provide text

    thisMachine.setTemplateTextToObject(ui->label_welcome_message);
    thisMachine.addCustomerLogoToLabel(ui->label_customer_logo);

    ui->label_printer_status->hide(); // always hide here, will show if enabled and has problems.

    if (thisMachine.hasReceiptPrinter())
    {
        checkReceiptPrinterStatus();
    }

    QString machine_logo_full_path = thisMachine.getTemplatePathFromName(MACHINE_LOGO_PATH);
    thisMachine.addPictureToLabel(ui->label_manufacturer_logo, machine_logo_full_path);
    ui->label_manufacturer_logo->setStyleSheet(styleSheet);

    idlePageTypeSelectorTimer->start(1000);
    _idlePageTypeSelectorTimerTimeoutSec = PAGE_IDLE_DELAY_BEFORE_ENTERING_IDLE_PRODUCTS;

    pollTemperatureTimer->start(1000);
    _pollTemperatureTimerTimeoutSec = PAGE_IDLE_POLL_TEMPERATURE_PERIOD_SECONDS;

    testForFrozenScreenTimer->start(1000);
    _testForFrozenScreenTimerTimeoutSec = PAGE_IDLE_TEST_FOR_FROZEN_SCREEN_PERIOD_SECONDS;

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
    video_label->setText("test");
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

    ui->label_printer_status->setText(QString::number(thisMachine.getTemperature_1(), 'f', 2));
}

void page_idle::changeToIdleProductsIfSet()
{
    if (thisMachine.getIdlePageType() == "static_products" || thisMachine.getIdlePageType() == "dynamic")
    {
        hideCurrentPageAndShowProvided(this->p_page_idle_products, false);
    }
}

void page_idle::registerUserInteraction(QWidget *page)
{
    QString page_name = page->objectName();
    qDebug() << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< User entered: " + page_name + " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
    QString event = "Entered Page";
    g_database->addUserInteraction(thisMachine.getSessionId(), thisMachine.getActiveRoleAsText(), page_name, event);
}

void page_idle::onTestForFrozenScreenTick()
{
    // sometimes the screen is frozen. It is not a hardware issue.
    // virtual mouse clicks also have no effect when frozen.
    // Nobody knows what causes it. Maybe a doubling up of the page? But no evidence of this is in any log.
    // So, we perform periodically a virtual mouse click. If it is caught by the 'next page' button, that means all is fine.
    // If not, we will go to select products page and automatically revert after some seconds. I hope that by reloading idle page, the 'freezing issue' is solved.

    // Only poll temperature after every PAGE_IDLE_TEST_FOR_FROZEN_SCREEN_PERIOD_SECONDS seconds
    _testForFrozenScreenTimerTimeoutSec--;
    if (_testForFrozenScreenTimerTimeoutSec >= 0)
    {
        return;
    }
    _testForFrozenScreenTimerTimeoutSec = PAGE_IDLE_TEST_FOR_FROZEN_SCREEN_PERIOD_SECONDS;

    // we will test the previous cycle's outcome. If there is a fail, that means it will take at least one whole period that it's undetected (for the sake of simplicity of implementation)

    if (stateScreenCheck == state_screen_check_not_initiated)
    {
    }
    else if (stateScreenCheck == state_screen_check_clicked_and_succes)
    {
        stateScreenCheck = state_screen_check_not_initiated;
    }
    else if (stateScreenCheck == state_screen_check_clicked_and_wait)
    {
        // still in this state? This means we have a fail!
        stateScreenCheck == state_screen_check_fail;

        qDebug() << "ERROR: Idle Screen Not resposive to click test. (or program lost focus?!...). Will automatically go to 'select products page'. Screen freeze test is only active on idle page. If this is ennoying you while working in Ubuntu, put the program in maintenance mode. ";
        hideCurrentPageAndShowProvided(p_pageSelectProduct, true); // will go to select products page and automatically revert after some seconds. I hope that by reloading idle page, the 'freezing issue' is solved.
        return;                                                    // we would create a monster if we continue, with multiple clicks and doubled up pages...
    }

    // prepare for next cycle
    stateScreenCheck = state_screen_check_clicked_and_wait;

    // TEST the function by every now and then not clicking.
    // std::srand(std::time(0)); // Seed the random number generator
    // int randomNumber = std::rand() % 5;     // Generate a random number between 0 and 4
    // bool randomBool = randomNumber > 2;

    // // execute virtual click.
    // if (randomBool)
    // {
    //     df_util::executeVirtualClick(200, 500);
    // }
    // else
    // {
    //     qDebug() << "Did not click. ";
    // }

    df_util::executeVirtualClick(200, 500);
}

// periodical temperature check initiated
void page_idle::onPollTemperatureTimerTick()
{
    // Only poll temperature after every PAGE_IDLE_POLL_TEMPERATURE_PERIOD_SECONDS seconds
    if (--_pollTemperatureTimerTimeoutSec >= 0)
        return;

    _pollTemperatureTimerTimeoutSec = PAGE_IDLE_POLL_TEMPERATURE_PERIOD_SECONDS;
    // qDebug() << "Check temperature.";

    thisMachine.getTemperatureFromController();

    float currentTemperature = thisMachine.getTemperature_1();
    bool isTemperatureHigh = thisMachine.isTemperatureTooHigh_1();

    // Update UI elements with the current temperature
    QString temperatureStr = QString::number(currentTemperature, 'f', 1);
    QString base_text = thisMachine.getTemplateTextByElementNameAndPage(ui->label_show_temperature);
    ui->label_show_temperature->setText(base_text.arg(temperatureStr));

    // Toggle visibility of temperature status label
    ui->label_temperature_status->setVisible(isTemperatureHigh && thisMachine.isAelenPillarElseSoapStand());

    if (isTemperatureHigh)
    {
        thisMachine.checkForHighTemperatureAndDisableProducts(); // todo test if it works
        qDebug() << "Temperature too high, block all slots.";

        // Update temperature status label
        QString base = thisMachine.getTemplateTextByElementNameAndPageAndIdentifier(ui->label_temperature_status, "temperature_too_high");
        ui->label_temperature_status->setText(base.arg(temperatureStr));

        qDebug() << "Temperature too high";
    }
    else
    {
        // qDebug() << "Temperature ok";
    }
}

// periodical check to transition to other idle page type
void page_idle::onIdlePageTypeSelectorTimerTick()
{
    _idlePageTypeSelectorTimerTimeoutSec -= 1;
    if (_idlePageTypeSelectorTimerTimeoutSec >= 0)
    {
    }
    else
    {

        changeToIdleProductsIfSet();
    }
}

void page_idle::checkReceiptPrinterStatus()
{
    qDebug() << "Check receipt printer functionality.";
    this->p_page_maintenance_general->send_check_printer_status_command();
    ui->pushButton_to_select_product_page->hide(); // when printer needs to be restarted, it can take some time. Make sure nobody presses the button in that interval (to prevent crashes)
}

void page_idle::printerStatusFeedback(bool isOnline, bool hasPaper)
{
    qDebug() << "Printer feedback received from fsm";

    if (!isOnline)
    {
        ui->label_printer_status->raise();
        thisMachine.setTemplateTextWithIdentifierToObject(ui->label_printer_status, "offline");
        ui->label_printer_status->show();
        // m_printer_isOnline_cached = false;
    }
    else if (!hasPaper)
    {
        ui->label_printer_status->raise();
        thisMachine.setTemplateTextWithIdentifierToObject(ui->label_printer_status, "nopaper");
        ui->label_printer_status->show();
        // m_printer_hasPaper_cached = false;
    }
    else
    {
        ui->label_printer_status->hide();
        // m_printer_isOnline_cached = true;
        // m_printer_hasPaper_cached = true;
    }
    ui->pushButton_to_select_product_page->show();
}

void page_idle::on_pushButton_to_select_product_page_clicked()
{
    qDebug() << "Clicked on to_product_page button."; // leave this for a while to investigate frozen screens in the field.
    if (stateScreenCheck == state_screen_check_clicked_and_wait)
    {
        stateScreenCheck = state_screen_check_clicked_and_succes;
    }
    else
    {
        this->hideCurrentPageAndShowProvided(p_pageSelectProduct, true);
    }
}

void page_idle::hideCurrentPageAndShowProvided(QWidget *pageToShow, bool createNewSessionId)
{
    // the moment there is a user interaction, a new session ID is created.
    // will only be relevant when user goes to select_products
    if (createNewSessionId)
    {
        thisMachine.createSessionId();
    }

    thisMachine.pageTransition(this, pageToShow);
    idlePageTypeSelectorTimer->stop();
    pollTemperatureTimer->stop();
    testForFrozenScreenTimer->stop();
}
void page_idle::on_pushButton_test_clicked()
{
    qDebug() << "pushButton_test clicked.. ";
}

// void page_idle::addCustomerLogoToLabel(QLabel *label)
// {
//     QString id = thisMachine.getCustomerId();
//     QString logo_path = QString(CLIENT_LOGO_PATH).arg(id);
//     addPictureToLabel(label, logo_path);
// }

// void page_idle::addPictureToButton(QPushButton *button, QString picturePath)
// {
//     QString p = selectedProduct->getProductPicturePath();
//     if (df_util::pathExists(p))
//     {
//         QPixmap im(p);
//         QIcon qi(im);
//         button->setIcon(qi);
//         button->setIconSize(QSize(271, 391));
//     }
// }

// void page_idle::addPictureToLabel(QLabel *label, QString picturePath)
// {
//     if (df_util::pathExists(picturePath))
//     {
//         QPixmap picture(picturePath);

//         int w = label->width();
//         int h = label->height();

//         // // set a scaled pixmap to a w x h window keeping its aspect ratio
//         label->setPixmap(picture.scaled(w, h, Qt::KeepAspectRatio));
//     }
//     else
//     {
//         qDebug() << "Can't add picture to label: " << label->objectName() << " " << picturePath;
//     }
// }

// void page_idle::addCssClassToObject(QWidget *element, QString classname, QString css_file_name)
// {
//     QString styleSheet = getCSS(css_file_name);
//     element->setProperty("class", classname);
//     element->setStyleSheet(styleSheet);
// }

// QString page_idle::getCSS(QString cssName)
// {
//     QString cssFilePath = thisMachine.getTemplatePathFromName(cssName);

//     QFile cssFile(cssFilePath);
//     QString styleSheet = "";
//     if (cssFile.open(QIODevice::ReadOnly | QIODevice::Text))
//     {
//         styleSheet = QString::fromUtf8(cssFile.readAll());
//     }
//     else
//     {
//         qDebug() << "CSS file could not be opened." << cssFilePath;
//     }
//     return styleSheet;
// }

// void page_idle::pageTransition(QWidget *pageToHide, QWidget *pageToShow)
// {
//     // page transition effects are not part of QT but of the operating system! // search for ubuntu settings program to set transition animations to "off"
//     pageToShow->showFullScreen();
//     pageToHide->hide();
// }

// void page_idle::setBackgroundPictureFromTemplateToPage(QWidget *p_widget, QString imageName)
// {
//     // on Page: if called from setPage: will not scale
//     // on Page: if called from showEvent: will scale to screen

//     QString image_path = imageName;
//     image_path = thisMachine.getTemplatePathFromName(imageName);
//     // qDebug()<< "background image path: " << image_path;
//     setBackgroundPictureToQWidget(p_widget, image_path);
// }

// void page_idle::setBackgroundPictureToQWidget(QWidget *p_widget, QString image_path)
// {
//     QPixmap background(image_path);

//     // background = background.scaled(p_widget->size(), Qt::IgnoreAspectRatio);
//     QPalette palette;
//     palette.setBrush(QPalette::Background, background);
//     p_widget->setPalette(palette);
//     p_widget->repaint();
//     p_widget->update();
// }

// void page_idle::setTemplateTextWithIdentifierToObject(QWidget *p_element, QString identifier)
// {
//     QString text = getTemplateTextByElementNameAndPageAndIdentifier(p_element, identifier);
//     setTextToObject(p_element, text);
// }

// QString page_idle::getTemplateTextByElementNameAndPageAndIdentifier(QWidget *p_element, QString identifier)
// {
//     // QString element_page_and_name = getTemplateTextByElementNameAndPage(p_element);
//     QString element_page_and_name = getCombinedElementPageAndName(p_element);

//     QString searchString = element_page_and_name + "->" + identifier;
//     return getTemplateText(searchString);
// }

// void page_idle::setTemplateTextToObject(QWidget *p_element)
// {
//     QString searchString = getTemplateTextByElementNameAndPage(p_element);
//     setTextToObject(p_element, searchString);
// }

// QString page_idle::getCombinedElementPageAndName(QWidget *p_element)
// {
//     QString elementName = p_element->objectName();
//     QWidget *parentWidget = p_element->parentWidget();

//     if (!parentWidget)
//     {
//         qDebug() << "No parent for the provided widget!! " << elementName;
//     }

//     QString pageName = parentWidget->objectName();

//     return pageName + "->" + elementName;
// }

// QString page_idle::getTemplateTextByElementNameAndPage(QWidget *p_element)
// {
//     QString pageName_elementName_combination = getCombinedElementPageAndName(p_element);
//     return getTemplateText(pageName_elementName_combination);
// }

// void page_idle::setTextToObject(QWidget *p_element, QString text)
// {
//     if (QLabel *label = qobject_cast<QLabel *>(p_element))
//     {
//         label->setText(text);
//     }
//     else if (QPushButton *button = qobject_cast<QPushButton *>(p_element))
//     {
//         button->setText(text);
//     }
//     else if (QMessageBox *msgBox = qobject_cast<QMessageBox *>(p_element))
//     {
//         msgBox->setText(text);
//     }
//     else
//     {
//         // Handle other types of elements if needed
//     }
// }

// // get a text that is not linked to an elements on a specific page by its identifier
// QString page_idle::getTemplateTextByPage(QWidget *page, QString identifier)
// {
//     QString pageName = page->objectName();
//     QString searchString = pageName + "->" + identifier;
//     return getTemplateText(searchString);
// }

// QString page_idle::getTemplateText(QString textName_to_find)
// {

//     std::string key = textName_to_find.toStdString();
//     auto it = textNameToTextMap_template.find(QString::fromStdString(key));
//     QString retval;
//     if (it != textNameToTextMap_template.end())
//     {
//         // std::cout << "Word found! Sentence: " << it->second ;
//         retval = it->second;
//     }
//     else
//     {
//         it = textNameToTextMap_default.find(QString::fromStdString(key));
//         if (it != textNameToTextMap_default.end())
//         {
//             retval = it->second;
//         }
//         else
//         {
//             qDebug() << "No template text value found for: " + textName_to_find;
//             retval = textName_to_find;
//         }
//     }

//     // \n values in the csv file get automatically escaped. We need to deescape them.
//     retval.replace("\\n", "\n");

//     return retval;
// }

// void page_idle::applyDynamicPropertiesFromTemplateToWidgetChildren(QWidget *widget)
// {
//     // in reality, send a page widget as argument. All the childeren will be checked. (i.e. buttons, labels,...)
//     QList<QObject *> allChildren = widget->findChildren<QObject *>();
//     foreach (QObject *child, allChildren)
//     {
//         QWidget *widget = qobject_cast<QWidget *>(child);
//         if (widget)
//         {
//             // not all child element are widgets.

//             QString combinedName = getCombinedElementPageAndName(widget);
//             // qDebug() << combinedName;
//             applyPropertiesToQWidget(widget);
//         }
//     }
// }

// void page_idle::applyPropertiesToQWidget(QWidget *widget)
// {

//     // example of line in text file with properties:
//     // page_idle->label_customer_logo,{"x":570, "y":1580, "width":351, "height":211,"isVisibleAtLoad":true}

//     QString combinedName = getCombinedElementPageAndName(widget);

//     auto it = elementDynamicPropertiesMap_template.find(combinedName);
//     QString jsonString;
//     bool valid = true;

//     if (it != elementDynamicPropertiesMap_template.end())
//     {
//         qDebug() << "element " << combinedName << "found in template. json string: " << it->second;
//         jsonString = it->second;
//     }
//     else
//     {
//         it = elementDynamicPropertiesMap_default.find(combinedName);
//         if (it != elementDynamicPropertiesMap_default.end())
//         {
//             qDebug() << "element " << combinedName << "found in default. json string: " << it->second;
//             jsonString = it->second;
//         }
//         else
//         {
//             // qDebug() << "No template text value found for: " + combinedName;
//             valid = false;
//         }
//     }

//     if (!valid)
//     {
//         return;
//     }

//     QJsonObject jsonObject = df_util::parseJsonString(jsonString);

//     int x = widget->x();
//     int y = widget->y();
//     int width = widget->width();
//     int height = widget->height();

//     if (jsonObject.contains("x"))
//     {
//         QJsonValue xValue = jsonObject.value("x");
//         x = xValue.toInt();
//         // qDebug() << "x found  " << x ;
//     }
//     if (jsonObject.contains("y"))
//     {
//         QJsonValue val = jsonObject.value("y");
//         y = val.toInt();
//         // qDebug() << "y found  " << y ;
//     }
//     if (jsonObject.contains("width"))
//     {
//         QJsonValue val = jsonObject.value("width");
//         width = val.toInt();
//         // qDebug() << "width found  " << width ;
//     }
//     if (jsonObject.contains("height"))
//     {
//         QJsonValue val = jsonObject.value("height");
//         height = val.toInt();
//         // qDebug() << "height found  " << height ;
//     }
//     widget->setGeometry(x, y, width, height);

//     if (jsonObject.contains("isVisibleAtLoad"))
//     {
//         QJsonValue val = jsonObject.value("isVisibleAtLoad");
//         bool isVisible = val.toBool();
//         // qDebug() << "visibility found  " << isVisible ;

//         widget->setVisible(isVisible);
//     }
// }

// void page_idle::loadElementDynamicPropertiesFromDefaultTemplate()
// {
//     qDebug() << "Load dynamic properties from default template file";
//     QString path = thisMachine.getDefaultTemplatePathFromName(UI_ELEMENT_PROPERTIES_PATH);
//     loadTextsFromCsv(path, &elementDynamicPropertiesMap_default);
//     // Print the word-sentence mapping
//     // for (const auto &pair : elementDynamicPropertiesMap_default)
//     // {
//     //     qDebug() << pair.first << ": " << pair.second;
//     // }
// }

// void page_idle::loadElementDynamicPropertiesFromTemplate()
// {
//     qDebug() << "Load dynamic properties from template file";
//     QString path = thisMachine.getTemplatePathFromName(UI_ELEMENT_PROPERTIES_PATH);
//     loadTextsFromCsv(path, &elementDynamicPropertiesMap_template);

//     // Print the word-sentence mapping
//     // for (const auto &pair : elementDynamicPropertiesMap_default)
//     // {
//     //     qDebug() << pair.first << ": " << pair.second;
//     // }
// }

// void page_idle::loadTextsFromTemplateCsv()
// {
//     qDebug() << "Load dynamic texts from template csv";
//     QString csv_path = thisMachine.getTemplatePathFromName(UI_TEXTS_CSV_PATH);
//     loadTextsFromCsv(csv_path, &textNameToTextMap_template);
// }

// void page_idle::loadTextsFromDefaultCsv()
// {
//     qDebug() << "Load dynamic texts from default csv";
//     QString csv_default_template_path = thisMachine.getDefaultTemplatePathFromName(UI_TEXTS_CSV_PATH);
//     loadTextsFromCsv(csv_default_template_path, &textNameToTextMap_default);
// }

// void page_idle::loadTextsFromCsv(QString csv_path, std::map<QString, QString> *dictionary)
// {
//     std::ifstream file(csv_path.toStdString());
//     if (file.is_open())
//     {
//         std::string line;
//         while (std::getline(file, line))
//         {
//             if (!line.empty() && line[0] != '#') // Skip empty lines and lines starting with '#'
//             {
//                 // qDebug() << QString::fromStdString(line);

//                 std::size_t delimiter_pos = line.find(','); // left of , is the element name, right is its text
//                 if (delimiter_pos != std::string::npos)
//                 {
//                     std::string word = line.substr(0, delimiter_pos);
//                     std::string sentence = line.substr(delimiter_pos + 1);
//                     QString qword = QString::fromStdString(word);
//                     QString qsentence = QString::fromStdString(sentence);
//                     (*dictionary)[qword] = qsentence;
//                 }
//             }
//         }
//         file.close();

//         // Print the word-sentence mapping
//         // for (const auto &pair : *dictionary)
//         // {
//         //     qDebug() << pair.first << ": " << pair.second;
//         // }
//     }
//     else
//     {
//         qDebug() << "Texts file path could not be opened: " + csv_path;
//     }
// }

// QStringList page_idle::getChildNames(QObject *parent)
// {
//     QStringList childNames;

//     if (!parent)
//     {
//         qDebug() << "Invalid parent object.";
//         return childNames;
//     }

//     QList<QObject *> allChildren = parent->findChildren<QObject *>();

//     foreach (QObject *child, allChildren)
//     {
//         childNames.append(child->objectName());
//     }

//     return childNames;
// }
