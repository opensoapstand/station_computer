//***************************************
//
// page_email.cpp
// GUI class for user to get drinkfill
//  page_email
//
//
// updated: 08-11-2022
// by: Ash Singla & Lode Ameije
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "page_email.h"
#include "ui_page_email.h"
#include "df_util.h"
#include "page_select_product.h"
#include "page_product.h"
#include "page_qr_payment.h"
#include "page_idle.h"

// CTOR
page_email::page_email(QWidget *parent) : QWidget(parent),
                                        ui(new Ui::page_email)
{
    // Fullscreen background setup
    ui->setupUi(this);

    helpIdleTimer = new QTimer(this);
    helpIdleTimer->setInterval(1000);
    connect(helpIdleTimer, SIGNAL(timeout()), this, SLOT(onHelpTimeoutTick()));
    connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(keyboardButtonPressed(int)));
}

// DTOR
page_email::~page_email()
{
    delete ui;
}

void page_email::setPage(page_dispenser *page_dispenser, page_idle *pageIdle, page_help *page_help)
{
    this->p_page_idle = pageIdle;
    this->p_page_dispenser = page_dispenser;
    this->p_page_help = page_help;
}

void page_email::showEvent(QShowEvent *event)
{

    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget
    
    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_EMAIL_CSS);
    ui->pushButton_to_idle->setProperty("class", "buttonNoBorder");
    ui->keyboardTextEntry->setProperty("class", "keyboardTextEntry");

    ui->pushButton_to_idle->setStyleSheet(styleSheet);
    ui->label_email_title->setStyleSheet(styleSheet);
    ui->keyboardTextEntry->setStyleSheet(styleSheet);

    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_to_idle);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_email_title);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->keyboardTextEntry);
    
    // ui->label_keyboardInfo->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "label_keyboardInfo")); //p_page_idle->thisMachine->setTemplateTextToObject(ui->label_keyboardInfo); // does not work because the parent is the keyboard, not the page.

    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_HELP_BACKGROUND_PATH);

    QString image_path = p_page_idle->thisMachine->getTemplatePathFromName(PAGE_HELP_BACKGROUND_GENERIC_WHITE);
    if (df_util::pathExists(image_path)){
        p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_HELP_BACKGROUND_GENERIC_WHITE);
    }

    // Keyboard settings
    ui->keyboard_3->show();
    foreach (QAbstractButton *button, ui->buttonGroup->buttons())
    {
        if (button->text() == "Space" || button->text() == "Done" || button->text() == "Cancel" || button->text() == "Clear" || button->text() == "Backspace"){
        }
        else
        {
            button->setText(button->text().toLower());
        }
    }
    QString email_placeholder = p_page_idle->thisMachine->getTemplateTextByPage(this, "keyboardTextEntry->email_placeholder");
    ui->keyboardTextEntry->setText(email_placeholder);
    ui->keyboardTextEntry->setEchoMode(QLineEdit::Normal);
    m_hasStartedTyping = false;
}

void page_email::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    helpIdleTimer->stop();
    ui->keyboard_3->hide();
    p_page_idle->thisMachine->pageTransition(this, pageToShow);
}

void page_email::on_pushButton_to_idle_clicked()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_email::onHelpTimeoutTick()
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

void page_email::on_pushButton_resetTimeout_clicked()
{
    _helpIdleTimeoutSec = 60;
}

void page_email::keyboardButtonPressed(int buttonID)
{
    qDebug() << "maintenance password Keyboard Button pressed";

    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    QString buttonText = buttonpressed->text();

    if (!m_hasStartedTyping){
        m_hasStartedTyping = true;
        ui->keyboardTextEntry->clear();
    }
    if (buttonText == "CAPS")
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
        // if (!p_page_idle->thisMachine->isAllowedAsMaintainer()){
        //     p_page_idle->thisMachine->processRolePassword(textEntry);
            
        //     if (p_page_idle->thisMachine->isAllowedAsMaintainer()){
        //         hideCurrentPageAndShowProvided(p_page_maintenance);
        //     }else{
        //         ui->keyboardTextEntry->setText("");
            
        //     }
        // }

        // if (p_page_idle->thisMachine->isAllowedAsMaintainer()){
        //     hideCurrentPageAndShowProvided(p_page_maintenance);

        // }
        // int compareResult_user = QString::compare(textEntry, p_page_idle->thisMachine->getMaintenanceAdminPassword(false), Qt::CaseInsensitive);
        // int compareResult_admin = QString::compare(textEntry, p_page_idle->thisMachine->getMaintenanceAdminPassword(true), Qt::CaseInsensitive);

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
