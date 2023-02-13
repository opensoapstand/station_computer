//***************************************
//
// page_help.cpp
// GUI class for user to get drinkfill
//  page_help and contactact info
//
//
// updated: 08-11-2022
// by: Ash Singla & Lode Ameije
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "page_help.h"
#include "ui_page_help.h"
#include "df_util.h"
#include "page_select_product.h"
#include "page_product.h"
#include "page_payment.h"
#include "page_idle.h"

// CTOR
page_help::page_help(QWidget *parent) : QWidget(parent),
                                        ui(new Ui::page_help)
{
    // Fullscreen background setup
    ui->setupUi(this);


    // view transactions button
    QFont font;
    font.setFamily(QStringLiteral("Brevia"));
    font.setPointSize(20);
    // font.setBold(true);
    // font.setWeight(75);
    font.setWeight(50);


    ui->transactions_Button->setStyleSheet("QPushButton { color:#003840; background-color: transparent; border: 0px }");
    // ui->transactions_Button->setStyleSheet("QPushButton { color:#FFFFFF;background-color: #5E8580; border: 1px solid #3D6675;box-sizing: border-box;border-radius: 20px;}");
    ui->transactions_Button->setFont(font);
    ui->transactions_Button->setText("Transaction History ->");
    
    ui->maintenance_page_Button->setStyleSheet("QPushButton { color:#003840; background-color: #FFFFFF; border: 0px ; text-align: centre;border-radius: 20px;border: none;}");
    ui->maintenance_page_Button->setFont(font);
    ui->maintenance_page_Button->setText("Settings");

    ui->feedback_Button->setStyleSheet("QPushButton { color:#003840; background-color: #FFFFFF; border: 0px ; text-align: centre;border-radius: 20px;border: none;}");
    ui->feedback_Button->setFont(font);
    ui->feedback_Button->setText("Contact Us");
    
    DbManager db(DB_PATH);
    bool showTransactions = db.showTransactions();
    db.closeDB();
    if(!showTransactions){
        ui->transactions_Button->hide();
    }

  
    ui->previousPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->refreshButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    ui->previousPage_Button_2->setStyleSheet("QPushButton { color:#003840; background-color: transparent; border: 0px }");
    ui->previousPage_Button_2->setFont(font);
    ui->previousPage_Button_2->setText("<- Back");

    helpIdleTimer = new QTimer(this);
    helpIdleTimer->setInterval(1000);
    connect(helpIdleTimer, SIGNAL(timeout()), this, SLOT(onHelpTimeoutTick()));

    connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(keyboardButtonPressed(int)));
    // ui->keyboardTextEntry->setText("LODE");
}

// DTOR
page_help::~page_help()
{
    delete ui;
}

void page_help::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Help >>>>>>>>>";
    QWidget::showEvent(event);

    // QPixmap background(PAGE_HELP_BACKGROUND_PATH);
    // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    // QPalette palette;
    // palette.setBrush(QPalette::Background, background);
    // this->setPalette(palette);
    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_HELP_BACKGROUND_PATH);

    DbManager db(DB_PATH);
    maintenance_pwd = db.getMaintenanceAdminPassword();
    db.closeDB();

    if (helpIdleTimer == nullptr)
    {
        helpIdleTimer = new QTimer(this);
        helpIdleTimer->setInterval(1000);
        connect(helpIdleTimer, SIGNAL(timeout()), this, SLOT(onHelpTimeoutTick()));
    }

    helpIdleTimer->start(1000);
    _helpIdleTimeoutSec = 60;
    ui->refreshLabel->hide();
    ui->keyboard_3->hide();

}

/*
 * Page Tracking reference
 */
void page_help::setPage(page_select_product *pageSelect, pageProduct *pageProduct, page_idle *pageIdle, page_payment *page_payment, page_transactions *pageTransactions, page_maintenance *pageMaintenance, page_sendFeedback *pageFeedback)
{
    this->p_page_idle = pageIdle;
    this->p_page_feedback = pageFeedback;
    this->paymentPage = page_payment;
    this->selectPage = pageProduct;
    this->p_page_select_product = pageSelect;
    this->p_page_transactions = pageTransactions;
    this->p_page_maintenance = pageMaintenance;
}

