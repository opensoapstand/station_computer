#include "page_maintenance.h"
#include "ui_page_maintenance.h"
#include <QProcess>

#include "idle.h"

int select1 = 1;
int select2 = 2;
int select3 = 3;
int select4 = 4;
int selection = 0;

// CTOR
page_maintenance::page_maintenance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_maintenance)
{
    // Fullscreen background setup
    ui->setupUi(this);
    QPixmap background1("/release/references/products/product1.png");
    QIcon ButtonIcon1(background1);

    QPixmap background2("/release/references/products/product2.png");
    QIcon ButtonIcon2(background2);

    QPixmap background3("/release/references/products/product3.png");
    QIcon ButtonIcon3(background3);

    QPixmap background4("/release/references/products/product4.png");
    QIcon ButtonIcon4(background4);


   // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, Qt::white);
    this->setPalette(palette);

    ui->product1_button->setIcon(ButtonIcon1);
    ui->product1_button->setIconSize(QSize(241,381));

    ui->product2_button->setIcon(ButtonIcon2);
    ui->product2_button->setIconSize(QSize(241,381));

    ui->product3_button->setIcon(ButtonIcon3);
    ui->product3_button->setIconSize(QSize(241,381));

    ui->product4_button->setIcon(ButtonIcon4);
    ui->product4_button->setIconSize(QSize(241,381));

    page_maintenanceEndTimer = new QTimer(this);
    page_maintenanceEndTimer->setInterval(1000);
    connect(page_maintenanceEndTimer, SIGNAL(timeout()), this, SLOT(onPage_maintenanceTimeoutTick()));
//    connect(ui->buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)),this, SLOT(on_buttonGroup_buttonClicked()));
    connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(buttonWasClicked(int)));

}

// DTOR
page_maintenance::~page_maintenance()
{
    delete ui;
}

void page_maintenance::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    DbManager db(DB_PATH);
//    db.addPageClick("PAGE_PAGE_MAINTENANCE PAGE ENTERED");

    if(page_maintenanceEndTimer == nullptr){
        page_maintenanceEndTimer = new QTimer(this);
        page_maintenanceEndTimer->setInterval(1000);
        connect(page_maintenanceEndTimer, SIGNAL(timeout()), this, SLOT(onPage_maintenanceTimeoutTick()));
    }

    //page_maintenanceEndTimer->start(1000);
    _page_maintenanceTimeoutSec = 30;

    ui->product1_label->setText(db.getProductName(1));
    ui->product2_label->setText(db.getProductName(2));
    ui->product3_label->setText(db.getProductName(3));
    ui->product4_label->setText(db.getProductName(4));
    ui->machineLabel->setText("Machine ID: " + db.getMachineID());

    QProcess process;
   
    process.start("iwgetid -r");
    process.waitForFinished(-1);
    QString stdout = process.readAllStandardOutput();
    ui->wifi_name->setText("Wifi Name: " + stdout);

    process.start("hostname -I");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->wifi_ip_address->setText("Wifi IP Address: " + stdout);

    process.start("nmcli -t -f STATE general");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->wifi_status->setText("Wifi State: " + stdout);

    process.start("nmcli networking connectivity");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->wifi_internet->setText("Wifi Connectivity: " + stdout);

    ui->wifiTable->setRowCount(0);

    ui->keyboard_2->hide();

    selection_PageOne->cancelTimers();
    p_pageProduct->cancelTimers();

    db.closeDB();

}

/*
 * Page Tracking reference
 */
void page_maintenance::setPage(idle* pageIdle, page_maintenance_dispenser* pageMaintain, pageproductsoverview *p_pageProduct, pageProduct *pagePaySelect)
{
    this->idlePage = pageIdle;
    this->maintainPage = pageMaintain;
    this->selection_PageOne = p_pageProduct;
    this->p_pageProduct = pagePaySelect;
}

void page_maintenance::on_backButton_clicked(){
//    qDebug() << "Back button clicked" << endl;

    page_maintenanceEndTimer->stop();
    idlePage->showFullScreen();
//    usleep(100);
    this->hide();
}

void page_maintenance::on_product1_button_clicked(){
//    qDebug() << "Product 1 button clicked" << endl;
    page_maintenanceEndTimer->stop();

    idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_1);
    idlePage->userDrinkOrder->setDrinkSize(DRINK1);

    maintainPage->resizeEvent(productSelection);
    maintainPage->showFullScreen();
//    usleep(100);
    this->hide();

}

//void page_maintenance::on_buttonGroup_buttonClicked(QAbstractButton*){}
//void page_maintenance::buttonWasClicked(int){}

void page_maintenance::on_product2_button_clicked(){
//    qDebug() << "Product 2 button clicked" << endl;
    page_maintenanceEndTimer->stop();

    idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_2);
    idlePage->userDrinkOrder->setDrinkSize(DRINK2);

    maintainPage->resizeEvent(productSelection);
    maintainPage->showFullScreen();
