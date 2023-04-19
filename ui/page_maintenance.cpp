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

    product_buttons[0] = ui->product1_button;
    product_buttons[1] = ui->product2_button;
    product_buttons[2] = ui->product3_button;
    product_buttons[3] = ui->product4_button;

    product_overlay_labels[0] = ui->product1_overlay_label;
    product_overlay_labels[1] = ui->product2_overlay_label;
    product_overlay_labels[2] = ui->product3_overlay_label;
    product_overlay_labels[3] = ui->product4_overlay_label;

    QString title = QString("Soapstand UI v%1").arg(UI_VERSION);
    ui->label_ui_version->setText(title);
}

// DTOR
page_maintenance::~page_maintenance()
{
    delete ui;
}

void page_maintenance::showEvent(QShowEvent *event)
{
    //    db.addPageClick("PAGE_PAGE_MAINTENANCE PAGE ENTERED");
    QWidget::showEvent(event);
    qDebug() << "<<<<<<< Page Enter: maintenance >>>>>>>>>";

    // p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_MAINTENANCE_BACKGROUND_PATH);
    // p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_HELP_BACKGROUND_PATH);

    for (int i = 0; i < SLOT_COUNT; i++)
    {
        QString p = p_page_idle->currentProductOrder->getProductPicturePath(i + 1);
        p_page_idle->dfUtility->fileExists(p);
        QPixmap im(p);
        QIcon qi(im);
        product_buttons[i]->setIcon(qi);
        product_buttons[i]->setIconSize(QSize(241, 381));
        product_overlay_labels[i]->hide();
    }

    if (page_maintenanceEndTimer == nullptr)
    {
        page_maintenanceEndTimer = new QTimer(this);
        page_maintenanceEndTimer->setInterval(1000);
        connect(page_maintenanceEndTimer, SIGNAL(timeout()), this, SLOT(onPage_maintenanceTimeoutTick()));
    }

    // page_maintenanceEndTimer->start(1000);
    _page_maintenanceTimeoutSec = PAGE_MAINTENANCE_TIMEOUT_SECONDS;

    // int product_sold_out [SLOT_COUNT];
    // bool product_slot_enabled [SLOT_COUNT];

    qDebug() << "db for names and id maintenance";
    DbManager db(DB_PATH);
    // ui->enable_empty_container_checkBox->setChecked(db.getEmptyContainerDetectionEnabled());
    // ui->enable_pump_ramping_checkBox->setChecked(db.getPumpRampingEnabled());
    // qDebug()<<"ramping?"<<db.getPumpRampingEnabled();
    // ui->enable_pump_ramping_checkBox->setChecked(true);

    // ui->product1_label->setText(db.getProductName(1));
    // ui->product2_label->setText(db.getProductName(2));
    // ui->product3_label->setText(db.getProductName(3));
    // ui->product4_label->setText(db.getProductName(4));
    ui->machineLabel->setText("Machine ID: " + db.getMachineID());

    db.closeDB();

    ui->product1_label->setText(p_page_idle->currentProductOrder->getProductName(1));
    ui->product2_label->setText(p_page_idle->currentProductOrder->getProductName(2));
    ui->product3_label->setText(p_page_idle->currentProductOrder->getProductName(3));
    ui->product4_label->setText(p_page_idle->currentProductOrder->getProductName(4));

    for (uint8_t i = 0; i < SLOT_COUNT; i++)
    {
        uint8_t slot = i + 1;
        qDebug() << "db for names and id maintenance";
        DbManager db(DB_PATH);
        int product_slot_enabled = db.getSlotEnabled(slot);
        bool product_sold_out = !(db.isProductVolumeInContainer(slot));
        QString product_status_text = db.getStatusText(slot);

        db.closeDB();

        // overlay product status
        if (!product_slot_enabled)
        {
            product_overlay_labels[i]->setText(product_status_text);
            product_overlay_labels[i]->setStyleSheet("background-color: rgba(255,255,255,170);");
            // selectProductPhotoLabels[i]->setStyleSheet("Qlabel {background-color: rgba(255,255,255,127);}");
        }
        else if (product_sold_out)
        {
            product_overlay_labels[i]->setText("Sold out");
            product_overlay_labels[i]->setStyleSheet("background-color: transparent;");
            // selectProductPhotoLabels[i]->setStyleSheet("Qlabel {background-color: rgba(255,255,255,127);}");
        }
        else
        {
            product_overlay_labels[i]->setText("");
            product_overlay_labels[i]->setStyleSheet("background-color: transparent;");

            // product_buttons[i]->setStyleSheet("QPushButton {background-color: transparent; border: 0px }");
        }
    }
    // p_pageSelectProduct->cancelTimers();
    // p_pageProduct->cancelTimers();
    qDebug() << "End maintenance load";
}

/*
 * Page Tracking reference
 */
void page_maintenance::setPage(page_idle *pageIdle, page_maintenance_dispenser *p_pageMaintenanceDispenser, page_maintenance_general *p_pageMaintenanceGeneral, page_select_product *p_pageProduct, pageProduct *pagePaySelect)
{
    this->p_page_idle = pageIdle;
    this->p_page_maintenance_product = p_pageMaintenanceDispenser;
    this->p_page_maintenance_general = p_pageMaintenanceGeneral;
    this->p_pageSelectProduct = p_pageProduct;
    this->p_pageProduct = pagePaySelect;
}

void page_maintenance::hidePage(QWidget *pageToShow)
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
        hidePage(p_page_idle);
    }
}

void page_maintenance::on_generalSettings_button_clicked()
{
    hidePage(p_page_maintenance_general);
}

void page_maintenance::on_backButton_clicked()
{
    hidePage(p_page_idle);
}

void page_maintenance::on_product1_button_clicked()
{
    p_page_idle->currentProductOrder->setSelectedSlot(1);
    p_page_idle->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);

    p_page_maintenance_product->resizeEvent(productSelection);
    hidePage(p_page_maintenance_product);
}

void page_maintenance::on_product2_button_clicked()
{
    p_page_idle->currentProductOrder->setSelectedSlot(2);
    p_page_idle->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);

    p_page_maintenance_product->resizeEvent(productSelection);
    hidePage(p_page_maintenance_product);
}

void page_maintenance::on_product3_button_clicked()
{
    p_page_idle->currentProductOrder->setSelectedSlot(3);
    p_page_idle->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);

    p_page_maintenance_product->resizeEvent(productSelection);
    hidePage(p_page_maintenance_product);
}

void page_maintenance::on_product4_button_clicked()
{
    p_page_idle->currentProductOrder->setSelectedSlot(4);
    p_page_idle->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);

    p_page_maintenance_product->resizeEvent(productSelection);
    hidePage(p_page_maintenance_product);
}

// void page_maintenance::on_product5_button_clicked(){
//     qDebug() << "Product 5 button clicked" << endl;

//}

// void page_maintenance::on_product6_button_clicked(){
//     qDebug() << "Product 6 button clicked" << endl;

//}

// void page_maintenance::on_product7_button_clicked(){
//     qDebug() << "Product 7 button clicked" << endl;

//}

// void page_maintenance::on_product8_button_clicked(){
//     qDebug() << "Product 8 button clicked" << endl;

//}

// void page_maintenance::on_product9_button_clicked(){
//     qDebug() << "Product 9 button clicked" << endl;

//}

int getSelection()
{
    return selection;
}
