//***************************************
//
// page_idle.cpp
// GUI class while machine is page_idle.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Product Page1
//
// created: 16-07-2020
// by: Jason Wang
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
    QPixmap background(PAGE_IDLE_BACKGROUND_PATH);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    ui->nextPageButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");          // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget

   
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

   
  
}

// DTOR
page_idle::~page_idle()
{
    delete ui;
}

void page_idle::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    qDebug() << "<<<<<<<Page Enter: idle >>>>>>>>>";
    //    DbManager db(DB_PATH);
    // ui->savedBottles_label->setText("THANKS TO YOU, THIS MACHINE HAS SAVED<br>OVER " + QString::number(db.getTotalTransactions()) + " PLASTIC CONTAINERS<br>FROM THE LANDFILL");
       // customer logo

    QString logo_folder = LOGO_FOLDER_PATH;
    QString logo_path = logo_folder + "C-2_logo_white.png";
    addPictureToLabel( ui->logo_label, logo_path);
    
}

/*
 * Screen click shows product page as full screen and hides page_idle screen
 */

void page_idle::on_nextPageButton_clicked()
{

    // qDebug() << "SEND LARGE FILL DEBUG TEST" << endl;
    // this->dfUtility->msg = "1dl;";
    // this->dfUtility->m_IsSendingFSM = true;
    // this->dfUtility->m_fsmMsg = SEND_DISPENSE_STOP;
    // this->dfUtility->send_to_FSM();

    p_pageSelectProduct->showFullScreen();

    // DO NOT HIDE IDLE PAGE
    // it's staying in the background to counter a hack UBC students found (when changing screens and tapping during the swap, they could get a hold of the machine)
    // Tapping on on the desktop wallpaper minimizes the application.
    // If the idle page is not hidden, and always on the background, there is never a wall paper showing. Effectively preventing this vulnerability to be exploited.
    // this->hide();
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
    qDebug() << "Signal: Enter maintenance mode" << endl;
    p_page_maintenance->showFullScreen();
    this->hide();
    this->p_pageSelectProduct->hide();
}



void page_idle::addPictureToLabel(QLabel* label, QString picturePath)

{
    //  QString logo_folder = LOGO_FOLDER_PATH;
    // QString logo_path = logo_folder + "C-2_logo_white.png";
    df_util::fileExists(picturePath);
    QPixmap picture(picturePath);

    int w = label->width();
    int h = label->height();
  

    // // set a scaled pixmap to a w x h window keeping its aspect ratio 
   label->setPixmap(picture.scaled(w,h,Qt::KeepAspectRatio));

}
