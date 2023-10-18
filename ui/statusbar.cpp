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
}

statusbar::~statusbar()
{
    delete ui;
}

void statusbar::showEvent(QShowEvent *event)
{
    qDebug() << "Statusbar show.";

    QWidget::showEvent(event);

    QString styleSheet = p_page_idle->thisMachine->getCSS(STATUSBAR_CSS);
    ui->label_active_role->setStyleSheet(styleSheet);
}

// void statusbar::hideCurrentPageAndShowProvided(QWidget *pageToShow)
// {
//     p_page_idle->thisMachine->pageTransition(this, pageToShow);
// }

void statusbar::hide()
{
    this->hide();
}

void statusbar::setPage(page_idle *pageIdle)
{
    this->p_page_idle = pageIdle;
    qDebug() << "Statusbar set page. ";
}
void statusbar::refresh()
{
    this->p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_active_role, this->p_page_idle->thisMachine->getActiveRoleAsText());

    // statusbar->label_active_role->user, User statusbar->label_active_role->maintainer, Maintainer statusbar->label_active_role->admin, Administrator
}
void statusbar::on_pushButton_clicked()
{
    qDebug() << "Statusbar button clicked.";
    // this->hide();
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