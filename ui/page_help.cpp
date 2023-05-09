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
#include "page_qr_payment.h"
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

    // ui->transactions_Button->setStyleSheet(
    //     "QPushButton {"

    //     "font-family: 'Brevia';"
    //     "font-style: normal;"
    //     "font-weight: 75;"
    //     "font-size: 32px;"
    //     "line-height: 99px;"
    //     "letter-spacing: 1.5px;"
    //     "color: #003840;"
    //     "text-align: center;"
    //     "qproperty-alignment: AlignCenter;"
    //     "border: none;"
    //     "}");
    // ui->transactions_Button->setStyleSheet("QPushButton { color:#FFFFFF;background-color: #5E8580; border: 1px solid #3D6675;box-sizing: border-box;border-radius: 20px;}");
    // ui->transactions_Button->setFont(font);
    ui->transactions_Button->setText("Transaction History ->");

    ui->maintenance_page_Button->setStyleSheet("QPushButton { color:#003840; background-color: #FFFFFF; border: 0px ; text-align: centre;border-radius: 20px;border: none;}");
    ui->maintenance_page_Button->setFont(font);
    ui->maintenance_page_Button->setText("Settings");

    ui->feedback_Button->setStyleSheet("QPushButton { color:#003840; background-color: #FFFFFF; border: 0px ; text-align: centre;border-radius: 20px;border: none;}");
    ui->feedback_Button->setFont(font);
    ui->feedback_Button->setText("Contact Us");

    // ui->back_Button->setStyleSheet(
    //     "QPushButton {"

    //     "font-family: 'Brevia';"
    //     "font-style: normal;"
    //     "font-weight: 75;"
    //     "font-size: 32px;"
    //     "line-height: 99px;"
    //     "letter-spacing: 1.5px;"
    //     "color: #003840;"
    //     "text-align: center;"
    //     "qproperty-alignment: AlignCenter;"
    //     "border: none;"
    //     "}");

    ui->back_Button->setText("<-back");

    ui->previousPage_Button_2->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    DbManager db(DB_PATH);
    bool showTransactions = db.showTransactions();
    db.closeDB();
    if (!showTransactions)
    {
        ui->transactions_Button->hide();
    }

    ui->previousPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->refreshButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    helpIdleTimer = new QTimer(this);
    helpIdleTimer->setInterval(1000);
    connect(helpIdleTimer, SIGNAL(timeout()), this, SLOT(onHelpTimeoutTick()));

    connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(keyboardButtonPressed(int)));



    
    QString cssFilePath = "/home/df-admin/drinkfill/lodetest.css";
    QFile cssFile(cssFilePath);

    
    qDebug() << "******************wefwef css file";
    if (cssFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "******************qewqwefqw opened file.";
        // QString styleSheet = QString::fromUtf8(cssFile.readAll());
        // ui->transactions_Button->setStyleSheet(styleSheet);
        // ui->back_Button->setStyleSheet(styleSheet);


            QString styleSheet = QString::fromUtf8(cssFile.readAll());
             QString buttonSelector = QString("QPushButton#%1").arg(ui->transactions_Button->objectName());
           
             QString buttonSelector2 = QString("QPushButton#%1").arg(ui->back_Button->objectName());
            qDebug() << buttonSelector;
            qDebug() << buttonSelector2;
            qDebug() << "******************button back name: .";
            ui->back_Button->setStyleSheet(styleSheet);
            ui->transactions_Button->setStyleSheet(styleSheet);

    }else{
        qDebug() << "Css file could not be opened." << cssFilePath ;
    }
}

// DTOR
page_help::~page_help()
{
    delete ui;
}

void page_help::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    helpIdleTimer->stop();
    ui->keyboardTextEntry->setText("");
    ui->keyboard_3->hide();
    p_page_idle->pageTransition(this, pageToShow);
}

void page_help::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Help >>>>>>>>>";
    QWidget::showEvent(event);

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_HELP_BACKGROUND_PATH);

    DbManager db(DB_PATH);
    maintenance_pwd = db.getMaintenanceAdminPassword();
    help_text_html = db.getHelpPageHtmlText();
    db.closeDB();

    ui->html_textBrowser->setHtml(help_text_html);

    if (helpIdleTimer == nullptr)
    {
        helpIdleTimer = new QTimer(this);
        helpIdleTimer->setInterval(1000);
        connect(helpIdleTimer, SIGNAL(timeout()), this, SLOT(onHelpTimeoutTick()));
    }

    helpIdleTimer->start(1000);
    _helpIdleTimeoutSec = 60;
    ui->keyboard_3->hide();
}

/*
 * Page Tracking reference
 */
void page_help::setPage(page_select_product *pageSelect, pageProduct *pageProduct, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_transactions *pageTransactions, page_maintenance *pageMaintenance, page_sendFeedback *pageFeedback)
{
    this->p_page_idle = pageIdle;
    this->p_page_feedback = pageFeedback;
    this->paymentPage = page_qr_payment;
    this->selectPage = pageProduct;
    this->p_page_select_product = pageSelect;
    this->p_page_transactions = pageTransactions;
    this->p_page_maintenance = pageMaintenance;
}

void page_help::on_previousPage_Button_clicked()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_help::on_previousPage_Button_2_clicked()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_help::onHelpTimeoutTick()
{
    if (--_helpIdleTimeoutSec >= 0)
    {
    }
    else
    {
        qDebug() << "Help Timer Done!" << _helpIdleTimeoutSec;
        hideCurrentPageAndShowProvided(p_page_idle);
    }
}

void page_help::on_refreshButton_clicked()
{
    _helpIdleTimeoutSec = 60;
}

void page_help::on_transactions_Button_clicked()
{
    hideCurrentPageAndShowProvided(p_page_transactions);
}

void page_help::on_maintenance_page_Button_clicked()
{
    _helpIdleTimeoutSec = 60;
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
        int shortcut = QString::compare(textEntry, "lll", Qt::CaseInsensitive);

        if (compareResult == 0)
        // if (compareResult == 0 || shortcut == 0) // shortcut for developing.
        {
            usleep(100000);
            qDebug() << "Password correct. Will open maintenance page";
            hideCurrentPageAndShowProvided(p_page_maintenance);
        }
        else
        {
            qDebug() << "Wrong password. Check db in database or contact soapstand.";
            ui->keyboardTextEntry->setText("");
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
    hideCurrentPageAndShowProvided(p_page_feedback);
}
