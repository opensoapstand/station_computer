#include "page_maintenance_dispenser.h"
#include "ui_page_maintenance_dispenser.h"
#include "page_idle.h"
#include "drinkorder.h"
#include <QInputDialog>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QQuickView>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QSlider>

// CTOR
page_maintenance_dispenser::page_maintenance_dispenser(QWidget *parent) : QWidget(parent),
                                                                          ui(new Ui::page_maintenance_dispenser)
{
    // Fullscreen background setup
    ui->setupUi(this);

    QPalette palette;
    palette.setBrush(QPalette::Background, Qt::white);
    this->setPalette(palette);

    ui->pumpLabel->setText("OFF");
    ui->pwmSlider->setTracking(true);

    maintainProductPageEndTimer = new QTimer(this);
    maintainProductPageEndTimer->setInterval(1000);
    connect(maintainProductPageEndTimer, SIGNAL(timeout()), this, SLOT(onMaintainProductPageTimeoutTick()));
    connect(ui->pwmSlider, SIGNAL(valueChanged(int)), this, SLOT(pwmSliderMoved(int)));
}

// DTOR
page_maintenance_dispenser::~page_maintenance_dispenser()
{
    delete ui;
}

void page_maintenance_dispenser::setSoldOutButtonText()
{
    qDebug() << "db call from soldoutbuttonsetting";
    DbManager db(DB_PATH); // TAKE CARE!!!! DO NOT NEST DB CALLS!!!;

#ifdef USE_OLD_DATABASE

    if (this->idlePage->isSlotAvailable(this->idlePage->currentProductOrder->getSelectedSlot()))
    {

        // if(db.getVolumeRemaining(product_slot___)>0){

#else
    int slot = idlePage->currentProductOrder->getSelectedSlot();

    if (db.getSlotEnabled(slot))
    {
#endif
        ui->soldOutButton->setText("Mark as Sold Out");
    }
    else
    {
        ui->soldOutButton->setText("Un-Mark as Sold Out");
    }
    db.closeDB();
}

void page_maintenance_dispenser::showEvent(QShowEvent *event)
{

    QWidget::showEvent(event);
    int product_slot___ = this->idlePage->currentProductOrder->getSelectedSlot();

    qDebug() << "*************************call db from maintenance select dispenser page" << endl;
    DbManager db(DB_PATH);
    this->units_selected_product = db.getUnits(this->idlePage->currentProductOrder->getSelectedSlot());
    // DbManager db_temperature(DB_PATH_TEMPERATURE);
    // db.addPageClick("PAGE_MAINTENANCE PAGE ENTERED");

    if (maintainProductPageEndTimer == nullptr)
    {
        maintainProductPageEndTimer = new QTimer(this);
        maintainProductPageEndTimer->setInterval(1000);
        connect(maintainProductPageEndTimer, SIGNAL(timeout()), this, SLOT(onMaintainProductPageTimeoutTick()));
    }

    //    maintainProductPageEndTimer->start(1000);
    _maintainProductPageTimeoutSec = 40;

    //ticks = db.getProductVolumePerTick(product_slot___);
    update_dispense_stats(0);
    // ui->vol_dispensed_label->setText("Volume Dispensed: 0 " + db.getUnits(product_slot___));
    // ui->ticksLabel->setText("Ticks (" + QString::number(ticks) + "ml/tick): 0");
    ui->pluLabel_s->setText(db.getPLU(product_slot___, 's'));
    ui->pluLabel_l->setText(db.getPLU(product_slot___, 'l'));
    // ui->temperatureLabel->setText(QString::number(db_temperature.getTemperature()) + " degrees Celcius");
    //    ui->temperatureLabel->setText("");
    ui->numberEntry->hide();
    ui->errorLabel->setText("");
    ui->titleLabel->setText("");





    price_small = false;
    price_large = false;
    target_s = false;
    target_l = false;
    vol_per_tick = false;
    full = false;
    pwm = false;
    //    plu_small = false;
    //    plu_large = false;

    if (db.getPaymentMethod(product_slot___) == "plu" || db.getPaymentMethod(product_slot___) == "barcode")
    {
        ui->pluButton_s->setVisible(true);
        ui->pluLabel_s->setVisible(true);
        ui->pluButton_s->setEnabled(true);
        ui->pluButton_l->setVisible(true);
        ui->pluLabel_l->setVisible(true);
        ui->pluButton_l->setEnabled(true);
    }
    else
    {
        ui->pluButton_s->setVisible(false);
        ui->pluLabel_s->setVisible(false);
        ui->pluButton_s->setEnabled(false);
        ui->pluButton_l->setVisible(false);
        ui->pluLabel_l->setVisible(false);
        ui->pluButton_l->setEnabled(false);
    }

    // ui->pwmSlider->setValue(round(double((db.getPWM(product_slot___)) * 100) / 255));
   
    db.closeDB();
    refreshLabels();
    setSoldOutButtonText();
}

/*
 * Page Tracking reference
 */
void page_maintenance_dispenser::setPage(page_maintenance *pageMaintenance, page_idle *pageIdle)
{
    this->p_page_maintenance = pageMaintenance;
    this->idlePage = pageIdle;

   refreshLabels();
}

void page_maintenance_dispenser::on_backButton_clicked()
{
    //    qDebug() << "Back button clicked" << endl;

    // Update Click DB
    //    DbManager db(DB_PATH);
    //    db.addPageClick("MAINTAIN PRODUCT PAGE EXITED");

    maintainProductPageEndTimer->stop();
    p_page_maintenance->showFullScreen();
    //    usleep(100);
    this->hide();

    // ui->name->setText("");
    // ui->price_small->setText("");
    // ui->price_large->setText("");
    // ui->target_volume_s->setText("");
    // ui->target_volume_l->setText("");
    // ui->volume_per_tick->setText("");
    // ui->refillLabel->setText("");
    // ui->soldOutLabel->setText("");
    // ui->full_volume->setText("");
    // ui->remainingLabel->setText("");
    // ui->volume_dispensed_total->setText("");
    // ui->volume_dispensed_since_last_restock->setText("");
    // ui->lastRefillLabel->setText("");

    dispense_test_end(true);
}

void page_maintenance_dispenser::refreshLabels()
{
    int product_slot___ = this->idlePage->currentProductOrder->getSelectedSlot();
    qDebug() << "db... refresh labels" << endl;
    DbManager db(DB_PATH);
    ticks = db.getProductVolumePerTick(product_slot___);
    ui->name->setText(db.getProductName(product_slot___));
    ui->price_small->setText("$" + QString::number(db.getProductPrice(product_slot___, 's')));
    ui->price_large->setText("$" + QString::number(db.getProductPrice(product_slot___, 'l')));
    ui->target_volume_s->setText(QString::number(db.getProductVolume(product_slot___, 's')) + " " + db.getUnits(product_slot___));
    ui->target_volume_l->setText(QString::number(db.getProductVolume(product_slot___, 'l')) + " " + db.getUnits(product_slot___));
    ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(product_slot___)) + " " + db.getUnits(product_slot___));
    ui->full_volume->setText(QString::number(db.getFullProduct(product_slot___)) + " " + db.getUnits(product_slot___));
    ui->volume_dispensed_total->setText(QString::number(db.getTotalDispensed(product_slot___)) + " " + db.getUnits(product_slot___));
    ui->volume_dispensed_since_last_restock->setText(QString::number(db.getVolumeDispensedSinceRestock(product_slot___)) + " " + db.getUnits(product_slot___));
    ui->remainingLabel->setText(QString::number(db.getVolumeRemaining(product_slot___)) + " " + db.getUnits(product_slot___));
    ui->lastRefillLabel->setText(db.getLastRefill(product_slot___));
    ui->pwmLabel->setText(QString::number(round(double((db.getPWM(product_slot___)) * 100) / 255)) + "%");
    ui->pluLabel_s->setText(db.getPLU(product_slot___, 's'));
    ui->pluLabel_l->setText(db.getPLU(product_slot___, 'l'));
    ui->pwmSlider->setValue(round(double((db.getPWM(product_slot___)) * 100) / 255));
     ui->pwmSlider->hide();

     db.closeDB();


    //   ui->name->setText("Product Name: ");
    // ui->price_small->setText("Product Price: ");
    // ui->price_large->setText("Product Price: ");
    // ui->target_volume_s->setText("Product Volume: ");
    // ui->target_volume_l->setText("Product Volume: ");
    // ui->volume_per_tick->setText("Product Volume Per Tick: ");
    // ui->refillLabel->setText("");
    // ui->soldOutLabel->setText("");
    // ui->full_volume->setText("");
    // ui->remainingLabel->setText("");
    // ui->volume_dispensed_total->setText("");
    // ui->volume_dispensed_since_last_restock->setText("");
    // ui->lastRefillLabel->setText("");
    ui->testLargeButton->setVisible(false);
    ui->testSmallButton->setVisible(false);

    ui->temperatureButton->setVisible(false);
    ui->temperatureLabel->setVisible(false);



    //  ui->name->setText(db.getProductName(product_slot___));
    // ui->price_small->setText("$" + QString::number(db.getProductPrice(product_slot___, 's')));
    // ui->price_large->setText("$" + QString::number(db.getProductPrice(product_slot___, 'l')));
    // ui->target_volume_s->setText(QString::number(db.getProductVolume(product_slot___, 's')) + " " + db.getUnits(product_slot___));
    // ui->target_volume_l->setText(QString::number(db.getProductVolume(product_slot___, 'l')) + " " + db.getUnits(product_slot___));
    // ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(product_slot___)) + " " + db.getUnits(product_slot___));
    // ui->full_volume->setText(QString::number(db.getFullProduct(product_slot___)) + " " + db.getUnits(product_slot___));
    // ui->volume_dispensed_total->setText(QString::number(db.getTotalDispensed(product_slot___)) + " " + db.getUnits(product_slot___));
    // ui->volume_dispensed_since_last_restock->setText(QString::number(db.getVolumeDispensedSinceRestock(product_slot___)) + " " + db.getUnits(product_slot___));
    // ui->remainingLabel->setText(QString::number(db.getVolumeRemaining(product_slot___)) + " " + db.getUnits(product_slot___));
    // ui->lastRefillLabel->setText(db.getLastRefill(product_slot___));
    // ui->pwmLabel->setText(QString::number(round(double((db.getPWM(product_slot___)) * 100) / 255)) + "%");
    

}

