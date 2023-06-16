#include "page_maintenance.h"
#include "ui_page_maintenance.h"
// #include <QProcess>

#include "page_idle.h"

int select1 = 1;
int select2 = 2;
int select3 = 3;
int select4 = 4;
int selection = 0;

// CTOR
page_maintenance::page_maintenance(QWidget *parent) : QWidget(parent),
                                                      ui(new Ui::page_maintenance)
{

    QPalette palette;
    palette.setBrush(QPalette::Background, Qt::white);
    this->setPalette(palette);

    // Fullscreen background setup
    ui->setupUi(this);

    // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);

    page_maintenanceEndTimer = new QTimer(this);
    page_maintenanceEndTimer->setInterval(1000);
    connect(page_maintenanceEndTimer, SIGNAL(timeout()), this, SLOT(onPage_maintenanceTimeoutTick()));

    pushButtons_product;
    pushButtons_product;
    pushButtons_product;
    pushButtons_product;

    labels_product_status[0] = ui->product1_overlay_label;
    labels_product_status[1] = ui->product2_overlay_label;
    labels_product_status[2] = ui->product3_overlay_label;
    labels_product_status[3] = ui->product4_overlay_label;
}

// DTOR
page_maintenance::~page_maintenance()
{
    delete ui;
}

void page_maintenance::showEvent(QShowEvent *event)
{
    p_page_idle->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_MAINTENANCE_BACKGROUND_PATH); // delays the page loading significantly.

    QString title = QString("Soapstand UI v%1").arg(UI_VERSION);
    ui->label_ui_version->setText(title);

    QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_CSS);
    p_page_idle->setTemplateTextToObject(ui->label_title_maintenance_mode);

    for (uint8_t slot_index = 0; slot_index < SLOT_COUNT; slot_index++)
    {
        labels_product_status[slot_index]->setObjectName("labels_product_status");
        labels_product_status[slot_index]->setStyleSheet(styleSheet);

        QString p = p_page_idle->products[slot_index].getProductPicturePath();
        p_page_idle->dfUtility->pathExists(p);
        QPixmap im(p);
        QIcon qi(im);
        pushButtons_products[slot_index]->setIcon(qi);

        pushButtons_product");
        pushButtons_products[slot_index]->setStyleSheet(styleSheet);
        pushButtons_products[slot_index]->setIconSize(QSize(241, 341));

        labels_product_status[slot_index]->show();
        pushButtons_products[slot_index]->setStyleSheet("background-color: transparent; border: 1px solid black;");
    }

    _page_maintenanceTimeoutSec = PAGE_MAINTENANCE_TIMEOUT_SECONDS;

    ui->label_machine_id->setText("Machine ID: " + p_page_idle->thisMachine.getMachineId());

    ui->label_product1->setText(p_page_idle->products[0].getProductName());
    ui->label_product2->setText(p_page_idle->products[1].getProductName());
    ui->label_product3->setText(p_page_idle->products[2].getProductName());
    ui->label_product4->setText(p_page_idle->products[3].getProductName());

    for (uint8_t slot_index = 0; slot_index < SLOT_COUNT; slot_index++)
    {
        int product_slot_enabled = p_page_idle->products[slot_index].getSlotEnabled();
        QString product_status_text = p_page_idle->products[slot_index].getStatusText();

        if (!(p_page_idle->products[slot_index].isProductVolumeInContainer()))
        {
            p_page_idle->setTemplateTextWithIdentifierToObject(labels_product_status[slot_index], "auto_empty");
        }
        else if (!p_page_idle->products[slot_index].getSlotEnabled())
        {
             p_page_idle->setTemplateTextWithIdentifierToObject(labels_product_status[slot_index], "not_enabled");
        }
        else if (product_status_text.compare("SLOT_STATE_AVAILABLE") == 0)
        {
            labels_product_status[slot_index]->setText("");
        }
        else if (product_status_text.compare("SLOT_STATE_AVAILABLE_LOW_STOCK") == 0)
        {
            p_page_idle->setTemplateTextWithIdentifierToObject(labels_product_status[slot_index], "almost_empty");
        }
        else if (product_status_text.compare("SLOT_STATE_PROBLEM_EMPTY") == 0)
        {
            p_page_idle->setTemplateTextWithIdentifierToObject(labels_product_status[slot_index], "sold_out");
        }
        else if (product_status_text.compare("SLOT_STATE_DISABLED_COMING_SOON") == 0)
        {
            p_page_idle->setTemplateTextWithIdentifierToObject(labels_product_status[slot_index], "coming_soon");
        }
        else if (product_status_text.compare("SLOT_STATE_PROBLEM_NEEDS_ATTENTION") == 0)
        {
            p_page_idle->setTemplateTextWithIdentifierToObject(labels_product_status[slot_index], "assistance");
        }
        else if (product_status_text.compare("SLOT_STATE_PROBLEM_EMPTY") == 0)
        {
            p_page_idle->setTemplateTextWithIdentifierToObject(labels_product_status[slot_index], "empty");
        }
        else
        {
            p_page_idle->setTemplateTextWithIdentifierToObject(labels_product_status[slot_index], "default");
        }

        ui->pushButton_to_previous_page->setProperty("class", "normal");
        ui->pushButton_general_settings->setProperty("class", "normal");
        ui->pushButton_to_previous_page->setStyleSheet(styleSheet);
        ui->pushButton_general_settings->setStyleSheet(styleSheet);
        ui->label_title_maintenance_mode->setStyleSheet(styleSheet);
        ui->label_machine_id->setProperty("class", "label_machine_ui");
        ui->label_machine_id->setStyleSheet(styleSheet);
        ui->label_ui_version->setProperty("class", "label_machine_ui");
        ui->label_ui_version->setStyleSheet(styleSheet);

        ui->label_product1_position->setProperty("class", "label_product_position");
        ui->label_product1_position->setStyleSheet(styleSheet);
        ui->label_product2_position->setProperty("class", "label_product_position");
        ui->label_product2_position->setStyleSheet(styleSheet);
        ui->label_product3_position->setProperty("class", "label_product_position");
        ui->label_product3_position->setStyleSheet(styleSheet);
        ui->label_product4_position->setProperty("class", "label_product_position");
        ui->label_product4_position->setStyleSheet(styleSheet);

        // ui->pushButton_to_previous_page->setText("<-Back");
        p_page_idle->setTemplateTextToObject(ui->pushButton_to_previous_page);
        p_page_idle->setTemplateTextToObject(ui->pushButton_general_settings);
        p_page_idle->setTemplateTextToObject(ui->label_product1_position);
        p_page_idle->setTemplateTextToObject(ui->label_product2_position);
        p_page_idle->setTemplateTextToObject(ui->label_product3_position);
        p_page_idle->setTemplateTextToObject(ui->label_product4_position);
        // ui->label_product1_position->setText("P1");
        // ui->label_product2_position->setText("P2");
        // ui->label_product3_position->setText("P3");
        // ui->label_product4_position->setText("P4");
        // p_pageSelectProduct->cancelTimers();
        // p_page_product->cancelTimers();
        qDebug() << "End maintenance load";
    }
}

