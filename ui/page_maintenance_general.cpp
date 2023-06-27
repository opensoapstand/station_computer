
#include "page_maintenance_general.h"
#include "ui_page_maintenance_general.h"
#include "page_idle.h"

#include <QInputDialog>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QQuickView>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QSlider>
#include <QProcess>

// CTOR
page_maintenance_general::page_maintenance_general(QWidget *parent) : QWidget(parent),
                                                                      ui(new Ui::page_maintenance_general)
{
    // Fullscreen background setup
    ui->setupUi(this);

    QPalette palette;
    palette.setBrush(QPalette::Background, Qt::white);
    this->setPalette(palette);

    connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(keyboardButtonPressed(int)));
}

// DTOR
page_maintenance_general::~page_maintenance_general()
{
    delete ui;
}

void page_maintenance_general::showEvent(QShowEvent *event)
{
    p_page_idle->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    ui->checkBox_enable_empty_container->setText("Enable auto empty detection. (If disabled, will display sold out if less than " + QString::number(CONTAINER_EMPTY_THRESHOLD_ML) + "ml remaining)");

    QProcess process;

    process.start("iwgetid -r");
    process.waitForFinished(-1);
    QString stdout = process.readAllStandardOutput();
    ui->label_wifi_name->setText("Wifi Name: " + stdout);

    process.start("hostname -I");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->label_wifi_ip_address->setText("Wifi IP Address: " + stdout);

    process.start("nmcli -t -f STATE general");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->label_wifi_status->setText("Wifi State: " + stdout);

    process.start("nmcli networking connectivity");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->label_wifi_internet->setText("Wifi Connectivity: " + stdout);

    ui->table_wifi_networks->setRowCount(0);

    ui->keyboard_2->hide();


    p_page_idle->setTemplateTextToObject(ui->pushButton_back);
    p_page_idle->setTemplateTextToObject(ui->label_connectivity);
    p_page_idle->setTemplateTextToObject(ui->pushButton_wifi_networks);
    p_page_idle->setTemplateTextToObject(ui->pushButton_network_status);
    p_page_idle->setTemplateTextToObject(ui->pushButton_rtunnel_restart);
    p_page_idle->setTemplateTextToObject(ui->label_receipt_printer);
    p_page_idle->setTemplateTextToObject(ui->pushButton_printer_check_status);
    p_page_idle->setTemplateTextToObject(ui->pushButton_printer_test_print);
    p_page_idle->setTemplateTextToObject(ui->pushButton_minimize);
    p_page_idle->setTemplateTextToObject(ui->label_settings);
    p_page_idle->setTemplateTextToObject(ui->pushButton_restart_electronics);
    p_page_idle->setTemplateTextToObject(ui->pushButton_restart_UI);
    p_page_idle->setTemplateTextToObject(ui->pushButton_reboot);
    p_page_idle->setTemplateTextToObject(ui->pushButton_shutdown);
    p_page_idle->setTemplateTextToObject(ui->pushButton_reboot);
    p_page_idle->setTemplateTextToObject(ui->label_feedback);
    p_page_idle->setTemplateTextToObject(ui->label_status_feedback);

    updateLabelValues();
}

/*
 * Page Tracking reference
 */
void page_maintenance_general::setPage(page_maintenance *pageMaintenance, page_idle *pageIdle, page_idle_products *p_page_idle_products)
{

    this->p_page_maintenance = pageMaintenance;
    this->p_page_idle = pageIdle;

    ui->pushButton_minimize->setStyleSheet("QPushButton {}"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
}

void page_maintenance_general::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    p_page_idle->pageTransition(this, pageToShow);
}
void page_maintenance_general::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void page_maintenance_general::updateLabelValues()
{
    p_page_idle->loadDynamicContent();
    ui->checkBox_enable_empty_container->setChecked(p_page_idle->thisMachine.getEmptyContainerDetectionEnabled());
    ui->checkBox_enable_pump_ramping->setChecked(p_page_idle->thisMachine.getPumpRampingEnabled());
}