void page_maintenance_dispenser::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    int product_slot___ = idlePage->currentProductOrder->getSelectedSlot();

    // DbManager db_temperature(DB_PATH_TEMPERATURE);

    QString bitmap_location;

    if (product_slot___ >= 1 && product_slot___ <= SLOT_COUNT)
    {
        qDebug() << " ********** 666564 call db from maintenance select dispenser page  resize event" << endl;
        DbManager db(DB_PATH);
        bitmap_location.append("/home/df-admin/production/references/product");
        bitmap_location.append(QString::number(idlePage->currentProductOrder->getSelectedSlot()));
        bitmap_location.append(".png");
        db.closeDB();
    }
    else
    {
        //        qDebug() << "out of range" << endl;
    }

    refreshLabels();
    update_dispense_stats(0);


    QPixmap background(bitmap_location);
    QIcon ButtonIcon(background);

    ui->image->setIcon(ButtonIcon);
    ui->image->setIconSize(QSize(271, 391));


    setSoldOutButtonText();
}

void page_maintenance_dispenser::on_image_clicked()
{
    _maintainProductPageTimeoutSec = 40;
}

void page_maintenance_dispenser::on_pumpButton_clicked()
{
    qDebug() << "call db from maintenance on pump button c licked" << endl;
    DbManager db(DB_PATH);

    int product_slot___ = idlePage->currentProductOrder->getSelectedSlot();
    if (product_slot___ > 0 && product_slot___ <= 9)
    {

        if (!pumping)
        {
            dispense_test_start();
        }
        else
        {
            dispense_test_end(true);
        }
    }

    db.closeDB();
}