/*
 * Page Tracking reference
 */
void page_maintenance::setPage(page_idle *pageIdle, page_maintenance_dispenser *p_pageMaintenanceDispenser, page_maintenance_general *p_pageMaintenanceGeneral, page_select_product *p_page_product, page_product *pagePaySelect)
{
    this->p_page_idle = pageIdle;
    this->p_page_maintenance_product = p_pageMaintenanceDispenser;
    this->p_page_maintenance_general = p_pageMaintenanceGeneral;
    this->p_pageSelectProduct = p_page_product;
    this->p_page_product = pagePaySelect;
}

void page_maintenance::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    page_maintenanceEndTimer->stop();
    p_page_idle->pageTransition(this, pageToShow);
}

void page_maintenance::onPage_maintenanceTimeoutTick()
{

    if (--_page_maintenanceTimeoutSec >= 0)
    {
    }
    else
    {
        hideCurrentPageAndShowProvided(p_page_idle);
    }
}

void page_maintenance::on_pushButton_general_settings_clicked()
{
    hideCurrentPageAndShowProvided(p_page_maintenance_general);
}

void page_maintenance::on_pushButton_to_previous_page_clicked()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_maintenance::on_pushButtons_product_clicked()
{
    p_page_idle->setSelectedProduct(1);
    hideCurrentPageAndShowProvided(p_page_maintenance_product);
}

void page_maintenance::on_pushButtons_product_clicked()
{
    p_page_idle->setSelectedProduct(2);
    hideCurrentPageAndShowProvided(p_page_maintenance_product);
}

void page_maintenance::on_pushButtons_product_clicked()
{
    p_page_idle->setSelectedProduct(3);
    hideCurrentPageAndShowProvided(p_page_maintenance_product);
}

void page_maintenance::on_pushButtons_product_clicked()
{
    p_page_idle->setSelectedProduct(4);
    hideCurrentPageAndShowProvided(p_page_maintenance_product);
}

int getSelection()
{
    return selection;
}
