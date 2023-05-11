//***************************************
//
// page_qr_payment.cpp
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to page_dispenser.
//
// created: 05-04-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "page_qr_payment.h"
#include "ui_page_qr_payment.h"

#include "page_product.h"
#include "page_dispenser.h"
#include "page_idle.h"

extern QString transactionLogging;

// CTOR

page_qr_payment::page_qr_payment(QWidget *parent) : QWidget(parent),
                                                    ui(new Ui::page_qr_payment)
{
    // Fullscreen background setup
    ui->setupUi(this);
    qDebug() << "QR Payment page" << endl;
    ui->pushButton_previous_page->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->pushButton_to_idle->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->payment_bypass_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->refreshButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    ui->payment_bypass_Button->setEnabled(false);

    // Idle Payment reset
    idlePaymentTimer = new QTimer(this);
    connect(idlePaymentTimer, SIGNAL(timeout()), this, SLOT(idlePaymentTimeout()));

    qrPeriodicalCheckTimer = new QTimer(this);
    connect(qrPeriodicalCheckTimer, SIGNAL(timeout()), this, SLOT(qrProcessedPeriodicalCheck()));

    showError = new QTimer(this);
    connect(showError, SIGNAL(timeout()), this, SLOT(showErrorPage()));

    ui->payment_bypass_Button->setEnabled(false);
    state_payment = s_init;
    QString css_title = "QLabel{"
                        "font-family: 'Brevia';"
                        "font-style: normal;"
                        "font-weight: 500;"
                        "font-size: 64px;"
                        "line-height: 86px;"
                        "text-align: center;"
                        "text-transform: lowercase;"
                        "color: #003840;"
                        "text-shadow: 0px 4px 4px rgba(0, 0, 0, 0.25);"
                        "}";
    ui->title_Label->setStyleSheet(css_title);
    ui->title_Label->setText("pay by phone");

    ui->scan_Label->setText(
        "Scan to Pay");
    QString css_scan = "QLabel{"
                       "text-align: center;"
                       "font-family: 'Montserrat';"
                       "font-style: normal;"
                       "font-weight: 600;"
                       "font-size: 48px;"
                       "color: #5E8580;"
                       "} .tab {"
                       "display: inline-block;"
                       "margin-left: 40px;"
                       "}";
    ui->scan_Label->setStyleSheet(css_scan);

    ui->steps_Label->setText(
        "<style>"
        "li:{margin-top:10px;}"
        "</style>"
        "<ol>"
        "<li><span class='tab'></span>Scan QR code with phone camera<br></li>"
        "<li><span class='tab'></span>Click to open the link that appears<br></li>"
        "<li><span class='tab'></span>Follow payment instructions on phone<br></li>"
        "<li><span class='tab'></span>The station will proceed after payment<br></li>"
        "<li><span class='tab'></span>Refill your soap!</li>"
        "</ol>");
    QString css_steps = "QLabel{"
                        "position: absolute;"
                        "width: 777px;"
                        "height: 306px;"
                        "left: 143px;"
                        "top: 1029px;"
                        "font-family: 'Montserrat';"
                        "font-style: normal;"
                        "font-weight: 600;"
                        "font-size: 36px;"
                        "line-height: 51px;"
                        "color: #003840;"
                        "}";
    ui->steps_Label->setStyleSheet(css_steps);

    ui->processing_Label->setText(
        "it can take a few moments for the station to<br>continue after your payment is confirmed");
    QString css_processing = "QLabel{"
                             "position: absolute;"
                             "width: 777px;"
                             "height: 306px;"
                             "left: 143px;"
                             "top: 1029px;"
                             "font-family: 'Montserrat';"
                             "font-style: normal;"
                             "font-weight: 600;"
                             "font-size: 36px;"
                             "line-height: 51px;"
                             "color: #003840;"
                             "}";
    ui->processing_Label->setStyleSheet(css_processing);
    ui->order_total_amount->hide();
}