void page_maintenance_dispenser::on_nameButton_clicked()
{
    //    qDebug() << "Name button clicked" << endl;
    _maintainProductPageTimeoutSec = 40;
}

void page_maintenance_dispenser::on_priceButton_s_clicked()
{
    //    qDebug() << "Price button clicked" << endl;
    price_small = true;
    //    DbManager db(DB_PATH);

    _maintainProductPageTimeoutSec = 40;

    //    ui->numberEntry->show();
    //    ui->textEntry->setText("");
    //    ui->titleLabel->setText("New Price:");

    //    ui->price_small->setText("$"+QString::number(db.getProductPrice(idlePage->currentProductOrder->getSelectedSlot(), 's')));
}

void page_maintenance_dispenser::on_priceButton_l_clicked()
{
    //    qDebug() << "Price button clicked" << endl;
    price_large = true;
    //    DbManager db(DB_PATH);

    _maintainProductPageTimeoutSec = 40;

    //    ui->numberEntry->show();
    //    ui->textEntry->setText("");
    //    ui->titleLabel->setText("New Price:");

    //    ui->price_large->setText("$"+QString::number(db.getProductPrice(idlePage->currentProductOrder->getSelectedSlot(), 'l')));
}

void page_maintenance_dispenser::on_target_volumeButton_s_clicked()
{
    //    qDebug() << "Target Volume button clicked" << endl;
    target_s = true;
    _maintainProductPageTimeoutSec = 40;
    //    DbManager db(DB_PATH);
    //    ui->numberEntry->show();
    //    ui->textEntry->setText("");
    //    ui->titleLabel->setText("New Volume:");
    //    ui->target_volume_s->setText(QString::number(db.getProductVolume(idlePage->currentProductOrder->getSelectedSlot(), 's')) + "ml");
}

void page_maintenance_dispenser::on_target_volumeButton_l_clicked()
{
    //    qDebug() << "Target Volume button clicked" << endl;
    target_l = true;
    _maintainProductPageTimeoutSec = 40;
    //    DbManager db(DB_PATH);
    //    ui->numberEntry->show();
    //    ui->textEntry->setText("");
    //    ui->titleLabel->setText("New Volume:");
    //    ui->target_volume_l->setText(QString::number(db.getProductVolume(idlePage->currentProductOrder->getSelectedSlot(), 'l')) + "ml");
}

void page_maintenance_dispenser::on_vol_per_tickButton_clicked()
{

    //    qDebug() << "Volume Per Tick button clicked" << endl;
    vol_per_tick = true;
    _maintainProductPageTimeoutSec = 40;
    //    DbManager db(DB_PATH);
    ui->numberEntry->show();
    ui->textEntry->setText("");
    ui->titleLabel->setText("New Volume Per Tick:");
    //    ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(idlePage->currentProductOrder->getSelectedSlot())) + "ml");
}

void page_maintenance_dispenser::dispense_test_start()
{
    if (!pumping)
    {
        qDebug() << "Start dispense in maintenance mode. (FYI: if app crashes, it's probably about the update volume interrupts caused by the controller sending data.)";
        QString command = QString::number(this->idlePage->currentProductOrder->getSelectedSlot());
        command.append("t");

        update_dispense_stats(0);
        // ui->vol_dispensed_label->setText("Volume Dispensed: 0 "); //+ db.getUnits(product_slot___));
        // ui->ticksLabel->setText("Ticks: 0");

        this->idlePage->dfUtility->msg = command;
        idlePage->dfUtility->m_IsSendingFSM = true;
        idlePage->dfUtility->m_fsmMsg = SEND_DISPENSE_START;
        idlePage->dfUtility->send_to_FSM();
        idlePage->dfUtility->m_IsSendingFSM = false;

        pumping = true;
        ui->pumpLabel->setText("Pump enabled status: ON");
    }
}

