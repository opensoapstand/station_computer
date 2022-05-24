#include "page_maintenance.h"
#include "ui_page_maintenance.h"
#include <QProcess>

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
    //    connect(ui->buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)),this, SLOT(on_buttonGroup_buttonClicked()));
    connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(keyboardButtonPressed(int)));

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
    qDebug() << "<<<<<<< PPage Enter: maintenance >>>>>>>>>";

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
    ui->enable_empty_container_checkBox->setChecked(db.getEmptyContainerDetectionEnabled());
    ui->enable_pump_ramping_checkBox->setChecked(db.getPumpRampingEnabled());
    // qDebug()<<"ramping?"<<db.getPumpRampingEnabled();
    // ui->enable_pump_ramping_checkBox->setChecked(true);

    ui->product1_label->setText(db.getProductName(1));
    ui->product2_label->setText(db.getProductName(2));
    ui->product3_label->setText(db.getProductName(3));
    ui->product4_label->setText(db.getProductName(4));
    ui->machineLabel->setText("Machine ID: " + db.getMachineID());
    db.closeDB();

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

    p_pageSelectProduct->cancelTimers();
    p_pageProduct->cancelTimers();
}

/*
 * Page Tracking reference
 */
void page_maintenance::setPage(page_idle *pageIdle, page_maintenance_dispenser *pageMaintain, page_select_product *p_pageProduct, pageProduct *pagePaySelect)
{
    this->p_page_idle = pageIdle;
    this->p_page_maintenance_product = pageMaintain;
    this->p_pageSelectProduct = p_pageProduct;
    this->p_pageProduct = pagePaySelect;
}

void page_maintenance::on_backButton_clicked()
{
    //    qDebug() << "Back button clicked" << endl;

    page_maintenanceEndTimer->stop();
    // qDebug() << "maintenance2 to idle";
    p_page_idle->showFullScreen();
    //    usleep(100);
    this->hide();
}

void page_maintenance::on_product1_button_clicked()
{
    //    qDebug() << "Product 1 button clicked" << endl;
    page_maintenanceEndTimer->stop();

    p_page_idle->currentProductOrder->setSelectedSlot(1);
    p_page_idle->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);

    p_page_maintenance_product->resizeEvent(productSelection);
    p_page_maintenance_product->showFullScreen();
    //    usleep(100);
    this->hide();
}

// void page_maintenance::on_buttonGroup_buttonClicked(QAbstractButton*){}
// void page_maintenance::keyboardButtonPressed(int){}

void page_maintenance::on_product2_button_clicked()
{
    //    qDebug() << "Product 2 button clicked" << endl;
    page_maintenanceEndTimer->stop();

    p_page_idle->currentProductOrder->setSelectedSlot(2);
    p_page_idle->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);

    p_page_maintenance_product->resizeEvent(productSelection);
    p_page_maintenance_product->showFullScreen();
    //    usleep(100);
    this->hide();
}

void page_maintenance::on_product3_button_clicked()
{
    //    qDebug() << "Product 3 button clicked" << endl;
    page_maintenanceEndTimer->stop();

    p_page_idle->currentProductOrder->setSelectedSlot(3);
    p_page_idle->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);

    p_page_maintenance_product->resizeEvent(productSelection);
    p_page_maintenance_product->showFullScreen();
    //    usleep(100);
    this->hide();
}

void page_maintenance::on_product4_button_clicked()
{
    //    qDebug() << "Product 4 button clicked" << endl;
    page_maintenanceEndTimer->stop();

    p_page_idle->currentProductOrder->setSelectedSlot(4);
    p_page_idle->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);

    p_page_maintenance_product->resizeEvent(productSelection);
    p_page_maintenance_product->showFullScreen();
    //    usleep(100);
    this->hide();
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