void page_qr_payment::stopPayTimers()
{

    if (idlePaymentTimer != nullptr)
    {
        qDebug() << "cancel page_idle payment Timer" << endl;
        idlePaymentTimer->stop();
    }

    if (paymentEndTimer != nullptr)
    {
        qDebug() << "cancel page_idle payment END Timer" << endl;
        paymentEndTimer->stop();
    }
    if (qrPeriodicalCheckTimer != nullptr)
    {
        // qDebug() << "*************************cancel qrPeriodicalCheckTimer" << endl;
        qrPeriodicalCheckTimer->stop();
    }

    if (showError != nullptr)
    {
        showError->stop();
    }
    //    qDebug() << "page_qr_payment: Stopped Timers" << endl;
}

/*
 * Page Tracking reference
 */
void page_qr_payment::setPage(pageProduct *pageSizeSelect, page_error_wifi *pageWifiError, page_dispenser *page_dispenser, page_idle *pageIdle, page_help *pageHelp)
{
    tmpCounter = 0;
    this->p_pageProduct = pageSizeSelect;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_dispense = page_dispenser;
    this->p_page_idle = pageIdle;
    this->p_page_help = pageHelp;
}

// DTOR
page_qr_payment::~page_qr_payment()
{
    delete ui;
}

/* ----- GUI ----- */

// Labels and button for tapping payment
void page_qr_payment::displayPaymentPending(bool isVisible)
{
}

void page_qr_payment::on_payment_bypass_Button_clicked()
{
    proceed_to_dispense();
}

void page_qr_payment::proceed_to_dispense()
{
    hideCurrentPageAndShowProvided(p_page_dispense);
}

void page_qr_payment::updateTotals(string drinkDescription, string drinkAmount, string orderTotal)
{
}

/*Cancel any previous payment*/
void page_qr_payment::cancelPayment()
{
    // finishSession(std::stoi(socketAddr), MAC_LABEL, MAC_KEY);
    checkPacketReceivedTimer->stop();
}

size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

QString page_qr_payment::getPaymentMethod()
{
    qDebug() << "db open245";
    int product_slot___ = p_page_idle->currentProductOrder->getSelectedSlot();
    DbManager db2(DB_PATH);
    QString payment_method = db2.getPaymentMethod(product_slot___);
    db2.closeDB();
    return payment_method;
}

void page_qr_payment::resizeEvent(QResizeEvent *event)
{
}

void page_qr_payment::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Payment >>>>>>>>>";
    QWidget::showEvent(event);
    state_payment = s_init;
    QString price = QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2);
    if(p_page_idle->currentProductOrder->getSelectedSizeAsChar()=='c'){
        price = QString::number(p_page_idle->currentProductOrder->getSelectedPriceCustom(), 'f', 2);
    }
    
    ui->qrCode->show();
    ui->productLabel->show();
    ui->order_drink_amount->show();
    ui->title_Label->setText("pay by phone");
    ui->scan_Label->setText("Scan to Pay");
    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_QR_PAY_BACKGROUND_PATH);
    ui->payment_bypass_Button->setEnabled(false);
    ui->productLabel->setText(p_page_idle->currentProductOrder->getSelectedProductName() + " " + p_page_idle->currentProductOrder->getSelectedSizeToVolumeWithCorrectUnits(true, true));
    ui->order_drink_amount->setText("$" +price);
    ui->order_total_amount->setText("Total: $" +price);
    ui->steps_Label->show();
    ui->processing_Label->hide();

    // p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_QR_PAY_BACKGROUND_PATH);
    paymentEndTimer = new QTimer(this);
    paymentEndTimer->setInterval(1000);
    connect(paymentEndTimer, SIGNAL(timeout()), this, SLOT(onTimeoutTick()));
    paymentEndTimer->start(1000);
    _pageTimeoutCounterSecondsLeft = QR_PAGE_TIMEOUT_SECONDS;
    this->ui->payment_countdownLabel->setText("");

    ui->refreshLabel->hide();

    setupQrOrder();
}

void page_qr_payment::setupQrOrder()
{

    if (createOrderIdAndSendToBackend())
    {

        transactionLogging += "\n 2: QR code - True";
        QPixmap map(360, 360);
        map.fill(QColor("black"));
        QPainter painter(&map);

        // build up qr content (link)
        QString qrdata = "https://soapstandportal.com/payment?oid=" + orderId;

        // create qr code graphics
        paintQR(painter, QSize(360, 360), qrdata, QColor("white"));
        ui->qrCode->setPixmap(map);
        // _paymentTimeoutSec = QR_PAGE_TIMEOUT_SECONDS;
        _paymentTimeLabel = QR_PAGE_TIMEOUT_SECONDS;

        _pageTimeoutCounterSecondsLeft = QR_PAGE_TIMEOUT_SECONDS;

        _qrProcessedPeriodicalCheckSec = QR_PROCESSED_PERIODICAL_CHECK_SECONDS;
        qrPeriodicalCheckTimer->start(1000);
    }
    else
    {
        ui->qrCode->show();
        ui->productLabel->show();
        ui->order_drink_amount->show();
        ui->title_Label->hide();
        ui->scan_Label->hide();
        ui->order_total_amount->hide();
        ui->steps_Label->hide();
        showError->start();
    }
}

