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
    // Fullscreen background setup
    ui->setupUi(this);

    // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, Qt::white);
    this->setPalette(palette);

    page_maintenanceEndTimer = new QTimer(this);
    page_maintenanceEndTimer->setInterval(1000);
    connect(page_maintenanceEndTimer, SIGNAL(timeout()), this, SLOT(onPage_maintenanceTimeoutTick()));

    pushButtons_products[0] = ui->pushButton_product_1;
    pushButtons_products[1] = ui->pushButton_product_2;
    pushButtons_products[2] = ui->pushButton_product_3;
    pushButtons_products[3] = ui->pushButton_product_4;

    labels_slot_status[0] = ui->label_product_1_status;
    labels_slot_status[1] = ui->label_product_2_status;
    labels_slot_status[2] = ui->label_product_3_status;
    labels_slot_status[3] = ui->label_product_4_status;

    labels_product_name[0] = ui->label_product_1_name;
    labels_product_name[1] = ui->label_product_2_name;
    labels_product_name[2] = ui->label_product_3_name;
    labels_product_name[3] = ui->label_product_4_name;

    labels_product_position[0] = ui->label_product_1_position;
    labels_product_position[1] = ui->label_product_2_position;
    labels_product_position[2] = ui->label_product_3_position;
    labels_product_position[3] = ui->label_product_4_position;

    statusbarLayout = new QVBoxLayout(this);
}

// DTOR
page_maintenance::~page_maintenance()
{
    delete ui;
}

void page_maintenance::showEvent(QShowEvent *event)
{
    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    statusbarLayout->addWidget(p_statusbar);            // Only one instance can be shown. So, has to be added/removed per page.
    statusbarLayout->setContentsMargins(0, 1874, 0, 0); // int left, int top, int right, int bottom);

    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this);                       // this is the 'page', the central or main widget
    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_MAINTENANCE_BACKGROUND_PATH); // delays the page loading significantly.

    QString qr_manual_full_path = p_page_idle->thisMachine->getTemplatePathFromName(QR_MANUAL_PATH);
    p_page_idle->thisMachine->addPictureToLabel(ui->label_qr_user_manual, qr_manual_full_path);
    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_MAINTENANCE_CSS);
    ui->pushButton_to_previous_page->setStyleSheet(styleSheet);
    ui->pushButton_general_settings->setStyleSheet(styleSheet);
    ui->label_title_maintenance_mode->setStyleSheet(styleSheet);

    ui->label_qr_manual_description->setProperty("class", "label_qr_user_manual");
    ui->label_qr_user_manual->setStyleSheet(styleSheet);
    ui->label_qr_manual_description->setProperty("class", "label_qr");
    ui->label_qr_manual_description->setStyleSheet(styleSheet);
    ui->label_machine_id->setProperty("class", "label_machine_ui");
    ui->label_machine_id->setStyleSheet(styleSheet);

    ui->label_ui_version->setProperty("class", "label_machine_ui");
    ui->label_ui_version->setStyleSheet(styleSheet);

    ui->label_role->setProperty("class", "label_machine_ui");
    ui->label_role->setStyleSheet(styleSheet);

    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_qr_manual_description);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_title_maintenance_mode);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_to_previous_page);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_general_settings);

    ui->label_machine_id->setText("Machine ID: " + p_page_idle->thisMachine->getMachineId());

    QString role_as_text = p_page_idle->thisMachine->getActiveRoleAsText();
    p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_role, role_as_text);

    QString title = QString("Soapstand UI v%1").arg(UI_VERSION);
    ui->label_ui_version->setText(title);

    uint8_t slot_count;

    // slot_count = p_page_idle->thisMachine->getSlotCount();

    // if (slot_count > MAINTENANCE_PAGE_SLOT_COUNT_MAX){
    //         slot_count = MAINTENANCE_PAGE_SLOT_COUNT_MAX;
    // }

    for (uint8_t slot_index = 0; slot_index < MAINTENANCE_PAGE_SLOT_COUNT_MAX; slot_index++)
    {

        p_page_idle->thisMachine->setTemplateTextToObject(labels_product_position[slot_index]);

        labels_slot_status[slot_index]->setProperty("class", "label_product_status");
        labels_slot_status[slot_index]->setStyleSheet(styleSheet);

        labels_product_position[slot_index]->setProperty("class", "label_product_position");
        labels_product_position[slot_index]->setStyleSheet(styleSheet);

        QString p = p_page_idle->thisMachine->getSlotBaseProduct(slot_index + 1)->getProductPicturePath();
        p_page_idle->thisMachine->dfUtility->pathExists(p);
        QPixmap im(p);
        QIcon qi(im);
        pushButtons_products[slot_index]->setIcon(qi);
        pushButtons_products[slot_index]->setStyleSheet(styleSheet);
        pushButtons_products[slot_index]->setIconSize(QSize(241, 341));

        pushButtons_products[slot_index]->setStyleSheet("background-color: transparent; border: 1px solid black;");
        pushButtons_products[slot_index]->raise();

        labels_product_name[slot_index]->setText(p_page_idle->thisMachine->getSlotBaseProduct(slot_index + 1)->getProductName());

        SlotState slot_state = p_page_idle->thisMachine->getSlotByPosition(slot_index + 1)->getSlotStatus();
        QString status_display_text = "";

        if (p_page_idle->thisMachine->getSlotByPosition(slot_index + 1)->isOnlyOneProductAvailable())
        {
            ProductState base_product_state = p_page_idle->thisMachine->getSlotBaseProduct(slot_index + 1)->getProductState();

            switch (base_product_state)
            {

            case PRODUCT_STATE_AVAILABLE:
            {
                break;
            }
            case PRODUCT_STATE_AVAILABLE_LOW_STOCK:
            {
                status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->almost_empty");
                break;
            }
            case PRODUCT_STATE_PROBLEM_EMPTY:
            {
                status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->assistance");
                break;
            }
            case PRODUCT_STATE_DISABLED:
            {
                status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->assistance");
                break;
            }
            case PRODUCT_STATE_INVALID:
            {
                status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->default");
                break;
            }
            default:
            {
                break;
            }
            }
        }

        switch (slot_state)
        {
        case (SLOT_STATE_DISABLED):
        {
            status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->slot_not_enabled");
            break;
        }
        case (SLOT_STATE_AVAILABLE):
        {
            // status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->available");
            break;
        }
        case (SLOT_STATE_PROBLEM_NEEDS_ATTENTION):
        {
            status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->slot_assistance");
            break;
        }
        default:
        {
            status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->default");
            break;
        }
        }

        labels_slot_status[slot_index]->setText(status_display_text);

        // if (!(p_page_idle->thisMachine->isProductVolumeInContainer(pnumber)))
        // {
        //     status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->auto_empty");
        // }
        // else if (!p_page_idle->thisMachine->getSlotByPosition(slot_index+1)->getIsSlotEnabled())
        // {
        //     status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->not_enabled");
        // }
        // else if (product_status_text.compare("SLOT_STATE_AVAILABLE") == 0)
        // {
        //     status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->available");
        // }
        // else if (product_status_text.compare("SLOT_STATE_AVAILABLE_LOW_STOCK") == 0)
        // {
        //     status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->almost_empty");
        // }
        // else if (product_status_text.compare("SLOT_STATE_DISABLED_COMING_SOON") == 0)
        // {
        //     status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->coming_soon");
        // }
        // else if (product_status_text.compare("SLOT_STATE_PROBLEM_NEEDS_ATTENTION") == 0)
        // {
        //     status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->assistance");
        // }
        // else if (product_status_text.compare("SLOT_STATE_PROBLEM_EMPTY") == 0)
        // {
        //     status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->empty");
        // }
        // else
        // {
        //     status_display_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "status_text->default");
        // }

        if (!p_page_idle->thisMachine->isSlotExisting(slot_index + 1))
        {
            labels_product_name[slot_index]->hide();
            labels_slot_status[slot_index]->hide();
            pushButtons_products[slot_index]->hide();
            labels_product_position[slot_index]->hide();
        }
    }
    _page_maintenanceTimeoutSec = PAGE_MAINTENANCE_TIMEOUT_SECONDS;
    page_maintenanceEndTimer->start();
    qDebug() << "End maintenance load";
}

