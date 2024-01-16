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

void keyboard::initializeKeyboard(bool isVisible, QLineEdit *widget)
{
    is_keyboard_visible = isVisible;
    widgetForTextEdit = widget;
    hasStartedTyping = false;
    refresh();
}

void keyboard::registerCallBack(const std::function<void()>& func) {
    callbackFunction = func;
}

void keyboard::keyboardButtonPressed(int buttonID)
{
    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    QString buttonText = buttonpressed->text();

    if (!hasStartedTyping){
        hasStartedTyping = true;
        widgetForTextEdit->clear();
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
        widgetForTextEdit->backspace();
    }
    else if (buttonText == "Clear")
    {
        widgetForTextEdit->setText("");
    }
    else if (buttonText == "Space")
    {
        widgetForTextEdit->setText(widgetForTextEdit->text() + " ");
    }
    else if (buttonText == "Return")
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
        //     p_page_overview->apply_promo_code(widgetForTextEdit->text());
        // }
        is_keyboard_visible = false;
        refresh();
    }
    else if (buttonText == "&&")
    {
        widgetForTextEdit->setText(widgetForTextEdit->text() + "&");
    }
    else
    {
        widgetForTextEdit->setText(widgetForTextEdit->text() + buttonText);
    }
}

void keyboard::showEvent(QShowEvent *event)
{
    qDebug() << "Keyboard show.";

    QWidget::showEvent(event);
    QString styleSheet = p_page_idle->thisMachine->getCSS(KEYBOARD_CSS);
    ui->keyboard_3->setStyleSheet(styleSheet);
    // ui->statusbar_bg->setStyleSheet(styleSheet);
    // ui->label_active_role->setStyleSheet(styleSheet);
    // ui->label_session_id->setStyleSheet(styleSheet);
    // ui->label_coupon_code->setStyleSheet(styleSheet);
    // ui->pushButton_hide->setStyleSheet(styleSheet);

    QString keyboard = UNIVERSAL_KEYBOARD_IMAGE_PATH;
    QString keyboard_picture_path = p_page_idle->thisMachine->getTemplatePathFromName(keyboard);
    p_page_idle->thisMachine->addPictureToLabel(ui->label_keyboard_background, keyboard_picture_path);

    // ui->pushButton_active_role->setStyleSheet(styleSheet);
    // p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_active_role, "normal", STATUSBAR_CSS);
    // p_page_idle->thisMachine->addCssClassToObject(ui->label_active_role, "normal", STATUSBAR_CSS);

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