void page_qr_payment::showErrorPage()
{
    qDebug() << "show error page";
    hideCurrentPageAndShowProvided(p_page_wifi_error);
}

bool page_qr_payment::createOrderIdAndSendToBackend()
{
    // an order Id is generated locally and the order is sent to the cloud.
    bool shouldShowQR = false;
    qDebug() << "Get cloud to create an order and retrieve the order id";
    QString MachineSerialNumber = p_page_idle->currentProductOrder->getMachineId();
    QString productUnits = p_page_idle->currentProductOrder->getUnitsForSelectedSlot();
    QString productId = p_page_idle->currentProductOrder->getSelectedProductId();
    QString contents = p_page_idle->currentProductOrder->getSelectedProductName();
    QString quantity_requested = p_page_idle->currentProductOrder->getSelectedSizeToVolumeWithCorrectUnits(false, false);
    char drinkSize = p_page_idle->currentProductOrder->getSelectedSizeAsChar();
    QString price = QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2);
    if(drinkSize=='c'){
        price = QString::number(p_page_idle->currentProductOrder->getSelectedPriceCustom(), 'f', 2);
    }

    // create a unique order id locally
    orderId = QUuid::createUuid().QUuid::toString();
    orderId = orderId.remove("{");
    orderId = orderId.remove("}");

    // send order details to backend
    QString curl_order_parameters_string = "orderId=" + orderId + "&size=" + drinkSize + "&MachineSerialNumber=" + MachineSerialNumber +
                                           "&contents=" + contents + "&price=" + price + "&productId=" + productId + "&quantity_requested=" + quantity_requested + "&size_unit=" + productUnits + "&logging=" + transactionLogging;
    curl_order_parameters = curl_order_parameters_string.toLocal8Bit();

    curl1 = curl_easy_init();

    if (!curl1)
    {
        qDebug() << "pagepayement cURL Failed to init : " + curl_order_parameters_string + "to: " + "https://soapstandportal.com/api/machine_data/createOrderInDb";
        return shouldShowQR;
    }

    curl_easy_setopt(curl1, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/createOrderInDb");
    curl_easy_setopt(curl1, CURLOPT_POSTFIELDS, curl_order_parameters.data());
    curl_easy_setopt(curl1, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl1, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl1, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);

    int createOrderInDbAttempts = 0;
    QString feedback;
    do
    {
        res1 = curl_easy_perform(curl1);
        feedback = QString::fromUtf8(readBuffer.c_str());
        createOrderInDbAttempts += 1;
    } while (feedback != "true" && createOrderInDbAttempts <= 3);
    if (feedback != "true")
    {
        qDebug() << "ERROR: Problem at create order in the cloud request. error code: " + QString::number(res1);
    }
    else
    {
        qDebug() << "create order in the cloud request sent to soapstandportal (" + curl_order_parameters_string + "). Server feedback: " << feedback;
    }
    if (feedback == "true")
    {
        shouldShowQR = true;
    }
    curl_easy_cleanup(curl1);
    readBuffer = "";
    feedback = "";

    _pageTimeoutCounterSecondsLeft = QR_PAGE_TIMEOUT_SECONDS;
    _qrProcessedPeriodicalCheckSec = QR_PROCESSED_PERIODICAL_CHECK_SECONDS;
    // qrPeriodicalCheckTimer->start(1000);
    return shouldShowQR;
}

