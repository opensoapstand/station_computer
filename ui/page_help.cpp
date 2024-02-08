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
#include "input_widget.h"
#include "ui_page_help.h"
#include "df_util.h"
#include "page_select_product.h"
#include "page_product.h"
#include "page_product_mixing.h"
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

    statusbarLayout = new QVBoxLayout(this);
}

// DTOR
page_help::~page_help()
{
    delete ui;
}
void page_help::setPage(page_select_product *pageSelect, page_product *page_product, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_transactions *pageTransactions, page_maintenance *pageMaintenance, page_sendFeedback *pageFeedback, page_how_to *page_howTo, statusbar *p_statusbar, keyboard *keyboard, input_widget *input_widget)
{
    this->p_page_idle = pageIdle;
    this->p_page_feedback = pageFeedback;
    this->p_page_payment = page_qr_payment;
    this->p_page_product = page_product;
    this->p_page_select_product = pageSelect;
    this->p_page_transactions = pageTransactions;
    this->p_page_maintenance = pageMaintenance;
    this->p_page_howTo = page_howTo;
    this->p_statusbar = p_statusbar;
    this->p_keyboard = keyboard;
    this->p_input_widget = input_widget;
}

void page_help::showEvent(QShowEvent *event)
{

    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget

    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_HELP_CSS);
    ui->pushButton_to_idle->setProperty("class", "buttonNoBorder");
    ui->pushButton_to_transactions->setProperty("class", "buttonNoBorder");
    ui->pushButton_resetTimeout->setProperty("class", "buttonTransparent");

    ui->pushButton_to_idle->setStyleSheet(styleSheet);
    ui->pushButton_to_transactions->setStyleSheet(styleSheet);
    ui->pushButton_resetTimeout->setStyleSheet(styleSheet);
    ui->pushButton_to_maintenance->setStyleSheet(styleSheet);
    ui->pushButton_to_feedback->setStyleSheet(styleSheet);
    ui->pushButton_to_howTo->setStyleSheet(styleSheet);
    ui->html_help_text->setStyleSheet(styleSheet);

    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_to_transactions);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_to_maintenance);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_to_feedback);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_to_idle);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_to_howTo);
    ui->label_keyboardInfo->setText(p_page_idle->thisMachine->getTemplateTextByPage(this, "label_keyboardInfo")); // p_page_idle->thisMachine->setTemplateTextToObject(ui->label_keyboardInfo); // does not work because the parent is the keyboard, not the page.

    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_HELP_BACKGROUND_PATH);

    help_text_html = p_page_idle->thisMachine->getHelpPageHtmlText();
    if (p_page_idle->thisMachine->getShowTransactionHistory())
    {
        ui->pushButton_to_transactions->show();
    }
    else
    {
        ui->pushButton_to_transactions->hide();
    }

    if (help_text_html != "")
    {

        ui->html_help_text->setHtml(help_text_html);
    }
    else
    {
        ui->html_help_text->hide();

        QString image_path = p_page_idle->thisMachine->getTemplatePathFromName(PAGE_HELP_BACKGROUND_GENERIC_WHITE);
        if (df_util::pathExists(image_path))
        {
            p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_HELP_BACKGROUND_GENERIC_WHITE);
        }
    }

    helpIdleTimer->start(1000);
    _helpIdleTimeoutSec = 60;

    if(p_page_idle->thisMachine->hasMixing()){
        p_keyboard->needCAPS(true);
        p_keyboard->setKeyboardVisibility(false, p_input_widget->findChild<QLineEdit *>("lineEdit_input"));
        p_input_widget->toggleInputWidget(false);
        statusbarLayout->removeWidget(p_keyboard);    
        statusbarLayout->removeWidget(p_input_widget);  

        ui->keyboard_3->hide();

        // when declaring custom widget, please remember to goto QT Creator and set the max size and min size for the parent cutsom widget 
        // (otherwise will have extra white space when delcare custom widget)
        // navigate to custom widget, right click on the parent widget, hover over Size Constraints, and select set Minimum Size and Maximum Size
        p_input_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        p_input_widget->setContentsMargins(0, 0, 0, 0); // int left, int top, int right, int bottom);

        p_keyboard->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        p_keyboard->setContentsMargins(0, 0, 0, 0);
        p_keyboard->findChild<QWidget *>("keyboard_3")->setGeometry(21, -25, 1040, 495); // int x, int y, int width, int height;

        p_statusbar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        p_statusbar->setContentsMargins(0, 0, 0, 0); 

        statusbarLayout->setSpacing(0);
        statusbarLayout->setContentsMargins(0, 0, 0, 0);
        statusbarLayout->addWidget(p_input_widget);  
        statusbarLayout->addWidget(p_keyboard);   
        statusbarLayout->addWidget(p_statusbar);   

        statusbarLayout->setAlignment(Qt::AlignBottom | Qt::AlignVCenter);

    }else{
        ui->pushButton_to_howTo->hide();
        ui->keyboard_3->hide();
        ui->keyboardTextEntry->setText("");
        statusbarLayout->addWidget(p_statusbar);            // Only one instance can be shown. So, has to be added/removed per page.
        statusbarLayout->setContentsMargins(0, 1874, 0, 0); // int left, int top, int right, int bottom);
    }
}