//    usleep(100);
    this->hide();

}

void page_maintenance::on_product3_button_clicked(){
//    qDebug() << "Product 3 button clicked" << endl;
    page_maintenanceEndTimer->stop();

    idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_3);
    idlePage->userDrinkOrder->setDrinkSize(DRINK3);

    maintainPage->resizeEvent(productSelection);
    maintainPage->showFullScreen();
//    usleep(100);
    this->hide();

}

void page_maintenance::on_product4_button_clicked(){
//    qDebug() << "Product 4 button clicked" << endl;
    page_maintenanceEndTimer->stop();

    idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_4);
    idlePage->userDrinkOrder->setDrinkSize(DRINK4);

    maintainPage->resizeEvent(productSelection);
    maintainPage->showFullScreen();
//    usleep(100);
    this->hide();

}

//void page_maintenance::on_product5_button_clicked(){
//    qDebug() << "Product 5 button clicked" << endl;

//}

//void page_maintenance::on_product6_button_clicked(){
//    qDebug() << "Product 6 button clicked" << endl;

//}

//void page_maintenance::on_product7_button_clicked(){
//    qDebug() << "Product 7 button clicked" << endl;

//}

//void page_maintenance::on_product8_button_clicked(){
//    qDebug() << "Product 8 button clicked" << endl;

//}

//void page_maintenance::on_product9_button_clicked(){
//    qDebug() << "Product 9 button clicked" << endl;

//}

void page_maintenance::on_wifiButton_clicked(){
//    qDebug() << "WiFi button clicked" << endl;
    _page_maintenanceTimeoutSec = 30;
    ui->wifiTable->setRowCount(0);

    // OPEN LIST OF WIFI CONNECTIONS AVAILABLE, AS BUTTONS, WHEN YOU CLICK ON A BUTTON, OPEN PASSWORD ENTRY

    QDBusInterface nm("org.freedesktop.NetworkManager", "/org/freedesktop/NetworkManager", "org.freedesktop.NetworkManager", QDBusConnection::systemBus());
    if (!nm.isValid()){
        qFatal("Failed to connect to the system bus");
    }
    QDBusMessage msg = nm.call("GetDevices");
    //qDebug() << "GetDevices reply: " << msg << endl;
    QDBusArgument arg = msg.arguments().at(0).value<QDBusArgument>();

    if (arg.currentType() != QDBusArgument::ArrayType){
        qFatal("Something went wrong with the device list");
    }

    QList<QDBusObjectPath> pathsList = qdbus_cast<QList<QDBusObjectPath> >(arg);
    foreach(QDBusObjectPath p, pathsList){
        //qDebug() << "DEV PATH: " << p.path();
        QDBusInterface device("org.freedesktop.NetworkManager", p.path(), "org.freedesktop.NetworkManager.Device", QDBusConnection::systemBus());
        if(device.property("DeviceType").toInt() != 2){
            continue;
        }
        QDBusInterface wifi_device("org.freedesktop.NetworkManager", p.path(), "org.freedesktop.NetworkManager.Device.Wireless", QDBusConnection::systemBus());
        QMap<QString, QVariant> argList;
        QDBusMessage msg = wifi_device.call("RequestScan", argList);
        QThread::sleep(2);

        msg = wifi_device.call("GetAllAccessPoints");
        //qDebug() << "Answer for GetAllAccessPoints: " << msg << endl;
        QDBusArgument ap_list_arg = msg.arguments().at(0).value<QDBusArgument>();
        QList<QDBusObjectPath> ap_path_list = qdbus_cast<QList<QDBusObjectPath> >(ap_list_arg);

        foreach(QDBusObjectPath p, ap_path_list){
            QDBusInterface ap_interface("org.freedesktop.NetworkManager", p.path(), "org.freedesktop.NetworkManager.AccessPoint", QDBusConnection::systemBus());
            if(ap_interface.property("Ssid").toString() != ""){
//                qDebug() << " SSID: " << ap_interface.property("Ssid").toString();
                ui->wifiTable->insertRow(ui->wifiTable->rowCount());
                ui->wifiTable->setRowHeight(ui->wifiTable->rowCount()-1, 60);
                QWidget* pWidget = new QWidget();
                QPushButton* btn = new QPushButton();
                btn->setText(ap_interface.property("Ssid").toString());
                btn->setObjectName(ap_interface.property("Ssid").toString());
                QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
                btn->setMinimumHeight(50);
                pLayout->addWidget(btn);
                //pLayout->setAlignment(Qt::AlignLeft);
                pLayout->setContentsMargins(0,0,0,0);
                pWidget->setLayout(pLayout);
                ui->wifiTable->setCellWidget(ui->wifiTable->rowCount()-1, 0, pWidget);
                connect(btn, SIGNAL(clicked()), this, SLOT(btn_clicked()));

            }
        }
    }

    QProcess process;

   process.start("iwgetid -r");
    process.waitForFinished(-1);
    QString stdout = process.readAllStandardOutput();
    ui->wifi_name->setText("Wifi Name: " + stdout);

    process.start("hostname -I");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->wifi_ip_address->setText("Wifi IP Address: " + stdout);

    process.start("nmcli -t -f STATE general");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->wifi_status->setText("Wifi State: " + stdout);

    process.start("nmcli networking connectivity");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->wifi_internet->setText("Wifi Connectivity: " + stdout);

}