void page_maintenance_dispenser::dispense_test_end(bool sendStopToController)
{
    if (pumping)
    {
        
        pumping = false;

        ui->pumpLabel->setText("Pump enabled status: OFF");

        // ui->vol_dispensed_label->setText("");

        if (sendStopToController)
        {
            qDebug() << "Manually finish dispense in maintenance mode.";
            QString command = QString::number(this->idlePage->currentProductOrder->getSelectedSlot());
            command.append("t");
            this->idlePage->dfUtility->msg = command;
            idlePage->dfUtility->m_IsSendingFSM = true;
            idlePage->dfUtility->m_fsmMsg = SEND_DISPENSE_STOP;
            idlePage->dfUtility->send_to_FSM();
            idlePage->dfUtility->m_IsSendingFSM = false;
        }else{
            qDebug() << "controller sent stop dispensing signal in maintenance mode.";
        }

        refreshLabels();
    }
}

void page_maintenance_dispenser::update_dispense_stats(double dispensed)
{

    //if (pumping){
        double vol_dispensed = dispensed;
        // qDebug() << "Signal: updatevol in maintenance mode" + QString::number(vol_dispensed);

        ui->vol_dispensed_label->setText("Volume Dispensed: " + QString::number(vol_dispensed) + this->units_selected_product);
        ui->ticksLabel->setText("Ticks (" + QString::number(ticks) + "ml/tick): " + QString::number(vol_dispensed / ticks));
    //}else{
        //qDebug() << "Error: update volume received while pump not enabled in maintenance." ;
    //}
}

void page_maintenance_dispenser::updateVolumeDisplayed(double dispensed, bool isFull)
{
    // gets called from the controller.

    // --> attention application can crash when there is content in here. combined with fsmReceiveTargetVolumeReached
    // DO THE MINIMUM HERE. NO DEBUG PRINTS. This must be an interrupt call.. probably crashes when called again before handled.
 if (pumping){
    update_dispense_stats(dispensed);
    /*
        // qDebug() << "ahoyy3" ;
        // DbManager db(DB_PATH);

        // double vol_dispensed = dispensed;
        // ui->vol_dispensed_label->setText("Volume Dispensed: " + QString::number(vol_dispensed) + " " +  db.getUnits(this->idlePage->currentProductOrder->getSelectedSlot()));

        // ui->ticksLabel->setText("Ticks: " + QString::number(vol_dispensed/ticks));

        // db.closeDB();
        */
        }else{
        qDebug() << "Error: update volume received while pump not enabled in maintenance." ;
    }
}

void page_maintenance_dispenser::fsmReceiveTargetVolumeReached()
{
    // gets called from the controller.

    // --> attention application can crash when there is content in here. combined with updateVolumeDisplayed
    // DO THE MINIMUM HERE. NO DEBUG PRINTS. This must be an interrupt call.. probably crashes when called again before handled.
    //qDebug() << "Signal: maintenance target hit. " << pumping;

    // ui->vol_dispensed_label->setText(ui->vol_dispensed_label->text() + " - TARGET HIT!");

    // maximum custom dispense volume applies here. controller stops at it when reached.
    dispense_test_end(false);
}

void page_maintenance_dispenser::on_refillButton_clicked()
{
    qDebug() << "refill clicked. slot: " << QString::number(this->idlePage->currentProductOrder->getSelectedSlot()) << endl;
    qDebug() << "refill clicked. size: " << QString::number(this->idlePage->currentProductOrder->getSelectedVolume()) << endl;

    DbManager db(DB_PATH);
    //    qDebug() << "Refill button clicked" << endl;

    _maintainProductPageTimeoutSec = 40;

    // ARE YOU SURE YOU WANT TO COMPLETE?
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    msgBox.setText("<p align=center>Are you sure you want to refill?</p>");
    msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int ret = msgBox.exec();

    switch (ret)
    {
    case QMessageBox::Yes:
        //            qDebug() << "YES CLICKED" << endl;

        if (db.refill(this->idlePage->currentProductOrder->getSelectedSlot()))
        {
            ui->refillLabel->setText("Refill Succesfull");
            ui->soldOutLabel->setText("");

            ui->volume_dispensed_total->setText(QString::number(db.getTotalDispensed(this->idlePage->currentProductOrder->getSelectedSlot())) + " " + db.getUnits(this->idlePage->currentProductOrder->getSelectedSlot()));
            ui->volume_dispensed_since_last_restock->setText(QString::number(db.getVolumeDispensedSinceRestock(this->idlePage->currentProductOrder->getSelectedSlot())) + " " + db.getUnits(this->idlePage->currentProductOrder->getSelectedSlot()));
            ui->remainingLabel->setText(QString::number(db.getVolumeRemaining(this->idlePage->currentProductOrder->getSelectedSlot())) + " " + db.getUnits(this->idlePage->currentProductOrder->getSelectedSlot()));
            ui->lastRefillLabel->setText(db.getLastRefill(this->idlePage->currentProductOrder->getSelectedSlot()));
            db.closeDB();
            curler();
            break;
        }
        else
        {
            ui->refillLabel->setText("Refill ERROR");
            ui->soldOutLabel->setText("");
            db.closeDB();
            break;
        }

    case QMessageBox::No:
        //            qDebug() << "No Clicked" << endl;
        msgBox.hide();
        db.closeDB();
        break;
    }
}