void page_help::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    helpIdleTimer->stop();
    if(p_page_idle->thisMachine->hasMixing()){
        statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.
        p_keyboard->keyboardButtonDefaultAllInCAPS();
    }else{
        ui->keyboard_3->hide();
        statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.
    }
    p_page_idle->thisMachine->pageTransition(this, pageToShow);

}

void page_help::on_pushButton_to_idle_clicked()
{
    hideCurrentPageAndShowProvided(p_page_idle);
    p_keyboard->keyboardButtonDefaultAllInCAPS();
}

void page_help::on_pushButton_to_howTo_clicked(){
    hideCurrentPageAndShowProvided(p_page_howTo);
    p_keyboard->keyboardButtonDefaultAllInCAPS();
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

    if (p_page_idle->thisMachine->isAllowedAsMaintainer())
    {
        // if already logged in, go straight to maintenance mode.
        hideCurrentPageAndShowProvided(p_page_maintenance);
    }
    else
    {
        // provide keyboard if not yet logged in. 
        if(p_page_idle->thisMachine->hasMixing()){
            p_keyboard->registerCallBack(std::bind(&page_help::doneButtonPressed, this));
            p_keyboard->setKeyboardVisibility(true, p_input_widget->findChild<QLineEdit *>("lineEdit_input"));
            p_input_widget->toggleInputWidget(true);
        }else{
            ui->keyboard_3->show();
        }
    }
}

void page_help::doneButtonPressed(){
    QString textEntry = p_input_widget->findChild<QLineEdit *>("lineEdit_input")->text();

    // if role was already set, do not check pwd.
    if (!p_page_idle->thisMachine->isAllowedAsMaintainer())
    {
        p_page_idle->thisMachine->processRolePassword(textEntry);

        if (p_page_idle->thisMachine->isAllowedAsMaintainer())
        {
            hideCurrentPageAndShowProvided(p_page_maintenance);
            p_keyboard->setKeyboardVisibility(false, p_input_widget->findChild<QLineEdit *>("lineEdit_input"));
        }
        else
        {
            p_input_widget->findChild<QLineEdit *>("lineEdit_input")->setText("");
            p_keyboard->setKeyboardVisibility(true, p_input_widget->findChild<QLineEdit *>("lineEdit_input"));
        }
    }
    p_keyboard->keyboardButtonDefaultAllInCAPS();
    if (p_page_idle->thisMachine->isAllowedAsMaintainer())
    {
        hideCurrentPageAndShowProvided(p_page_maintenance);
    }
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
        if (!p_page_idle->thisMachine->isAllowedAsMaintainer())
        {
            p_page_idle->thisMachine->processRolePassword(textEntry);

            if (p_page_idle->thisMachine->isAllowedAsMaintainer())
            {
                hideCurrentPageAndShowProvided(p_page_maintenance);
            }
            else
            {
                ui->keyboardTextEntry->setText("");
            }
        }

        if (p_page_idle->thisMachine->isAllowedAsMaintainer())
        {
            hideCurrentPageAndShowProvided(p_page_maintenance);
        }
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

void page_help::on_pushButton_to_feedback_clicked()
{
    hideCurrentPageAndShowProvided(p_page_feedback);
}

