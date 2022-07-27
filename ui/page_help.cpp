//***************************************
//
// page_help.cpp
// GUI class for user to get drinkfill
//  page_help and contactact info
//
//
// created: 28-05-2021
// by: Paddy Riley
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
page_help::page_help(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_help)
{
    // Fullscreen background setup
    ui->setupUi(this);

    ui->previousPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->previousPage_Button_2->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->refreshButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

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

    helpIdleTimer = new QTimer(this);
    helpIdleTimer->setInterval(1000);
    connect(helpIdleTimer, SIGNAL(timeout()), this, SLOT(onHelpTimeoutTick()));
    
    connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(keyboardButtonPressed(int)));
    //ui->keyboardTextEntry->setText("LODE");
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
    

    if(helpIdleTimer == nullptr){
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
void page_help::setPage(page_select_product *pageSelect, pageProduct* pageProduct, page_idle* pageIdle, page_payment *page_payment, page_transactions *pageTransactions, page_maintenance* pageMaintenance)
{
    this->p_page_idle = pageIdle;
    this->paymentPage = page_payment;
    this->selectPage = pageProduct;
    this->p_page_select_product = pageSelect;
    this->p_page_transactions = pageTransactions;
    this->p_page_maintenance = pageMaintenance;
}

void page_help::on_previousPage_Button_clicked(){
    helpIdleTimer->stop();
    p_page_idle->pageTransition(this, p_page_idle);
}

void page_help::on_previousPage_Button_2_clicked(){
    helpIdleTimer->stop();
    // qDebug() << "help2 to idle";
    // p_page_idle->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_idle);
}

void page_help::onHelpTimeoutTick(){
    if(-- _helpIdleTimeoutSec >= 0) {
      // qDebug() << "Help Tick Down: " << _helpIdleTimeoutSec;
    } else {
       qDebug() << "Help Timer Done!" << _helpIdleTimeoutSec;

        helpIdleTimer->stop();
        // qDebug() << "help to idle";
        // p_page_idle->showFullScreen();
        // this->hide();
        p_page_idle->pageTransition(this, p_page_idle);
    }

    if(_helpIdleTimeoutSec<10){
        ui->refreshLabel->show();
    }
}

void page_help::on_refreshButton_clicked(){
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
    qDebug()<<"Butttton pressed";

    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    // qDebug() << buttonpressed->text();
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
        // QString password = ui->keyboardTextEntry->text();
        // //        qDebug() << "Password: " << password;
        // // ATTEMPT nmcli connection

        // QString connect_string = "nmcli dev wifi connect '" + ui->wifiPassLabel->text() + "' password '" + ui->keyboardTextEntry->text() + "'";
        // QByteArray ba = connect_string.toLocal8Bit();
        // const char *c_str = ba.data();
        // //        qDebug() << c_str;
        // system(c_str);

        // ui->d->hide();

        // QProcess process;
        // process.start("iwgetid -r");
        // process.waitForFinished(-1);
        // QString stdout = process.readAllStandardOutput();
        // ui->wifi_name->setText("Wifi Name: " + stdout);

        // process.start("hostname -I");
        // process.waitForFinished(-1);
        // stdout = process.readAllStandardOutput();
        // ui->wifi_ip_address->setText("Wifi IP Address: " + stdout);

        // process.start("nmcli -t -f STATE general");
        // process.waitForFinished(-1);
        // stdout = process.readAllStandardOutput();
        // ui->wifi_status->setText("Wifi State: " + stdout);

        // process.start("nmcli networking connectivity");
        // process.waitForFinished(-1);
        // stdout = process.readAllStandardOutput();
        // ui->wifi_internet->setText("Wifi Connectivity: " + stdout);
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

    QString textEntry = ui->keyboardTextEntry->text();
    int isEqual = QString::compare(textEntry, maintenance_pwd, Qt::CaseInsensitive);
    qDebug() << isEqual << "iiiijijjijj";
    qDebug() << textEntry << "iiiijijjijj";
    qDebug() << maintenance_pwd << "iiiijijjijj";
    if (isEqual!= -1){
    // if (maintenance_pwd == textEntry){
        qDebug()<< "password correct";
        p_page_idle->pageTransition(this, p_page_maintenance);
        ui->keyboard_3->hide();
        ui->keyboardTextEntry->setText("");
    }
}