void page_qr_payment::isQrProcessedCheckOnline()
{

    curl = curl_easy_init();
    if (!curl)
    {
        qDebug() << "cURL failed to page_init";
        return;
    }

    QString curl_param = "oid=" + orderId;
    curl_param_array = curl_param.toLocal8Bit();

    curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/check_order_status");

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curl_param_array.data());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);

    res = curl_easy_perform(curl);
    qDebug() << "Send check qr status for order " << orderId;

    if (res != CURLE_OK)
    {
        // string test = std::string(curl_easy_strerror(res));
        qDebug() << "ERROR: cURL fail at page_qr_payment. Error code: " + QString::number(res);
    }
    else
    {
        QString feedback = QString::fromUtf8(readBuffer.c_str());
        qDebug() << "curl success. Server feedback readbuffer: " << feedback;

        if (readBuffer == "true")
        {
            transactionLogging += "\n 4: Order Paid - True";
            qDebug() << "QR processed. It's time to dispense.";
            proceed_to_dispense();
            state_payment = s_payment_done;
        }
        else if (readBuffer == "Not paid")
        {
            qDebug() << "Initial page, user has not yet scanned qr code (or it is scanned, but not yet confirmed by backed)";
            // do nothing
            state_payment = s_init;
        }
        else if (readBuffer == "In progress")
        {
            if (!transactionLogging.contains("\n 3: QR Scanned - True"))
            {
                transactionLogging += "\n 3: QR Scanned - True";
            }
            qDebug() << "Wait for QR processed. User must have finished transaction to continue.";
            // user scanned qr code and is processing transaction. Delete qr code and make it harder for user to leave page.
            state_payment = s_payment_processing;
            ui->qrCode->hide();
            ui->productLabel->hide();
            ui->order_drink_amount->hide();
            ui->order_total_amount->hide();
            ui->steps_Label->hide();

            ui->processing_Label->show();
            ui->scan_Label->setText("Please finalize transaction");
            ui->title_Label->setText("almost there");
        }
        else
        {
            qDebug() << "ASSERT ERROR: Unknown message from Server";
        }
    }
    curl_easy_cleanup(curl);
    readBuffer = "";
}

void page_qr_payment::qrProcessedPeriodicalCheck()
{
    if (--_qrProcessedPeriodicalCheckSec >= 0)
    {
    }
    else
    {
        qDebug() << "Periodical check timer trigger. Check API endpoint for handled QR transaction.";
        isQrProcessedCheckOnline();
        _qrProcessedPeriodicalCheckSec = QR_PROCESSED_PERIODICAL_CHECK_SECONDS;
    }
}

void page_qr_payment::on_refreshButton_clicked()
{
    ui->refreshLabel->hide();
    _pageTimeoutCounterSecondsLeft = QR_PAGE_TIMEOUT_SECONDS;
}

// XXX: Remove this when interrupts and flow sensors work!
void page_qr_payment::onTimeoutTick()
{
    if (--_pageTimeoutCounterSecondsLeft >= 0)
    {
        QString label_text = "Transaction will be cancelled in " + QString::number(_pageTimeoutCounterSecondsLeft) + "s.\nTOUCH THE SCREEN\n if you need more time \n";
        ui->refreshLabel->setText(label_text);
    }
    else
    {
        qDebug() << "Timer Done!" << _pageTimeoutCounterSecondsLeft << endl;
        transactionLogging += "\n 5: Timeout - True";

        idlePaymentTimeout();
    }
    if (_pageTimeoutCounterSecondsLeft < QR_PAGE_TIMEOUT_WARNING_SECONDS)
    {
        ui->refreshLabel->show();
    }
}

bool page_qr_payment::setpaymentProcess(bool status)
{
    return (paymentProcessing = status);
}

void page_qr_payment::setProgressLabel(QLabel *label, int dot)
{
}

// Local storge for now.  Will need to refactor logger to do a nightly push to AWS
void page_qr_payment::storePaymentEvent(QSqlDatabase db, QString event)
{
}

bool page_qr_payment::exitConfirm()
{
    qDebug() << "In exit confirm";
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::FramelessWindowHint); // do not show messagebox header with program name
    if (state_payment == s_payment_processing || state_payment == s_payment_done)
    {
        msgBox.setText("<p align=center><br><br>Cancel transaction and exit page?<br><br>It can take up to 30 seconds for dispensing to start after a payment is completed. <br></p>");
    }
    else if (state_payment == s_init)
    {
        msgBox.setText("<p align=center><br><br>Are you sure you want to cancel this order?<br><br>The QR code won't be valid anymore if this order is cancelled.<br><br>In case a payment was made with an invalid QR code or cancelled order, a refund will be issued within 24-48 hours. <br></p>");
    }
    msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px; font-weight: bold; font-style: normal;  font-family: 'Montserrat';} QPushButton{font-size: 24px; min-width: 300px; min-height: 300px;}");

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int ret = msgBox.exec();
    bool success;
    switch (ret)
    {
    case QMessageBox::Yes:
    {
       
        return true;
    }
    break;
    case QMessageBox::No:
    {
        return false;
    }
    break;
    }
}

