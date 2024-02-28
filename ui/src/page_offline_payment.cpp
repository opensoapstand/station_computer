//***************************************
//
// page_offline_payment.cpp
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to page_dispenser.
//
// created: 05-04-2022
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "page_offline_payment.h"
#include "ui_page_offline_payment.h"

#include "page_product.h"
#include "page_product_mixing.h"
#include "page_dispenser.h"
#include "page_idle.h"

// extern QString transactionLogging;

// CTOR

page_offline_payment::page_offline_payment(QWidget *parent) : QWidget(parent),
                                                    ui(new Ui::page_offline_payment)
{
    // Fullscreen background setup
    ui->setupUi(this);

    paymentEndTimer = new QTimer(this);
    paymentEndTimer->setInterval(1000);
    connect(paymentEndTimer, SIGNAL(timeout()), this, SLOT(onTimeoutTick()));


    showErrorTimer = new QTimer(this);
    connect(showErrorTimer, SIGNAL(timeout()), this, SLOT(showErrorTimerPage()));
    
    connect(ui->pushButton_promo_input, SIGNAL(clicked()), this, SLOT(on_lineEdit_promo_codeInput_clicked()));

    statusbarLayout = new QVBoxLayout(this);
}

/*
 * Page Tracking reference
 */
void page_offline_payment::setPage(page_product *p_page_product, page_error_wifi *pageWifiError, page_dispenser *page_dispenser, page_idle *pageIdle, page_help *pageHelp, statusbar *p_statusbar, page_product_mixing *p_page_product_mixing,keyboard * keyboard)
{
    this->p_page_product = p_page_product;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_dispense = page_dispenser;
    this->p_page_idle = pageIdle;
    this->p_page_help = pageHelp;
    this->p_statusbar = p_statusbar;
    this->p_page_product_mixing = p_page_product_mixing;
    this->p_keyboard = keyboard;

}

// DTOR
page_offline_payment::~page_offline_payment()
{
    delete ui;
}

/* ----- GUI ----- */

void page_offline_payment::proceed_to_dispense()
{
    hideCurrentPageAndShowProvided(p_page_dispense);
}


