#include "maintain_product.h"
#include "ui_maintain_product.h"
#include "idle.h"
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
maintain_product::maintain_product(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::maintain_product)
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
maintain_product::~maintain_product()
{
    delete ui;
}

void maintain_product::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    int checkOption = idlePage->userDrinkOrder->getOption();

    DbManager db(DB_PATH);
    //DbManager db_temperature(DB_PATH_TEMPERATURE);
    //db.addPageClick("MAINTENANCE PAGE ENTERED");

    if(maintainProductPageEndTimer == nullptr){
        maintainProductPageEndTimer = new QTimer(this);
        maintainProductPageEndTimer->setInterval(1000);
        connect(maintainProductPageEndTimer, SIGNAL(timeout()), this, SLOT(onMaintainProductPageTimeoutTick()));
    }

//    maintainProductPageEndTimer->start(1000);
    _maintainProductPageTimeoutSec = 40;

    if(db.getRemaining(checkOption)>0){
        ui->soldOutButton->setText("Mark as Sold Out");
    }else{
        ui->soldOutButton->setText("Un-Mark as Sold Out");
    }

    ticks = db.getProductVolumePerTick(checkOption);

    ui->name->setText(db.getProductName(checkOption));
    ui->price_small->setText("$"+QString::number(db.getProductPrice(checkOption, 's')));
    ui->price_large->setText("$"+QString::number(db.getProductPrice(checkOption, 'l')));
    ui->target_volume_s->setText(QString::number(db.getProductVolume(checkOption, 's')) + " " + db.getUnits(checkOption));
    ui->target_volume_l->setText(QString::number(db.getProductVolume(checkOption, 'l')) + " " + db.getUnits(checkOption));
    ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(checkOption)) + " " + db.getUnits(checkOption));
    ui->full_volume->setText(QString::number(db.getFullProduct(checkOption)) + " " + db.getUnits(checkOption));
    ui->volume_dispensed_total->setText(QString::number(db.getTotalDispensed(checkOption)) + " " + db.getUnits(checkOption));
    ui->remainingLabel->setText(QString::number(db.getRemaining(checkOption)) + " " + db.getUnits(checkOption));
    ui->lastRefillLabel->setText(db.getLastRefill(checkOption));
    ui->pwmLabel->setText(QString::number(round(double((db.getPWM(checkOption))*100)/255)) + "%");
    ui->vol_dispensed_label->setText("Volume Dispensed: 0 " + db.getUnits(checkOption));
    ui->ticksLabel->setText("Ticks: 0");
    ui->pluLabel_s->setText(db.getPLU(checkOption, 's'));
    ui->pluLabel_l->setText(db.getPLU(checkOption, 'l'));
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

    if(db.getPaymentMethod(checkOption)=="plu" || db.getPaymentMethod(checkOption)=="barcode"){
        ui->pluButton_s->setVisible(true);
        ui->pluLabel_s->setVisible(true);
        ui->pluButton_s->setEnabled(true);
        ui->pluButton_l->setVisible(true);
        ui->pluLabel_l->setVisible(true);
        ui->pluButton_l->setEnabled(true);
    }else{
        ui->pluButton_s->setVisible(false);
        ui->pluLabel_s->setVisible(false);
        ui->pluButton_s->setEnabled(false);
        ui->pluButton_l->setVisible(false);
        ui->pluLabel_l->setVisible(false);
        ui->pluButton_l->setEnabled(false);
    }

    ui->pwmSlider->setValue(round(double((db.getPWM(checkOption))*100)/255));
    ui->pwmSlider->hide();

    db.closeDB();

}

/*
 * Page Tracking reference
 */
