//***************************************
//
// pagethankyou.cpp
// GUI class to show user dispense has been
// completed and route back to page_idle
//
// created: 05-04-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "pagethankyou.h"
#include "ui_pagethankyou.h"
#include "page_sendFeedback.h"

// static QPointer<QFile> log_file = nullptr;
extern QString transactionLogging;

// CTOR
pagethankyou::pagethankyou(QWidget *parent) : QWidget(parent),
                                              ui(new Ui::pagethankyou)
{
    ui->setupUi(this);

    //popup
    popup = new QDialog(this);
    ui->popup->setFixedSize(500,100);
    popup->setWindowTitle("Help");

    /*hacky transparent button*/
    ui->mainPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");



    // ui->extra_message_label->setText("<p align=center>Water rinse coming in<br>5</p>");
    ui->extra_message_label->hide();
    connect(ui->notifyUs_Button, SIGNAL(clicked()), this, SLOT(on_notifyUs_Button_clicked()));

//    //connect the in-pop-up buttons with their slots
//     connect(ui->option1_Button, &QPushButton::clicked, this, &pagethankyou::option1Clicked);
//     connect(ui->option2_Button, &QPushButton::clicked, this, &pagethankyou::option2Clicked);
//     connect(ui->option3_Button, &QPushButton::clicked, this, &pagethankyou::option3Clicked);
//     connect(ui->option4_Button, &QPushButton::clicked, this, &pagethankyou::option4Clicked);
//     connect(ui->option5_Button, &QPushButton::clicked, this, &pagethankyou::option5Clicked);
    //hiding the contents of the popup
    ui->selectProblem_Label->setVisible(false);
    ui->option1_Button->setVisible(false);
    ui->option2_Button->setVisible(false);
    ui->option3_Button->setVisible(false);
    ui->option4_Button->setVisible(false);
    ui->option5_Button->setVisible(false);
   


    thankYouEndTimer = new QTimer(this);
    thankYouEndTimer->setInterval(1000);
    connect(thankYouEndTimer, SIGNAL(timeout()), this, SLOT(onThankyouTimeoutTick()));

    is_in_state_thank_you = false;
    
}

/*
 * Page Tracking reference
 */
void pagethankyou::setPage(page_dispenser *page_dispenser, page_idle *pageIdle, page_payment *page_payment, page_sendFeedback *page_sendFeedback)
{
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->paymentPage = page_payment;
    this->p_page_sendFeedback = page_sendFeedback;
}

// DTOR
pagethankyou::~pagethankyou()
{
    delete ui;
}