void page_maintenance_general::printerStatusFeedback(bool isOnline, bool hasPaper)
{
    qDebug() << "Maintenance general. Receipt printer Feedback from FSM.";

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
    ui->pushButton_printer_check_status->show();
}

void page_maintenance_general::send_check_printer_status_command()
{
    qDebug() << "Send check printer status to controller";
    p_page_idle->dfUtility->send_command_to_FSM("p");
    usleep(50000);
    usleep(1200000); // minimum
    // usleep(5000000); 
    // p_page_idle->dfUtility->send_command_to_FSM("1");
    // usleep(5000000); 
    p_page_idle->dfUtility->send_command_to_FSM("a");
    usleep(50000);
}

void page_maintenance_general::on_pushButton_printer_check_status_clicked()
{
    // qDebug() << "Maintenance general. yoooo.";
    send_check_printer_status_command();
    ui->pushButton_printer_check_status->hide();
}

void page_maintenance_general::on_printer_test_print_button_clicked()
{
    qDebug() << "Send test printer to controller";
    p_page_idle->dfUtility->send_command_to_FSM("p");
    usleep(50000);
    p_page_idle->dfUtility->send_command_to_FSM("1");
    usleep(50000);
    p_page_idle->dfUtility->send_command_to_FSM("q");
}

void page_maintenance_general::on_checkBox_enable_pump_ramping_clicked(bool checked)
{
    if (!p_page_idle->thisMachine.isAllowedAsAdmin())
    {
        QMessageBox::information(this, "Admininstrator role required", "You do not have the rights to change these values. Please enter maintenance mode with the admin password.", QMessageBox::Ok);
        return;
    }

    if (checked != p_page_idle->thisMachine.getPumpRampingEnabled())
    {
        p_page_idle->thisMachine.setPumpRampingEnabled(checked);
        updateLabelValues();
    }
}

void page_maintenance_general::on_checkBox_enable_empty_container_clicked(bool checked)
{
    if (!p_page_idle->thisMachine.isAllowedAsAdmin())
    {
        QMessageBox::information(this, "Admininstrator role required", "You do not have the rights to change these values. Please enter maintenance mode with the admin password.", QMessageBox::Ok);
        return;
    }

    if (checked != p_page_idle->thisMachine.getEmptyContainerDetectionEnabled())
    {
        p_page_idle->thisMachine.setEmptyContainerDetectionEnabled(checked);
        updateLabelValues();
    }
}

void page_maintenance_general::on_pushButton_back_clicked()
{
    hideCurrentPageAndShowProvided(p_page_maintenance);
}

void page_maintenance_general::on_pushButton_minimize_clicked()
{
    if (!p_page_idle->thisMachine.isAllowedAsAdmin())
    {
        QMessageBox::information(this, "Admininstrator role required", "You do not have the rights to change these values. Please enter maintenance mode with the admin password.", QMessageBox::Ok);
        return;
    }
    qDebug() << "Maintenance Minimize button pressed.";
    this->showMinimized();
}

void page_maintenance_general::on_pushButton_reboot_clicked()
{
    if (!p_page_idle->thisMachine.isAllowedAsAdmin())
    {
        QMessageBox::information(this, "Admininstrator role required", "You do not have the rights to change these values. Please enter maintenance mode with the admin password.", QMessageBox::Ok);
        return;
    }
    qDebug() << "Maintenance Reboot button pressed.";
    QString command = "echo 'D@nkF1ll$' | sudo -S shutdown -r 0";
    system(qPrintable(command));
}

void page_maintenance_general::on_pushButton_shutdown_clicked()
{
    if (!p_page_idle->thisMachine.isAllowedAsAdmin())
    {
        QMessageBox::information(this, "Admininstrator role required", "You do not have the rights to change these values. Please enter maintenance mode with the admin password.", QMessageBox::Ok);
        return;
    }
    qDebug() << "Maintenance Shutdown button pressed.";

    QString command = "echo 'D@nkF1ll$' | sudo -S shutdown -h 0";
    system(qPrintable(command));
}

