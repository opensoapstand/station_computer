#include "ui_keyboard.h"
#include "page_productOverview.h"
#include "page_idle.h"
#include "statusbar.h"
#include "keyboard.h"
#include <QLineEdit>
keyboard::keyboard(QWidget *parent) : QWidget(parent),
                                        ui(new Ui::keyboard)
{
    ui->setupUi(this);

    refreshTimer = new QTimer(this);
    refreshTimer->setInterval(1000);
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(onRefreshTimerTick()));
    connect(ui->buttonGroup, SIGNAL(buttonPressed(int)), this, SLOT(keyboardButtonPressed(int)));

    refreshTimer->start(1000);
    _refreshTimerTimeoutSec = STATUS_BAR_REFRESH_PERIOD_SECONDS;
    is_keyboard_visible = false;
    roleTimeOutTrailingText = "";
}

keyboard::~keyboard()
{
    delete ui;
}

// for QLineEdit
void keyboard::initializeKeyboard(bool isVisible, QLineEdit *widget)
{
    is_keyboard_visible = isVisible;
    widgetForLineEdit = widget;
    hasStartedTyping = false;
    lineEdit = isVisible;
    refresh();
}

// for QTextEdit
void keyboard::initializeKeyboard(bool isVisible, QTextEdit *widget)
{
    is_keyboard_visible = isVisible;
    widgetForTextEdit = widget;
    hasStartedTyping = false;
    lineEdit = false;
    refresh();
}

void keyboard::needCAPS(bool capsYorN){
    needCAPSbutton = capsYorN;
}

void keyboard::setTimeoutSec(int* seconds, bool timeoutYorN){
    timeoutSec = seconds;
    timeout = timeoutYorN;
}
void keyboard::adjustTimeoutSec(int seconds){
    *timeoutSec = seconds;
}

void keyboard::registerCallBack(const std::function<void()>& func) {
    callbackFunction = func;
}

void keyboard::registerCancelCallBack(const std::function<void()>& func) {
    cancelCallbackFunction = func;
}

void keyboard::keyboardButtonDefaultAllInCAPS(){
    foreach (QAbstractButton *button, ui->buttonGroup->buttons())
    {
        if (button->text() == "Space" || button->text() == "Enter" || button->text() == "Done" || button->text() == "Cancel" || button->text() == "Clear" || button->text() == "Backspace" || button->text() == "Return")
        {
            // doing nothing
        }
        else
        {
            button->setText(button->text().toUpper());
        }
    }
}

void keyboard::keyboardButtonPressed(int buttonID)
{
    if(timeout){
        adjustTimeoutSec(120);
    }
    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    QString buttonText = buttonpressed->text();
    if (!hasStartedTyping){
        hasStartedTyping = true;
        widgetForLineEdit->clear();
    }
    if (buttonText == "CAPS")
    {
        foreach (QAbstractButton *button, ui->buttonGroup->buttons())
        {
            if (button->text() == "Space" || button->text() == "Enter" || button->text() == "Done" || button->text() == "Cancel" || button->text() == "Clear" || button->text() == "Backspace" || button->text() == "Return")
            {
            }
            else
            {
                button->setText(button->text().toLower());
            }
        }
        // change background keyboard to lower case
        if(lineEdit){
            QString keyboard = UNIVERSAL_LINE_EDIT_KEYBOARD_WITH_CAPS_LOWERCASE_IMAGE_PATH;
            QString keyboard_picture_path = p_page_idle->thisMachine->getTemplatePathFromName(keyboard);
            p_page_idle->thisMachine->addPictureToLabel(ui->label_keyboard_background, keyboard_picture_path);
        }else{
            QString keyboard = UNIVERSAL_TEXT_EDIT_KEYBOARD_LOWERCASE_IMAGE_PATH;
            QString keyboard_picture_path = p_page_idle->thisMachine->getTemplatePathFromName(keyboard);
            p_page_idle->thisMachine->addPictureToLabel(ui->label_keyboard_background, keyboard_picture_path);
        }
    }
    else if (buttonText == "caps")
    {
        foreach (QAbstractButton *button, ui->buttonGroup->buttons())
        {
            if (button->text() == "Space" || button->text() == "Enter" || button->text() == "Done" || button->text() == "Cancel" || button->text() == "Clear" || button->text() == "Backspace" || button->text() == "Return")
            {
                // doing nothing
            }
            else
            {
                button->setText(button->text().toUpper());
            }
        }
        // change background keyboard to upper case
        if(lineEdit){
            QString keyboard = UNIVERSAL_LINE_EDIT_KEYBOARD_WITH_CAPS_UPPERCASE_IMAGE_PATH;
            QString keyboard_picture_path = p_page_idle->thisMachine->getTemplatePathFromName(keyboard);
            p_page_idle->thisMachine->addPictureToLabel(ui->label_keyboard_background, keyboard_picture_path);
        }else{
            QString keyboard = UNIVERSAL_TEXT_EDIT_KEYBOARD_UPPERCASE_IMAGE_PATH;
            QString keyboard_picture_path = p_page_idle->thisMachine->getTemplatePathFromName(keyboard);
            p_page_idle->thisMachine->addPictureToLabel(ui->label_keyboard_background, keyboard_picture_path);
        }
    }
    else if (buttonText == "Backspace")
    {
        if(lineEdit){
            widgetForLineEdit->backspace();
        }else{
            QTextCursor cursor = widgetForTextEdit->textCursor();
            cursor.deletePreviousChar();
        }

    }
    else if (buttonText == "Clear")
    {
        if(lineEdit){
            widgetForLineEdit->setText("");
        }else{
            widgetForTextEdit->setPlainText("");
        }
    }
    else if (buttonText == "Space")
    {
        if(lineEdit){
            widgetForLineEdit->setText(widgetForLineEdit->text() + " ");
        }else{
            widgetForTextEdit->insertPlainText(" ");
        }
    }
    else if (buttonText == "Cancel"){
        //callback
        if(cancelCallbackFunction){
            cancelCallbackFunction();
        }
    }
    else if (buttonText == "Enter"){
        if(lineEdit){
            widgetForLineEdit->setText(widgetForLineEdit->text() + "\n");
        }else{
            widgetForTextEdit->insertPlainText("\n");
        }
    }
    else if (buttonText == "Return" || buttonText == "Done")
    {
        if(callbackFunction){
            callbackFunction();
        }
        // if (p_page_overview->m_readyToSendCoupon && p_page_idle->thisMachine->getCouponState() != enabled_processing_input)
        // {
        //     p_page_overview->m_readyToSendCoupon = false;

        //     // hack, sometimes it appears like the 'done' button code is called twice.
        //     p_page_idle->thisMachine->setCouponState(enabled_processing_input);
        //     p_page_overview->reset_and_show_page_elements();
        //     p_page_overview->apply_promo_code(widgetForLineEdit->text());
        // }
        // is_keyboard_visible = false;
        // refresh();
    }
    else if (buttonText == "&&")
    {
        if(lineEdit){
            widgetForLineEdit->setText(widgetForLineEdit->text() + "&");
        }else{
            widgetForTextEdit->insertPlainText("&");
        }
    }
    else
    {
        if(lineEdit){
            widgetForLineEdit->setText(widgetForLineEdit->text() + buttonText);
        }else{
            widgetForTextEdit->insertPlainText(buttonText);
        }
    }
}

