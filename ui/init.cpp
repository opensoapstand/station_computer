//***************************************
//
// init.cpp
// GUI class while machine is initializing.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Idle Page
//
// created: 16-04-2021
// by: Paddy Riley
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "init.h"
#include "idle.h"
#include "ui_init.h"

// CTOR
init::init(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::init)
{
    // Background Set here; Inheritance on forms places image on all elements otherwise.
    ui->setupUi(this);
    QPixmap background(":/light/0_init.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    // IPC Networking
    dfUtility = new df_util();
    dfUtility->m_IsSendingFSM = false;

}

/*
 * Navigation to Product item
 */
void init::setPage(idle *pageIdle)
{
    // Chained to KB Listener
    this->idlePage = pageIdle;
}

// DTOR
init::~init()
{
    delete ui;
}

void init::initReadySlot(void){
    idlePage->showFullScreen();
    this->hide();
}