void page_offline_payment::showEvent(QShowEvent *event)
{

    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);
    // statusbarLayout->setContentsMargins(0, 1874, 0, 0);                                 // int left, int top, int right, int bottom);
    p_keyboard->resetKeyboard();
    statusbarLayout->removeWidget(p_keyboard);

    p_keyboard->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    p_keyboard->setContentsMargins(0, 0, 0, 0);
    p_keyboard->findChild<QWidget *>("keyboard_3")->setGeometry(21, 0, 1040, 495);

    p_statusbar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    p_statusbar->setContentsMargins(0, 0, 0, 0); 

    statusbarLayout->setSpacing(0);
    statusbarLayout->setContentsMargins(0, 0, 0, 0);

    statusbarLayout->addWidget(p_keyboard);   
    statusbarLayout->addWidget(p_statusbar);   

    statusbarLayout->setAlignment(Qt::AlignBottom | Qt::AlignVCenter);
    p_keyboard->registerCallBack(std::bind(&page_offline_payment::enterButtonPressed, this));

    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this);

    // p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ox2, "button_problems_message");
    p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_title, "pay_by_phone");
    p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_scan, "label_scan_1");
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_steps);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_processing);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_previous_page);

    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_OFFLINE_PAYMENT_CSS);

    ui->pushButton_refresh->setProperty("class", "invisible_button");

    ui->pushButton_previous_page->setStyleSheet(styleSheet);

    ui->pushButton_refresh->setStyleSheet(styleSheet);
    ui->label_title->setStyleSheet(styleSheet);
    ui->label_scan->setStyleSheet(styleSheet);
    ui->label_steps->setStyleSheet(styleSheet);
    ui->label_processing->setStyleSheet(styleSheet);
    ui->label_product_price->setStyleSheet(styleSheet);
    ui->label_product_information->setStyleSheet(styleSheet);
    ui->label_qr_background->setStyleSheet(styleSheet);
    ui->label_gif->setStyleSheet(styleSheet);

    ui->pushButton_promo_input->setStyleSheet(styleSheet);
    ui->lineEdit_promo_code->setProperty("class", "promoCode");
    ui->lineEdit_promo_code->setStyleSheet(styleSheet);


    ui->lineEdit_promo_code->show();
    ui->pushButton_promo_input->show(); 
 

    msgBox = nullptr;

    state_payment = s_init_offline;

    int pnumber_selected = p_page_idle->thisMachine->getSelectedProduct()->getPNumber();
    double price = p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize();

    if (p_page_idle->thisMachine->getSelectedProduct()->getSelectedSize() == SIZE_CUSTOM_INDEX)
    {
        ui->label_product_information->setText(p_page_idle->thisMachine->getSelectedProduct()->getProductName() + p_page_idle->thisMachine->getTemplateTextByElementNameAndPageAndIdentifier(ui->label_product_information, "volume_up_to") + p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(true, true));
        QString base_text = p_page_idle->thisMachine->getTemplateTextByElementNameAndPageAndIdentifier(ui->label_product_price, "custom_size");
        QString label_product_price_text = base_text.arg(price);
        ui->label_product_price->setText(label_product_price_text);
    }
    else
    {
        ui->label_product_information->setText(p_page_idle->thisMachine->getSelectedProduct()->getProductName() + " " + p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(true, true));
        QString base_text = p_page_idle->thisMachine->getTemplateTextByElementNameAndPageAndIdentifier(ui->label_product_price, "fixed_size");
        QString label_product_price_text = base_text.arg(price);
        ui->label_product_price->setText(label_product_price_text);
    }

    ui->label_qrCode->show();
    ui->label_product_information->show();
    ui->label_product_price->show();

    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_QR_PAY_BACKGROUND_PATH);

    ui->label_steps->show();
    ui->label_processing->hide();
    paymentEndTimer->start(1000);
    _pageTimeoutCounterSecondsLeft = QR_PAGE_TIMEOUT_SECONDS;

    // ui->label_refresh_page->hide();
    // ui->pushButton_refresh->raise(); // make sure refresh button is on top.
    ui->pushButton_previous_page->raise();
    ui->label_gif->hide();

    if(p_page_idle->thisMachine->hasMixing()){
        ui->label_qr_background->show();
        ui->label_refresh_page->setStyleSheet(styleSheet);
        ui->label_refresh_page_background->setStyleSheet(styleSheet);
        QString warning_icon_full_path = p_page_idle->thisMachine->getTemplatePathFromName(PAGE_QR_WARNING_ICON);
        p_page_idle->thisMachine->addPictureToLabel(ui->label_refresh_page_icon, warning_icon_full_path);

        ui->label_refresh_page_background->hide();
        ui->label_refresh_page_icon->hide();
    }else{
        ui->label_qr_background->hide();
        ui->label_refresh_page_background->hide();
        ui->label_refresh_page_icon->hide();
    }

    setupQrOrder();
}

void page_offline_payment::enterButtonPressed(){
   
        qDebug() << "Done clicked, initiated apply promo.";
        apply_code(ui->lineEdit_promo_code->text());
        p_keyboard->resetKeyboard();
}

void page_offline_payment::apply_code(QString promocode){
    if(promocode == secretCode){
        hideCurrentPageAndShowProvided(p_page_dispense);
    }
}

