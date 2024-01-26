#include "page_how_to.h"
#include "ui_page_how_to.h"
#include "page_help.h"
#include "input_widget.h"
#include "df_util.h"
#include "page_select_product.h"
#include "page_product.h"
#include "page_product_mixing.h"
#include "page_qr_payment.h"
#include "page_idle.h"

page_how_to::page_how_to(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_how_to)
{
    ui->setupUi(this);
    helpIdleTimer = new QTimer(this);
    helpIdleTimer->setInterval(1000);
    connect(helpIdleTimer, SIGNAL(timeout()), this, SLOT(onHelpTimeoutTick()));

    statusbarLayout = new QVBoxLayout(this);
}

page_how_to::~page_how_to()
{
    delete ui;
}

void page_how_to::setPage(page_help *pageHelp, page_idle *pageIdle, page_transactions *pageTransactions, page_maintenance *pageMaintenance, page_sendFeedback *pageFeedback, statusbar *p_statusbar, keyboard *keyboard, input_widget *input_widget)
{
    this->p_page_help = pageHelp;
    this->p_page_idle = pageIdle;
    this->p_page_feedback = pageFeedback;
    this->p_page_transactions = pageTransactions;
    this->p_page_maintenance = pageMaintenance;
    this->p_statusbar = p_statusbar;
    this->p_keyboard = keyboard;
    this->p_input_widget = input_widget;
}

void page_how_to::showEvent(QShowEvent *event)
{
    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);
    helpIdleTimer->start(1000);
    _helpIdleTimeoutSec = 60;
    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_HOWTO_CSS);
    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_HOWTO_BACKGROUND_PATH);
    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget

    ui->label_step01_heading->setProperty("class", "setpHeadings");
    ui->label_step02_heading->setProperty("class", "setpHeadings");
    ui->label_step03_heading->setProperty("class", "setpHeadings");

    ui->label_step01_image->setProperty("class", "stepImages");
    ui->label_step02_image->setProperty("class", "stepImages");
    ui->label_step03_image->setProperty("class", "stepImages");

    ui->label_step01_instruction->setProperty("class", "setpInstruction");
    ui->label_step02_instruction->setProperty("class", "setpInstruction");
    ui->label_step03_instruction->setProperty("class", "setpInstruction");
    ui->label_step01_instruction->setWordWrap(true);
    ui->label_step02_instruction->setWordWrap(true);
    ui->label_step03_instruction->setWordWrap(true);

    ui->pushButton_to_feedback->setProperty("class", "featureButtons");
    ui->pushButton_to_maintenance->setProperty("class", "featureButtons");
    ui->pushButton_resetTimeout->setProperty("class", "buttonBGTransparent");

    QString image_path_1 = p_page_idle->thisMachine->getTemplatePathFromName(PAGE_HOWTO_STEP1);
    styleSheet.replace("%IMAGE_PATH1%", image_path_1);
    QString image_path_2 = p_page_idle->thisMachine->getTemplatePathFromName(PAGE_HOWTO_STEP2);
    styleSheet.replace("%IMAGE_PATH2%", image_path_2);
    QString image_path_3 = p_page_idle->thisMachine->getTemplatePathFromName(PAGE_HOWTO_STEP3);
    styleSheet.replace("%IMAGE_PATH3%", image_path_3);

    ui->label_page_title->setStyleSheet(styleSheet);
    ui->label_step01_heading->setStyleSheet(styleSheet);
    ui->label_step01_image->setStyleSheet(styleSheet);
    ui->label_step01_instruction->setStyleSheet(styleSheet);
    ui->label_step02_heading->setStyleSheet(styleSheet);
    ui->label_step02_image->setStyleSheet(styleSheet);
    ui->label_step02_instruction->setStyleSheet(styleSheet);
    ui->label_step03_heading->setStyleSheet(styleSheet);
    ui->label_step03_image->setStyleSheet(styleSheet);
    ui->label_step03_instruction->setStyleSheet(styleSheet);
    ui->pushButton_to_feedback->setStyleSheet(styleSheet);
    ui->pushButton_to_help->setStyleSheet(styleSheet);
    ui->pushButton_to_maintenance->setStyleSheet(styleSheet);
    ui->pushButton_resetTimeout->setStyleSheet(styleSheet);

    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_page_title);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_step01_heading);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_step01_instruction);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_step02_heading);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_step02_instruction);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_step03_heading);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_step03_instruction);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_to_feedback);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_to_help);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_to_maintenance);

    p_keyboard->initializeKeyboard(false, p_input_widget->findChild<QLineEdit *>("lineEdit_input"));
    p_input_widget->toggleInputWidget(false);
    statusbarLayout->removeWidget(p_keyboard);    
    statusbarLayout->removeWidget(p_input_widget);  

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
}

void page_how_to::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    helpIdleTimer->stop();
    statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.

    p_page_idle->thisMachine->pageTransition(this, pageToShow);

}

void page_how_to::on_pushButton_to_maintenance_clicked()
{
    _helpIdleTimeoutSec = 60;

    if (p_page_idle->thisMachine->isAllowedAsMaintainer())
    {
        // if already logged in, go straight to maintenance mode.
        hideCurrentPageAndShowProvided(p_page_maintenance);
    }
    else
    {
        p_keyboard->registerCallBack(std::bind(&page_how_to::doneButtonPressed, this));
        p_keyboard->initializeKeyboard(true, p_input_widget->findChild<QLineEdit *>("lineEdit_input"));
        p_input_widget->toggleInputWidget(true);
    }
}

void page_how_to::doneButtonPressed(){
    QString textEntry = p_input_widget->findChild<QLineEdit *>("lineEdit_input")->text();

    // if role was already set, do not check pwd.
    if (!p_page_idle->thisMachine->isAllowedAsMaintainer())
    {
        p_page_idle->thisMachine->processRolePassword(textEntry);

        if (p_page_idle->thisMachine->isAllowedAsMaintainer())
        {
            hideCurrentPageAndShowProvided(p_page_maintenance);
            p_keyboard->initializeKeyboard(false, p_input_widget->findChild<QLineEdit *>("lineEdit_input"));
        }
        else
        {
            p_input_widget->findChild<QLineEdit *>("lineEdit_input")->setText("");
            p_keyboard->initializeKeyboard(true, p_input_widget->findChild<QLineEdit *>("lineEdit_input"));
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

void page_how_to::on_pushButton_to_help_clicked()
{
    hideCurrentPageAndShowProvided(p_page_help);
}

void page_how_to::onHelpTimeoutTick()
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

void page_how_to::on_pushButton_resetTimeout_clicked()
{
    _helpIdleTimeoutSec = 60;
}

void page_how_to::on_pushButton_to_feedback_clicked()
{
    hideCurrentPageAndShowProvided(p_page_feedback);
}
