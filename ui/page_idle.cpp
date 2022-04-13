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
void page_idle::setPage(page_select_product *p_pageProduct, page_maintenance *pageMaintenance)
{
    // Chained to KB Listener
    this->p_pageSelectProduct = p_pageProduct;
    this->p_page_maintenance = pageMaintenance;
    setBackgroundPictureFromTemplateToPage(this, PAGE_IDLE_BACKGROUND_PATH);
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

    ui->welcome_message_label->setText("tap to explore our <br>soap selections");
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

    // reset promovalue
    currentProductOrder->setDiscountPercentageFraction(0.0);

    addCompanyLogoToLabel(ui->logo_label);

    addPictureToLabel(ui->drinkfill_logo_label,DRINKFILL_LOGO_VERTICAL_PATH);
}

/*
 * Screen click shows product page as full screen and hides page_idle screen
 */

void page_idle::on_toSelectProductPageButton_clicked()
{
    qDebug() << "Proceed to next page button clicked. ";
    p_pageSelectProduct->showFullScreen();
    this->lower();

    // DO NOT HIDE IDLE PAGE
    // it's staying in the background to counter a hack UBC students found (when changing screens and tapping during the swap, they could get a hold of the machine)
    // Tapping on on the desktop wallpaper minimizes the application.
    // If the idle page is not hidden, and always on the background, there is never a wall paper showing. Effectively preventing this vulnerability to be exploited.
    // this->hide();
}

void page_idle::on_testButton_clicked()
{
    ui->testButton->setText(ui->testButton->text() + ".");
    qDebug() << "test buttonproceeed clicked.. ";
    p_pageSelectProduct->showFullScreen();
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
    p_page_maintenance->showFullScreen();
    this->hide();
    this->p_pageSelectProduct->hide();
}

void page_idle::addCompanyLogoToLabel(QLabel *label)
{
#ifdef ENABLE_DYNAMIC_UI
    DbManager db(DB_PATH);
    QString id = db.getCustomerId();
    db.closeDB();
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
    return m_templatePath + backgroundPictureName;
}

void page_idle::setTemplateFolder(QString rootPath, QString templateFolder)
{
    m_templatePath = rootPath + templateFolder + "/";
    qDebug() << "Template path set to: " + m_templatePath;
}

void page_idle::setBackgroundPictureFromTemplateToPage(QWidget *p_widget, QString imageName)
{
    QString image_path = imageName;
#ifdef ENABLE_DYNAMIC_UI
    image_path = getTemplatePathFromName(imageName);
#endif
#define USE_PIXMAP
#ifdef USE_PIXMAP
#ifdef ENABLE_DYNAMIC_UI

    QPixmap background(image_path);
#else
    QPixmap background(imageName);
#endif

    background = background.scaled(p_widget->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    p_widget->setPalette(palette);

#else

    p_widget->setStyleSheet("QWidget { border-image: url(" + image_path + "); }");
    // p_widget->setStyleSheet("background-image: url("+ image_path +")");
#endif
}
