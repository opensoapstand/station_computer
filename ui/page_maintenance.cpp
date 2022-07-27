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

    


#ifdef ENABLE_DYNAMIC_UI

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

#else
    QPixmap background1(PRODUCT_1_IMAGE_PATH);
    QIcon ButtonIcon1(background1);

    QPixmap background2(PRODUCT_2_IMAGE_PATH);
    QIcon ButtonIcon2(background2);

    QPixmap background3(PRODUCT_3_IMAGE_PATH);
    QIcon ButtonIcon3(background3);

    QPixmap background4(PRODUCT_4_IMAGE_PATH);
    QIcon ButtonIcon4(background4);

    ui->product1_button->setIcon(ButtonIcon1);
    ui->product1_button->setIconSize(QSize(241, 381));

    ui->product2_button->setIcon(ButtonIcon2);
    ui->product2_button->setIconSize(QSize(241, 381));

    ui->product3_button->setIcon(ButtonIcon3);
    ui->product3_button->setIconSize(QSize(241, 381));

    ui->product4_button->setIcon(ButtonIcon4);
    ui->product4_button->setIconSize(QSize(241, 381));

#endif
    if (page_maintenanceEndTimer == nullptr)
    {
        page_maintenanceEndTimer = new QTimer(this);
        page_maintenanceEndTimer->setInterval(1000);
        connect(page_maintenanceEndTimer, SIGNAL(timeout()), this, SLOT(onPage_maintenanceTimeoutTick()));
    }

    // page_maintenanceEndTimer->start(1000);
    _page_maintenanceTimeoutSec = PAGE_MAINTENANCE_TIMEOUT_SECONDS;

    qDebug() << "db for names and id";
    DbManager db(DB_PATH);
    // ui->enable_empty_container_checkBox->setChecked(db.getEmptyContainerDetectionEnabled());
    // ui->enable_pump_ramping_checkBox->setChecked(db.getPumpRampingEnabled());
    // qDebug()<<"ramping?"<<db.getPumpRampingEnabled();
    // ui->enable_pump_ramping_checkBox->setChecked(true);

    ui->product1_label->setText(db.getProductName(1));
    ui->product2_label->setText(db.getProductName(2));
    ui->product3_label->setText(db.getProductName(3));
    ui->product4_label->setText(db.getProductName(4));
    ui->machineLabel->setText("Machine ID: " + db.getMachineID());
    db.closeDB();




    p_pageSelectProduct->cancelTimers();
    p_pageProduct->cancelTimers();
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

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_MAINTENANCE_BACKGROUND_PATH);
    
}

void page_maintenance::onPage_maintenanceTimeoutTick()
{

    if (--_page_maintenanceTimeoutSec >= 0)
    {
        //        qDebug() << "Page_page_maintenance Tick Down: " << _page_maintenanceTimeoutSec << endl;
    }
    else
    {
        //        qDebug() << "Page_page_maintenance Timer Done!" << _page_maintenanceTimeoutSec << endl;

        page_maintenanceEndTimer->stop();
        // qDebug() << "maintenance to idle";
         // p_page_maintenance_product->showFullScreen();
        // this->hide();
        p_page_idle->pageTransition(this, p_page_idle);
    }
}

void page_maintenance::on_generalSettings_button_clicked()
{
    page_maintenanceEndTimer->stop();
    // p_page_maintenance_general->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_maintenance_general);
}

void page_maintenance::on_backButton_clicked()
{
    page_maintenanceEndTimer->stop();
    // p_page_idle->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_idle);
}

void page_maintenance::on_product1_button_clicked()
{
    //    qDebug() << "Product 1 button clicked" << endl;
    page_maintenanceEndTimer->stop();

    p_page_idle->currentProductOrder->setSelectedSlot(1);
    p_page_idle->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);

    p_page_maintenance_product->resizeEvent(productSelection);
    // p_page_maintenance_product->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_maintenance_product);
}

void page_maintenance::on_product2_button_clicked()
{
    //    qDebug() << "Product 2 button clicked" << endl;
    page_maintenanceEndTimer->stop();

    p_page_idle->currentProductOrder->setSelectedSlot(2);
    p_page_idle->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);

    p_page_maintenance_product->resizeEvent(productSelection);
    // p_page_maintenance_product->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_maintenance_product);
}

void page_maintenance::on_product3_button_clicked()
{
    //    qDebug() << "Product 3 button clicked" << endl;
    page_maintenanceEndTimer->stop();

    p_page_idle->currentProductOrder->setSelectedSlot(3);
    p_page_idle->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);

    p_page_maintenance_product->resizeEvent(productSelection);
    // p_page_maintenance_product->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_maintenance_product);
}

void page_maintenance::on_product4_button_clicked()
{
    //    qDebug() << "Product 4 button clicked" << endl;
    page_maintenanceEndTimer->stop();

    p_page_idle->currentProductOrder->setSelectedSlot(4);
    p_page_idle->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);

    p_page_maintenance_product->resizeEvent(productSelection);
    // p_page_maintenance_product->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_maintenance_product);
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


