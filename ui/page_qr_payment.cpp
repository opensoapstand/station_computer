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
    ui->pushButton_payment_bypass->setEnabled(false);

    paymentEndTimer = new QTimer(this);
    paymentEndTimer->setInterval(1000);
    connect(paymentEndTimer, SIGNAL(timeout()), this, SLOT(onTimeoutTick()));

    qrPeriodicalCheckTimer = new QTimer(this);
    connect(qrPeriodicalCheckTimer, SIGNAL(timeout()), this, SLOT(qrProcessedPeriodicalCheck()));

    showErrorTimer = new QTimer(this);
    connect(showErrorTimer, SIGNAL(timeout()), this, SLOT(showErrorTimerPage()));

    ui->pushButton_payment_bypass->setEnabled(false);
    state_payment = s_init;
    
//    ui->label_title->setText("pay by phone");

    // ui->label_scan->setText(
    //     "Scan to Pay");
    
    // ui->label_steps->setText(
    //     "<style>"
    //     "li:{margin-top:10px;}"
    //     "</style>"
    //     "<ol>"
    //     "<li><span class='tab'></span>Scan QR code with phone camera<br></li>"
    //     "<li><span class='tab'></span>Click to open the link that appears<br></li>"
    //     "<li><span class='tab'></span>Follow payment instructions on phone<br></li>"
    //     "<li><span class='tab'></span>The station will proceed after payment<br></li>"
    //     "<li><span class='tab'></span>Refill your soap!</li>"
    //     "</ol>");
    
    // ui->label_processing->setText(
    //     "it can take a few moments for the station to<br>continue after your payment is confirmed");
    
    ui->order_total_amount->hide();
}

void page_qr_payment::stopPayTimers()
{
}

/*
 * Page Tracking reference
 */
void page_qr_payment::setPage(page_product *p_page_product, page_error_wifi *pageWifiError, page_dispenser *page_dispenser, page_idle *pageIdle, page_help *pageHelp)
{
    tmpCounter = 0;
    this->p_page_product = p_page_product;
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

void page_qr_payment::on_pushButton_payment_bypass_clicked()
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

// QString page_qr_payment::getPaymentMethod()
// {
//     qDebug() << "db open245";
//     int product_slot___ = p_page_idle->selectedProduct->getSlot();
//     DbManager db2(DB_PATH);
//     QString payment_method = db2.getPaymentMethod(product_slot___);
//     db2.closeDB();
//     return payment_method;
// }

void page_qr_payment::resizeEvent(QResizeEvent *event)
{
}

void page_qr_payment::showEvent(QShowEvent *event)
{

    p_page_idle->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);
    // p_page_idle->setTemplateTextWithIdentifierToObject(ox2, "button_problems_message");
    p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_title, "pay_by_phone");
    p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_scan, "label_scan_1");
    p_page_idle->setTemplateTextToObject(ui->label_steps);
    p_page_idle->setTemplateTextToObject(ui->label_processing);
    p_page_idle->setTemplateTextToObject(ui->pushButton_previous_page);
    
    // ui->pushButton_previous_page->setText("<-back");
    
  


    QString styleSheet = p_page_idle->getCSS(PAGE_QR_PAYMENT_CSS);

    ui->pushButton_payment_bypass->setProperty("class", "invisible_button");
    ui->pushButton_refresh->setProperty("class", "invisible_button");

    ui->pushButton_previous_page->setStyleSheet(styleSheet);

    ui->pushButton_payment_bypass->setStyleSheet(styleSheet);
    ui->pushButton_refresh->setStyleSheet(styleSheet);
    ui->label_title->setStyleSheet(styleSheet);
    ui->label_scan->setStyleSheet(styleSheet);
    ui->label_steps->setStyleSheet(styleSheet);
    ui->label_processing->setStyleSheet(styleSheet);

    state_payment = s_init;
    double originalPrice = p_page_idle->selectedProduct->getPrice();
    if (p_page_idle->selectedProduct->getSizeAsChar() == 'c')
    {
        originalPrice = p_page_idle->selectedProduct->getPriceCustom();
    }
    QString price = QString::number(p_page_idle->getPriceCorrectedAfterDiscount(originalPrice), 'f', 2);


    ui->qrCode->show();
    ui->productLabel->show();
    ui->order_drink_amount->show();

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_QR_PAY_BACKGROUND_PATH);
    ui->pushButton_payment_bypass->setEnabled(false);
    ui->productLabel->setText(p_page_idle->selectedProduct->getProductName() + " " + p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(true, true));
    ui->order_drink_amount->setText("$" + price);
    ui->order_total_amount->setText("Total: $" + price);
    ui->label_steps->show();
    ui->label_processing->hide();

    paymentEndTimer->start(1000);
    _pageTimeoutCounterSecondsLeft = QR_PAGE_TIMEOUT_SECONDS;

    this->ui->payment_countdownLabel->setText("");

    ui->refreshLabel->hide();
    ui->pushButton_refresh->raise(); // make sure refresh button is on top. 
    ui->pushButton_previous_page->raise();

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
        ui->label_title->hide();
        ui->label_scan->hide();
        ui->order_total_amount->hide();
        ui->label_steps->hide();
        showErrorTimer->start();
    }
}

