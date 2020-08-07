//***************************************
//
// thankyoupage.cpp
// GUI class to show user dispense has been
// completed and route back to idle
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "thankyoupage.h"
#include "ui_thankyoupage.h"

#include "idle.h"
#include "dispensepage.h"

// CTOR
thankYouPage::thankYouPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::thankYouPage)
{
    ui->setupUi(this);
    QPixmap background(":/light/7_thank_you_page.jpg");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    QString path = "/home/df-admin/Project/drinkfill/sqlite/";
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path + "drinkfill-sqlite.db");

    if(!db.open())
    {
        qDebug() << "Can't Connect to DB !";
    }
    else
    {
        qDebug() << "Connected Successfully to" + db.databaseName();
        QSqlQuery query;
        query.prepare("SELECT MAX(inventory_id) FROM inventory;");
        if(!query.exec())
        {
            qDebug() << "Can't Execute Query !";
        }
        else
        {
            qDebug() << "Query Executed Successfully !";
            while(query.next())
            {
                qDebug() << "Vendor Name : " << query.value(0).toString();
                qDebug() << "Vendor Full Address: " << query.value(1).toString();
            }
        }
    }


    /*hacky transparent button*/
    ui->mainPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    ui->Counter->setStyleSheet("background-color : #F1F2F2; color: #CBA580");
    ui->Counter->setText("1000");
}

/*
 * Page Tracking reference
 */
void thankYouPage::setPage(dispensePage *pageDispense, idle *pageIdle)
{
    this->idlePage = pageIdle;
    this->dispensingPage = pageDispense;
}

// DTOR
thankYouPage::~thankYouPage()
{
    delete ui;
}

void thankYouPage::on_mainPage_Button_clicked()
{
    this->hide();
    idlePage->showFullScreen();
}