void page_offline_payment::setupQrOrder()
{
    QString productId = p_page_idle->thisMachine->getSelectedProduct()->getAwsProductId();
    QString quantity_requested = p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(false, false);
    char drinkSize = p_page_idle->thisMachine->getSelectedProduct()->getSelectedSizeAsChar();
    double originalPrice = p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize();
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QString price = QString::number(p_page_idle->thisMachine->getPriceWithDiscount(originalPrice), 'f', 2);
    QString curl_params = QString("size=%1&price=%2&pid=%3&quantity=%4&session=%5")
                          .arg(drinkSize)
                          .arg(price)
                          .arg(productId)
                          .arg(quantity_requested)
                          .arg(time);
    secretCode = generateCode(curl_params);
    QPixmap map;
    if(p_page_idle->thisMachine->hasMixing()){
        map = QPixmap(451, 451);
        map.fill(QColor("#895E25"));
    }else{
        map = QPixmap(360, 360);
        map.fill(QColor("black"));
    }
    QPainter painter(&map);
    QString portal_base_url = p_page_idle->thisMachine->getPortalBaseUrl();
    // build up qr content (link)
    QString qrdata = portal_base_url + "offlinePayment?" + curl_params;
    if ((p_page_idle->thisMachine->getMachineId()).left(2) == "AP")
    {
        qrdata = portal_base_url+"paymentAelen?oid=" + curl_params;
    }
    // create qr code graphics
    p_page_idle->thisMachine->hasMixing() ? paintQR(painter, QSize(451, 451), qrdata, QColor("white")) : paintQR(painter, QSize(360, 360), qrdata, QColor("white"));
    ui->label_qrCode->setPixmap(map);
    // ui->label_qrCode->show();
    ui->label_product_information->show();
    ui->label_product_price->show();
    ui->label_title->show();
    ui->label_scan->show();
    ui->label_steps->show();
    // showErrorTimer->start();
    
}

void page_offline_payment::showErrorTimerPage()
{
    qDebug() << "show error page";
    hideCurrentPageAndShowProvided(p_page_wifi_error);
}


void page_offline_payment::on_pushButton_refresh_clicked()
{
    ui->label_refresh_page->hide();
    if(p_page_idle->thisMachine->hasMixing()){
        ui->label_refresh_page_background->hide();
        ui->label_refresh_page_icon->hide();
    }
    _pageTimeoutCounterSecondsLeft = QR_PAGE_TIMEOUT_SECONDS;
}

// XXX: Remove this when interrupts and flow sensors work!
void page_offline_payment::onTimeoutTick()
{
    if (--_pageTimeoutCounterSecondsLeft >= 0)
    {
        // qDebug() << "Tick Down: " << _pageTimeoutCounterSecondsLeft;

        QString label_text = "Transaction will be cancelled in " + QString::number(_pageTimeoutCounterSecondsLeft) + "s.\nTOUCH THE SCREEN\n if you need more time \n";
        ui->label_refresh_page->setText(label_text);
    }
    else
    {
        qDebug() << "Timer Done!" << _pageTimeoutCounterSecondsLeft;
        p_page_idle->thisMachine->addToTransactionLogging("\n 5: Timeout - True");
        // transactionLogging += "\n 5: Timeout - True";

        idlePaymentTimeout();
    }
    if (_pageTimeoutCounterSecondsLeft < QR_PAGE_TIMEOUT_WARNING_SECONDS)
    {
        ui->label_refresh_page->show();
        if(p_page_idle->thisMachine->hasMixing()){
            ui->label_refresh_page_background->show();
            ui->label_refresh_page_icon->show();
            ui->label_refresh_page_background->raise();
            ui->label_refresh_page_icon->raise();
            ui->label_refresh_page->raise();
        }
    }
}