#ifndef USE_OLD_DATABASE
void page_maintenance_dispenser::on_soldOutButton_clicked()
{
    DbManager db(DB_PATH);
    qDebug() << "soldout clicked. slot: " << QString::number(this->idlePage->currentProductOrder->getSelectedSlot()) << endl;
    qDebug() << "soldout clicked. size: " << QString::number(this->idlePage->currentProductOrder->getSelectedVolume()) << endl;

    _maintainProductPageTimeoutSec = 40;

    // if (db.getVolumeRemaining(this->idlePage->currentProductOrder->getSelectedSlot()) > 0){
    if (db.getSlotEnabled(this->idlePage->currentProductOrder->getSelectedSlot()))
    {

        // ARE YOU SURE YOU WANT TO COMPLETE?
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint);
        msgBox.setText("<p align=center>Are you sure you want to mark as Sold out???</p>");
        msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();

        switch (ret)
        {
        case QMessageBox::Yes:
            if (db.updateSlotAvailability(this->idlePage->currentProductOrder->getSelectedSlot(), 0))
            {
                ui->soldOutLabel->setText("Sold Out Succesfull");
                ui->refillLabel->setText("");
                break;
            }
            else
            {
                ui->soldOutLabel->setText("Sold Out ERROR");
                ui->refillLabel->setText("");
                break;
            }

        case QMessageBox::No:
            msgBox.hide();
            break;
        }
    }
    else
    {
        // ARE YOU SURE YOU WANT TO COMPLETE?
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint);
        msgBox.setText("<p align=center>Are you sure you want to un-mark product as sold out???</p>");
        msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();

        switch (ret)
        {
        case QMessageBox::Yes:

            if (db.updateSlotAvailability(this->idlePage->currentProductOrder->getSelectedSlot(), 1))
            {
                ui->soldOutLabel->setText("Un-Sold Out Succesfull");
                ui->refillLabel->setText("");
                break;
            }
            else
            {
                ui->soldOutLabel->setText("Un-Sold Out ERROR");
                ui->refillLabel->setText("");
                break;
            }

        case QMessageBox::No:
            //            qDebug() << "No Clicked" << endl;
            msgBox.hide();
            break;
        }
    }

    db.closeDB();

    setSoldOutButtonText();
}
#else
void page_maintenance_dispenser::on_soldOutButton_clicked()
{
    qDebug() << "ahoyy4";
    DbManager db(DB_PATH);
    //    qDebug() << "Sold Out button clicked" << endl;

    // qDebug() << QString::number(db.getVolumeRemaining(this->idlePage->currentProductOrder->getSelectedSlot());
    qDebug() << "soldout clicked. slot: " << QString::number(this->idlePage->currentProductOrder->getSelectedSlot()) << endl;
    qDebug() << "soldout clicked. size: " << QString::number(this->idlePage->currentProductOrder->getSelectedVolume()) << endl;

    _maintainProductPageTimeoutSec = 40;

    // if (db.getVolumeRemaining(this->idlePage->currentProductOrder->getSelectedSlot()) > 0){

    if (this->idlePage->isSlotAvailable(this->idlePage->currentProductOrder->getSelectedSlot()))
    {

        // ARE YOU SURE YOU WANT TO COMPLETE?
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint);
        msgBox.setText("<p align=center>Are you sure you want to mark as Sold Out?</p>");
        msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();

        switch (ret)
        {
        case QMessageBox::Yes:
        {
            //            qDebug() << "YES CLICKED" << endl;

            this->idlePage->setSlotAvailability(this->idlePage->currentProductOrder->getSelectedSlot(), false);

            //                qDebug() << "SOLD OUT!" << endl;
            ui->soldOutLabel->setText("Sold Out Succesfull. Will be reset at program restart.");
            ui->refillLabel->setText("");
            // Update Click DB
            //                DbManager db(DB_PATH);
            //                db.addPageClick("PRODUCT SOLD OUT");
            // ui->volume_dispensed_total->setText(QString::number(db.getTotalDispensed(this->idlePage->currentProductOrder->getSelectedSlot())) + " " +  db.getUnits(this->idlePage->currentProductOrder->getSelectedSlot()));
            // ui->remainingLabel->setText(QString::number(db.getVolumeRemaining(this->idlePage->currentProductOrder->getSelectedSlot())) + " " +  db.getUnits(this->idlePage->currentProductOrder->getSelectedSlot()));
            // ui->soldOutButton->setText("Un-Mark as Sold Out");
        }
        break;

        case QMessageBox::No:
            //            qDebug() << "No Clicked" << endl;
            msgBox.hide();
            break;
        }
    }
    else
    {
        // ARE YOU SURE YOU WANT TO COMPLETE?
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint);
        msgBox.setText("<p align=center>Are you sure you want to un-mark product as sold out?</p>");
        msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();

        switch (ret)
        {
        case QMessageBox::Yes:
        {
            //            qDebug() << "YES CLICKED" << endl;

            this->idlePage->setSlotAvailability(this->idlePage->currentProductOrder->getSelectedSlot(), true);

            //                qDebug() << "SOLD OUT!" << endl;
            ui->soldOutLabel->setText("Un-Sold Out Succesfull");
            ui->refillLabel->setText("");
            // Update Click DB
            //                DbManager db(DB_PATH);
            //                db.addPageClick("PRODUCT SOLD OUT");
            // ui->volume_dispensed_total->setText(QString::number(db.getTotalDispensed(this->idlePage->currentProductOrder->getSelectedSlot())) + " " +  db.getUnits(this->idlePage->currentProductOrder->getSelectedSlot()));
            // ui->remainingLabel->setText(QString::number(db.getVolumeRemaining(this->idlePage->currentProductOrder->getSelectedSlot())) + " " +  db.getUnits(this->idlePage->currentProductOrder->getSelectedSlot()));
            // ui->soldOutButton->setText("Mark as Sold Out");
        }
        case QMessageBox::No:
            //            qDebug() << "No Clicked" << endl;
            msgBox.hide();
            break;
        }
    }

    db.closeDB();
    setSoldOutButtonText();
}

