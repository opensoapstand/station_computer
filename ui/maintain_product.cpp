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

    ui->pump_label->setText("OFF");

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

    if (pumping) {
        qDebug() << "Stopping pump" << endl;
        pumping = false;
        QString command = QString::number(this->idlePage->userDrinkOrder->getOption());
        command.append("s");
        ui->pump_label->setText("OFF");
        ui->vol_dispensed_label->setText("");

        this->idlePage->dfUtility->msg = command;
        idlePage->dfUtility->m_IsSendingFSM = true;
        idlePage->dfUtility->m_fsmMsg = SEND_CLEAN;
        idlePage->dfUtility->send_to_FSM();
        idlePage->dfUtility->m_IsSendingFSM = false;
    }
}

void maintain_product::setValues(int option){

//    DbManager db(DB_PATH);

//    ui->name->setText("");
//    ui->price_s->setText("");
//    ui->price_l->setText("");
//    ui->target_volume_s->setText("");
//    ui->target_volume_l->setText("");
//    ui->volume_per_tick->setText("");

//    qDebug() << db.getProductVolume(1, SMALL_DRINK) << endl;

//    switch (option){
//        case 1:
//            ui->name->setText(db.getProductName(1));
//            ui->price_s->setText(QString::number(db.getProductPrice(1, SMALL_DRINK)));
//            ui->price_l->setText(QString::number(db.getProductPrice(1, LARGE_DRINK)));
//            ui->target_volume_s->setText(QString::number(db.getProductVolume(1, SMALL_DRINK)));
//            ui->target_volume_l->setText(QString::number(db.getProductVolume(1, LARGE_DRINK)));
//            ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(1)));
//            break;
//        case 2:
//            ui->name->setText(db.getProductName(2));
//            ui->price_s->setText(QString::number(db.getProductPrice(2, SMALL_DRINK)));
//            ui->price_l->setText(QString::number(db.getProductPrice(2, LARGE_DRINK)));
//            ui->target_volume_s->setText(QString::number(db.getProductVolume(2, SMALL_DRINK)));
//            ui->target_volume_l->setText(QString::number(db.getProductVolume(2, LARGE_DRINK)));
//            ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(2)));
//            break;
//        case 3:
//            ui->name->setText(db.getProductName(3));
//            ui->price_s->setText(QString::number(db.getProductPrice(3, SMALL_DRINK)));
//            ui->price_l->setText(QString::number(db.getProductPrice(3, LARGE_DRINK)));
//            ui->target_volume_s->setText(QString::number(db.getProductVolume(3, SMALL_DRINK)));
//            ui->target_volume_l->setText(QString::number(db.getProductVolume(3, LARGE_DRINK)));
//            ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(3)));
//            break;
//        case 4:
//            ui->name->setText(db.getProductName(4));
//            ui->price_s->setText(QString::number(db.getProductPrice(4, SMALL_DRINK)));
//            ui->price_l->setText(QString::number(db.getProductPrice(4, LARGE_DRINK)));
//            ui->target_volume_s->setText(QString::number(db.getProductVolume(4, SMALL_DRINK)));
//            ui->target_volume_l->setText(QString::number(db.getProductVolume(4, LARGE_DRINK)));
//            ui->volume_per_tick->setText(QString::number(db.getProductVolumePerTick(4)));
//            break;
//    }
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

void maintain_product::updateVolumeDisplayed(int dispensed){
    int vol_dispensed = dispensed;
    ui->vol_dispensed_label->setText(QString::number(vol_dispensed) + "ml");
}

void maintain_product::targetHitDisplay(){
    ui->vol_dispensed_label->setText(ui->vol_dispensed_label->text() + " - TARGET HIT!");
}

void maintain_product::on_refillButton_clicked(){
    qDebug() << "Refill button clicked" << endl;
    DbManager db(DB_PATH);
    if(db.refill(this->idlePage->userDrinkOrder->getOption())){
        qDebug() << "REFILLED!" << endl;
        ui->refillLabel->setText("Refill Succesfull");
    }
    else{
        ui->refillLabel->setText("Refill ERROR");
    }

}