void keyboard::showEvent(QShowEvent *event)
{
    qDebug() << "Keyboard show.";

    QWidget::showEvent(event);
    QString styleSheet = p_page_idle->thisMachine->getCSS(KEYBOARD_CSS);
    ui->keyboard_3->setStyleSheet(styleSheet);
    if(lineEdit){
        // if no cancel callback function, change keyboard picture path to the one without cancel button and enter button also hide those buttons
        QString keyboard = UNIVERSAL_LINE_EDIT_KEYBOARD_WITH_CAPS_UPPERCASE_IMAGE_PATH;
        QString keyboard_picture_path = p_page_idle->thisMachine->getTemplatePathFromName(keyboard);
        // hide Enter button and Cacncel button
        // enter button hide
        ui->keyboard_3->findChild<QPushButton *>("pushButton_417")->hide();
        // cancel button hide
        ui->keyboard_3->findChild<QPushButton *>("pushButton_450")->hide();
        // move caps button to enter button position
        ui->keyboard_3->findChild<QPushButton *>("pushButton_451")->move(930, 261);
        if(!needCAPSbutton){
            // hide CAPS button and change background
            ui->keyboard_3->findChild<QPushButton *>("pushButton_451")->hide();
            keyboard = UNIVERSAL_LINE_EDIT_KEYBOARD_WITHOUT_CAPS_IMAGE_PATH;
            keyboard_picture_path = p_page_idle->thisMachine->getTemplatePathFromName(keyboard);
        }else{
            // show CAPS button and not change the background
            ui->keyboard_3->findChild<QPushButton *>("pushButton_451")->show();
        }
        p_page_idle->thisMachine->addPictureToLabel(ui->label_keyboard_background, keyboard_picture_path);
    }else{
        // enter button show
        ui->keyboard_3->findChild<QPushButton *>("pushButton_417")->show();
        // cancel button show
        ui->keyboard_3->findChild<QPushButton *>("pushButton_450")->show();
        // caps button show and move back to original position
        ui->keyboard_3->findChild<QPushButton *>("pushButton_451")->show();
        ui->keyboard_3->findChild<QPushButton *>("pushButton_451")->move(930, 192);

        QString keyboard = UNIVERSAL_TEXT_EDIT_KEYBOARD_UPPERCASE_IMAGE_PATH;
        QString keyboard_picture_path = p_page_idle->thisMachine->getTemplatePathFromName(keyboard);
        p_page_idle->thisMachine->addPictureToLabel(ui->label_keyboard_background, keyboard_picture_path);
    }

    refresh();
}

void keyboard::setPage(page_idle *pageIdle, page_product_overview *page_Overview)
{
    this->p_page_idle = pageIdle;
    this->p_page_overview = page_Overview;
    qDebug() << "Keyboard set page. ";
}

void keyboard::refresh()
{
    if (is_keyboard_visible)
    {
        this->show();
    }
    else
    {
        this->hide();
    }
}


void keyboard::onRefreshTimerTick()
{
    _refreshTimerTimeoutSec -= 1;
    if (_refreshTimerTimeoutSec >= 0)
    {
    }
    else
    {
        _refreshTimerTimeoutSec = STATUS_BAR_REFRESH_PERIOD_SECONDS;
    }
}