int getSelection(){
    return selection;
}

void page_maintenance::btn_clicked(){
    QObject* button = QObject::sender();
//    qDebug() << "btn clicked -> " << button->objectName();
    _page_maintenanceTimeoutSec = 30;
    // OPEN ON-SCREEN KEYBOARD FOR PASSWORD ENTRY

    ui->keyboard_2->show();
    ui->wifiPassLabel->setText(button->objectName());
    ui->keyboardTextEntry->setText("");

    QProcess process;
    process.start("iwgetid -r"); // nmcli -t -f NAME connection show --active
    process.waitForFinished(-1);
    QString stdout = process.readAllStandardOutput();
    ui->wifi_name->setText("Wifi Name: " + stdout);
    
    process.start("hostname -I");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->wifi_ip_address->setText("Wifi IP Address: " + stdout);

    process.start("nmcli networking connectivity");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->wifi_internet->setText("Wifi Connectivity: " + stdout);
    
    process.start("nmcli -t -f STATE general");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->wifi_status->setText("Wifi State: " + stdout);

}

void page_maintenance::onPage_maintenanceTimeoutTick(){

    if(-- _page_maintenanceTimeoutSec >= 0) {
//        qDebug() << "Page_page_maintenance Tick Down: " << _page_maintenanceTimeoutSec << endl;
    } else {
//        qDebug() << "Page_page_maintenance Timer Done!" << _page_maintenanceTimeoutSec << endl;

        page_maintenanceEndTimer->stop();
        idlePage->showFullScreen();
//        usleep(100);
        this->hide();
    }
}

void page_maintenance::buttonWasClicked(int buttonID){

    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    //qDebug() << buttonpressed->text();
    QString buttonText = buttonpressed->text();

    if(buttonText=="Cancel"){
        ui->keyboard_2->hide();
        ui->keyboardTextEntry->setText("");
    }
    else if(buttonText=="CAPS"){
        foreach (QAbstractButton *button, ui->buttonGroup->buttons()) {
            if (button->text()=="Space" || button->text()=="Done" || button->text()=="Cancel" || button->text()=="Clear" || button->text()=="Backspace"){
                //qDebug() << "doing nothing";
            }else{
                button->setText(button->text().toLower());
            }
        }
    }
    else if(buttonText=="caps"){
        foreach (QAbstractButton *button, ui->buttonGroup->buttons()) {
            if (button->text()=="Space" || button->text()=="Done" || button->text()=="Cancel" || button->text()=="Clear" || button->text()=="Backspace"){
                //doing nothing
            }else{
                button->setText(button->text().toUpper());
            }
        }
    }
    else if(buttonText=="Backspace"){
        ui->keyboardTextEntry->backspace();
    }
    else if(buttonText=="Clear"){
        ui->keyboardTextEntry->setText("");
    }
    else if(buttonText=="Done"){
        QString password = ui->keyboardTextEntry->text();
//        qDebug() << "Password: " << password;
        // ATTEMPT nmcli connection

        QString connect_string = "nmcli dev wifi connect '" + ui->wifiPassLabel->text() +"' password '" + ui->keyboardTextEntry->text() + "'";
        QByteArray ba = connect_string.toLocal8Bit();
        const char *c_str = ba.data();
//        qDebug() << c_str;
        system(c_str);

        ui->keyboard_2->hide();

        QProcess process;
        process.start("iwgetid -r");
        process.waitForFinished(-1);
        QString stdout = process.readAllStandardOutput();
        ui->wifi_name->setText("Wifi Name: " + stdout);

        process.start("hostname -I");
        process.waitForFinished(-1);
        stdout = process.readAllStandardOutput();
        ui->wifi_ip_address->setText("Wifi IP Address: " + stdout);

        process.start("nmcli -t -f STATE general");
        process.waitForFinished(-1);
        stdout = process.readAllStandardOutput();
        ui->wifi_status->setText("Wifi State: " + stdout);

        process.start("nmcli networking connectivity");
        process.waitForFinished(-1);
        stdout = process.readAllStandardOutput();
        ui->wifi_internet->setText("Wifi Connectivity: " + stdout);
    }
    else if(buttonText=="Space"){
        ui->keyboardTextEntry->setText(ui->keyboardTextEntry->text()+" ");
    }
    else if(buttonText=="&&"){
        ui->keyboardTextEntry->setText(ui->keyboardTextEntry->text()+"&");
    }
    else{
        ui->keyboardTextEntry->setText(ui->keyboardTextEntry->text() + buttonText);
    }
}
