
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
    // ui->pwmSlider->setTracking(true);

    maintainProductPageEndTimer = new QTimer(this);
    maintainProductPageEndTimer->setInterval(1000);
    connect(maintainProductPageEndTimer, SIGNAL(timeout()), this, SLOT(onMaintainProductPageTimeoutTick()));
    connect(ui->pwmSlider, SIGNAL(valueChanged(int)), this, SLOT(pwmSliderMoved(int)));
    ui->refillButton->setStyleSheet("QPushButton {font-size: 36px;}");
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

    if (this->p_page_idle->isSlotAvailable(this->p_page_idle->currentProductOrder->getSelectedSlot()))
    {

        // if(db.getVolumeRemaining(product_slot___)>0){

#else
    int slot = p_page_idle->currentProductOrder->getSelectedSlot();

    if (db.getSlotEnabled(slot))
    {
#endif
        ui->soldOutButton->setText("Make \n unavailable");
        ui->soldOutButton->setStyleSheet("QPushButton { background-color: #5E8680;font-size: 36px; }");
    }
    else
    {
        ui->soldOutButton->setText("Make \n available");
        ui->soldOutButton->setStyleSheet("QPushButton { background-color: #E0A0A0;font-size: 36px;  }");
    }
    db.closeDB();
}

void page_maintenance_dispenser::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< PPPage Enter: maintenance dispense >>>>>>>>>";

    QWidget::showEvent(event);
    int product_slot___ = this->p_page_idle->currentProductOrder->getSelectedSlot();

    qDebug() << "*************************call db from maintenance select dispenser page" << endl;
    DbManager db(DB_PATH);
    this->units_selected_product = db.getUnits(this->p_page_idle->currentProductOrder->getSelectedSlot());
    // DbManager db_temperature(DB_PATH_TEMPERATURE);
    // db.addPageClick("PAGE_MAINTENANCE PAGE ENTERED");

    if (maintainProductPageEndTimer == nullptr)
    {
        maintainProductPageEndTimer = new QTimer(this);
        maintainProductPageEndTimer->setInterval(1000);
        connect(maintainProductPageEndTimer, SIGNAL(timeout()), this, SLOT(onMaintainProductPageTimeoutTick()));
    }

    //    maintainProductPageEndTimer->start(1000);
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

    // ticks = db.getProductVolumePerTick(product_slot___);
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
    this->p_page_idle = pageIdle;
    selectedProductOrder = p_page_idle->currentProductOrder;

    // refreshLabels();
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
    // ui->infoLabel->setText("");
    // ui->full_volume->setText("");
    // ui->remainingLabel->setText("");
    // ui->volume_dispensed_total->setText("");
    // ui->volume_dispensed_since_last_restock->setText("");
    // ui->lastRefillLabel->setText("");

    dispense_test_end(true);
}

void page_maintenance_dispenser::refreshLabels()
{
    ui->pwmSlider->hide();

    volume_per_tick_buffer = selectedProductOrder->getVolumePerTickForSelectedSlot();

    ui->volume_per_tick->setText(selectedProductOrder->getVolumePerTickAsStringForSelectedSlot());

    ui->name->setText(selectedProductOrder->getSelectedProductName());

    ui->price_small->setText("$" + QString::number(selectedProductOrder->getPrice(SIZE_SMALL_INDEX)));
    ui->price_large->setText("$" + QString::number(selectedProductOrder->getPrice(SIZE_LARGE_INDEX)));

    ui->target_volume_s->setText(selectedProductOrder->getSizeToVolumeWithCorrectUnitsForSelectedSlot(SIZE_SMALL_INDEX, false, true));
    ui->target_volume_l->setText(selectedProductOrder->getSizeToVolumeWithCorrectUnitsForSelectedSlot(SIZE_LARGE_INDEX, false, true));

    ui->full_volume->setText(selectedProductOrder->getFullVolumeCorrectUnits(true));
    ui->volume_dispensed_total->setText(selectedProductOrder->getTotalDispensedCorrectUnits());
    ui->volume_dispensed_since_last_restock->setText(selectedProductOrder->getVolumeDispensedSinceRestockCorrectUnits());
    ui->remainingLabel->setText(selectedProductOrder->getVolumeRemainingCorrectUnits());
    ui->pwmLabel->setText(QString::number(selectedProductOrder->getSelectedDispenseSpeedPercentage()) + "%");

    int product_slot___ = selectedProductOrder->getSelectedSlot();
    qDebug() << "db... refresh labels" << endl;
    DbManager db(DB_PATH);
    ui->lastRefillLabel->setText(db.getLastRefill(product_slot___));
    ui->pluLabel_s->setText(db.getPLU(product_slot___, 's'));
    ui->pluLabel_l->setText(db.getPLU(product_slot___, 'l'));

    db.closeDB();

    ui->infoLabel->setText("");

    ui->testLargeButton->setVisible(false);
    ui->testSmallButton->setVisible(false);

    ui->temperatureButton->setVisible(false);
    ui->temperatureLabel->setVisible(false);
}