#endif

void page_maintenance_dispenser::on_fullButton_clicked()
{
    //    qDebug() << "Full Volume button clicked" << endl;
    full = true;
    _maintainProductPageTimeoutSec = 40;
    //    DbManager db(DB_PATH);
    //    ui->numberEntry->show();
    //    ui->textEntry->setText("");
    //    ui->titleLabel->setText("New Full Volume:");
    //    ui->full_volume->setText(QString::number(db.getFullProduct(idlePage->currentProductOrder->getSelectedSlot())) + "ml");
}

void page_maintenance_dispenser::on_remainingButton_clicked()
{
    //    qDebug() << "Remaining button clicked" << endl;
    _maintainProductPageTimeoutSec = 40;
}

void page_maintenance_dispenser::on_dispensedButton_clicked()
{
    //    qDebug() << "Remaining button clicked" << endl;
    _maintainProductPageTimeoutSec = 40;
}

void page_maintenance_dispenser::on_lastRefillButton_clicked()
{
    //    qDebug() << "Last Refill button clicked" << endl;
    _maintainProductPageTimeoutSec = 40;
}

void page_maintenance_dispenser::on_temperatureButton_clicked()
{
    //    qDebug() << "Temperature button clicked" << endl;
    //    DbManager db_temperature(DB_PATH_TEMPERATURE);
    //    ui->temperatureLabel->setText(QString::number(db_temperature.getTemperature()) + " degrees Celcius");
    _maintainProductPageTimeoutSec = 40;
    //    db_temperature.closeDB();
}

void page_maintenance_dispenser::onMaintainProductPageTimeoutTick()
{

    if (--_maintainProductPageTimeoutSec >= 0)
    {
        //        qDebug() << "Maintain Product Tick Down: " << _maintainProductPageTimeoutSec << endl;
    }
    else
    {
        //        qDebug() << "Maintain Product Timer Done!" << _maintainProductPageTimeoutSec << endl;

        // Update Click DB
        qDebug() << "ahoyy5";
        DbManager db(DB_PATH);
        //        db.addPageClick("MAINTAIN PRODUCT PAGE TIME OUT");

        dispense_test_end(true);

        db.closeDB();
        maintainProductPageEndTimer->stop();
        idlePage->showFullScreen();
        this->hide();
    }
}

void page_maintenance_dispenser::on_pwmButton_clicked()
{
    //    qDebug() << "Remaining button clicked" << endl;
    pwm = true;
    _maintainProductPageTimeoutSec = 40;
    //    DbManager db(DB_PATH);
    ui->numberEntry->show();
    ui->textEntry->setText("");
    ui->titleLabel->setText("New Pump Speed:");
    ui->buttonPoint->hide();
    //    ui->pwmLabel->setText(QString::number(db.getPWM(idlePage->currentProductOrder->getSelectedSlot())) + "%");
}

// void page_maintenance_dispenser::on_pluButton_s_clicked(){
//     qDebug() << "PLU Button clicked" << endl;
//     plu_small=true;
//     _maintainProductPageTimeoutSec=40;
//     DbManager db(DB_PATH);
//     ui->numberEntry->show();
//     ui->textEntry->setText("");
//     ui->titleLabel->setText("New PLU/Barcode:");
//     ui->buttonPoint->hide();
//     ui->pluLabel->setText(QString::number(db.getPLU(idlePage->currentProductOrder->getSelectedSlot())));

//}

// void page_maintenance_dispenser::on_pluButton_l_clicked(){
//     qDebug() << "PLU Button clicked" << endl;
//     plu_large=true;
//     _maintainProductPageTimeoutSec=40;
//     DbManager db(DB_PATH);
//     ui->numberEntry->show();
//     ui->textEntry->setText("");
//     ui->titleLabel->setText("New PLU/Barcode:");
//     ui->buttonPoint->hide();
//     ui->pluLabel->setText(QString::number(db.getPLU(idlePage->currentProductOrder->getSelectedSlot())));