/*
 * Page Tracking reference
 */
void page_maintenance::setPage(page_idle *pageIdle, page_maintenance_dispenser *p_pageMaintenanceDispenser, page_maintenance_general *p_pageMaintenanceGeneral, page_select_product *p_page_product, page_product *pagePaySelect, statusbar *p_statusbar)
{
    this->p_page_idle = pageIdle;
    this->p_page_maintenance_dispenser = p_pageMaintenanceDispenser;
    this->p_page_maintenance_general = p_pageMaintenanceGeneral;
    this->p_pageSelectProduct = p_page_product;
    this->p_page_product = pagePaySelect;
    this->p_statusbar = p_statusbar;
}

void page_maintenance::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    page_maintenanceEndTimer->stop();
    statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.
    p_page_idle->thisMachine->pageTransition(this, pageToShow);
}

void page_maintenance::onPage_maintenanceTimeoutTick()
{
    if (--_page_maintenanceTimeoutSec >= 0)
    {
        // qDebug() << "page_maintenance: Tick Down - " << _page_maintenanceTimeoutSec;
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

void page_maintenance::on_pushButton_product_1_clicked()
{
    // p_page_idle->thisMachine->setSelectedProduct(0);
    qDebug() << "Clicked slot 1";
    p_page_idle->thisMachine->setSelectedSlot(1);
    hideCurrentPageAndShowProvided(p_page_maintenance_dispenser);
}

void page_maintenance::on_pushButton_product_2_clicked()
{
    // p_page_idle->thisMachine->setSelectedProduct(0);
    p_page_idle->thisMachine->setSelectedSlot(2);
    hideCurrentPageAndShowProvided(p_page_maintenance_dispenser);
}

void page_maintenance::on_pushButton_product_3_clicked()
{
    // p_page_idle->thisMachine->setSelectedProduct(0);
    p_page_idle->thisMachine->setSelectedSlot(3);
    hideCurrentPageAndShowProvided(p_page_maintenance_dispenser);
}

void page_maintenance::on_pushButton_product_4_clicked()
{
    p_page_idle->thisMachine->setSelectedSlot(4);
    // p_page_idle->thisMachine->setSelectedProduct(0);
    hideCurrentPageAndShowProvided(p_page_maintenance_dispenser);
}

int getSelection()
{
    return selection;
}

// void page_maintenance::on_label_qr_user_manual_linkActivated(const QString &link)
// {

// }