void pagethankyou::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Thank you >>>>>>>>>";

    QWidget::showEvent(event);

    // QPixmap background(PAGE_THANK_YOU_BACKGROUND_PATH);
    // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    // QPalette palette;
    // palette.setBrush(QPalette::Background, background);
    // this->setPalette(palette);

    ui->thank_you_message_label->setStyleSheet(
        "QLabel {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 700;"
        "font-size: 85px;"
        "line-height: 95px;"
        "text-align: center;"
        "letter-spacing: 1.5px;"
        "text-transform: lowercase;"
        "color: #FFFFFF;"
        "qproperty-alignment: AlignCenter;"
        "}");

    ui->thank_you_subtitle_message_label->setStyleSheet(
        "QLabel {"
        "font-family: 'Montserrat';"
        "font-style: normal;"
        "font-weight: 600;"
        "font-size: 36px;"
        "line-height: 44px;"
        "text-align: center;"
        "letter-spacing: 1.5px;"
        "color: #FFFFFF;"
        "qproperty-alignment: AlignCenter;"
        "}");

        ui->notifyUs_Button->setStyleSheet("QPushButton { color:#555555; background-image: url(:/home/df-admin/production/references/helpButton.png); border: px }");
        ui->notifyUs_Button->setStyleSheet(
                "QPushButton {"

                "font-family: 'Brevia';"
                "font-style: normal;"
                "font-weight: 75;"
                "font-size: 32px;"
                "line-height: 99px;"
                "letter-spacing: 1.5px;"
                "color: #FFFFFF;"
                "text-align: center;"
                "qproperty-alignment: AlignCenter;"
                "border: none;"
                "}");
        ui->notifyUs_Button->setText("Help");
        QPixmap pixmap(":/home/df-admin/production/references/helpButton.png");
        ui->notifyUs_Button->setIcon(pixmap);

        //popup
    ui->popup = new QDialogButtonBox(this);
    ui->popup->setWindowTitle("Pop-up Window");
    ui->popup->setFixedSize(400,200);
        

       




    p_page_idle->addCompanyLogoToLabel(ui->thank_you_logo_label);

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_THANK_YOU_BACKGROUND_PATH);

    qDebug() << "db open24";
    DbManager db(DB_PATH);
    QString paymentMethod = db.getPaymentMethod(p_page_idle->currentProductOrder->getSelectedSlot());
    bool hasReceiptPrinter = db.hasReceiptPrinter();
    db.closeDB();

    if (hasReceiptPrinter)
    {
        ui->thank_you_message_label->setText("Please take <br>your receipt!");
        ui->thank_you_subtitle_message_label->setText("By refilling you've helped keep a<br>plastic bottle out of our landfills.<br><br>Thank you!");
    }
    else if (paymentMethod == "qr" || paymentMethod == "tap")
    {
        ui->thank_you_message_label->setText("Thank you!");
        ui->thank_you_subtitle_message_label->setText("By refilling you've helped keep a<br>plastic bottle out of our landfills.");
    }
    else
    {
        ui->thank_you_message_label->setText("Thank you!");
        ui->thank_you_subtitle_message_label->setText("By refilling you've helped keep a<br>plastic bottle out of our landfills.");
    }

    is_in_state_thank_you = true;

    thankYouEndTimer = new QTimer(this);
    thankYouEndTimer->setInterval(1000);
    connect(thankYouEndTimer, SIGNAL(timeout()), this, SLOT(onThankyouTimeoutTick()));

    // THIS WILL HAVE TO BE CHANGED SO THE SYSTEM CHECKS IF IT IS A DF / SS MACHINE

    // reset promovalue
    p_page_idle->currentProductOrder->setDiscountPercentageFraction(0.0);
    // ui->extra_message_label->hide();
    ui->mainPage_Button->setEnabled(true);
    ui->mainPage_Button->raise();

    is_controller_finished = false;
    is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = false;
    exitIsForceable = false;

    if (paymentMethod == "qr" || paymentMethod == "tap")
    {
        sendDispenseEndToCloud();
    }
    else
    {
        is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
    }

    // ui->extra_message_label->hide();
    p_page_idle->addPictureToLabel(ui->drinkfill_logo_label2, DRINKFILL_LOGO_VERTICAL_PATH);
}