//}

void page_maintenance_dispenser::on_button1_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "1");
}

void page_maintenance_dispenser::on_button2_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "2");
}

void page_maintenance_dispenser::on_button3_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "3");
}

void page_maintenance_dispenser::on_button4_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "4");
}

void page_maintenance_dispenser::on_button5_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "5");
}

void page_maintenance_dispenser::on_button6_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "6");
}

void page_maintenance_dispenser::on_button7_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "7");
}

void page_maintenance_dispenser::on_button8_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "8");
}

void page_maintenance_dispenser::on_button9_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "9");
}

void page_maintenance_dispenser::on_button0_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + "0");
}

void page_maintenance_dispenser::on_buttonBack_clicked()
{
    ui->textEntry->backspace();
}

void page_maintenance_dispenser::on_buttonPoint_clicked()
{
    ui->textEntry->setText(ui->textEntry->text() + ".");
}

void page_maintenance_dispenser::on_buttonDone_clicked()
{
    if (pwm)
    {
        if (ui->textEntry->text().toInt() > 100)
        {
            ui->errorLabel->setText("Error: Enter number less than 100");
            ui->textEntry->setText("");
        }
        else
        {
            text_entered = ui->textEntry->text();
            ui->buttonPoint->show();
            ui->numberEntry->hide();
            updateValues();
        }
    }
    else
    {
        text_entered = ui->textEntry->text();
        ui->numberEntry->hide();
        updateValues();
    }
}

void page_maintenance_dispenser::on_buttonCancel_clicked()
{
    ui->numberEntry->hide();
    ui->textEntry->setText("");
    ui->errorLabel->setText("");
    text_entered = "";
    price_small = false;
    price_large = false;
    target_s = false;
    target_l = false;
    vol_per_tick = false;
    full = false;
    pwm = false;
    //    plu_small=false;
    //    plu_large=false;
}

void page_maintenance_dispenser::updateValues()
{
    qDebug() << "ahoyy6";
    DbManager db(DB_PATH);
    int product_slot___ = idlePage->currentProductOrder->getSelectedSlot();

    if (price_small)
    {
        db.updatePriceSmall(product_slot___, text_entered.toDouble());
        ui->price_small->setText("$" + QString::number(db.getProductPrice(product_slot___, 's')));
    }
    else if (price_large)
    {
        db.updatePriceLarge(product_slot___, text_entered.toDouble());
        ui->price_large->setText("$" + QString::number(db.getProductPrice(product_slot___, 'l')));
    }
    else if (target_s)
    {
        db.updateTargetVolume_s(product_slot___, text_entered.toDouble());
        ui->target_volume_s->setText(QString::number(db.getProductVolume(product_slot___, 's')) + " " + db.getUnits(product_slot___));
    }
    else if (target_l)
    {
        db.updateTargetVolume_l(product_slot___, text_entered.toDouble());
        ui->target_volume_l->setText(QString::number(db.getProductVolume(product_slot___, 'l')) + " " + db.getUnits(product_slot___));
    }
    else if (vol_per_tick)
    {
        db.updateVolumePerTick(product_slot___, text_entered.toDouble());
        ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(product_slot___)) + " " + db.getUnits(product_slot___));
        ticks = db.getProductVolumePerTick(product_slot___);
    }
    else if (full)
    {
        db.updateFullVolume(product_slot___, text_entered.toDouble());
        ui->full_volume->setText(QString::number(db.getFullProduct(product_slot___)) + db.getUnits(product_slot___));
    }
    else if (pwm)
    {
        int new_pwm = round(((text_entered.toInt()) * 255) / 100);
        //        qDebug() << "New Pump Speed: " << text_entered.toInt() << "% equals = " << (new_pwm) << endl;
        db.updatePWM(product_slot___, new_pwm);
        ui->pwmLabel->setText(QString::number(round(double((db.getPWM(product_slot___)) * 100) / 255)) + "%");
    }
    //    else if(plu_small){
    //        db.updatePluSmall(product_slot___, text_entered);
    //    }else if(plu_large){
    //        db.updatePluLarge(product_slot___, text_entered);
    //    }

    price_small = false;
    price_large = false;
    target_s = false;
    target_l = false;
    vol_per_tick = false;
    full = false;
    pwm = false;
    //    plu_small=false;
    //    plu_large=false;

    ui->textEntry->setText("");
    ui->titleLabel->setText("");
    ui->errorLabel->setText("");

    db.closeDB();
}

void page_maintenance_dispenser::pwmSliderMoved(int percentage)
{
    int value = ui->pwmSlider->value();
    //    qDebug() << "Slider Value: " << value << endl;

    QString command = QString::number(this->idlePage->currentProductOrder->getSelectedSlot());
    command.append("P");
    command.append(QString::number(value));

    //    qDebug() << "In PWMSlider: " << command << endl;

    //    this->idlePage->dfUtility->msg = command;
    //    idlePage->dfUtility->m_IsSendingFSM = true;
    //    idlePage->dfUtility->m_fsmMsg = SEND_PWM;
    //    idlePage->dfUtility->send_to_FSM();
    //    idlePage->dfUtility->m_IsSendingFSM = false;

    //    command = "";
}

