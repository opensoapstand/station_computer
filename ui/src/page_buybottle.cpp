#include "page_buybottle.h"
#include "page_select_product.h"
#include "ui_page_buybottle.h"
#include <QtWidgets>
#include <QtMultimediaWidgets>

page_buyBottle::page_buyBottle(QWidget *parent) : QWidget(parent),
                                                  ui(new Ui::page_buyBottle)
{
    ui->setupUi(this);

    // userRoleTimeOutTimer = new QTimer(this);
    // userRoleTimeOutTimer->setInterval(1000);
    // connect(userRoleTimeOutTimer, SIGNAL(timeout()), this, SLOT(onUserRoleTimeOutTimerTick()));

    bottlePageEndTimer = new QTimer(this);
    bottlePageEndTimer->setInterval(1000);
    connect(bottlePageEndTimer, SIGNAL(timeout()), this, SLOT(onbottlePageTimeoutTick()));

    statusbarLayout = new QVBoxLayout(this);
}

page_buyBottle::~page_buyBottle()
{
    delete ui;
}

void page_buyBottle::setPage(page_idle *pageIdle, page_select_product *p_page_select_product, page_help *pageHelp, statusbar *p_statusbar)
{
    this->p_page_idle = pageIdle;
    this->p_page_select_product = p_page_select_product;
    this->p_page_help = pageHelp;
    this->p_statusbar = p_statusbar;
}

void page_buyBottle::showEvent(QShowEvent *event)
{
    p_page_idle->thisMachine->registerUserInteraction(this);
    p_statusbar->refresh();

    statusbarLayout->addWidget(p_statusbar);            // Only one instance can be shown. So, has to be added/removed per page.
    statusbarLayout->setContentsMargins(0, 1874, 0, 0); // int left, int top, int right, int bottom);

    // userRoleTimeOutTimer->start(1000);
    // _userRoleTimeOutTimerSec = PAGE_IDLE_USER_ROLE_TIMEOUT_SECONDS;

    bottlePageEndTimer->start(1000);
    _bottlePageTimeoutSec = PAGE_BOTTLE_PAGE_TIMEOUT_SECONDS;

    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_BUY_BOTTLE_CSS);
    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_BUY_BOTTLE_BACKGROUND_PATH);
    QString full_path = p_page_idle->thisMachine->getTemplatePathFromName(IMAGE_BUTTON_HELP);
    p_page_idle->thisMachine->addPictureToLabel(ui->label_help, full_path);

    ui->label_buyBottle_title->setStyleSheet(styleSheet);
    ui->pushButton_to_idle->setStyleSheet(styleSheet);
    ui->pushButton_help_page->setStyleSheet(styleSheet);

    ui->pushButton_no->setProperty("class", "pushButton_selectBottle"); // apply class BEFORE setStyleSheet!!
    ui->pushButton_no->setStyleSheet(styleSheet);
    ui->pushButton_yes_1->setProperty("class", "pushButton_selectBottle"); // apply class BEFORE setStyleSheet!!
    ui->pushButton_yes_1->setStyleSheet(styleSheet);
    ui->pushButton_yes_2->setProperty("class", "pushButton_selectBottle"); // apply class BEFORE setStyleSheet!!
    ui->pushButton_yes_2->setStyleSheet(styleSheet);

    ui->label_button_no_bg->setStyleSheet(styleSheet);
    ui->label_button_yes_bg_1->setStyleSheet(styleSheet);
    ui->label_button_yes_bg_2->setStyleSheet(styleSheet);

    ui->label_button_no_text_L->setStyleSheet(styleSheet);
    ui->label_button_no_text_S->setStyleSheet(styleSheet);
    ui->label_button_yes_text_L_1->setProperty("class", "label_button_yes_L"); // apply class BEFORE setStyleSheet!!
    ui->label_button_yes_text_L_1->setStyleSheet(styleSheet);
    ui->label_button_yes_text_S_1->setProperty("class", "label_button_yes_S"); // apply class BEFORE setStyleSheet!!
    ui->label_button_yes_text_S_1->setStyleSheet(styleSheet);
    ui->label_button_yes_text_L_2->setProperty("class", "label_button_yes_L"); // apply class BEFORE setStyleSheet!!
    ui->label_button_yes_text_L_2->setStyleSheet(styleSheet);
    ui->label_button_yes_text_S_2->setProperty("class", "label_button_yes_S"); // apply class BEFORE setStyleSheet!!
    ui->label_button_yes_text_S_2->setStyleSheet(styleSheet);

    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_buyBottle_title);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_to_idle);

    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_button_no_text_L);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_button_no_text_S);
    ui->label_button_yes_text_L_1->setText("Yes");
    // ui->label_button_yes_text_L_1->setText("Yes, " + getBottleVolumeText(p_page_idle->thisMachine->m_pNumber_bottle_1));
    // ui->label_button_yes_text_L_2->setText("Yes, " + getBottleVolumeText(p_page_idle->thisMachine->m_pNumber_bottle_2));
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_button_yes_text_S_1);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_button_yes_text_S_2);

    if (!areBothBottleButtonsActivated())
    {
        // set no button and yes button position to middle
        ui->pushButton_no->move(221, 1331);
        ui->label_button_no_bg->move(221, 1331);
        ui->label_button_no_text_L->move(333, 1361);
        ui->label_button_no_text_S->move(296, 1423);
        ////////////// for displaying multiple bottle product with bottle size /////////////////////////
        // if(isBottleButtonActivated(p_page_idle->thisMachine->m_pNumber_bottle_1)){
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // only display selected bottle name and not the bottle size and only display one bottle type
        ui->pushButton_yes_1->move(565, 1331);
        ui->label_button_yes_bg_1->move(565, 1331);
        ui->label_button_yes_text_L_1->move(600, 1361);
        ui->label_button_yes_text_S_1->move(637, 1423);
        ui->pushButton_yes_2->hide();
        ui->label_button_yes_bg_2->hide();
        ui->label_button_yes_text_L_2->hide();
        ui->label_button_yes_text_S_2->hide();
        ////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////// for displaying multiple bottle product with bottle size /////////////////////////
        // }else{
        //     ui->pushButton_yes_2->move(565, 1331);
        //     ui->label_button_yes_bg_2->move(565, 1331);
        //     ui->label_button_yes_text_L_2->move(600, 1361);
        //     ui->label_button_yes_text_S_2->move(637, 1423);
        //     ui->pushButton_yes_1->hide();
        //     ui->label_button_yes_bg_1->hide();
        //     ui->label_button_yes_text_L_1->hide();
        //     ui->label_button_yes_text_S_1->hide();
        // }
        ////////////////////////////////////////////////////////////////////////////////////////////////
    }
}

