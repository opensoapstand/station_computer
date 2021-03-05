#include "maintain_product.h"
#include "ui_maintain_product.h"
#include "idle.h"
#include "drinkorder.h"

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

    //ui->pump_label->setText("OFF");

}

// DTOR
maintain_product::~maintain_product()
{
    delete ui;
}

/*
 * Page Tracking reference
 */
void maintain_product::setPage(maintenancePage* pageMaintenance, idle* pageIdle)
{
    this->maintenanceMode = pageMaintenance;
    this->idlePage = pageIdle;

    ui->name->setText("Product Name: ");
    ui->price_s->setText("Product Price: ");
    ui->price_l->setText("Product Price: ");
    ui->target_volume_s->setText("Product Volume: ");
    ui->target_volume_l->setText("Product Volume: ");
    ui->volume_per_tick->setText("Product Volume Per Tick: ");
    ui->refillLabel->setText("");
    ui->soldOutLabel->setText("");
    ui->full_volume->setText("");
}

void maintain_product::on_backButton_clicked(){
    qDebug() << "Back button clicked" << endl;
    maintenanceMode->showFullScreen();
    this->hide();

    ui->name->setText("");
    ui->price_s->setText("");
    ui->price_l->setText("");
    ui->target_volume_s->setText("");
    ui->target_volume_l->setText("");
    ui->volume_per_tick->setText("");
    ui->refillLabel->setText("");
    ui->soldOutLabel->setText("");
    ui->full_volume->setText("");


    if (pumping) {
        qDebug() << "Stopping pump" << endl;
        pumping = false;
        QString command = QString::number(this->idlePage->userDrinkOrder->getOption());
        command.append("s");
//        ui->pump_label->setText("OFF");
//        ui->vol_dispensed_label->setText("");

        this->idlePage->dfUtility->msg = command;
        idlePage->dfUtility->m_IsSendingFSM = true;
        idlePage->dfUtility->m_fsmMsg = SEND_CLEAN;
        idlePage->dfUtility->send_to_FSM();
        idlePage->dfUtility->m_IsSendingFSM = false;
    }
}

void maintain_product::setValues(int option){

    DbManager db(DB_PATH);

    ui->name->setText("");
    ui->price_s->setText("");
    ui->price_l->setText("");
    ui->target_volume_s->setText("");
    ui->target_volume_l->setText("");
    ui->volume_per_tick->setText("");
    ui->full_volume->setText("");

    switch (option){
        case 1:
            ui->name->setText(db.getProductName(1));
            ui->price_s->setText("$"+QString::number(db.getProductPrice(1, 's')));
            ui->price_l->setText("$"+QString::number(db.getProductPrice(1, 'l')));
            ui->target_volume_s->setText(QString::number(db.getProductVolume(1, 's')) + "ml");
            ui->target_volume_l->setText(QString::number(db.getProductVolume(1, 'l')) + "ml");
            ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(1)) + "ml");
            ui->full_volume->setText(QString::number(db.getFullProduct(1)) + "ml");
            break;
        case 2:
            ui->name->setText(db.getProductName(2));
            ui->price_s->setText("$"+QString::number(db.getProductPrice(2, 's')));
            ui->price_l->setText("$"+QString::number(db.getProductPrice(2, 'l')));
            ui->target_volume_s->setText(QString::number(db.getProductVolume(2, 's')) + "ml");
            ui->target_volume_l->setText(QString::number(db.getProductVolume(2, 'l')) + "ml");
            ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(2)) + "ml");
            ui->full_volume->setText(QString::number(db.getFullProduct(2)) + "ml");
            break;
        case 3:
            ui->name->setText(db.getProductName(3));
            ui->price_s->setText("$"+QString::number(db.getProductPrice(3, 's')));
            ui->price_l->setText("$"+QString::number(db.getProductPrice(3, 'l')));
            ui->target_volume_s->setText(QString::number(db.getProductVolume(3, 's')) + "ml");
            ui->target_volume_l->setText(QString::number(db.getProductVolume(3, 'l')) + "ml");
            ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(3)) + "ml");
            ui->full_volume->setText(QString::number(db.getFullProduct(3)) + "ml");
            break;
        case 4:
            ui->name->setText(db.getProductName(4));
            ui->price_s->setText("$"+QString::number(db.getProductPrice(4, 's')));
            ui->price_l->setText("$"+QString::number(db.getProductPrice(4, 'l')));
            ui->target_volume_s->setText(QString::number(db.getProductVolume(4, 's')) + "ml");
            ui->target_volume_l->setText(QString::number(db.getProductVolume(4, 'l')) + "ml");
            ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(4)) + "ml");
            ui->full_volume->setText(QString::number(db.getFullProduct(4)) + "ml");
            break;
    }
}