void maintain_product::setPage(maintenancePage* pageMaintenance, idle* pageIdle)
{
    this->maintenanceMode = pageMaintenance;
    this->idlePage = pageIdle;

    ui->name->setText("Product Name: ");
    ui->price_small->setText("Product Price: ");
    ui->price_large->setText("Product Price: ");
    ui->target_volume_s->setText("Product Volume: ");
    ui->target_volume_l->setText("Product Volume: ");
    ui->volume_per_tick->setText("Product Volume Per Tick: ");
    ui->refillLabel->setText("");
    ui->soldOutLabel->setText("");
    ui->full_volume->setText("");
    ui->remainingLabel->setText("");
    ui->volume_dispensed_total->setText("");
    ui->lastRefillLabel->setText("");
    ui->testLargeButton->setVisible(false);
    ui->testSmallButton->setVisible(false);

    ui->temperatureButton->setVisible(false);
    ui->temperatureLabel->setVisible(false);

}

void maintain_product::on_backButton_clicked(){
//    qDebug() << "Back button clicked" << endl;

    //Update Click DB
//    DbManager db(DB_PATH);
//    db.addPageClick("MAINTAIN PRODUCT PAGE EXITED");

    maintainProductPageEndTimer->stop();
    maintenanceMode->showFullScreen();
//    usleep(100);
    this->hide();

    ui->name->setText("");
    ui->price_small->setText("");
    ui->price_large->setText("");
    ui->target_volume_s->setText("");
    ui->target_volume_l->setText("");
    ui->volume_per_tick->setText("");
    ui->refillLabel->setText("");
    ui->soldOutLabel->setText("");
    ui->full_volume->setText("");
    ui->remainingLabel->setText("");
    ui->volume_dispensed_total->setText("");
    ui->lastRefillLabel->setText("");


    if (pumping) {
//        qDebug() << "Stopping pump" << endl;
        pumping = false;
        QString command = QString::number(this->idlePage->userDrinkOrder->getOption());
        command.append("s");
        ui->pumpLabel->setText("OFF");
        ui->vol_dispensed_label->setText("");

        this->idlePage->dfUtility->msg = command;
        idlePage->dfUtility->m_IsSendingFSM = true;
        idlePage->dfUtility->m_fsmMsg = SEND_CLEAN;
        idlePage->dfUtility->send_to_FSM();
        idlePage->dfUtility->m_IsSendingFSM = false;
    }
}

void maintain_product::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);

    int checkOption = idlePage->userDrinkOrder->getOption();

    DbManager db(DB_PATH);
   // DbManager db_temperature(DB_PATH_TEMPERATURE);

    QString bitmap_location;

    if(checkOption > 0 && checkOption <= 9) {
        bitmap_location.append("/release/references/products/product");
        bitmap_location.append(QString::number(idlePage->userDrinkOrder->getOption()));
        bitmap_location.append(".png");
    } else {
//        qDebug() << "out of range" << endl;
    }

    ticks = db.getProductVolumePerTick(checkOption);

    ui->name->setText(db.getProductName(checkOption));
    ui->price_small->setText("$"+QString::number(db.getProductPrice(checkOption, 's')));
    ui->price_large->setText("$"+QString::number(db.getProductPrice(checkOption, 'l')));
    ui->target_volume_s->setText(QString::number(db.getProductVolume(checkOption, 's')) + " " +  db.getUnits(checkOption));
    ui->target_volume_l->setText(QString::number(db.getProductVolume(checkOption, 'l')) + " " +  db.getUnits(checkOption));
    ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(checkOption)) + " " +  db.getUnits(checkOption));
    ui->full_volume->setText(QString::number(db.getFullProduct(checkOption)) + " " +  db.getUnits(checkOption));
    ui->volume_dispensed_total->setText(QString::number(db.getTotalDispensed(checkOption)) + " " +  db.getUnits(checkOption));
    ui->remainingLabel->setText(QString::number(db.getRemaining(checkOption)) + " " +  db.getUnits(checkOption));
    ui->lastRefillLabel->setText(db.getLastRefill(checkOption));
