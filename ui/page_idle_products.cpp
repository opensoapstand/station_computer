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
// by: Lode Ameije & Ash Singla & Udbhav Kansal
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
page_idle_products::page_idle_products(QWidget *parent) : QWidget(parent),
                                        ui(new Ui::page_idle_products)
{
    // IPC Networking
    // dfUtility = new df_util();

    // Background Set here; Inheritance on forms places image on all elements otherwise.
    ui->setupUi(this);

  

 
}
/*
 * Navigation to Product item
 */
void page_idle_products::setPage(page_select_product *p_pageProduct, page_idle *p_page_idle, page_maintenance *pageMaintenance, page_maintenance_general *pageMaintenanceGeneral)
{
  this->p_pageSelectProduct = p_pageProduct;
    this->p_page_idle = page_idle ;
    this->p_page_maintenance = pageMaintenance;
    this->p_page_maintenance_general = pageMaintenanceGeneral;
}



void page_idle_products::showEvent(QShowEvent *event)
{

    this->lower();
    qDebug() << "<<<<<<< Page Enter: idle >>>>>>>>>";
    QWidget::showEvent(event);

    

    
/*
 * Screen click shows product page as full screen and hides page_idle screen
 */

}
