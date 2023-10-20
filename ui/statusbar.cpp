#include "statusbar.h"
#include "ui_statusbar.h"
#include "page_idle.h"
#include "statusbar.h"

statusbar::statusbar(QWidget *parent) : QWidget(parent),
                                        ui(new Ui::statusbar)
{
    ui->setupUi(this);

    refreshTimer = new QTimer(this);
    refreshTimer->setInterval(1000);
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(onRefreshTimerTick()));

    refreshTimer->start(1000);
    _refreshTimerTimeoutSec = STATUS_BAR_REFRESH_PERIOD_SECONDS;
    is_statusbar_visible = false;
    roleTimeOutTrailingText = "";
}

statusbar::~statusbar()
{
    delete ui;
}

void statusbar::autoSetVisibility()
{
    bool isVisible = false;
    if (p_page_idle->thisMachine->getRole() != UserRole::user)
    {
        isVisible = true;
    }
    if (this->p_page_idle->thisMachine->getCouponState() == enabled_valid_active)
    {
        isVisible = true;
    }

    setVisibility(isVisible);
}

void statusbar::setVisibility(bool isVisible)
{
    is_statusbar_visible = isVisible;
}

void statusbar::showEvent(QShowEvent *event)
{
    qDebug() << "Statusbar show.";

    QWidget::showEvent(event);

    QString styleSheet = p_page_idle->thisMachine->getCSS(STATUSBAR_CSS);
    ui->label_active_role->setStyleSheet(styleSheet);
    ui->label_session_id->setStyleSheet(styleSheet);
    ui->label_coupon_code->setStyleSheet(styleSheet);

    // ui->pushButton_active_role->setStyleSheet(styleSheet);
    // p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_active_role, "normal", STATUSBAR_CSS);
    p_page_idle->thisMachine->addCssClassToObject(ui->label_active_role, "normal", STATUSBAR_CSS);

    setRoleTimeOutTrailingText(""); // reset count down.

    refresh();
}

// void statusbar::hideCurrentPageAndShowProvided(QWidget *pageToShow)
// {
//     p_page_idle->thisMachine->pageTransition(this, pageToShow);
// }

// void statusbar::hide()
// {

// }

void statusbar::setPage(page_idle *pageIdle)
{
    this->p_page_idle = pageIdle;
    qDebug() << "Statusbar set page. ";
}

void statusbar::setRoleTimeOutTrailingText(QString text)
{
    roleTimeOutTrailingText = text;
    // ui->pushButton_active_role
    if (text == "")
    {
        // p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_active_role, "normal", STATUSBAR_CSS);
        p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_hide, "normal", STATUSBAR_CSS);
    }
    else
    {
        // p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_active_role, "alert", STATUSBAR_CSS);
        p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_hide, "alert", STATUSBAR_CSS);
    }
}

void statusbar::refresh()
{
    autoSetVisibility();

    if (is_statusbar_visible)
    {
        this->show();

        this->p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_active_role, this->p_page_idle->thisMachine->getActiveRoleAsText());
        QString base = p_page_idle->thisMachine->getTemplateTextByElementNameAndPage(ui->pushButton_hide);
        ui->pushButton_hide->setText(base.arg(roleTimeOutTrailingText));

        if (this->p_page_idle->thisMachine->getCouponState() == enabled_valid_active)
        {
            QString coupon_code = this->p_page_idle->thisMachine->getCouponCode();
            QString coupon_status_text = this->p_page_idle->thisMachine->getTemplateTextByElementNameAndPage(ui->label_coupon_code);
            ui->label_coupon_code->setText(coupon_status_text.arg(coupon_code));
            ui->label_coupon_code->show();
        }
        else
        {
            ui->label_coupon_code->hide();
        }

        ui->label_session_id->setText(p_page_idle->thisMachine->getSessionId());
    }
    else
    {
        this->hide();
    }
}

void statusbar::on_pushButton_hide_clicked()
{
    qDebug() << "Statusbar: reset and hide! ";

    p_page_idle->thisMachine->resetUserState();

    refresh();
}

void statusbar::onRefreshTimerTick()
{
    _refreshTimerTimeoutSec -= 1;
    if (_refreshTimerTimeoutSec >= 0)
    {
    }
    else
    {

        qDebug() << "Statusbar: refresh";
        refresh();

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
