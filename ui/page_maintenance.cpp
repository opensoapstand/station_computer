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
    p_page_idle->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    // p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_MAINTENANCE_BACKGROUND_PATH); // delays the page loading significantly.
    QString styleSheet = p_page_idle->getCSS(PAGE_MAINTENANCE_CSS);

    for (int i = 0; i < 4; i++)
    {
        product_overlay_labels[i]->setStyleSheet(styleSheet);
    }

    for (int i = 0; i < SLOT_COUNT; i++)
    {
        QString p = p_page_idle->products[i].getProductPicturePath();
        p_page_idle->dfUtility->pathExists(p);
        QPixmap im(p);
        QIcon qi(im);
        product_buttons[i]->setIcon(qi);
        product_buttons[i]->setIconSize(QSize(281, 341));
        product_overlay_labels[i]->hide();
        product_buttons[i]->setStyleSheet("background-color: transparent; border: none;");

    }

    
    // page_maintenanceEndTimer->start(1000);
    _page_maintenanceTimeoutSec = PAGE_MAINTENANCE_TIMEOUT_SECONDS;

    ui->machineLabel->setText("Machine ID: " + p_page_idle->thisMachine.getMachineId());

    ui->product1_label->setText(p_page_idle->products[0].getProductName());
    ui->product2_label->setText(p_page_idle->products[1].getProductName());
    ui->product3_label->setText(p_page_idle->products[2].getProductName());
    ui->product4_label->setText(p_page_idle->products[3].getProductName());

    for (uint8_t slot_index = 0; slot_index < SLOT_COUNT; slot_index++)
    {
        int product_slot_enabled = p_page_idle->products[slot_index].getSlotEnabled();
        QString product_status_text = p_page_idle->products[slot_index].getStatusText();

        if (product_status_text.compare("SLOT_STATE_AVAILABLE") == 0)
        {
            product_overlay_labels[slot_index]->setText("");
        }
        else if (product_status_text.compare("SLOT_STATE_AVAILABLE_LOW_STOCK") == 0)
        {
            product_overlay_labels[slot_index]->setText("Almost Empty");
            // p_page_idle->setTemplateTextWithIdentifierToObject(product_overlay_labels[slot_index], "almost_empty");
        }
        else if (product_status_text.compare("SLOT_STATE_PROBLEM_EMPTY") == 0)
        {
            product_overlay_labels[slot_index]->setText("Sold Out");
        }
        else if (product_status_text.compare("SLOT_STATE_DISABLED_COMING_SOON") == 0)
        {
            product_overlay_labels[slot_index]->setText("Coming Soon");
            // p_page_idle->setTemplateTextWithIdentifierToObject(product_overlay_labels[slot_index], "coming_soon");
        }
        else if (product_status_text.compare("SLOT_STATE_PROBLEM_NEEDS_ATTENTION") == 0)

        {
            product_overlay_labels[slot_index]->setText("Assistance Needed");
        }
        else
        {
            product_overlay_labels[slot_index]->setText("Assistance Needed");
        }

        if (!(p_page_idle->products[slot_index].isProductVolumeInContainer()))
        {
            product_overlay_labels[slot_index]->setText("Auto Sold Out");
        }

        // // overlay product status
        // if (!product_slot_enabled)
        // {
        //     product_overlay_labels[slot_index]->setText(product_status_text);
        //     product_overlay_labels[slot_index]->setStyleSheet("background-color: rgba(255,255,255,170);");
        //     // labels_product_picture[slot_index]->setStyleSheet("Qlabel {background-color: rgba(255,255,255,127);}");
        // }
        // // else if (product_sold_out)
        // // {
        // //     product_overlay_labels[slot_index]->setText("Sold out");
        // //     product_overlay_labels[slot_index]->setStyleSheet("background-color: transparent;");
        // //     // labels_product_picture[slot_index]->setStyleSheet("Qlabel {background-color: rgba(255,255,255,127);}");
        // // }
        // else
        // {
        // product_overlay_labels[slot_index]->setText("");
        // product_buttons[slot_index]->setStyleSheet("QPushButton {background-color: transparent; border: 0px }");
        // }
    }
    // p_pageSelectProduct->cancelTimers();
    // p_page_product->cancelTimers();
    qDebug() << "End maintenance load";
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

void page_maintenance::on_generalSettings_button_clicked()
{
    hideCurrentPageAndShowProvided(p_page_maintenance_general);
}

void page_maintenance::on_pushButton_to_previous_page_clicked()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_maintenance::on_product1_button_clicked()
{
    p_page_idle->setSelectedProduct(1);
    p_page_maintenance_product->resizeEvent(productSelection);
    hideCurrentPageAndShowProvided(p_page_maintenance_product);
}

void page_maintenance::on_product2_button_clicked()
{
    p_page_idle->setSelectedProduct(2);
    p_page_maintenance_product->resizeEvent(productSelection);
    hideCurrentPageAndShowProvided(p_page_maintenance_product);
}

void page_maintenance::on_product3_button_clicked()
{
    p_page_idle->setSelectedProduct(3);
    p_page_maintenance_product->resizeEvent(productSelection);
    hideCurrentPageAndShowProvided(p_page_maintenance_product);
}

void page_maintenance::on_product4_button_clicked()
{
    p_page_idle->setSelectedProduct(4);
    p_page_maintenance_product->resizeEvent(productSelection);
    hideCurrentPageAndShowProvided(p_page_maintenance_product);
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