void page_qr_payment::showErrorTimerPage()
{
    qDebug() << "show error page";
    hideCurrentPageAndShowProvided(p_page_wifi_error);
}

bool page_qr_payment::createOrderIdAndSendToBackend()
{
    // an order Id is generated locally and the order is sent to the cloud.
    bool shouldShowQR = false;
    qDebug() << "Get cloud to create an order and retrieve the order id";
    QString MachineSerialNumber = p_page_idle->thisMachine.getMachineId();
    QString productUnits = p_page_idle->selectedProduct->getUnitsForSlot();
    QString productId = p_page_idle->selectedProduct->getAwsProductId();
    QString contents = p_page_idle->selectedProduct->getProductName();
    QString quantity_requested = p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(false, false);
    char drinkSize = p_page_idle->selectedProduct->getSizeAsChar();
    double originalPrice = p_page_idle->selectedProduct->getPriceCorrected();
    if (drinkSize == 'c')
    {
        originalPrice = p_page_idle->selectedProduct->getPriceCustom();
    }

    QString price = QString::number(p_page_idle->getPriceCorrectedAfterDiscount(originalPrice), 'f', 2);

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
            ui->label_steps->hide();

            ui->label_processing->show();
            // ui->label_scan->setText("Please finalize transaction");
            p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_scan, "finalize_transaction");

            // ui->label_scan->setText("Please finalize transaction");
            // ui->label_title->setText("almost there");
            p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_title, "almost_there");

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

void page_qr_payment::on_pushButton_refresh_clicked()
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
        qDebug() << "Timer Done!" << _pageTimeoutCounterSecondsLeft ;
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
    QString styleSheet = p_page_idle->getCSS(PAGE_QR_PAYMENT_CSS);
    msgBox.setProperty("class", "msgBoxbutton msgBox"); // set property goes first!!
    msgBox.setStyleSheet(styleSheet);

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
    transactionLogging = "";

    paymentEndTimer->stop();
    qrPeriodicalCheckTimer->stop();
    showErrorTimer->stop();

    response = true;
    qDebug() << "Cancelled";

    p_page_idle->pageTransition(this, pageToShow);
}

// Navigation: Back to Drink Size Selection
void page_qr_payment::on_pushButton_previous_page_clicked()
{
    qDebug() << "In previous page button" ;
    if (exitConfirm())
    {
        // p_page_idle->pageTransition(this, p_page_product);
        hideCurrentPageAndShowProvided(p_page_product);
    }
}

void page_qr_payment::idlePaymentTimeout()
{
    hideCurrentPageAndShowProvided(p_page_idle);
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