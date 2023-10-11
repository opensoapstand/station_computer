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
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
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

    helpIdleTimer = new QTimer(this);
    helpIdleTimer->setInterval(1000);
    connect(helpIdleTimer, SIGNAL(timeout()), this, SLOT(onHelpTimeoutTick()));
    connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(keyboardButtonPressed(int)));
}

// DTOR
page_help::~page_help()
{
    delete ui;
}
void page_help::setPage(page_select_product *pageSelect, page_product *page_product, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_transactions *pageTransactions, page_maintenance *pageMaintenance, page_sendFeedback *pageFeedback)
{
    this->p_page_idle = pageIdle;
    this->p_page_feedback = pageFeedback;
    this->p_page_payment = page_qr_payment;
    this->p_page_product = page_product;
    this->p_page_select_product = pageSelect;
    this->p_page_transactions = pageTransactions;
    this->p_page_maintenance = pageMaintenance;
}

void page_help::showEvent(QShowEvent *event)
{

    p_page_idle->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    p_page_idle->thisMachine.applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget
    
    QString styleSheet = p_page_idle->thisMachine.getCSS(PAGE_HELP_CSS);
    ui->pushButton_to_idle->setProperty("class", "buttonNoBorder");
    ui->pushButton_to_transactions->setProperty("class", "buttonNoBorder");
    ui->pushButton_resetTimeout->setProperty("class", "buttonTransparent");

    ui->pushButton_to_idle->setStyleSheet(styleSheet);
    ui->pushButton_to_transactions->setStyleSheet(styleSheet);
    ui->pushButton_resetTimeout->setStyleSheet(styleSheet);
    ui->pushButton_to_maintenance->setStyleSheet(styleSheet);
    ui->pushButton_to_feedback->setStyleSheet(styleSheet);

    p_page_idle->thisMachine.setTemplateTextToObject(ui->pushButton_to_transactions);
    p_page_idle->thisMachine.setTemplateTextToObject(ui->pushButton_to_maintenance);
    p_page_idle->thisMachine.setTemplateTextToObject(ui->pushButton_to_feedback);
    p_page_idle->thisMachine.setTemplateTextToObject(ui->pushButton_to_idle);
    ui->label_keyboardInfo->setText(p_page_idle->thisMachine.getTemplateTextByPage(this, "label_keyboardInfo")); //p_page_idle->thisMachine.setTemplateTextToObject(ui->label_keyboardInfo); // does not work because the parent is the keyboard, not the page.

    p_page_idle->thisMachine.setBackgroundPictureFromTemplateToPage(this, PAGE_HELP_BACKGROUND_PATH);

    help_text_html = p_page_idle->thisMachine.getHelpPageHtmlText();
    if (p_page_idle->thisMachine.getShowTransactionHistory())
    {
        ui->pushButton_to_transactions->show();
    }
    else
    {
        ui->pushButton_to_transactions->hide();
    }
    
    if (help_text_html != ""){

        ui->html_help_text->setHtml(help_text_html);
    }
    else
    {
        ui->html_help_text->hide();

        QString image_path = p_page_idle->thisMachine.getTemplatePathFromName(PAGE_HELP_BACKGROUND_GENERIC_WHITE);
        if (df_util::pathExists(image_path)){
            p_page_idle->thisMachine.setBackgroundPictureFromTemplateToPage(this, PAGE_HELP_BACKGROUND_GENERIC_WHITE);
        }
    }

    helpIdleTimer->start(1000);
    _helpIdleTimeoutSec = 60;
    ui->keyboard_3->hide();
    ui->keyboardTextEntry->setText("");
}

void page_help::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    helpIdleTimer->stop();
    ui->keyboard_3->hide();
    p_page_idle->thisMachine.pageTransition(this, pageToShow);
}

void page_help::on_pushButton_to_idle_clicked()
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

void page_help::on_pushButton_resetTimeout_clicked()
{
    _helpIdleTimeoutSec = 60;
}

void page_help::on_pushButton_to_transactions_clicked()
{
    hideCurrentPageAndShowProvided(p_page_transactions);
}

void page_help::on_pushButton_to_maintenance_clicked()
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

        // if role was already set, do not check pwd. 
        if (!p_page_idle->thisMachine.isAllowedAsMaintainer()){
            p_page_idle->thisMachine.processRolePassword(textEntry);
            
            if (p_page_idle->thisMachine.isAllowedAsMaintainer()){
                hideCurrentPageAndShowProvided(p_page_maintenance);
            }else{
                ui->keyboardTextEntry->setText("");
            
            }
        }

        if (p_page_idle->thisMachine.isAllowedAsMaintainer()){
            hideCurrentPageAndShowProvided(p_page_maintenance);

        }
        // int compareResult_user = QString::compare(textEntry, p_page_idle->thisMachine.getMaintenanceAdminPassword(false), Qt::CaseInsensitive);
        // int compareResult_admin = QString::compare(textEntry, p_page_idle->thisMachine.getMaintenanceAdminPassword(true), Qt::CaseInsensitive);

        // if (compareResult_user == 0)
        // {
        //     usleep(100000);
        //     qDebug() << "Maintenance user password correct.";
        //     hideCurrentPageAndShowProvided(p_page_maintenance);
        // }
        // else if (compareResult_user == 0){
        //     qDebug() << "Maintenance admin password correct.";
        //     hideCurrentPageAndShowProvided(p_page_maintenance);

        // }
        // else
        // {
        //     qDebug() << "Maintenance use password wrong . Check db in database or contact soapstand.";
        //     ui->keyboardTextEntry->setText("");
        // }
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

void page_help::on_pushButton_to_feedback_clicked()
{
    hideCurrentPageAndShowProvided(p_page_feedback);
}