//    ui->temperatureLabel->setText(QString::number(db_temperature.getTemperature()) + " degrees Celcius");
    ui->pwmLabel->setText(QString::number(round(double((db.getPWM(checkOption))*100)/255)) + "%");
    ui->vol_dispensed_label->setText("Volume Dispensed: 0 " + db.getUnits(checkOption));
    ui->ticksLabel->setText("Ticks: 0");
    ui->pluLabel_s->setText(db.getPLU(checkOption, 's'));
    ui->pluLabel_l->setText(db.getPLU(checkOption, 'l'));

//    ui->temperatureLabel->setText("");

    if(db.getRemaining(checkOption)>0){
        ui->soldOutButton->setText("Mark as Sold Out");
    }else{
        ui->soldOutButton->setText("Un-Mark as Sold Out");
    }

    QPixmap background(bitmap_location);
    QIcon ButtonIcon(background);

    ui->image->setIcon(ButtonIcon);
    ui->image->setIconSize(QSize(271,391));

    ui->pwmSlider->setValue(round(double((db.getPWM(checkOption))*100)/255));

    db.closeDB();


}

void maintain_product::on_image_clicked(){
    _maintainProductPageTimeoutSec=40;
}

void maintain_product::on_pumpButton_clicked(){
    DbManager db(DB_PATH);

    int checkOption = idlePage->userDrinkOrder->getOption();
    if(checkOption > 0 && checkOption <= 9) {
        QString command = QString::number(this->idlePage->userDrinkOrder->getOption());
        if (!pumping){
            command.append("t");

            ui->vol_dispensed_label->setText("Volume Dispensed: 0 " + db.getUnits(checkOption));
            ui->ticksLabel->setText("Ticks: 0");

            this->idlePage->dfUtility->msg = command;
            idlePage->dfUtility->m_IsSendingFSM = true;
            idlePage->dfUtility->m_fsmMsg = SEND_DRINK;
            idlePage->dfUtility->send_to_FSM();
            idlePage->dfUtility->m_IsSendingFSM = false;

            pumping = true;
            ui->pumpLabel->setText("ON");
        }
        else {
            pumping = false;
            ui->pumpLabel->setText("OFF");
            command = QString::number(this->idlePage->userDrinkOrder->getOption());
            command.append("t");

            this->idlePage->dfUtility->msg = command;
            idlePage->dfUtility->m_IsSendingFSM = true;
            idlePage->dfUtility->m_fsmMsg = SEND_CLEAN;
            idlePage->dfUtility->send_to_FSM();
            idlePage->dfUtility->m_IsSendingFSM = false;
        }
    }

    db.closeDB();

}

//void maintain_product::on_testSmallButton_clicked(){
//    int checkOption = idlePage->userDrinkOrder->getOption();
//    if(checkOption > 0 && checkOption <= 9) {
//        QString command = QString::number(this->idlePage->userDrinkOrder->getOption());
//        if (!pumping){
//            command.append("s");

//            ui->vol_dispensed_label->setText("Volume Dispensed: 0ml");

//            this->idlePage->dfUtility->msg = command;
//            idlePage->dfUtility->m_IsSendingFSM = true;
//            idlePage->dfUtility->m_fsmMsg = SEND_DRINK;
//            idlePage->dfUtility->send_to_FSM();
//            idlePage->dfUtility->m_IsSendingFSM = false;

//            pumping = true;
//            ui->pumpLabel->setText("ON");
//        }
//        else {
//            pumping = false;
//            ui->pumpLabel->setText("OFF");
//            //ui->vol_dispensed_label->setText("");
//            command = QString::number(this->idlePage->userDrinkOrder->getOption());
//            command.append("s");

//            this->idlePage->dfUtility->msg = command;
//            idlePage->dfUtility->m_IsSendingFSM = true;
//            idlePage->dfUtility->m_fsmMsg = SEND_CLEAN;
//            idlePage->dfUtility->send_to_FSM();
//            idlePage->dfUtility->m_IsSendingFSM = false;
//        }
//    }
//}