void page_maintenance::on_wifiButton_clicked()
{
    //    qDebug() << "WiFi button clicked" << endl;
    _page_maintenanceTimeoutSec = PAGE_MAINTENANCE_TIMEOUT_SECONDS;
    ui->wifiTable->setRowCount(0);

    // OPEN LIST OF WIFI CONNECTIONS AVAILABLE, AS BUTTONS, WHEN YOU CLICK ON A BUTTON, OPEN PASSWORD ENTRY

    QDBusInterface nm("org.freedesktop.NetworkManager", "/org/freedesktop/NetworkManager", "org.freedesktop.NetworkManager", QDBusConnection::systemBus());
    if (!nm.isValid())
    {
        qFatal("Failed to connect to the system bus");
    }
    QDBusMessage msg = nm.call("GetDevices");
    // qDebug() << "GetDevices reply: " << msg << endl;
    QDBusArgument arg = msg.arguments().at(0).value<QDBusArgument>();

    if (arg.currentType() != QDBusArgument::ArrayType)
    {
        qFatal("Something went wrong with the device list");
    }

    QList<QDBusObjectPath> pathsList = qdbus_cast<QList<QDBusObjectPath>>(arg);
    foreach (QDBusObjectPath p, pathsList)
    {
        // qDebug() << "DEV PATH: " << p.path();
        QDBusInterface device("org.freedesktop.NetworkManager", p.path(), "org.freedesktop.NetworkManager.Device", QDBusConnection::systemBus());
        if (device.property("DeviceType").toInt() != 2)
        {
            continue;
        }
        QDBusInterface wifi_device("org.freedesktop.NetworkManager", p.path(), "org.freedesktop.NetworkManager.Device.Wireless", QDBusConnection::systemBus());
        QMap<QString, QVariant> argList;
        QDBusMessage msg = wifi_device.call("RequestScan", argList);
        QThread::sleep(2);

        msg = wifi_device.call("GetAllAccessPoints");
        // qDebug() << "Answer for GetAllAccessPoints: " << msg << endl;
        QDBusArgument ap_list_arg = msg.arguments().at(0).value<QDBusArgument>();
        QList<QDBusObjectPath> ap_path_list = qdbus_cast<QList<QDBusObjectPath>>(ap_list_arg);

        foreach (QDBusObjectPath p, ap_path_list)
        {
            QDBusInterface ap_interface("org.freedesktop.NetworkManager", p.path(), "org.freedesktop.NetworkManager.AccessPoint", QDBusConnection::systemBus());
            if (ap_interface.property("Ssid").toString() != "")
            {
                //                qDebug() << " SSID: " << ap_interface.property("Ssid").toString();
                ui->wifiTable->insertRow(ui->wifiTable->rowCount());
                ui->wifiTable->setRowHeight(ui->wifiTable->rowCount() - 1, 60);
                QWidget *pWidget = new QWidget();
                QPushButton *btn = new QPushButton();
                btn->setText(ap_interface.property("Ssid").toString());
                btn->setObjectName(ap_interface.property("Ssid").toString());
                QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
                btn->setMinimumHeight(50);
                pLayout->addWidget(btn);
                // pLayout->setAlignment(Qt::AlignLeft);
                pLayout->setContentsMargins(0, 0, 0, 0);
                pWidget->setLayout(pLayout);
                ui->wifiTable->setCellWidget(ui->wifiTable->rowCount() - 1, 0, pWidget);
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

int getSelection()
{
    return selection;
}

void page_maintenance::btn_clicked()
{
    QObject *button = QObject::sender();
    //    qDebug() << "btn clicked -> " << button->objectName();
    _page_maintenanceTimeoutSec = PAGE_MAINTENANCE_TIMEOUT_SECONDS;
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
        p_page_idle->showFullScreen();
        //        usleep(100);
        this->hide();
    }
}

void page_maintenance::keyboardButtonPressed(int buttonID)
{

    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    // qDebug() << buttonpressed->text();
    QString buttonText = buttonpressed->text();

    if (buttonText == "Cancel")
    {
        ui->keyboard_2->hide();
        ui->keyboardTextEntry->setText("");
    }
    else if (buttonText == "CAPS")
    {
        foreach (QAbstractButton *button, ui->buttonGroup->buttons())
        {
            if (button->text() == "Space" || button->text() == "Done" || button->text() == "Cancel" || button->text() == "Clear" || button->text() == "Backspace")
            {
                // qDebug() << "doing nothing";
            }
            else
            {
                button->setText(button->text().toLower());
            }
        }
    }
    else if (buttonText == "caps")
    {
        foreach (QAbstractButton *button, ui->buttonGroup->buttons())
        {
            if (button->text() == "Space" || button->text() == "Done" || button->text() == "Cancel" || button->text() == "Clear" || button->text() == "Backspace")
            {
                // doing nothing
            }
            else
            {
                button->setText(button->text().toUpper());
            }
        }
    }
    else if (buttonText == "Backspace")
    {
        ui->keyboardTextEntry->backspace();
    }
    else if (buttonText == "Clear")
    {
        ui->keyboardTextEntry->setText("");
    }
    else if (buttonText == "Done")
    {
        QString password = ui->keyboardTextEntry->text();
        //        qDebug() << "Password: " << password;
        // ATTEMPT nmcli connection

        QString connect_string = "nmcli dev wifi connect '" + ui->wifiPassLabel->text() + "' password '" + ui->keyboardTextEntry->text() + "'";
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
    else if (buttonText == "Space")
    {
        ui->keyboardTextEntry->setText(ui->keyboardTextEntry->text() + " ");
    }
    else if (buttonText == "&&")
    {
        ui->keyboardTextEntry->setText(ui->keyboardTextEntry->text() + "&");
    }
    else
    {
        ui->keyboardTextEntry->setText(ui->keyboardTextEntry->text() + buttonText);
    }
}

void page_maintenance::on_printer_test_button_clicked()
{

    qDebug() << "Send test printer to controller";

    // Send to fsm

    p_page_idle->dfUtility->send_command_to_FSM("p");
    usleep(50000);
    p_page_idle->dfUtility->send_command_to_FSM("1");
    usleep(50000);
    p_page_idle->dfUtility->send_command_to_FSM("q");
}

void page_maintenance::printerStatusFeedback(bool isOnline, bool hasPaper)
{
    qDebug() << "Feeback received . printer";

    QString printerStatus = "Printer is offline";
    if (isOnline)
    {
        printerStatus = "Printer is online";
    }

    QString printerHasPaper = "No paper detected.";
    if (hasPaper)
    {
        printerHasPaper = "Paper ok";
    }
    ui->printer_isOnline_label->setText(printerStatus);
    ui->printer_hasPaper_label->setText(printerHasPaper);
}

void page_maintenance::on_printer_check_status_clicked()
{
    qDebug() << "Send test printer to controller";
    p_page_idle->dfUtility->send_command_to_FSM("p");
    usleep(50000);
    p_page_idle->dfUtility->send_command_to_FSM("a");
    // usleep(50000);
    // p_page_idle->dfUtility->send_command_to_FSM("q"); // go back to fsm idle state is done in controller
}

void page_maintenance::on_enable_pump_ramping_checkBox_clicked(bool checked)
{
   //qDebug() << "test ramp clicked" << checked;
    DbManager db(DB_PATH);
    // qDebug() << "test empty db: " << db.getPumpRampingEnabled();
    if (checked != db.getPumpRampingEnabled())
    {
        qDebug() << "Write to db: Pump ramping enabled?" << checked;
        db.setPumpRampingEnabled(checked);
        ui->enable_pump_ramping_checkBox->setChecked(db.getPumpRampingEnabled());
    }
    db.closeDB();
}

void page_maintenance::on_enable_empty_container_checkBox_clicked(bool checked)
{
    // qDebug() << "test empty clicked" << checked;
    DbManager db(DB_PATH);
    // qDebug() << "test empty db: " << db.getEmptyContainerDetectionEnabled();
    if (checked != db.getEmptyContainerDetectionEnabled())
    {
        qDebug() << "Empty container detection enabled?" << checked;
        db.setEmptyContainerDetectionEnabled(checked);
        ui->enable_empty_container_checkBox->setChecked(db.getEmptyContainerDetectionEnabled());
    }
    db.closeDB();
}
