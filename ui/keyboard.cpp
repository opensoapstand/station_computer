#include "ui_keyboard.h"
#include "page_idle.h"
#include "statusbar.h"
#include "keyboard.h"

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

void keyboard::autoSetVisibility()
{
    bool isVisible = false;
    if (p_page_idle->thisMachine->getCouponState() == enabled_show_keyboard)
    {
        isVisible = true;
    }

    // setVisibility(isVisible);
}

void keyboard::setVisibility(bool isVisible, QWidget *widget)
{
    is_keyboard_visible = isVisible;
    if(isVisible){
        this->show();
    }else{
        this->hide();
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

    // QString keyboard = KEYBOARD_IMAGE_PATH;
    // QString keyboard_picture_path = p_page_idle->thisMachine->getTemplatePathFromName(KEYBOARD_IMAGE_PATH);
    // p_page_idle->thisMachine->addPictureToLabel(ui->label_keyboard_background, keyboard_picture_path);

    // ui->pushButton_active_role->setStyleSheet(styleSheet);
    // p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_active_role, "normal", STATUSBAR_CSS);
    // p_page_idle->thisMachine->addCssClassToObject(ui->label_active_role, "normal", STATUSBAR_CSS);

    // setRoleTimeOutTrailingText(""); // reset count down.

    refresh();
}

// void statusbar::hideCurrentPageAndShowProvided(QWidget *pageToShow)
// {
//     p_page_idle->thisMachine->pageTransition(this, pageToShow);
// }

// void statusbar::hide()
// {

// }

void keyboard::setPage(page_idle *pageIdle)
{
    this->p_page_idle = pageIdle;
    qDebug() << "Keyboard set page. ";
}

// void statusbar::setRoleTimeOutTrailingText(QString text)
// {
//     roleTimeOutTrailingText = text;
//     // ui->pushButton_active_role
//     if (text == "")
//     {
//         // p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_active_role, "normal", STATUSBAR_CSS);
//         p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_hide, "normal", STATUSBAR_CSS);
//     }
//     else
//     {
//         // p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_active_role, "alert", STATUSBAR_CSS);
//         p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_hide, "alert", STATUSBAR_CSS);
//     }
// }

void keyboard::refresh()
{
    autoSetVisibility();

    if (is_keyboard_visible)
    {
        this->show();
    }
    else
    {
        this->hide();
    }
}

void keyboard::on_pushButton_hide_clicked()
{
    qDebug() << "Statusbar: reset and hide! ";

    p_page_idle->thisMachine->resetUserState();

    // refresh();
}

void keyboard::onRefreshTimerTick()
{
    _refreshTimerTimeoutSec -= 1;
    if (_refreshTimerTimeoutSec >= 0)
    {
    }
    else
    {

        // qDebug() << "Statusbar: refresh";
        // refresh();

        _refreshTimerTimeoutSec = STATUS_BAR_REFRESH_PERIOD_SECONDS;
    }
}
// void statusbar::on_pushButton_active_role_clicked()
// {
//     if (p_page_idle->thisMachine->getRole() == UserRole::user)
//     {
//     }
//     else
//     {
//         p_page_idle->thisMachine->setRole(UserRole::user);
//     }
//     autoSetVisibility();
//     refresh();
// }

void keyboard::keyboardButtonPressed(int buttonID)
{
    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    QString buttonText = buttonpressed->text();
    if (buttonText == "backspace")
    {
        // ui->lineEdit_promo_code->backspace();
        qDebug() << "######### BACKSPACE";
    }
    else if (buttonText == "done")
    {
        // if (m_readyToSendCoupon && p_page_idle->thisMachine->getCouponState() != enabled_processing_input)
        // {
        //     m_readyToSendCoupon = false;
        //     qDebug() << "Done clicked, initiated apply promo.";

        //     // hack, sometimes it appears like the 'done' button code is called twice.
        //     p_page_idle->thisMachine->setCouponState(enabled_processing_input);
        //     reset_and_show_page_elements();
        //     // apply_promo_code(ui->lineEdit_promo_code->text());
        // }
        // else
        // {
        //     qDebug() << "ASSERT ERROR: Illegal press. Still processing other call.";
        // }
        qDebug() << "############ DONE";
    }
    else if (buttonText.mid(0, 3) == "num")
    {
        qDebug() << "######### NUM" << buttonText.mid(3, 1);
        // ui->lineEdit_promo_code->setText(ui->lineEdit_promo_code->text() + buttonText.mid(3, 1));
    }
    else
    {
        qDebug() << "############ TEXT" << buttonText;
        // ui->lineEdit_promo_code->setText(ui->lineEdit_promo_code->text() + buttonText);
    }

    // qDebug() << "Promo code input field: " << ui->lineEdit_promo_code->text();
}
