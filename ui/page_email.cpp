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
#include <iostream>
#include <regex>
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

    statusbarLayout = new QVBoxLayout(this);
}

// DTOR
page_email::~page_email()
{
    delete ui;
}

void page_email::setPage(page_dispenser *page_dispenser, page_idle *pageIdle, page_help *page_help, page_product_overview *page_product_overview, statusbar *p_statusbar)
{
    this->p_page_idle = pageIdle;
    this->p_page_dispenser = page_dispenser;
    this->p_page_help = page_help;
    this->p_page_product_overview = page_product_overview;
    this->p_statusbar = p_statusbar;
}

void page_email::showEvent(QShowEvent *event)
{

    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    statusbarLayout->addWidget(p_statusbar);            // Only one instance can be shown. So, has to be added/removed per page.
    statusbarLayout->setContentsMargins(0, 1860, 0, 0); // int left, int top, int right, int bottom);
    
    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget
    
    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_EMAIL_CSS);
    ui->pushButton_to_product_overview->setProperty("class", "buttonNoBorder");
    ui->keyboardTextEntry->setProperty("class", "keyboardTextEntry");
    ui->pushButton_continue->setProperty("class", "buttonContinue");

    QString full_path = p_page_idle->thisMachine->getTemplatePathFromName(IMAGE_BUTTON_HELP);
    p_page_idle->thisMachine->addPictureToLabel(ui->label_help, full_path);
    ui->label_help->lower();

    ui->pushButton_to_help->setStyleSheet(styleSheet);
    ui->pushButton_to_product_overview->setStyleSheet(styleSheet);
    ui->label_email_title->setStyleSheet(styleSheet);
    ui->keyboardTextEntry->setStyleSheet(styleSheet);
    ui->pushButton_continue->setStyleSheet(styleSheet);

    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_to_product_overview);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_email_title);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->keyboardTextEntry);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_continue);
    
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
        button->setProperty("class", "button");
        button->setStyleSheet(styleSheet);
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
    statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.

    p_page_idle->thisMachine->pageTransition(this, pageToShow);
}

void page_email::on_pushButton_to_product_overview_clicked()
{
    hideCurrentPageAndShowProvided(p_page_product_overview);
}

void page_email::on_pushButton_continue_clicked()
{
    QString textEntry = ui->keyboardTextEntry->text();
    QString sessionId = p_page_idle->thisMachine->getSessionId();
    QString activeRole = p_page_idle->thisMachine->getActiveRoleAsText();
    QString page_name = this->objectName();
    QString event = "Entered Email";

    if(emailValid(textEntry)){
        p_page_idle->thisMachine->getDb()->addUserInteraction(sessionId, activeRole, page_name, event, textEntry);
        qDebug() << "EMAIL CORRECT: continue with free sample dispense";
        hideCurrentPageAndShowProvided(p_page_dispenser);
    }else{
        p_page_idle->thisMachine->addCssClassToObject(ui->keyboardTextEntry, "enteredEmail_invalid", PAGE_EMAIL_CSS);
        QString email_input_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "keyboardTextEntry->invalid");
        ui->keyboardTextEntry->setText(email_input_text);
        ui->keyboardTextEntry->show();
        m_hasStartedTyping = false;
    }
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

void page_email::on_pushButton_to_help_clicked()
{
    hideCurrentPageAndShowProvided(p_page_help);
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
        p_page_idle->thisMachine->addCssClassToObject(ui->keyboardTextEntry, "keyboardTextEntry", PAGE_EMAIL_CSS);
    }
    if (buttonText == "CAPS")
    {
        foreach (QAbstractButton *button, ui->buttonGroup->buttons())
        {
            if (button->text() == "Space" || button->text() == "Done" || button->text() == "Cancel" || button->text() == "Clear" || button->text() == "Backspace")
            {
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

bool page_email::emailValid(QString email)
{
    std::string emailString = email.toStdString();
    const std::regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
    return std::regex_match(emailString, pattern);
}