//void maintain_product::on_testLargeButton_clicked(){
//    int checkOption = idlePage->userDrinkOrder->getOption();
//    if(checkOption > 0 && checkOption <= 9) {
//        QString command = QString::number(this->idlePage->userDrinkOrder->getOption());
//        if (!pumping){
//            command.append("l");

//            ui->vol_dispensed_label->setText("Volume Dispensed: 0ml");

//            this->idlePage->dfUtility->msg = command;
//            idlePage->dfUtility->m_IsSendingFSM = true;
//            idlePage->dfUtility->m_fsmMsg = SEND_DRINK;
//            idlePage->dfUtility->send_to_FSM();
//            idlePage->dfUtility->m_IsSendingFSM = false;

//            pumping = true;
//            ui->pumpLabel->setText("ON");
//        }
//        else {
//            pumping = false;
//            ui->pumpLabel->setText("OFF");
//            //ui->vol_dispensed_label->setText("");
//            command = QString::number(this->idlePage->userDrinkOrder->getOption());
//            command.append("l");

//            this->idlePage->dfUtility->msg = command;
//            idlePage->dfUtility->m_IsSendingFSM = true;
//            idlePage->dfUtility->m_fsmMsg = SEND_CLEAN;
//            idlePage->dfUtility->send_to_FSM();
//            idlePage->dfUtility->m_IsSendingFSM = false;
//        }
//    }
//}

void maintain_product::on_nameButton_clicked(){
//    qDebug() << "Name button clicked" << endl;
    _maintainProductPageTimeoutSec=40;
}


void maintain_product::on_priceButton_s_clicked(){
//    qDebug() << "Price button clicked" << endl;
    price_small = true;
//    DbManager db(DB_PATH);

    _maintainProductPageTimeoutSec=40;

//    ui->numberEntry->show();
//    ui->textEntry->setText("");
//    ui->titleLabel->setText("New Price:");

//    ui->price_small->setText("$"+QString::number(db.getProductPrice(idlePage->userDrinkOrder->getOption(), 's')));

}

void maintain_product::on_priceButton_l_clicked(){
//    qDebug() << "Price button clicked" << endl;
    price_large = true;
//    DbManager db(DB_PATH);

    _maintainProductPageTimeoutSec=40;

//    ui->numberEntry->show();
//    ui->textEntry->setText("");
//    ui->titleLabel->setText("New Price:");

//    ui->price_large->setText("$"+QString::number(db.getProductPrice(idlePage->userDrinkOrder->getOption(), 'l')));
}


void maintain_product::on_target_volumeButton_s_clicked(){
//    qDebug() << "Target Volume button clicked" << endl;
    target_s=true;
    _maintainProductPageTimeoutSec=40;
//    DbManager db(DB_PATH);
//    ui->numberEntry->show();
//    ui->textEntry->setText("");
//    ui->titleLabel->setText("New Volume:");
//    ui->target_volume_s->setText(QString::number(db.getProductVolume(idlePage->userDrinkOrder->getOption(), 's')) + "ml");
}

void maintain_product::on_target_volumeButton_l_clicked(){
//    qDebug() << "Target Volume button clicked" << endl;
    target_l=true;
    _maintainProductPageTimeoutSec=40;
//    DbManager db(DB_PATH);
//    ui->numberEntry->show();
//    ui->textEntry->setText("");
//    ui->titleLabel->setText("New Volume:");
//    ui->target_volume_l->setText(QString::number(db.getProductVolume(idlePage->userDrinkOrder->getOption(), 'l')) + "ml");
}


void maintain_product::on_vol_per_tickButton_clicked(){
//    qDebug() << "Volume Per Tick button clicked" << endl;
    vol_per_tick=true;
    _maintainProductPageTimeoutSec=40;
//    DbManager db(DB_PATH);
    ui->numberEntry->show();
    ui->textEntry->setText("");
    ui->titleLabel->setText("New Volume Per Tick:");
//    ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(idlePage->userDrinkOrder->getOption())) + "ml");
}