void page_maintenance_dispenser::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    int slot = p_page_idle->currentProductOrder->getSelectedSlot();

    // DbManager db_temperature(DB_PATH_TEMPERATURE);

#ifdef ENABLE_DYNAMIC_UI
    QString p = p_page_idle->currentProductOrder->getProductPicturePath(p_page_idle->currentProductOrder->getSelectedSlot());
    p_page_idle->dfUtility->fileExists(p);
    QPixmap im(p);
    QIcon qi(im);
    ui->productPhotoButton->setIcon(qi);
    ui->productPhotoButton->setIconSize(QSize(271, 391));
#else
    QString bitmap_location;

    bitmap_location.append("/home/df-admin/production/references/product");
    bitmap_location.append(QString::number(p_page_idle->currentProductOrder->getSelectedSlot()));
    bitmap_location.append(".png");

    QPixmap background(bitmap_location);
    QIcon ButtonIcon(background);

    ui->productPhotoButton->setIcon(ButtonIcon);
    ui->productPhotoButton->setIconSize(QSize(271, 391));
#endif
    refreshLabels();
    update_dispense_stats(0);
    setSoldOutButtonText();
}

void page_maintenance_dispenser::on_image_clicked()
{
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_pumpButton_clicked()
{
    qDebug() << "call db from maintenance on pump button clicked" << endl;
    // DbManager db(DB_PATH);

    int product_slot___ = p_page_idle->currentProductOrder->getSelectedSlot();
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

    refreshLabels(); // fsm did not yet respond at this time. wait for feedback.
    // db.closeDB();
}

void page_maintenance_dispenser::on_nameButton_clicked()
{
    //    qDebug() << "Name button clicked" << endl;
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_priceButton_s_clicked()
{
    //    qDebug() << "Price button clicked" << endl;
    price_small = true;
    //    DbManager db(DB_PATH);

    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

    //    ui->numberEntry->show();
    //    ui->textEntry->setText("");
    //    ui->titleLabel->setText("New Price:");

    //    ui->price_small->setText("$"+QString::number(db.getProductPrice(p_page_idle->currentProductOrder->getSelectedSlot(), 's')));
}

void page_maintenance_dispenser::on_priceButton_l_clicked()
{
    //    qDebug() << "Price button clicked" << endl;
    price_large = true;
    //    DbManager db(DB_PATH);

    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

    //    ui->numberEntry->show();
    //    ui->textEntry->setText("");
    //    ui->titleLabel->setText("New Price:");

    //    ui->price_large->setText("$"+QString::number(db.getProductPrice(p_page_idle->currentProductOrder->getSelectedSlot(), 'l')));
}

void page_maintenance_dispenser::on_target_volumeButton_s_clicked()
{
    //    qDebug() << "Target Volume button clicked" << endl;
    target_s = true;
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
    //    DbManager db(DB_PATH);
    //    ui->numberEntry->show();
    //    ui->textEntry->setText("");
    //    ui->titleLabel->setText("New Volume:");
    //    ui->target_volume_s->setText(QString::number(db.getProductVolume(p_page_idle->currentProductOrder->getSelectedSlot(), 's')) + "ml");
}

void page_maintenance_dispenser::on_target_volumeButton_l_clicked()
{
    //    qDebug() << "Target Volume button clicked" << endl;
    target_l = true;
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
    //    DbManager db(DB_PATH);
    //    ui->numberEntry->show();
    //    ui->textEntry->setText("");
    //    ui->titleLabel->setText("New Volume:");
    //    ui->target_volume_l->setText(QString::number(db.getProductVolume(p_page_idle->currentProductOrder->getSelectedSlot(), 'l')) + "ml");
}

void page_maintenance_dispenser::on_vol_per_tickButton_clicked()
{

    //    qDebug() << "Volume Per Tick button clicked" << endl;
    vol_per_tick = true;
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
    //    DbManager db(DB_PATH);
    ui->numberEntry->show();
    ui->textEntry->setText("");
    ui->titleLabel->setText("New Volume Per Tick:");
    //    ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(p_page_idle->currentProductOrder->getSelectedSlot())) + "ml");
}

void page_maintenance_dispenser::dispense_test_start()
{
    if (!pumping)
    {
        qDebug() << "Start dispense in maintenance mode. (FYI: if app crashes, it's probably about the update volume interrupts caused by the controller sending data.)";
        QString command = QString::number(this->p_page_idle->currentProductOrder->getSelectedSlot());
        command.append("t");
        command.append(SEND_DISPENSE_START);

        update_dispense_stats(0);
        p_page_idle->dfUtility->send_command_to_FSM(command);

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

        if (sendStopToController)
        {
            qDebug() << "Manually finish dispense in maintenance mode.";
            QString command = QString::number(this->p_page_idle->currentProductOrder->getSelectedSlot());
            command.append("t");
            command.append(SEND_DISPENSE_STOP);
            p_page_idle->dfUtility->send_command_to_FSM(command);
        }
        else
        {
            qDebug() << "controller sent stop dispensing signal in maintenance mode.";
        }

        refreshLabels();
    }
}

void page_maintenance_dispenser::update_dispense_stats(double dispensed)
{

    // if (pumping){
    double vol_dispensed = dispensed;
    // qDebug() << "Signal: updatevol in maintenance mode" + QString::number(vol_dispensed);

    if (this->units_selected_product == "oz")
    {
        ui->ticksLabel->setText("Ticks (" + QString::number(df_util::convertMlToOz(volume_per_tick_buffer), 'f', 2) + "oz/tick): " + QString::number(vol_dispensed / volume_per_tick_buffer));
        // ui->vol_dispensed_label->setText("Volume Dispensed: " + QString::number(df_util::convertMlToOz(vol_dispensed), 'f', 2) + "oz");
        ui->vol_dispensed_label->setText("Volume Dispensed: " + df_util::getConvertedStringVolumeFromMl(vol_dispensed, "oz", false, true));
    }
    else
    {
        ui->ticksLabel->setText("Ticks (" + QString::number(volume_per_tick_buffer) + "ml/tick): " + QString::number(vol_dispensed / volume_per_tick_buffer));
        ui->vol_dispensed_label->setText("Volume Dispensed: " + QString::number(vol_dispensed) + "ml");
    }

    //}else{
    // qDebug() << "Error: update volume received while pump not enabled in maintenance." ;
    //}
}

void page_maintenance_dispenser::updateVolumeDisplayed(double dispensed, bool isFull)
{
    // gets called from the controller.

    // --> attention application can crash when there is content in here. combined with fsmReceiveTargetVolumeReached
    // DO THE MINIMUM HERE. NO DEBUG PRINTS. This must be an interrupt call.. probably crashes when called again before handled.
    if (pumping)
    {
        update_dispense_stats(dispensed);
        /*
            // qDebug() << "ahoyy3" ;
            // DbManager db(DB_PATH);

            // double vol_dispensed = dispensed;
            // ui->vol_dispensed_label->setText("Volume Dispensed: " + QString::number(vol_dispensed) + " " +  db.getUnits(this->p_page_idle->currentProductOrder->getSelectedSlot()));

            // ui->ticksLabel->setText("Ticks: " + QString::number(vol_dispensed/ticks));

            // db.closeDB();
            */
    }
    else
    {
        qDebug() << "Error: update volume received while pump not enabled in maintenance.";
    }
}

void page_maintenance_dispenser::fsmReceiveTargetVolumeReached()
{
    // gets called from the controller.

    // --> attention application can crash when there is content in here. combined with updateVolumeDisplayed
    // DO THE MINIMUM HERE. NO DEBUG PRINTS. This must be an interrupt call.. probably crashes when called again before handled.
    // qDebug() << "Signal: maintenance target hit. " << pumping;

    // ui->vol_dispensed_label->setText(ui->vol_dispensed_label->text() + " - TARGET HIT!");

    // maximum custom dispense volume applies here. controller stops at it when reached.
    dispense_test_end(false);
}
void page_maintenance_dispenser::fsmReceiveNoFlowAbort()
{
    // gets called from the controller.
    // dispense_test_end(false);
    qDebug() << "Received no flow from controller. , will not take any action";
}

void page_maintenance_dispenser::on_refillButton_clicked()
{
    qDebug() << "refill clicked. slot: " << QString::number(this->p_page_idle->currentProductOrder->getSelectedSlot()) << endl;
    qDebug() << "refill clicked. size: " << QString::number(this->p_page_idle->currentProductOrder->getSelectedVolume()) << endl;

    //    qDebug() << "Refill button clicked" << endl;

    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

    // ARE YOU SURE YOU WANT TO COMPLETE?
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    msgBox.setText("<p align=center>Are you sure you want to refill?</p>");
    msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int ret = msgBox.exec();
    bool success;
    switch (ret)
    {
    case QMessageBox::Yes:
    {
        DbManager db(DB_PATH);
        success = db.refill(selectedProductOrder->getSelectedSlot());
        db.closeDB();

        if (success)
        {
            sendRestockToCloud();
            refreshLabels();
            ui->infoLabel->setText("Refill Succesfull");
        }
        else
        {
            ui->infoLabel->setText("Refill ERROR");
        }
    }
    break;
    case QMessageBox::No:
    {
        msgBox.hide();
    }
    break;
    }
}

#ifndef USE_OLD_DATABASE
void page_maintenance_dispenser::on_soldOutButton_clicked()
{
    qDebug() << "soldout clicked. slot: " << QString::number(this->p_page_idle->currentProductOrder->getSelectedSlot());

    // qDebug() << "soldout clicked. size: " << QString::number(this->p_page_idle->currentProductOrder->getSelectedVolume());

    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
    bool success;

    DbManager db(DB_PATH);
    success = db.getSlotEnabled(selectedProductOrder->getSelectedSlot());
    db.closeDB();
    QString infoLabelText="";
    if (success)
    {

        // ARE YOU SURE YOU WANT TO COMPLETE?
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint);
        msgBox.setText("<p align=center>Are you sure you want to Disable product?</p>");
        msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();

        switch (ret)
        {
        case QMessageBox::Yes:
        {
            QMessageBox msgBox2;
            msgBox2.setWindowFlags(Qt::FramelessWindowHint);
            msgBox2.setText("<p align=center>Is this product coming soon?</p>");
            msgBox2.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

            msgBox2.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            int ret2 = msgBox2.exec();
            switch(ret2)
            {
                case QMessageBox::Yes:
                    {
                         infoLabelText = "Coming Soon";
                    }
                break;
                case QMessageBox::No:
                {
                         infoLabelText = "Sold Out";
                } 
                break;
            }
            

            DbManager db2(DB_PATH);
            bool success = db2.updateSlotAvailability(selectedProductOrder->getSelectedSlot(), 0, infoLabelText);
            db2.closeDB();

            if (!success)
            {
                infoLabelText = "Set Disabled ERROR";
            }
            ui->infoLabel->setText(infoLabelText);

            break;
        }
        case QMessageBox::No:
        {
            msgBox.hide();
        }
        break;
        }
    }
    else
    {
        // ARE YOU SURE YOU WANT TO COMPLETE?
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint);
        msgBox.setText("<p align=center>Are you sure you want to Enable Product?</p>");
        msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();

        switch (ret)
        {
        case QMessageBox::Yes:
        {
            QString infoLabelText = "Set Enabled Succesful";
            DbManager db3(DB_PATH);
            bool success = db3.updateSlotAvailability(this->p_page_idle->currentProductOrder->getSelectedSlot(), 1, "");
            db3.closeDB();
            if (!success)
            {
                infoLabelText = "Set Enabled ERROR";
            }
            ui->infoLabel->setText(infoLabelText);

            break;
        }
        case QMessageBox::No:
        {
            msgBox.hide();
            break;
        }
        }
    }

    setSoldOutButtonText();
}
#else
void page_maintenance_dispenser::on_soldOutButton_clicked()
{
    qDebug() << "ahoyy4";
    DbManager db(DB_PATH);
    //    qDebug() << "Sold Out button clicked" << endl;

    qDebug() << "soldout clicked. slot: " << QString::number(this->p_page_idle->currentProductOrder->getSelectedSlot()) << endl;

    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;

    if (this->p_page_idle->isSlotAvailable(this->p_page_idle->currentProductOrder->getSelectedSlot()))
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

            this->p_page_idle->setSlotAvailability(this->p_page_idle->currentProductOrder->getSelectedSlot(), false);

            //                qDebug() << "SOLD OUT!" << endl;
            ui->infoLabel->setText("Sold Out Succesfull. Will be reset at program restart.");
            // Update Click DB
            //                DbManager db(DB_PATH);
            //                db.addPageClick("PRODUCT SOLD OUT");
            // ui->volume_dispensed_total->setText(QString::number(db.getTotalDispensed(this->p_page_idle->currentProductOrder->getSelectedSlot())) + " " +  db.getUnits(this->p_page_idle->currentProductOrder->getSelectedSlot()));
            // ui->remainingLabel->setText(QString::number(db.getVolumeRemaining(this->p_page_idle->currentProductOrder->getSelectedSlot())) + " " +  db.getUnits(this->p_page_idle->currentProductOrder->getSelectedSlot()));
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

            this->p_page_idle->setSlotAvailability(this->p_page_idle->currentProductOrder->getSelectedSlot(), true);

            //                qDebug() << "SOLD OUT!" << endl;
            ui->infoLabel->setText("Un-Sold Out Succesfull");
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
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_remainingButton_clicked()
{
    //    qDebug() << "Remaining button clicked" << endl;
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_dispensedButton_clicked()
{
    //    qDebug() << "Remaining button clicked" << endl;
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_lastRefillButton_clicked()
{
    //    qDebug() << "Last Refill button clicked" << endl;
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::on_temperatureButton_clicked()
{
    //    qDebug() << "Temperature button clicked" << endl;
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
}

void page_maintenance_dispenser::onMaintainProductPageTimeoutTick()
{

    if (--_maintainProductPageTimeoutSec >= 0)
    {
        //        qDebug() << "Maintain Product Tick Down: " << _maintainProductPageTimeoutSec << endl;
    }
    else
    {
        qDebug() << "Maintenance dispenser page timeout";

        // Update Click DB
        qDebug() << "ahoyy5";
        DbManager db(DB_PATH);
        dispense_test_end(true);

        db.closeDB();

        maintainProductPageEndTimer->stop();
        // qDebug() << "maintenance dispenser to idle";
        p_page_idle->showFullScreen();
        this->hide();
    }
}

void page_maintenance_dispenser::on_pwmButton_clicked()
{
    //    qDebug() << "Remaining button clicked" << endl;
    pwm = true;
    _maintainProductPageTimeoutSec = PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
    //    DbManager db(DB_PATH);
    ui->numberEntry->show();
    ui->textEntry->setText("");
    ui->titleLabel->setText("New Pump Speed:");
    ui->buttonPoint->hide();
    //    ui->pwmLabel->setText(QString::number(db.getPWM(p_page_idle->currentProductOrder->getSelectedSlot())) + "%");
}

// void page_maintenance_dispenser::on_pluButton_s_clicked(){
//     qDebug() << "PLU Button clicked" << endl;
//     plu_small=true;
//     _maintainProductPageTimeoutSec=PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
//     DbManager db(DB_PATH);
//     ui->numberEntry->show();
//     ui->textEntry->setText("");
//     ui->titleLabel->setText("New PLU/Barcode:");
//     ui->buttonPoint->hide();
//     ui->pluLabel->setText(QString::number(db.getPLU(p_page_idle->currentProductOrder->getSelectedSlot())));

//}

// void page_maintenance_dispenser::on_pluButton_l_clicked(){
//     qDebug() << "PLU Button clicked" << endl;
//     plu_large=true;
//     _maintainProductPageTimeoutSec=PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS;
//     DbManager db(DB_PATH);
//     ui->numberEntry->show();
//     ui->textEntry->setText("");
//     ui->titleLabel->setText("New PLU/Barcode:");
//     ui->buttonPoint->hide();
//     ui->pluLabel->setText(QString::number(db.getPLU(p_page_idle->currentProductOrder->getSelectedSlot())));

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
    // qDebug() << "ahoyy6";
    // DbManager db(DB_PATH);
    // int product_slot___ = selectedProductOrder->getSelectedSlot();

    if (price_small)
    {
        selectedProductOrder->setPriceSelected(SIZE_SMALL_INDEX, text_entered.toDouble());
        // ui->price_small->setText("$" + QString::number(selectedProductOrder->getPrice(SIZE_SMALL_INDEX)));
    }
    else if (price_large)
    {
        selectedProductOrder->setPriceSelected(SIZE_LARGE_INDEX, text_entered.toDouble());
        // db.updatePriceLarge(product_slot___, text_entered.toDouble());
        // ui->price_large->setText("$" + QString::number(selectedProductOrder->getPrice(SIZE_LARGE_INDEX)));
    }
    else if (target_s)
    {
        // db.updateTargetVolume_s(product_slot___, text_entered.toDouble());
        selectedProductOrder->setSizeToVolumeForSelectedSlot(text_entered, SIZE_SMALL_INDEX);
    }
    else if (target_l)
    {
        selectedProductOrder->setSizeToVolumeForSelectedSlot(text_entered, SIZE_LARGE_INDEX);
    }
    else if (vol_per_tick)
    {
        selectedProductOrder->setVolumePerTickForSelectedSlot(text_entered);
    }
    else if (full)
    {
        selectedProductOrder->setFullVolumeCorrectUnits(text_entered);
    }
    else if (pwm)
    {
        selectedProductOrder->setSelectedDispenseSpeedPercentage(text_entered.toInt());
        // ui->pwmLabel->setText(QString::number(selectedProductOrder->getSelectedDispenseSpeedPercentage()) + "%");
    }
    //    else if(plu_small){
    //        db.updatePluSmall(product_slot___, text_entered);
    //    }else if(plu_large){
    //        db.updatePluLarge(product_slot___, text_entered);
    //    }

    refreshLabels();

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

    // db.closeDB();
}

void page_maintenance_dispenser::pwmSliderMoved(int percentage)
{
    qDebug() << "Slider not implemented";
    //     int value = ui->pwmSlider->value();
    //     //    qDebug() << "Slider Value: " << value << endl;

    //     QString command = QString::number(this->p_page_idle->currentProductOrder->getSelectedSlot());
    //     command.append("P");
    //     command.append(QString::number(value));

    //     //    qDebug() << "In PWMSlider: " << command << endl;

    //     //    this->p_page_idle->dfUtility->msg = command;
    //     //    p_page_idle->dfUtility->m_IsSendingFSM = true;
    //     //    p_page_idle->dfUtility->m_fsmMsg = SEND_PWM;
    //     //    p_page_idle->dfUtility->send_to_FSM();
    //     //    p_page_idle->dfUtility->m_IsSendingFSM = false;

    //     //    command = "";
}

size_t WriteCallback3(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void page_maintenance_dispenser::sendRestockToCloud()
{
    QString curl_param = "pid=" + p_page_idle->currentProductOrder->getSelectedProductId() + "&volume_full=" + p_page_idle->currentProductOrder->getFullVolumeCorrectUnits(false);
    // QString curl_param = "pid=" + db.getProductID(product_slot___) + "&volume_full=" + QString::number(db.getFullProduct(product_slot___));

    curl_param_array = curl_param.toLocal8Bit();
    curl_data = curl_param_array.data();

    curl = curl_easy_init();
    if (!curl)
    {
        qDebug() << "cURL failed to page_init at thank you end";
        restockTransactionToFile(curl_data);
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/resetStock");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curl_param_array.data());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback3);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);

    res = curl_easy_perform(curl);

    // error code 6 (cannot resolve host) showed up when not connected to wifi. Make distinct!
    if (res != CURLE_OK)
    {
        qDebug() << "pagemaintenancedispenser. cURL fail. (6=could not resolve host (no internet)) Error code: " + QString::number(res);
        restockTransactionToFile(curl_data);


    }
    else
    {

        QString feedback = QString::fromUtf8(readBuffer.c_str());
        qDebug() << "ERROR: pagemaintenancedispenser cURL success. Server feedback readbuffer: " << feedback;

        // readbuffer is a string. "true" or "false"
        if (readBuffer == "true")
        {
            // return data
        }
    }
    curl_easy_cleanup(curl);
    readBuffer = "";
}

void page_maintenance_dispenser::restockTransactionToFile(char *curl_params)
{
    qDebug() << "Write Restock transaction to file ";
    QString data_out = curl_params;
    //p_page_idle->dfUtility->write_to_file_timestamped(TRANSACTIONS_RESTOCK_OFFINE_PATH, data_out);
    p_page_idle->dfUtility->write_to_file(TRANSACTIONS_RESTOCK_OFFINE_PATH, data_out);
    // char filetime[50];
    // time(&rawtime);
    // timeinfo = localtime(&rawtime);
    // strftime(filetime, 50, "%F %T", timeinfo);
    // std::string filelocation = "/home/df-admin/curlBuffer/";
    // std::string filetype = "_MM.txt";
    // std::string filename = filelocation + filetime + filetype;
    // //    std::cout << "filename is: " << filename << endl;
    // std::ofstream out;
    // out.open(filename);
    // if (!out.is_open())
    // {
    //     //        std::cout << "Cannot open output file!";
    // }
    // else
    // {
    //     out << curl_params;
    //     out.close();
    // }
}

// void page_maintenance_dispenser::on_testSmallButton_clicked(){
//     int product_slot___ = p_page_idle->currentProductOrder->getSelectedSlot();
//     if(product_slot___ > 0 && product_slot___ <= 9) {
//         QString command = QString::number(this->p_page_idle->currentProductOrder->getSelectedSlot());
//         if (!pumping){
//             command.append("s");

//            ui->vol_dispensed_label->setText("Volume Dispensed: 0ml");

//            this->p_page_idle->dfUtility->mswefwefg = command;
//            p_page_idle->dfUtility->m_IsSendingFSM = true;
//            p_page_idle->dfUtility->m_fsmMsg = SENwefwefD_DISPENSE_START;
//            p_page_idle->dfUtility->send_to_FSM();
//            p_page_idle->dfUtility->m_IsSendingFSM = false;

//            pumping = true;
//            ui->pumpLabel->setText("ON");
//        }
//        else {
//            pumping = false;
//            ui->pumpLabel->setText("OFF");
//            //ui->vol_dispensed_label->setText("");
//            command = QString::number(this->p_page_idle->currentProductOrder->getSelectedSlot());
//            command.append("s");

//            this->p_page_idle->dfUtility->msg = command;
//            p_page_idle->dfUtility->m_IsSendingFSM = true;
//            p_page_idle->dfUtility->m_fsmMsg = SEND_DISPENSE_STOP;
//            p_page_idle->dfUtility->send_to_FSM();
//            p_page_idle->dfUtility->m_IsSendingFSM = false;
//        }
//    }
//}

// void page_maintenance_dispenser::on_testLargeButton_clicked(){
//     int product_slot___ = p_page_idle->currentProductOrder->getSelectedSlot();
//     if(product_slot___ > 0 && product_slot___ <= 9) {
//         QString command = QString::number(this->p_page_idle->currentProductOrder->getSelectedSlot());
//         if (!pumping){
//             command.append("l");

//            ui->vol_dispensed_label->setText("Volume Dispensed: 0ml");

//            this->p_page_idle->dfUtility->mswfwefg = command;
//            p_page_idle->dfUtility->m_IsSendingFSM = true;
//            p_page_idle->dfUtility->m_fsmMsg = SEND_DISwfewefwefPENSE_START;
//            p_page_idle->dfUtility->send_to_FSM();
//            p_page_idle->dfUtility->m_IsSendingFSM = false;

//            pumping = true;
//            ui->pumpLabel->setText("ON");
//        }
//        else {
//            pumping = false;
//            ui->pumpLabel->setText("OFF");
//            //ui->vol_dispensed_label->setText("");
//            command = QString::number(this->p_page_idle->currentProductOrder->getSelectedSlot());
//            command.append("l");

//            this->p_page_idle->dfUtility->msg = command;
//            p_page_idle->dfUtility->m_IsSendingFSM = true;
//            p_page_idle->dfUtility->m_fsmMsg = SEND_DISPENSE_STOP;
//            p_page_idle->dfUtility->send_to_FSM();
//            p_page_idle->dfUtility->m_IsSendingFSM = false;
//        }
//    }
//}