size_t WriteCallback3(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void page_maintenance_dispenser::curler()
{
    qDebug() << "ahoyy7";
    DbManager db(DB_PATH);
    int product_slot___ = idlePage->currentProductOrder->getSelectedSlot();

    QString curl_param = "pid=" + db.getProductID(product_slot___) + "&volume_full=" + QString::number(db.getFullProduct(product_slot___));
    curl_param_array = curl_param.toLocal8Bit();
    curl_data = curl_param_array.data();

    curl = curl_easy_init();
    if (!curl)
    {
        //        qDebug() << "cURL failed to page_init" << endl;
    }
    else
    {
        //        qDebug() << "cURL page_init success" << endl;

        curl_easy_setopt(curl, CURLOPT_URL, "http://Drinkfill-env.eba-qatmjpdr.us-east-2.elasticbeanstalk.com/api/machine_data/resetStock");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curl_param_array.data());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback3);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        //        qDebug() << "Curl Setup done" << endl;

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            //            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            bufferCURL(curl_data);
        }
        else
        {
            //            qDebug() << "CURL SUCCESS!" << endl;
            //            std::cout <<"Here's the output:\n" << readBuffer << endl;
            if (readBuffer == "true")
            {
                curl_easy_cleanup(curl);
                readBuffer = "";
            }
            else if (readBuffer == "false")
            {
                // TODO: Curl Buffer here but not sure of return state (currently false)
                curl_easy_cleanup(curl);
                readBuffer = "";
            }
            else
            {
                curl_easy_cleanup(curl);
                readBuffer = "";
            }
        }
    }
    db.closeDB();
}

void page_maintenance_dispenser::bufferCURL(char *curl_params)
{
    char filetime[50];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(filetime, 50, "%F %T", timeinfo);
    std::string filelocation = "/home/df-admin/curlBuffer/";
    std::string filetype = "_MM.txt";
    std::string filename = filelocation + filetime + filetype;
    //    std::cout << "filename is: " << filename << endl;
    std::ofstream out;
    out.open(filename);
    if (!out.is_open())
    {
        //        std::cout << "Cannot open output file!";
    }
    else
    {
        out << curl_params;
        out.close();
    }
}

// void page_maintenance_dispenser::on_testSmallButton_clicked(){
//     int product_slot___ = idlePage->currentProductOrder->getSelectedSlot();
//     if(product_slot___ > 0 && product_slot___ <= 9) {
//         QString command = QString::number(this->idlePage->currentProductOrder->getSelectedSlot());
//         if (!pumping){
//             command.append("s");

//            ui->vol_dispensed_label->setText("Volume Dispensed: 0ml");

//            this->idlePage->dfUtility->msg = command;
//            idlePage->dfUtility->m_IsSendingFSM = true;
//            idlePage->dfUtility->m_fsmMsg = SEND_DISPENSE_START;
//            idlePage->dfUtility->send_to_FSM();
//            idlePage->dfUtility->m_IsSendingFSM = false;

//            pumping = true;
//            ui->pumpLabel->setText("ON");
//        }
//        else {
//            pumping = false;
//            ui->pumpLabel->setText("OFF");
//            //ui->vol_dispensed_label->setText("");
//            command = QString::number(this->idlePage->currentProductOrder->getSelectedSlot());
//            command.append("s");

//            this->idlePage->dfUtility->msg = command;
//            idlePage->dfUtility->m_IsSendingFSM = true;
//            idlePage->dfUtility->m_fsmMsg = SEND_DISPENSE_STOP;
//            idlePage->dfUtility->send_to_FSM();
//            idlePage->dfUtility->m_IsSendingFSM = false;
//        }
//    }
//}

// void page_maintenance_dispenser::on_testLargeButton_clicked(){
//     int product_slot___ = idlePage->currentProductOrder->getSelectedSlot();
//     if(product_slot___ > 0 && product_slot___ <= 9) {
//         QString command = QString::number(this->idlePage->currentProductOrder->getSelectedSlot());
//         if (!pumping){
//             command.append("l");

//            ui->vol_dispensed_label->setText("Volume Dispensed: 0ml");

//            this->idlePage->dfUtility->msg = command;
//            idlePage->dfUtility->m_IsSendingFSM = true;
//            idlePage->dfUtility->m_fsmMsg = SEND_DISPENSE_START;
//            idlePage->dfUtility->send_to_FSM();
//            idlePage->dfUtility->m_IsSendingFSM = false;

//            pumping = true;
//            ui->pumpLabel->setText("ON");
//        }
//        else {
//            pumping = false;
//            ui->pumpLabel->setText("OFF");
//            //ui->vol_dispensed_label->setText("");
//            command = QString::number(this->idlePage->currentProductOrder->getSelectedSlot());
//            command.append("l");

//            this->idlePage->dfUtility->msg = command;
//            idlePage->dfUtility->m_IsSendingFSM = true;
//            idlePage->dfUtility->m_fsmMsg = SEND_DISPENSE_STOP;
//            idlePage->dfUtility->send_to_FSM();
//            idlePage->dfUtility->m_IsSendingFSM = false;
//        }
//    }
//}