void page_qr_payment::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    resetPaymentPage();
    p_page_idle->pageTransition(this, pageToShow);

}

// Navigation: Back to Drink Size Selection
void page_qr_payment::on_pushButton_previous_page_clicked()
{
    qDebug() << "In previous page button" << endl;
    if (exitConfirm())
    {
        // p_page_idle->pageTransition(this, p_pageProduct);
        hideCurrentPageAndShowProvided(p_pageProduct);
    }
}

void page_qr_payment::on_pushButton_to_idle_clicked()
{
    if (exitConfirm())
    {
        hideCurrentPageAndShowProvided(p_page_help);
    }
}

void page_qr_payment::idlePaymentTimeout()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}
void page_qr_payment::resetPaymentPage()
{
    ui->title_Label->hide();
    transactionLogging = "";
    stopPayTimers();
    response = true;
    qDebug() << "Cancelled";
}

std::string page_qr_payment::toSvgString(const QrCode &qr, int border)
{
    if (border < 0)
        throw std::domain_error("Border must be non-negative");
    if (border > INT_MAX / 2 || border * 2 > INT_MAX - qr.getSize())
        throw std::overflow_error("Border too large");

    std::ostringstream sb;
    sb << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    sb << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
    sb << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" viewBox=\"0 0 ";
    sb << (qr.getSize() + border * 2) << " " << (qr.getSize() + border * 2) << "\" stroke=\"none\">\n";
    sb << "\t<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n";
    sb << "\t<path d=\"";
    for (int y = 0; y < qr.getSize(); y++)
    {
        for (int x = 0; x < qr.getSize(); x++)
        {
            if (qr.getModule(x, y))
            {
                if (x != 0 || y != 0)
                    sb << " ";
                sb << "M" << (x + border) << "," << (y + border) << "h1v1h-1z";
            }
        }
    }
    sb << "\" fill=\"#000000\"/>\n";
    sb << "</svg>\n";
    return sb.str();
}

// Prints the given QrCode object to the console.
void page_qr_payment::printQr(const QrCode &qr)
{
    int border = 4;
    for (int y = -border; y < qr.getSize() + border; y++)
    {
        for (int x = -border; x < qr.getSize() + border; x++)
        {
            std::cout << (qr.getModule(x, y) ? "##" : "  ");
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void page_qr_payment::testQRgen()
{

    QPixmap map(400, 400);
    QPainter painter(&map);

    paintQR(painter, QSize(400, 400), "SoapStand RULES", QColor("white"));
    ui->qrCode->setPixmap(map);
}

void page_qr_payment::paintQR(QPainter &painter, const QSize sz, const QString &data, QColor fg)
{
    // NOTE: At this point you will use the API to get the encoding and format you want, instead of my hardcoded stuff:
    qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(data.toUtf8().constData(), qrcodegen::QrCode::Ecc::HIGHH);
    const int s = qr.getSize() > 0 ? qr.getSize() : 1;
    const double w = sz.width();
    const double h = sz.height();
    const double aspect = w / h;
    const double size = ((aspect > 1.0) ? h : w);
    const double scale = size / (s + 2);
    // NOTE: For performance reasons my implementation only draws the foreground parts in supplied color.
    // It expects background to be prepared already (in white or whatever is preferred).

    painter.setPen(Qt::NoPen);

    painter.setBrush(fg);
    for (int y = 0; y < s; y++)
    {
        for (int x = 0; x < s; x++)
        {
            const int color = qr.getModule(x, y); // 0 for white, 1 for black
            if (0 != color)
            {
                const double rx1 = (x + 1) * scale, ry1 = (y + 1) * scale;
                QRectF r(rx1, ry1, scale, scale);
                painter.drawRects(&r, 1);
            }
        }
    }
}
