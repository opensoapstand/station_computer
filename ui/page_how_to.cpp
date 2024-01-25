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

    statusbarLayout = new QVBoxLayout(this);
}

page_how_to::~page_how_to()
{
    delete ui;
}

void page_how_to::setPage(page_select_product *pageSelect, page_product *page_product, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_transactions *pageTransactions, page_maintenance *pageMaintenance, page_sendFeedback *pageFeedback, statusbar *p_statusbar, keyboard *keyboard, input_widget *input_widget)
{
    this->p_page_idle = pageIdle;
    this->p_page_feedback = pageFeedback;
    this->p_page_payment = page_qr_payment;
    this->p_page_product = page_product;
    this->p_page_select_product = pageSelect;
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
    // Can set number of steps here for image path
    number_of_steps = 3;
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

    QString increment_text = "%IMAGE_PATH%1%";
    QString image_path_for_position = increment_text.arg(1);
    QString image_path_1 = p_page_idle->thisMachine->getTemplatePathFromName(PAGE_HOWTO_STEP1);
    styleSheet.replace(image_path_for_position, image_path_1);

    // image_path_for_position = increment_text.arg(2);
    // QString image_path_2 = p_page_idle->thisMachine->getTemplatePathFromName(PAGE_HOWTO_STEP2);
    // styleSheet.replace(image_path_for_position, image_path_2);

    // image_path_for_position = increment_text.arg(3);
    // QString image_path_3 = p_page_idle->thisMachine->getTemplatePathFromName(PAGE_HOWTO_STEP3);
    // styleSheet.replace(image_path_for_position, image_path_3);

}
