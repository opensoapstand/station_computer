#include "maintenancePage.h"
#include "ui_maintenancePage.h"

#include "idle.h"

int select1 = 1;
int select2 = 2;
int select3 = 3;
int select4 = 4;
int selection = 0;

// CTOR
maintenancePage::maintenancePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::maintenancePage)
{
    // Fullscreen background setup
    ui->setupUi(this);
    QPixmap background1(":/maintenance/product1.png");
    QIcon ButtonIcon1(background1);

    QPixmap background2(":/maintenance/product2.png");
    QIcon ButtonIcon2(background2);

    QPixmap background3(":/maintenance/product3.png");
    QIcon ButtonIcon3(background3);

    QPixmap background4(":/maintenance/product4.png");
    QIcon ButtonIcon4(background4);


   // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, Qt::white);
    this->setPalette(palette);

    ui->product1_button->setIcon(ButtonIcon1);
    ui->product1_button->setIconSize(QSize(101,111));

    ui->product2_button->setIcon(ButtonIcon2);
    ui->product2_button->setIconSize(QSize(101,111));

    ui->product3_button->setIcon(ButtonIcon3);
    ui->product3_button->setIconSize(QSize(101,111));

    ui->product4_button->setIcon(ButtonIcon4);
    ui->product4_button->setIconSize(QSize(101,111));

    maintenancePageEndTimer = new QTimer(this);
    maintenancePageEndTimer->setInterval(1000);
    connect(maintenancePageEndTimer, SIGNAL(timeout()), this, SLOT(onMaintenancePageTimeoutTick()));

}

// DTOR
maintenancePage::~maintenancePage()
{
    delete ui;
}

void maintenancePage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    DbManager db(DB_PATH);
    db.addPageClick("MAINTENANCE PAGE ENTERED");

    if(maintenancePageEndTimer == nullptr){
        maintenancePageEndTimer = new QTimer(this);
        maintenancePageEndTimer->setInterval(1000);
        connect(maintenancePageEndTimer, SIGNAL(timeout()), this, SLOT(onMaintenancePageTimeoutTick()));
    }

    maintenancePageEndTimer->start(1000);
    _maintenancePageTimeoutSec = 15;
}

/*
 * Page Tracking reference
 */
void maintenancePage::setPage(idle* pageIdle, maintain_product* pageMaintain, productPage_1 *pageProduct)
{
    this->idlePage = pageIdle;
    this->maintainPage = pageMaintain;
    this->selection_PageOne = pageProduct;
}

void maintenancePage::on_backButton_clicked(){
    qDebug() << "Back button clicked" << endl;

    DbManager db(DB_PATH);
    db.addPageClick("MAINTENANCE PAGE EXITED");

    maintenancePageEndTimer->stop();
    idlePage->showFullScreen();
    this->hide();
}

void maintenancePage::on_product1_button_clicked(){
    qDebug() << "Product 1 button clicked" << endl;
    maintenancePageEndTimer->stop();

    DbManager db(DB_PATH);
    db.addPageClick("MAINTAIN PRODUCT 1");

    idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_1);
    idlePage->userDrinkOrder->setDrinkSize(DRINK1);

    maintainPage->resizeEvent(productSelection);
    maintainPage->showFullScreen();
    this->hide();

}

void maintenancePage::on_product2_button_clicked(){
    qDebug() << "Product 2 button clicked" << endl;
    maintenancePageEndTimer->stop();

    DbManager db(DB_PATH);
    db.addPageClick("MAINTAIN PRODUCT 2");

    idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_2);
    idlePage->userDrinkOrder->setDrinkSize(DRINK2);

    maintainPage->resizeEvent(productSelection);
    maintainPage->showFullScreen();
    this->hide();

}

void maintenancePage::on_product3_button_clicked(){
    qDebug() << "Product 3 button clicked" << endl;
    maintenancePageEndTimer->stop();

    DbManager db(DB_PATH);
    db.addPageClick("MAINTAIN PRODUCT 3");

    idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_3);
    idlePage->userDrinkOrder->setDrinkSize(DRINK3);

    maintainPage->resizeEvent(productSelection);
    maintainPage->showFullScreen();
    this->hide();

}

void maintenancePage::on_product4_button_clicked(){
    qDebug() << "Product 4 button clicked" << endl;
    maintenancePageEndTimer->stop();

    DbManager db(DB_PATH);
    db.addPageClick("MAINTAIN PRODUCT 4");

    idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_4);
    idlePage->userDrinkOrder->setDrinkSize(DRINK4);

    maintainPage->resizeEvent(productSelection);
    maintainPage->showFullScreen();
    this->hide();

}

//void maintenancePage::on_product5_button_clicked(){
//    qDebug() << "Product 5 button clicked" << endl;

//}

//void maintenancePage::on_product6_button_clicked(){
//    qDebug() << "Product 6 button clicked" << endl;

//}

//void maintenancePage::on_product7_button_clicked(){
//    qDebug() << "Product 7 button clicked" << endl;

//}

//void maintenancePage::on_product8_button_clicked(){
//    qDebug() << "Product 8 button clicked" << endl;

//}

//void maintenancePage::on_product9_button_clicked(){
//    qDebug() << "Product 9 button clicked" << endl;

//}

//void maintenancePage::on_wifi_button_clicked(){
//    qDebug() << "WiFi button clicked" << endl;

//}

//void maintenancePage::on_clean_button_clicked(){
//    qDebug() << "Clean button clicked" << endl;

//}

//void maintenancePage::on_restock_button_clicked(){
//    qDebug() << "Restock button clicked" << endl;

//}

int getSelection(){
    return selection;
}

void maintenancePage::onMaintenancePageTimeoutTick(){

    if(-- _maintenancePageTimeoutSec >= 0) {
        qDebug() << "Maintenance Tick Down: " << _maintenancePageTimeoutSec << endl;
    } else {
        qDebug() << "Maintenance Timer Done!" << _maintenancePageTimeoutSec << endl;

        //Update Click DB
        DbManager db(DB_PATH);
        db.addPageClick("MAINTENANCE PAGE TIME OUT");

        maintenancePageEndTimer->stop();
        this->hide();
        idlePage->showFullScreen();
    }
}
