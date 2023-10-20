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
    // ui->pushButton_active_role->setStyleSheet(styleSheet);
     p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_active_role, "normal", STATUSBAR_CSS);
    ui->label_coupon_code->setStyleSheet(styleSheet);

    setRoleTimeOutTrailingText(""); // reset count down. 
    autoSetVisibility();
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
    if (text == ""){
    p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_active_role, "normal", STATUSBAR_CSS);

    }else{
    p_page_idle->thisMachine->addCssClassToObject(ui->pushButton_active_role, "alert", STATUSBAR_CSS);

    }
}

void statusbar::refresh()
{
    if (is_statusbar_visible)
    {
        this->show();
        // this->p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_active_role, this->p_page_idle->thisMachine->getActiveRoleAsText());
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

        QString role_as_text = p_page_idle->thisMachine->getActiveRoleAsText();
        QString base = p_page_idle->thisMachine->getTemplateTextByElementNameAndPageAndIdentifier(ui->pushButton_active_role, role_as_text);
        ui->pushButton_active_role->setText(base.arg(roleTimeOutTrailingText));
    }
    else
    {
        this->hide();
    }
}

void statusbar::on_pushButton_hide_clicked()
{
    qDebug() << "Statusbar button clicked.";
    is_statusbar_visible = false;
    refresh();

    // ui->pushButton_hide->hide();
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
void statusbar::on_pushButton_active_role_clicked()
{
    if (p_page_idle->thisMachine->getRole() == UserRole::user)
    {
    }
    else
    {
        p_page_idle->thisMachine->setRole(UserRole::user);
    }
    autoSetVisibility();
    refresh();
}