// void page_buyBottle::onUserRoleTimeOutTimerTick()
// {
//     _userRoleTimeOutTimerSec--;
//     if (_userRoleTimeOutTimerSec >= 0)
//     {
//     }
//     else
//     {
//         userRoleTimeOutTimer->stop();
//         p_statusbar->setRoleTimeOutTrailingText("");
//         p_statusbar->refresh();

//         _userRoleTimeOutTimerSec = PAGE_IDLE_USER_ROLE_TIMEOUT_SECONDS;
//     }
// }

void page_buyBottle::onbottlePageTimeoutTick()
{
    _bottlePageTimeoutSec = _bottlePageTimeoutSec - 1;
    if (_bottlePageTimeoutSec >= 0)
    {
        // qDebug() << "Tick Down: " << _bottlePageTimeoutSec;
    }
    else
    {
        _bottlePageTimeoutSec = PAGE_BOTTLE_PAGE_TIMEOUT_SECONDS;
        // qDebug() << "Timer Done!" << _bottlePageTimeoutSec;
        hideCurrentPageAndShowProvided(p_page_idle);
    }
}

bool page_buyBottle::areBothBottleButtonsActivated()
{
    if ((p_page_idle->thisMachine->m_pNumber_bottle_1 != DUMMY_PNUMBER) && (p_page_idle->thisMachine->m_pNumber_bottle_2 != DUMMY_PNUMBER))
    {
        return true;
    }
    else
    {
        return false;
    }
}

// bool page_buyBottle::isBottleButtonActivated(int bottlePNumber){
//     if(bottlePNumber){
//         return true;
//     }else{
//         return false;
//     }
// }

QString page_buyBottle::getBottleVolumeText(int bottlOption)
{
    QString volume;
    QString unit = p_page_idle->thisMachine->getSizeUnit();
    if (unit == "oz")
    {
        volume = p_page_idle->thisMachine->getProductByPNumber(bottlOption)->getSizeAsVolumeWithCorrectUnits(1, true, true);
    }
    else
    {
        volume = QString::number(p_page_idle->thisMachine->getProductByPNumber(bottlOption)->getVolumeOfSelectedBottle()) + unit;
    }
    return volume;
}

void page_buyBottle::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    bottlePageEndTimer->stop();
    qDebug() << "Exit buy bottle page.";
    this->raise();
    statusbarLayout->removeWidget(p_statusbar);
    p_page_idle->thisMachine->pageTransition(this, pageToShow);
}

void page_buyBottle::on_pushButton_no_clicked()
{
    qDebug() << "NO button pressed for page_buyBottle";
    if (p_page_idle->thisMachine->hasSelectedBottle())
    {
        p_page_idle->thisMachine->resetSelectedBottle();
    }
    hideCurrentPageAndShowProvided(p_page_select_product);
}

void page_buyBottle::on_pushButton_yes_1_clicked()
{
    qDebug() << "YES button size 1 pressed for page_buyBottle";
    p_page_idle->thisMachine->setSelectedBottle(p_page_idle->thisMachine->m_pNumber_bottle_1);
    QString data = "Bottle size 1: " + QString::number(p_page_idle->thisMachine->m_pNumber_bottle_1);
    if (p_page_idle->thisMachine->hasSelectedBottle())
    {
        p_page_idle->thisMachine->m_db->addUserInteraction(p_page_idle->thisMachine->getSessionId(), p_page_idle->thisMachine->getActiveRoleAsText(), this->objectName(), "Selected bottle", data);
        hideCurrentPageAndShowProvided(p_page_select_product);
    }
}

void page_buyBottle::on_pushButton_yes_2_clicked()
{
    qDebug() << "YES button size 2 pressed for page_buyBottle";
    p_page_idle->thisMachine->setSelectedBottle(p_page_idle->thisMachine->m_pNumber_bottle_2);
    QString data = "Bottle size 2: " + QString::number(p_page_idle->thisMachine->m_pNumber_bottle_2);
    if (p_page_idle->thisMachine->hasSelectedBottle())
    {
        p_page_idle->thisMachine->m_db->addUserInteraction(p_page_idle->thisMachine->getSessionId(), p_page_idle->thisMachine->getActiveRoleAsText(), this->objectName(), "Selected bottle", data);
        hideCurrentPageAndShowProvided(p_page_select_product);
    }
}

void page_buyBottle::on_pushButton_to_idle_clicked()
{
    qDebug() << "Back to Idle Page Button pressed";
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_buyBottle::on_pushButton_help_page_clicked()
{
    qDebug() << "Help_Button pressed";
    hideCurrentPageAndShowProvided(p_page_help);
}