void maintain_product::updateVolumeDisplayed(double dispensed){
    DbManager db(DB_PATH);

    double vol_dispensed = dispensed;
    ui->vol_dispensed_label->setText("Volume Dispensed: " + QString::number(vol_dispensed) + " " +  db.getUnits(this->idlePage->userDrinkOrder->getOption()));

    ui->ticksLabel->setText("Ticks: " + QString::number(vol_dispensed/ticks));

    db.closeDB();

}

void maintain_product::targetHitDisplay(){
    ui->vol_dispensed_label->setText(ui->vol_dispensed_label->text() + " - TARGET HIT!");
}

void maintain_product::on_refillButton_clicked(){
    DbManager db(DB_PATH);
//    qDebug() << "Refill button clicked" << endl;

    _maintainProductPageTimeoutSec=40;

    // ARE YOU SURE YOU WANT TO COMPLETE?
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    msgBox.setText("<p align=center>Are you sure you want to refill?</p>");
    msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int ret = msgBox.exec();

    switch(ret){
        case QMessageBox::Yes:
//            qDebug() << "YES CLICKED" << endl;

            if(db.refill(this->idlePage->userDrinkOrder->getOption())){
//                qDebug() << "REFILLED!" << endl;
                ui->refillLabel->setText("Refill Succesfull");
                ui->soldOutLabel->setText("");
                //Update Click DB
//                DbManager db(DB_PATH);
//                db.addPageClick("PRODUCT REFILLED");
                ui->volume_dispensed_total->setText(QString::number(db.getTotalDispensed(this->idlePage->userDrinkOrder->getOption())) + " " +  db.getUnits(this->idlePage->userDrinkOrder->getOption()));
                ui->remainingLabel->setText(QString::number(db.getRemaining(this->idlePage->userDrinkOrder->getOption())) + " " +  db.getUnits(this->idlePage->userDrinkOrder->getOption()));
                ui->lastRefillLabel->setText(db.getLastRefill(this->idlePage->userDrinkOrder->getOption()));
                db.closeDB();
                curler();
                break;
            }
            else{
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

void maintain_product::on_soldOutButton_clicked(){
    DbManager db(DB_PATH);
//    qDebug() << "Sold Out button clicked" << endl;

    _maintainProductPageTimeoutSec=40;

    if (db.getRemaining(this->idlePage->userDrinkOrder->getOption()) > 0){

        // ARE YOU SURE YOU WANT TO COMPLETE?
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint);
        msgBox.setText("<p align=center>Are you sure you want to mark as Sold Out?</p>");
        msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();

        switch(ret){
        case QMessageBox::Yes:
//            qDebug() << "YES CLICKED" << endl;

            if(db.sellout(this->idlePage->userDrinkOrder->getOption())){
//                qDebug() << "SOLD OUT!" << endl;
                ui->soldOutLabel->setText("Sold Out Succesfull");
                ui->refillLabel->setText("");
                //Update Click DB
//                DbManager db(DB_PATH);
//                db.addPageClick("PRODUCT SOLD OUT");
                ui->volume_dispensed_total->setText(QString::number(db.getTotalDispensed(this->idlePage->userDrinkOrder->getOption())) + " " +  db.getUnits(this->idlePage->userDrinkOrder->getOption()));
                ui->remainingLabel->setText(QString::number(db.getRemaining(this->idlePage->userDrinkOrder->getOption())) + " " +  db.getUnits(this->idlePage->userDrinkOrder->getOption()));
                ui->soldOutButton->setText("Un-Mark as Sold Out");
                break;
            }
            else{
                ui->soldOutLabel->setText("Sold Out ERROR");
                ui->refillLabel->setText("");
                break;
            }

        case QMessageBox::No:
//            qDebug() << "No Clicked" << endl;
            msgBox.hide();
            break;
        }

    }else{
        // ARE YOU SURE YOU WANT TO COMPLETE?
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint);
        msgBox.setText("<p align=center>Are you sure you want to un-mark product as sold out?</p>");
        msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();

        switch(ret){
        case QMessageBox::Yes:
//            qDebug() << "YES CLICKED" << endl;

            if(db.unsellout(this->idlePage->userDrinkOrder->getOption())){
//                qDebug() << "UN-SOLD OUT!" << endl;
                ui->soldOutLabel->setText("Un-Sold Out Succesfull");
                ui->refillLabel->setText("");
                //Update Click DB
//                DbManager db(DB_PATH);
//                db.addPageClick("PRODUCT UN-SOLD OUT");
                ui->volume_dispensed_total->setText(QString::number(db.getTotalDispensed(this->idlePage->userDrinkOrder->getOption())) + " " +  db.getUnits(this->idlePage->userDrinkOrder->getOption()));
                ui->remainingLabel->setText(QString::number(db.getRemaining(this->idlePage->userDrinkOrder->getOption())) + " " +  db.getUnits(this->idlePage->userDrinkOrder->getOption()));
                ui->soldOutButton->setText("Mark as Sold Out");
                break;
            }
            else{
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


}

void maintain_product::on_fullButton_clicked(){
//    qDebug() << "Full Volume button clicked" << endl;
    full=true;
    _maintainProductPageTimeoutSec=40;
//    DbManager db(DB_PATH);
//    ui->numberEntry->show();
//    ui->textEntry->setText("");
//    ui->titleLabel->setText("New Full Volume:");
//    ui->full_volume->setText(QString::number(db.getFullProduct(idlePage->userDrinkOrder->getOption())) + "ml");
}

void maintain_product::on_remainingButton_clicked(){
//    qDebug() << "Remaining button clicked" << endl;
    _maintainProductPageTimeoutSec=40;
}

void maintain_product::on_dispensedButton_clicked(){
//    qDebug() << "Remaining button clicked" << endl;
    _maintainProductPageTimeoutSec=40;
}

void maintain_product::on_lastRefillButton_clicked(){
//    qDebug() << "Last Refill button clicked" << endl;
    _maintainProductPageTimeoutSec=40;
}

void maintain_product::on_temperatureButton_clicked(){
//    qDebug() << "Temperature button clicked" << endl;
//    DbManager db_temperature(DB_PATH_TEMPERATURE);
//    ui->temperatureLabel->setText(QString::number(db_temperature.getTemperature()) + " degrees Celcius");
    _maintainProductPageTimeoutSec=40;
//    db_temperature.closeDB();
}

void maintain_product::onMaintainProductPageTimeoutTick(){

    if(-- _maintainProductPageTimeoutSec >= 0) {
//        qDebug() << "Maintain Product Tick Down: " << _maintainProductPageTimeoutSec << endl;
    } else {
//        qDebug() << "Maintain Product Timer Done!" << _maintainProductPageTimeoutSec << endl;

        //Update Click DB
        DbManager db(DB_PATH);
//        db.addPageClick("MAINTAIN PRODUCT PAGE TIME OUT");

        if (pumping) {
//            qDebug() << "Stopping pump" << endl;
            pumping = false;
            QString command = QString::number(this->idlePage->userDrinkOrder->getOption());
            command.append("s");
            ui->pumpLabel->setText("OFF");
            ui->vol_dispensed_label->setText("");

            this->idlePage->dfUtility->msg = command;
            idlePage->dfUtility->m_IsSendingFSM = true;
            idlePage->dfUtility->m_fsmMsg = SEND_CLEAN;
            idlePage->dfUtility->send_to_FSM();
            idlePage->dfUtility->m_IsSendingFSM = false;
        }

        db.closeDB();
        maintainProductPageEndTimer->stop();
        idlePage->showFullScreen();
        this->hide();
    }
}

void maintain_product::on_pwmButton_clicked(){
//    qDebug() << "Remaining button clicked" << endl;
    pwm=true;
    _maintainProductPageTimeoutSec=40;
//    DbManager db(DB_PATH);
    ui->numberEntry->show();
    ui->textEntry->setText("");
    ui->titleLabel->setText("New Pump Speed:");
    ui->buttonPoint->hide();
//    ui->pwmLabel->setText(QString::number(db.getPWM(idlePage->userDrinkOrder->getOption())) + "%");
}

//void maintain_product::on_pluButton_s_clicked(){
//    qDebug() << "PLU Button clicked" << endl;
//    plu_small=true;
//    _maintainProductPageTimeoutSec=40;
//    DbManager db(DB_PATH);
//    ui->numberEntry->show();
//    ui->textEntry->setText("");
//    ui->titleLabel->setText("New PLU/Barcode:");
//    ui->buttonPoint->hide();
//    ui->pluLabel->setText(QString::number(db.getPLU(idlePage->userDrinkOrder->getOption())));

//}

//void maintain_product::on_pluButton_l_clicked(){
//    qDebug() << "PLU Button clicked" << endl;
//    plu_large=true;
//    _maintainProductPageTimeoutSec=40;
//    DbManager db(DB_PATH);
//    ui->numberEntry->show();
//    ui->textEntry->setText("");
//    ui->titleLabel->setText("New PLU/Barcode:");
//    ui->buttonPoint->hide();
//    ui->pluLabel->setText(QString::number(db.getPLU(idlePage->userDrinkOrder->getOption())));

//}

void maintain_product::on_button1_clicked(){
    ui->textEntry->setText(ui->textEntry->text()+"1");

}

void maintain_product::on_button2_clicked(){
    ui->textEntry->setText(ui->textEntry->text()+"2");

}

void maintain_product::on_button3_clicked(){
    ui->textEntry->setText(ui->textEntry->text()+"3");

}

void maintain_product::on_button4_clicked(){
    ui->textEntry->setText(ui->textEntry->text()+"4");

}

void maintain_product::on_button5_clicked(){
    ui->textEntry->setText(ui->textEntry->text()+"5");

}

void maintain_product::on_button6_clicked(){
    ui->textEntry->setText(ui->textEntry->text()+"6");

}

void maintain_product::on_button7_clicked(){
    ui->textEntry->setText(ui->textEntry->text()+"7");

}

void maintain_product::on_button8_clicked(){
    ui->textEntry->setText(ui->textEntry->text()+"8");

}

void maintain_product::on_button9_clicked(){
    ui->textEntry->setText(ui->textEntry->text()+"9");

}

void maintain_product::on_button0_clicked(){
    ui->textEntry->setText(ui->textEntry->text()+"0");

}

void maintain_product::on_buttonBack_clicked(){
    ui->textEntry->backspace();

}

void maintain_product::on_buttonPoint_clicked(){
    ui->textEntry->setText(ui->textEntry->text()+".");

}

void maintain_product::on_buttonDone_clicked(){
    if(pwm){
        if(ui->textEntry->text().toInt()>100){
            ui->errorLabel->setText("Error: Enter number less than 100");
            ui->textEntry->setText("");
        }else{
            text_entered = ui->textEntry->text();
            ui->buttonPoint->show();
            ui->numberEntry->hide();
            updateValues();
        }
    }else{
        text_entered = ui->textEntry->text();
        ui->numberEntry->hide();
        updateValues();
    }
}

void maintain_product::on_buttonCancel_clicked(){
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

void maintain_product::updateValues(){
    DbManager db(DB_PATH);
    int checkOption = idlePage->userDrinkOrder->getOption();

    if(price_small){
        db.updatePriceSmall(checkOption, text_entered.toDouble());
        ui->price_small->setText("$" + QString::number(db.getProductPrice(checkOption, 's')));

    }else if(price_large){
        db.updatePriceLarge(checkOption, text_entered.toDouble());
        ui->price_large->setText("$" + QString::number(db.getProductPrice(checkOption, 'l')));

    }else if(target_s){
        db.updateTargetVolume_s(checkOption, text_entered.toDouble());
        ui->target_volume_s->setText(QString::number(db.getProductVolume(checkOption, 's')) + " " +  db.getUnits(checkOption));

    }else if(target_l){
        db.updateTargetVolume_l(checkOption, text_entered.toDouble());
        ui->target_volume_l->setText(QString::number(db.getProductVolume(checkOption, 'l')) + " " +  db.getUnits(checkOption));
    }else if(vol_per_tick){
        db.updateVolumePerTick(checkOption, text_entered.toDouble());
        ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(checkOption)) + " " +  db.getUnits(checkOption));
        ticks = db.getProductVolumePerTick(checkOption);

    }else if(full){
        db.updateFullVolume(checkOption, text_entered.toDouble());
        ui->full_volume->setText(QString::number(db.getFullProduct(checkOption)) + db.getUnits(checkOption));

    }else if(pwm){
        int new_pwm = round(((text_entered.toInt())*255)/100);
//        qDebug() << "New Pump Speed: " << text_entered.toInt() << "% equals = " << (new_pwm) << endl;
        db.updatePWM(checkOption, new_pwm);
        ui->pwmLabel->setText(QString::number(round(double((db.getPWM(checkOption))*100)/255)) + "%");
    }
//    else if(plu_small){
//        db.updatePluSmall(checkOption, text_entered);
//    }else if(plu_large){
//        db.updatePluLarge(checkOption, text_entered);
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


void maintain_product::pwmSliderMoved(int percentage){
    int value = ui->pwmSlider->value();
//    qDebug() << "Slider Value: " << value << endl;

    QString command = QString::number(this->idlePage->userDrinkOrder->getOption());
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

size_t WriteCallback3(char* contents, size_t size, size_t nmemb, void *userp){
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void maintain_product::curler(){

    DbManager db(DB_PATH);
    int checkOption = idlePage->userDrinkOrder->getOption();

    QString curl_param = "pid="+db.getProductID(checkOption)+"&volume_full="+QString::number(db.getFullProduct(checkOption));
    curl_param_array = curl_param.toLocal8Bit();
    curl_data = curl_param_array.data();

    curl = curl_easy_init();
    if (!curl){
//        qDebug() << "cURL failed to init" << endl;
    }else{
//        qDebug() << "cURL init success" << endl;

        curl_easy_setopt(curl, CURLOPT_URL, "http://Drinkfill-env.eba-qatmjpdr.us-east-2.elasticbeanstalk.com/api/machine_data/resetStock");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curl_param_array.data());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback3);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
//        qDebug() << "Curl Setup done" << endl;

        res = curl_easy_perform(curl);

        if (res != CURLE_OK){
//            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            bufferCURL(curl_data);
        }else{
//            qDebug() << "CURL SUCCESS!" << endl;
//            std::cout <<"Here's the output:\n" << readBuffer << endl;
            if (readBuffer == "true"){
                curl_easy_cleanup(curl);
                readBuffer = "";
            }else if (readBuffer == "false"){
                // TODO: Curl Buffer here but not sure of return state (currently false)
                curl_easy_cleanup(curl);
                readBuffer = "";
            }else{
                curl_easy_cleanup(curl);
                readBuffer = "";
            }
        }
    }
    db.closeDB();
}

void maintain_product::bufferCURL(char *curl_params){
    char filetime[50];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(filetime, 50, "%F %T", timeinfo);
    std::string filelocation = "/home/df-admin/curlBuffer/";
    std::string filetype = "_MM.txt";
    std::string filename = filelocation+filetime+filetype;
//    std::cout << "filename is: " << filename << endl;
    std::ofstream out;
    out.open(filename);
    if (!out.is_open()){
//        std::cout << "Cannot open output file!";
    }else{
        out << curl_params;
        out.close();
    }
}

