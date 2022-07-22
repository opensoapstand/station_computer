
#include "page_maintenance_general.h"
#include "ui_page_maintenance_general.h"
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

    // maintainProductPageEndTimer = new QTimer(this);
    // maintainProductPageEndTimer->setInterval(1000);
    // connect(maintainProductPageEndTimer, SIGNAL(timeout()), this, SLOT(onMaintainProductPageTimeoutTick()));
    // connect(ui->pwmSlider, SIGNAL(valueChanged(int)), this, SLOT(pwmSliderMoved(int)));
}

// DTOR
page_maintenance_general::~page_maintenance_general()
{
    delete ui;
}

void page_maintenance_general::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: maintenance general machine settings >>>>>>>>>";
  
}

/*
 * Page Tracking reference
 */
void page_maintenance_general::setPage(page_maintenance *pageMaintenance, page_idle *pageIdle)
{

    this->p_page_maintenance = pageMaintenance;
    this->p_page_idle = pageIdle;

    // refreshLabels();
}

void page_maintenance_general::refreshLabels()
{
    
}

void page_maintenance_general::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

}


void page_maintenance_general::on_printer_test_button_clicked()
{

    qDebug() << "Send test printer to controller";

    // Send to fsm

    p_page_idle->dfUtility->send_command_to_FSM("p");
    usleep(50000);
    p_page_idle->dfUtility->send_command_to_FSM("1");
    usleep(50000);
    p_page_idle->dfUtility->send_command_to_FSM("q");
}

void page_maintenance_general::printerStatusFeedback(bool isOnline, bool hasPaper)
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

void page_maintenance_general::on_printer_check_status_clicked()
{
    qDebug() << "Send test printer to controller";
    p_page_idle->dfUtility->send_command_to_FSM("p");
    usleep(50000);
    p_page_idle->dfUtility->send_command_to_FSM("a");
    // usleep(50000);
    // p_page_idle->dfUtility->send_command_to_FSM("q"); // go back to fsm idle state is done in controller
}


void page_maintenance_general::on_back_Button_clicked()
{
    // p_page_maintenance->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_maintenance);
}

void page_maintenance_general::on_minimize_Button_clicked()
{
    this->showMinimized();
}

void page_maintenance_general::on_reboot_Button_clicked()
{
    // QProcess process;
    // process.start("reboot now");
    // process.waitForFinished(-1);
    // QString stdout = process.readAllStandardOutput();
    // qDebug()<<"reboot test finished.";
 qApp->exit();
}

void page_maintenance_general::on_shutdown_Button_clicked()
{

}