void page_maintenance_general::on_test_lineEdit_textChanged(const QString &arg1)
{
}

void page_maintenance_general::btn_clicked()
{
    QObject *button = QObject::sender();
    //    qDebug() << "btn clicked -> " << button->objectName();

    // _page_maintenanceTimeoutSec = PAGE_MAINTENANCE_TIMEOUT_SECONDS;

    // OPEN ON-SCREEN KEYBOARD FOR PASSWORD ENTRY

    ui->keyboard_2->show();
    ui->wifiPassLabel->setText(button->objectName());
    ui->keyboardTextEntry->setText("");

    QProcess process;
    process.start("iwgetid -r"); // nmcli -t -f NAME connection show --active
    process.waitForFinished(-1);
    QString stdout = process.readAllStandardOutput();
    ui->label_wifi_name->setText("Wifi Nfrfame: " + stdout);

    process.start("hostname -I");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->label_wifi_ip_address->setText("Wifi IP Address: " + stdout);

    process.start("nmcli networking connectivity");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->label_wifi_internet->setText("Wifi Connectivity: " + stdout);

    process.start("nmcli -t -f STATE general");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->label_wifi_status->setText("Wifi State: " + stdout);
}

void page_maintenance_general::keyboardButtonPressed(int buttonID)
{

    qDebug() << "button press maintenance mooode.....";
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
        ui->label_wifi_name->setText("Wifi Name: " + stdout);

        process.start("hostname -I");
        process.waitForFinished(-1);
        stdout = process.readAllStandardOutput();
        ui->label_wifi_ip_address->setText("Wifi IP Address: " + stdout);

        process.start("nmcli -t -f STATE general");
        process.waitForFinished(-1);
        stdout = process.readAllStandardOutput();
        ui->label_wifi_status->setText("Wifi State: " + stdout);

        process.start("nmcli networking connectivity");
        process.waitForFinished(-1);
        stdout = process.readAllStandardOutput();
        ui->label_wifi_internet->setText("Wifi Connectivity: " + stdout);
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

void page_maintenance_general::on_pushButton_wifi_networks_clicked()
{
    //    qDebug() << "WiFi button clicked" << endl;
    // _page_maintenanceTimeoutSec = PAGE_MAINTENANCE_TIMEOUT_SECONDS;
    ui->table_wifi_networks->setRowCount(0);

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
                ui->table_wifi_networks->insertRow(ui->table_wifi_networks->rowCount());
                ui->table_wifi_networks->setRowHeight(ui->table_wifi_networks->rowCount() - 1, 60);
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
                ui->table_wifi_networks->setCellWidget(ui->table_wifi_networks->rowCount() - 1, 0, pWidget);
                connect(btn, SIGNAL(clicked()), this, SLOT(btn_clicked()));
            }
        }
    }

    // set label for
    QProcess process;

    process.start("iwgetid -r");
    process.waitForFinished(-1);
    QString stdout = process.readAllStandardOutput();
    ui->label_wifi_name->setText("Wifi Name: " + stdout);

    process.start("hostname -I");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->label_wifi_ip_address->setText("Wifi IP Address: " + stdout);

    process.start("nmcli -t -f STATE general");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->label_wifi_status->setText("Wifi State: " + stdout);

    process.start("nmcli networking connectivity");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    ui->label_wifi_internet->setText("Wifi Connectivity: " + stdout);
}