bool page_offline_payment::exitConfirm()
{
    qDebug() << "In exit confirm";
    // QMessageBox msgBox;
    msgBox = new QMessageBox();

    msgBox->setWindowFlags(Qt::FramelessWindowHint| Qt::Dialog);
    QString searchString;
    searchString = this->objectName() + "->msgBox_cancel->default";
    p_page_idle->thisMachine->setTextToObject(msgBox, p_page_idle->thisMachine->getTemplateText(searchString));
    
    int remainingTime = MESSAGE_BOX_TIMEOUT_EXIT_QR_CONFIRM_SECONDS; // Initial value in seconds
    QString templateText = p_page_idle->thisMachine->getTemplateText(searchString);
    QString autoCloseText = QString("Closing in %1 seconds...").arg(remainingTime);
    QString messageBoxText = templateText + "\n" + autoCloseText;

    msgBox->setText(messageBoxText);

    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_OFFLINE_PAYMENT_CSS);
    msgBox->setProperty("class", "msgBoxbutton msgBox");
    msgBox->setStyleSheet(styleSheet);

    QTimer *timeoutTimer = new QTimer(msgBox);
    QObject::connect(timeoutTimer, &QTimer::timeout, [this, &remainingTime, &templateText, timeoutTimer]()
                     {
        remainingTime--;
        QString autoCloseText = QString("Closing in %1 seconds...").arg(remainingTime);
        QString messageBoxText = templateText + "\n" + autoCloseText;
        msgBox->setText(messageBoxText);
        msgBox->raise(); // not reproducable bug for Lode but, for UBC machine, the msgbox keeps focus but is set to background, so it's unclickable
        if (remainingTime <= 0) {
            timeoutTimer->stop();
            msgBox->hide();
            msgBox->deleteLater();
            msgBox = nullptr;
        } });

    timeoutTimer->start(1000); // Update every 1000 milliseconds (1 second)

    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox->exec();
        msgBox = nullptr;
        timeoutTimer->stop();
        bool exitPage = false; //return false as default e.g. if message box timed out
        switch (ret)
        {
        case QMessageBox::Yes:
            exitPage = true;
            break;
            // return true;
        case QMessageBox::No:
            exitPage = false;
            break;
            // return false;
        }
        qDebug() << "Exit page" << exitPage;
        // WARNING: if messagbox still active while TAP page times out, it will still run in the background. 
        return exitPage; 
}


void page_offline_payment::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{

    resetPaymentPage();
    statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.
    p_page_idle->thisMachine->pageTransition(this, pageToShow);

    if (msgBox != nullptr)
    {
        msgBox->hide();
        msgBox->deleteLater();
        msgBox = nullptr;
    }
}

// Navigation: Back to Product Size Selection
void page_offline_payment::on_pushButton_previous_page_clicked()
{
    qDebug() << "In previous page button";
    if (exitConfirm())
    {
        qDebug() << "Confirmed";
        p_page_idle->thisMachine->hasMixing() ? hideCurrentPageAndShowProvided(p_page_product_mixing) : hideCurrentPageAndShowProvided(p_page_product);
    }
}

void page_offline_payment::idlePaymentTimeout()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_offline_payment::resetPaymentPage()
{
    p_page_idle->thisMachine->resetTransactionLogging();
    paymentEndTimer->stop();
    showErrorTimer->stop();
}

std::string page_offline_payment::toSvgString(const QrCode &qr, int border)
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
void page_offline_payment::printQr(const QrCode &qr)
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

void page_offline_payment::paintQR(QPainter &painter, const QSize sz, const QString &data, QColor fg)
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
                if (p_page_idle->thisMachine->hasMixing()){
                    QColor customColor("#FFF7ED");
                    painter.setBrush(customColor);
                }
                const double rx1 = (x + 1) * scale, ry1 = (y + 1) * scale;
                QRectF r(rx1, ry1, scale, scale);
                painter.drawRects(&r, 1);
            }
        }
    }
}

QString page_offline_payment::generateCode(QString inputString) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, inputString.toStdString().c_str(), inputString.size());
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);

    // Extract the last 6 hexadecimal digits directly into a QString
    QString code;
    for (int i = hash_len - 6; i < hash_len; i++) {
        code += QString("%1").arg((int)hash[i], 2, 16, QChar('0'));
    }
    bool ok;    
    quint64 decimalValue = code.toULongLong(&ok, 16);
    QString secretCode = QString::number(decimalValue%1000000);
    return secretCode;
}

void page_offline_payment::on_lineEdit_promo_codeInput_clicked()
{
    p_keyboard->registerCallBack(std::bind(&page_offline_payment::enterButtonPressed, this));
    p_keyboard->setKeyboardVisibility(true, ui->lineEdit_promo_code);
    ui->lineEdit_promo_code->clear();
    ui->lineEdit_promo_code->show();
}