void maintain_product::resizeEvent(QResizeEvent *event){

    int checkOption = idlePage->userDrinkOrder->getOption();

    QString bitmap_location;

    if(checkOption > 0 && checkOption <= 9) {
        bitmap_location.append(":/maintenance/product");
        bitmap_location.append(QString::number(idlePage->userDrinkOrder->getOption()));
        bitmap_location.append(".png");
    } else {
        qDebug() << "out of range" << endl;
    }

    setValues(checkOption);

    QPixmap background(bitmap_location);
    QIcon ButtonIcon(background);

    ui->image->setIcon(ButtonIcon);
    ui->image->setIconSize(QSize(271,391));

}

void maintain_product::on_image_clicked(){
//    int checkOption = idlePage->userDrinkOrder->getOption();
//    if(checkOption > 0 && checkOption <= 9) {
//        QString command = QString::number(this->idlePage->userDrinkOrder->getOption());
//        if (!pumping){
//            command.append("s");

//            this->idlePage->dfUtility->msg = command;
//            idlePage->dfUtility->m_IsSendingFSM = true;
//            idlePage->dfUtility->m_fsmMsg = SEND_DRINK;
//            idlePage->dfUtility->send_to_FSM();
//            idlePage->dfUtility->m_IsSendingFSM = false;

//            pumping = true;
//            ui->pump_label->setText("ON");
//        }
//        else {
//            pumping = false;
//            ui->pump_label->setText("OFF");
//            ui->vol_dispensed_label->setText("");
//            command = QString::number(this->idlePage->userDrinkOrder->getOption());
//            command.append("s");

//            this->idlePage->dfUtility->msg = command;
//            idlePage->dfUtility->m_IsSendingFSM = true;
//            idlePage->dfUtility->m_fsmMsg = SEND_CLEAN;
//            idlePage->dfUtility->send_to_FSM();
//            idlePage->dfUtility->m_IsSendingFSM = false;
//        }
//    }

}

void maintain_product::on_nameButton_clicked(){
    qDebug() << "Name button clicked" << endl;
}


void maintain_product::on_priceButton_clicked(){
    qDebug() << "Price button clicked" << endl;
}


void maintain_product::on_target_volumeButton_clicked(){
    qDebug() << "Target Volume button clicked" << endl;
}


void maintain_product::on_vol_per_tickButton_clicked(){
    qDebug() << "Volume Per Tick button clicked" << endl;
}

//void maintain_product::updateVolumeDisplayed(int dispensed){
//    int vol_dispensed = dispensed;
//    ui->vol_dispensed_label->setText(QString::number(vol_dispensed) + "ml");
//}

//void maintain_product::targetHitDisplay(){
//    ui->vol_dispensed_label->setText(ui->vol_dispensed_label->text() + " - TARGET HIT!");
//}

void maintain_product::on_refillButton_clicked(){
    DbManager db(DB_PATH);
    qDebug() << "Refill button clicked" << endl;

    // ARE YOU SURE YOU WANT TO COMPLETE?
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    msgBox.setText("<p align=center>Are you sure you want to refill?</p>");
    msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int ret = msgBox.exec();

    switch(ret){
        case QMessageBox::Yes:
            qDebug() << "YES CLICKED" << endl;

            if(db.refill(this->idlePage->userDrinkOrder->getOption())){
                qDebug() << "REFILLED!" << endl;
                ui->refillLabel->setText("Refill Succesfull");
                break;
            }
            else{
                ui->refillLabel->setText("Refill ERROR");
                break;
            }

        case QMessageBox::No:
            qDebug() << "No Clicked" << endl;
            msgBox.hide();
        break;
    }
}

void maintain_product::on_soldOutButton_clicked(){
    DbManager db(DB_PATH);
    qDebug() << "Sold Out button clicked" << endl;

    // ARE YOU SURE YOU WANT TO COMPLETE?
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    msgBox.setText("<p align=center>Are you sure you want to mark as Sold Out?</p>");
    msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int ret = msgBox.exec();

    switch(ret){
        case QMessageBox::Yes:
            qDebug() << "YES CLICKED" << endl;

            if(db.sellout(this->idlePage->userDrinkOrder->getOption())){
                qDebug() << "REFILLED!" << endl;
                ui->soldOutLabel->setText("Sold Out Succesfull");
                break;
            }
            else{
                ui->soldOutLabel->setText("Sold Out ERROR");
                break;
            }

        case QMessageBox::No:
            qDebug() << "No Clicked" << endl;
            msgBox.hide();
        break;
    }
}

void maintain_product::on_fullButton_clicked(){
    qDebug() << "Full Volume button clicked" << endl;
}