void page_maintenance_general::on_pushButton_rtunnel_restart_clicked()
{

    // https://stackoverflow.com/questions/75689836/system-command-executed-from-qt-does-not-work-for-service

    // QProcess process;
    // process.start("echo 'D@nkF1ll$' | sudo -S -n systemctl start rtunnel.service");
    // process.start("echo 'D@nkF1ll$' | sudo -S -n journalctl -o cat -r -u  rtunnel.service");
    // process.start("echo 'D@nkF1ll$' | sudo -S -n service rtunnel restart");

    // QProcess process;
    // process.start("sudo -S -n systemctl restart rtunnel.service");
    // process.waitForStarted();
    // // usleep(1000000);
    // process.write("D@nkF1ll$");

    // process.waitForFinished(-1);
    // QString feedback = process.readAllStandardOutput();
    // // QString feedback = process.readAllStandardError();
    // qDebug() << "rtunnel restart status: " << feedback;
    // ui->label_status_feedback->setText("rtunnel restart " + feedback);

    QProcess process;
    process.start("bash");
    process.waitForStarted();
    process.write("echo 'D@nkF1ll$' | sudo -S systemctl restart rtunnel.service\n");
    // process.write("echo 'D@nkF1ll$' | sudo -S shutdown -h now\n");

    process.write("echo 'D@nkF1ll$' | sudo -S journalctl -r -u rtunnel.service | head -50\n");

    process.write("exit\n");
    process.waitForFinished(-1);
    QString feedback = process.readAllStandardOutput();

    ui->label_status_feedback->setText("rtunnel restart " + feedback);

    // https://stackoverflow.com/questions/23322739/how-to-execute-complex-linux-commands-in-qt

    // QProcess process1;
    // QProcess process2;

    // process1.setStandardOutputProcess(&process2);

    // process1.start("echo 'D@nkF1ll$'");
    // process2.start("sudo -S systemctl restart rtunnel.service");
    // process2.setProcessChannelMode(QProcess::ForwardedChannels);

    // // Wait for it to start
    // process1.waitForStarted();

    // bool retval = false;
    // QByteArray buffer;
    // // To be fair: you only need to wait here for a bit with shutdown,
    // // but I will still leave the rest here for a generic solution
    // while ((retval = process2.waitForFinished())){
    //     buffer.append(process2.readAll());
    // }

    // if (!retval)
    // {
    //     qDebug() << "Process 2 error:" << process2.errorString();
    //     ui->label_status_feedback->setText("Process 2 error:" + process2.errorString());
    //     return;
    // }
    // ui->label_status_feedback->setText("Process 2 success:");
}

void page_maintenance_general::on_pushButton_network_status_clicked()
{
    // iwconfig wlo2 | awk -F'[ =]+' '/Signal level/

    QProcess process;
    process.start("bash");
    process.waitForStarted();
    process.write("iwconfig\n");

    process.write("exit\n");
    process.waitForFinished(-1);
    QString feedback = process.readAllStandardOutput();

    ui->label_status_feedback->setText(feedback);
}

void page_maintenance_general::on_pushButton_restart_UI_clicked()
{
    if (!p_page_idle->thisMachine.isAllowedAsAdmin())
    {
        QMessageBox::information(this, "Admininstrator role required", "You do not have the rights to change these values. Please enter maintenance mode with the admin password.", QMessageBox::Ok);
        return;
    }
    // app is configured as an auto start service. So, dont bother about restarting.
    // QProcess::startDetached(qApp->applicationFilePath()); // if needed to start app
    qApp->exit();
}

void page_maintenance_general::on_pushButton_restart_electronics_clicked()
{
    if (!p_page_idle->thisMachine.isAllowedAsAdmin())
    {
        QMessageBox::information(this, "Admininstrator role required", "You do not have the rights to change these values. Please enter maintenance mode with the admin password.", QMessageBox::Ok);
        return;
    }

    QProcess process;
    process.start("bash");
    process.waitForStarted();
    process.write("echo 'D@nkF1ll$' | sudo -S systemctl restart controller_soapstand.service\n");
    process.write("exit\n");
    process.waitForFinished(-1);
    QString feedback = process.readAllStandardOutput();
    ui->label_status_feedback->setText(feedback);

    qApp->exit(); // restart UI. The problem with only controller restart is that there are two screens now.
}