size_t WriteCallback2(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void pagethankyou::sendDispenseEndToCloud()
{
    QString order_id = this->paymentPage->getOID();
    QString dispensed_correct_units = this->p_page_dispense->getMostRecentDispensed();
    QString promoCode = this->p_page_dispense->getPromoCodeUsed();
    qDebug() << "Send data at finish of order : " << order_id << ". Total dispensed: " << this->p_page_dispense->getMostRecentDispensed() << "corrected units send to soapstandportal: " << dispensed_correct_units;
    if(dispensed_correct_units== 0){
        transactionLogging += "\n ERROR: No Volume dispensed";
     }
    QString curl_param = "oid=" + order_id + "&dispensed_amount=" + dispensed_correct_units+"&coupon=" + promoCode+"&logging="+transactionLogging;;
    qDebug() << "Curl params" << curl_param << endl;
    curl_param_array = curl_param.toLocal8Bit();
    curl_data = curl_param_array.data();

    curl = curl_easy_init();
    if (!curl)
    {
        qDebug() << "pagethankyou: cURL failed to init. parameters:" + curl_param;
        transactionToFile(curl_data);
        is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/updateOrder");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curl_param_array.data());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback2);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);
    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        qDebug() << "ERROR: Transaction NOT sent to cloud. cURL fail. Error code: " + QString::number(res);

        transactionToFile(curl_data);
        is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
    }
    else
    {
        QString feedback = QString::fromUtf8(readBuffer.c_str());
        qDebug() << "Transaction sent to cloud. cURL success. Server feedback readbuffer: " << feedback;

        // readbuffer is a string. "true" or "false"
        if (readBuffer == "true" || "Order Completed")
        {
            // return data
            is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
        }
    }
    curl_easy_cleanup(curl);
    readBuffer = "";
}


// void pagethankyou::showPopup()

// {
//     qDebug() << "IN SHow popup";
//     //popup
//     ui->popup = new QDialogButtonBox();
//     ui->popup->setWindowTitle("Help");
//     ui->popup->setFixedSize(700,500);
//     ui->popup->show();
//     ui->selectProblem_Label->setVisible(true);
//     ui->option1_Button->setVisible(true);
//     ui->option2_Button->setVisible(true);
//     ui->option3_Button->setVisible(true);
//     ui->option4_Button->setVisible(true);
//     ui->option5_Button->setVisible(true);

//     // show the window
//     popup->exec();
//     //adding buttons for help options in the popup
//     ui->selectProblem_Label->setVisible(true);
//     ui->option1_Button->setVisible(true);
//     ui->option2_Button->setVisible(true);
//     ui->option3_Button->setVisible(true);
//     ui->option4_Button->setVisible(true);
//     ui->option5_Button->setVisible(true);
     
        
// }


void pagethankyou::controllerFinishedTransaction()
{
    if (is_in_state_thank_you)
    {
        qDebug() << "Thank you page: Controller msg: All done for transaction.";
        is_controller_finished = true;
        thankYouEndTimer->start(1000);
        _thankYouTimeoutSec = PAGE_THANK_YOU_TIMEOUT_SECONDS;
    }
    else
    {
        qDebug() << "WARNING: Transaction end received while not in thank you page.";
    }
}

void pagethankyou::transactionToFile(char *curl_params)
{
    QString data_out = curl_params;
    // p_page_idle->dfUtility->write_to_file_timestamped(TRANSACTION_DISPENSE_END_OFFINE_PATH, data_out);
    p_page_idle->dfUtility->write_to_file(TRANSACTION_DISPENSE_END_OFFINE_PATH, data_out);
}

void pagethankyou::onThankyouTimeoutTick()
{
    if (--_thankYouTimeoutSec >= 0)
    {
    }
    else
    {
        exitPage();
        exitIsForceable = true;
    }
}

void pagethankyou::on_mainPage_Button_clicked()
{
    qDebug() << "main page button clicked.";
    exitPage();
}

void pagethankyou::exitPage()
{
    transactionLogging = "";
    if ((is_controller_finished && is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER) || exitIsForceable)
    {
        thankYouEndTimer->stop();
        // qDebug() << "thank you to idle";
        is_in_state_thank_you = false;
        p_page_idle->currentProductOrder->setPromoCode("");

        // p_page_idle->showFullScreen();
        // this->hide();
        p_page_idle->pageTransition(this, p_page_idle);

        if (exitIsForceable)
        {
            qDebug() << "ERROR?!:Forced exit. controller ok?: " << is_controller_finished << " is payment finished?:" << is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER;
        }
    }
    else
    {
        // ui->extra_message_label->setText("<p align=center><br>Waiting for end<br>of transaction...</p>");
        // ui->extra_message_label->show();
        ui->thank_you_message_label->setText("Finishing<br>transaction");
        ui->thank_you_subtitle_message_label->hide();

        thankYouEndTimer->start(1000);
        _thankYouTimeoutSec = PAGE_THANK_YOU_TIMEOUT_SECONDS;
    }
}



void pagethankyou::on_notifyUs_Button_clicked()
{

    qDebug() << "Transition to Help Page";
    thankYouEndTimer->stop();
    // p_page_help->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_sendFeedback);
}

// showPopup();

    // page_notifyus *notifyUsPage = new page_notifyus;
    // notifyUsPage->show();
    // new UI for popup with buttons and labels

    // QVBoxLayout* layout = new QVBoxLayout();
    
    // //revert
    // QDialog* dialog = new QDialog(this);
    
    // //to remove title bar
    // dialog->setWindowFlags(Qt::CustomizeWindowHint);
    // dialog->setLayout(layout);
    // dialog->resize(900,1000);
    


    // //     QDialogButtonBox* buttonBox = new QDialogButtonBox();

    //     QLabel* selectProblem_Label = new QLabel("Select problems from the option below");
    //     selectProblem_Label->setAlignment(Qt::AlignCenter);
    //    // selectProblem_Label->setAttribute(Qt::WA_TransparentForMouseEvents);
        
    //     // QPushButton* option1_Button = buttonBox->addButton("No product dispensed", QDialogButtonBox::AcceptRole);
    //     // QPushButton* option2_Button = buttonBox->addButton("Less Volume dispensed", QDialogButtonBox::AcceptRole);
    //     // QPushButton* option3_Button = buttonBox->addButton("Wrong Product dispensed", QDialogButtonBox::AcceptRole);
    //     // QPushButton* option4_Button = buttonBox->addButton("Charged more than once", QDialogButtonBox::AcceptRole);
    //     // QPushButton* option5_Button = buttonBox->addButton("Screen was Frozen", QDialogButtonBox::AcceptRole);

    //     //test
    //     //to create a QDialogButtonBox with a "Send" button
    //     QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

    //     QButtonGroup *group = new QButtonGroup(this);
    //     QPushButton* option1_Button = new QPushButton("No product dispensed");
    //     QPushButton* option2_Button = new QPushButton("Less Volume dispensed");
    //     QPushButton* option3_Button = new QPushButton("Wrong Product dispensed");
    //     QPushButton* option4_Button = new QPushButton("Charged more than once");
    //     QPushButton* option5_Button = new QPushButton("Screen was Frozen");
    //     QPushButton* send_Button = new QPushButton("Send");
    //     layout->addWidget(selectProblem_Label);
    //     layout->addWidget(option1_Button);
    //     layout->addWidget(option2_Button);
    //     layout->addWidget(option3_Button);
    //     layout->addWidget(option4_Button);
    //     layout->addWidget(option5_Button);
    //     layout->addWidget(send_Button);
    //     option1_Button->setAutoFillBackground(true);
    //     option2_Button->setAutoFillBackground(true);
    //     option3_Button->setAutoFillBackground(true);
    //     option4_Button->setAutoFillBackground(true);
    //     option5_Button->setAutoFillBackground(true);

    //     option1_Button->setCheckable(true);
    //     group->addButton(option1_Button);
    //     option2_Button->setCheckable(true);
    //     group->addButton(option2_Button);
    //     option3_Button->setCheckable(true);
    //     group->addButton(option3_Button);
    //     option3_Button->setCheckable(true);
    //     group->addButton(option3_Button);
    //     option4_Button->setCheckable(true);
    //     group->addButton(option4_Button);
    //     option5_Button->setCheckable(true);
    //     group->addButton(option5_Button);

        

    //     dialog->setStyleSheet("background-color: rgba(94, 133, 128, 0.983); border-radius: 30px; border: 2px solid black;");
    //     selectProblem_Label->setFixedHeight(40);
    //     selectProblem_Label->setStyleSheet("border: none; font-family: 'Brevia'; font-style: normal; font-weight: 85; font-size: 40px; line-height: 70px; letter-spacing: 1.5px;");        
    //     option1_Button->setStyleSheet("font-family: 'Brevia'; font-style: normal; font-weight: 75; font-size: 32px; line-height: 99px; letter-spacing: 1.5px; border-radius:25px; border: 2px solid black; background-color: white;");
    //     option2_Button->setStyleSheet("font-family: 'Brevia'; font-style: normal; font-weight: 75; font-size: 32px; line-height: 99px; letter-spacing: 1.5px; border-radius:25px; border: 2px solid black; background-color: white;");
    //     option3_Button->setStyleSheet("font-family: 'Brevia'; font-style: normal; font-weight: 75; font-size: 32px; line-height: 99px; letter-spacing: 1.5px; border-radius:25px; border: 2px solid black; background-color: white;");
    //     option4_Button->setStyleSheet("font-family: 'Brevia'; font-style: normal; font-weight: 75; font-size: 32px; line-height: 99px; letter-spacing: 1.5px; border-radius:25px; border: 2px solid black; background-color: white;");
    //     option5_Button->setStyleSheet("font-family: 'Brevia'; font-style: normal; font-weight: 75; font-size: 32px; line-height: 99px; letter-spacing: 1.5px; border-radius:25px; border: 2px solid black; background-color: white;");
    //     send_Button->setStyleSheet("font-family: 'Brevia'; font-style: normal; font-weight: 75; font-size: 32px; line-height: 50px; letter-spacing: 1.5px; border-radius:25px; border: 2px solid black; background-color: white;");

    //     option1_Button->setMinimumHeight(100);
    //     option2_Button->setMinimumHeight(100);
    //     option3_Button->setMinimumHeight(100);
    //     option4_Button->setMinimumHeight(100);
    //     option5_Button->setMinimumHeight(100);


 