void page_help::on_previousPage_Button_clicked()
{
    exit_page();
}

void page_help::on_previousPage_Button_2_clicked()
{
    exit_page();
}

void page_help::exit_page(){
    helpIdleTimer->stop()   ;
    p_page_idle->pageTransition(this, p_page_idle);
    ui->keyboardTextEntry->setText("");

}

void page_help::onHelpTimeoutTick()
{
    if (--_helpIdleTimeoutSec >= 0)
    {
        // qDebug() << "Help Tick Down: " << _helpIdleTimeoutSec;
    }
    else
    {
        qDebug() << "Help Timer Done!" << _helpIdleTimeoutSec;

        exit_page();
    }

    if (_helpIdleTimeoutSec < 10)
    {
        ui->refreshLabel->show();
    }
}

void page_help::on_refreshButton_clicked()
{
    _helpIdleTimeoutSec = 60;
    ui->refreshLabel->hide();
}

void page_help::on_transactions_Button_clicked()
{
    helpIdleTimer->stop();
    // p_page_transactions->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_transactions);
}

void page_help::on_maintenance_page_Button_clicked()
{
    helpIdleTimer->stop();
    // p_page_idle->pageTransition(this, p_page_maintenance);
    ui->keyboard_3->show();
}

void page_help::keyboardButtonPressed(int buttonID)
{
    qDebug() << "maintenance password Keyboard Button pressed";

    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    QString buttonText = buttonpressed->text();

    if (buttonText == "Cancel")
    {
        ui->keyboard_3->hide();
        ui->keyboardTextEntry->setText("");
    }
    else if (buttonText == "CAPS")
    {
        foreach (QAbstractButton *button, ui->buttonGroup->buttons())
        {
            if (button->text() == "Space" || button->text() == "Done" || button->text() == "Cancel" || button->text() == "Clear" || button->text() == "Backspace")
            {
                // qDebug() << "doing nothing";
            }
            else
            {
                button->setText(button->text().toLower());
            }
        }
    }
    else if (buttonText == "caps")
    {
        foreach (QAbstractButton *button, ui->buttonGroup->buttons())
        {
            if (button->text() == "Space" || button->text() == "Done" || button->text() == "Cancel" || button->text() == "Clear" || button->text() == "Backspace")
            {
                // doing nothing
            }
            else
            {
                button->setText(button->text().toUpper());
            }
        }
    }
    else if (buttonText == "Backspace")
    {
        ui->keyboardTextEntry->backspace();
    }
    else if (buttonText == "Clear")
    {
        ui->keyboardTextEntry->setText("");
    }
    else if (buttonText == "Done")
    {
        qDebug() << "DONE CLICKED";
        QString textEntry = ui->keyboardTextEntry->text();

        int compareResult = QString::compare(textEntry, maintenance_pwd, Qt::CaseInsensitive);

        if (compareResult == 0)
        {

            ui->keyboardTextEntry->setText("");
            usleep(100000);
            qDebug() << "Password correct. Will open maintenance page";
            qDebug() << compareResult;
            p_page_idle->pageTransition(this, p_page_maintenance);
            ui->keyboard_3->hide();
        }
    }
    else if (buttonText == "Space")
    {
        ui->keyboardTextEntry->setText(ui->keyboardTextEntry->text() + " ");
    }
    else if (buttonText == "&&")
    {
        ui->keyboardTextEntry->setText(ui->keyboardTextEntry->text() + "&");
    }
    else
    {
        ui->keyboardTextEntry->setText(ui->keyboardTextEntry->text() + buttonText);
    }
}

void page_help::on_feedback_Button_clicked()
{
    helpIdleTimer->stop()   ;
   
    p_page_idle->pageTransition(this, p_page_feedback);
}
