#include "survey.h"
#include "ui_survey.h"
#include "mainwindow.h"
#include "payment.h"

#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

survey::survey(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::survey)
{
    ui->setupUi(this);
    /*background setup*/
    QPixmap background(":/assets/V6/ev2_ui_assets_v6-07.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /*label setup*/
    QFont font("Arial", 70, QFont::Bold);
    ui->surveyLabel->setFont(font);
    ui->surveyLabel->setStyleSheet("color: white");
    ui->surveyLabel->setAlignment(Qt::AlignCenter);
    ui->surveyLabel->setText("Drinkfill Survey");

    QFont questionFont("Arial", 40, QFont::Bold);
    ui->drinkLabel->setFont(questionFont);
    ui->drinkLabel->setStyleSheet("color: white");
    ui->drinkLabel->setText("What other drinks would you like us to stock?");
    ui->drinkLabel->setWordWrap(true);

    ui->bottleSizeLabel->setFont(questionFont);
    ui->bottleSizeLabel->setStyleSheet("color: white");
    ui->bottleSizeLabel->setText("What size of reusable bottle do you currently use?");
    ui->bottleSizeLabel->setWordWrap(true);

    ui->emailLabel->setFont(questionFont);
    ui->emailLabel->setStyleSheet("color: white");
    ui->emailLabel->setText("Join our email list for more discounts, and to track our next launch?");
    ui->emailLabel->setWordWrap(true);

    QFont invalidFont("Arial", 20, QFont::Bold);
    ui->invalidLabel->setFont(invalidFont);
    ui->invalidLabel->setStyleSheet("color: red");
    ui->invalidLabel->setText("Invalid email format, please try again");
    ui->invalidLabel->setWordWrap(true);
    ui->invalidLabel->setAlignment(Qt::AlignCenter);
    ui->invalidLabel->hide();

    /*radio button setup*/
    QFont optionFont("Arial", 17, QFont::Bold);
    ui->ozRB_12->setFont(optionFont);
    ui->ozRB_16->setFont(optionFont);
    ui->ozRB_24->setFont(optionFont);
    ui->ozRB_DK->setFont(optionFont);
    ui->ozRB_NA->setFont(optionFont);

//    ui->ozRB_12->setStyleSheet("QRadioButton{color: white;}"
//                               "QRadioButton::indicator:checked{color: black;}");
//    ui->ozRB_16->setStyleSheet("color: white;");
//    ui->ozRB_24->setStyleSheet("color: white;");
//    ui->ozRB_DK->setStyleSheet("color: white;");
//    ui->ozRB_NA->setStyleSheet("color: white;");

    ui->hiddenButton->hide();
    ui->hiddenButton->click();


    /*line edit setup*/
    /*allow one ore more occurrences of any letter, digit or special symbols ._%+-
    followed by @
    followed by one ore more occurrences of any letter, digit or special symbols ._
    followed by . (dot)
    followed by two, three or four letters
    */
    QRegularExpression emailRx("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b",
                              QRegularExpression::CaseInsensitiveOption);
    ui->emailEdit->setValidator(new QRegularExpressionValidator(emailRx,this));

    QRegularExpression questionRx("\\b[A-Z0-9]{3,100}\\b",
                              QRegularExpression::CaseInsensitiveOption);
    ui->drinkEdit->setValidator(new QRegularExpressionValidator(questionRx,this));

    /*push button setup*/
    QFont buttonFont("Arial", 30, QFont::Bold);
    ui->saveButton->setFont(buttonFont);
    ui->saveButton->setText("SAVE $1.00");
    ui->saveButton->setStyleSheet("border-image:url(:/assets/V6/ev2_ui_assets_v6-17.png); color: white;");

    ui->goBackButton->setStyleSheet("border-image:url(:/assets/V6/ev2_ui_assets_v6-16.png); color: white;");
    //QRect rect(0,0,200-2,200-2);
    //QRegion region(rect, QRegion::Ellipse);
    //ui->goBackButton->setMask(region); //masking the region to be circle for the button
    ui->goBackButton->setFont(buttonFont);
    ui->goBackButton->setText("Go Back");

//    /*image use for the payment page*/
//    QPixmap logo(":/assets/logo-white.png");
//    logo = logo.scaled(ui->logoLabel->size(), Qt::IgnoreAspectRatio);
//    ui->logoLabel->setPixmap(logo);
//    ui->logoLabel->hide();
}

void survey::setPages(MainWindow *main)
{
    this->mainPage = main;
}

bool survey::getSurveyFilled()
{
    return surveyFilled;
}

void survey::resetSurveyFilled()
{
    surveyFilled = false;
}

void survey::storeToDatabase(database_logger logger, QString drink,QString email,QString stat)
{
    if(logger.checkDataBaseConnection())
        logger.reconnectDatabase();

    logger.surveyLog(QDateTime::currentDateTime(), drink, email, stat, radioButtonSelect);
}

survey::~survey()
{
    delete ui;
}

bool survey::checkEmailD(database_logger logger, QString email)
{
    return logger.checkEmailDuplication(email);
}


void survey::on_saveButton_clicked()
{
    if (radioButtonSelect == "")
    {
        ui->invalidLabel->setText("Question(s) not answered, please try again");
        ui->invalidLabel->show();
        surveyFilled = false;

        storeToDatabase(mainPage->getDatabase(), ui->drinkEdit->text(), ui->emailEdit->text(),
                        "Radio Button Not Select");
    }
    else{
        if(!ui->emailEdit->hasAcceptableInput() && !ui->drinkEdit->hasAcceptableInput())
        {
            ui->emailEdit->setStyleSheet("QLineEdit { color: red;}");
            ui->drinkEdit->setStyleSheet("QLineEdit { color: red;}");
            ui->invalidLabel->setText("Please answer for Question 1 & 3, please try again");
            ui->invalidLabel->show();
            surveyFilled = false;

            storeToDatabase(mainPage->getDatabase(), ui->drinkEdit->text(), ui->emailEdit->text(), "Both Invalid");
        }
        else if(!ui->emailEdit->hasAcceptableInput()){
            ui->emailEdit->setStyleSheet("QLineEdit { color: red;}");
            ui->drinkEdit->setStyleSheet("QLineEdit { color: black;}");
            ui->invalidLabel->setText("Please answer for Question 3 format, please try again");
            ui->invalidLabel->show();
            surveyFilled = false;

            storeToDatabase(mainPage->getDatabase(), ui->drinkEdit->text(), ui->emailEdit->text(),
                            "Email Invalid");
        }
        /*drink invalid is fine other two answers are more interesting*/
//        else if(!ui->drinkEdit->hasAcceptableInput()){
//            ui->drinkEdit->setStyleSheet("QLineEdit { color: red;}");
//            ui->emailEdit->setStyleSheet("QLineEdit { color: black;}");
//            ui->invalidLabel->setText("Invalid Question 1 format, need to be more than three characters, please try again");
//            ui->invalidLabel->show();
//            surveyFilled = false;

//            storeToDatabase(mainPage->getDatabase(),ui->drinkEdit->text(), ui->emailEdit->text(),
//                            "Drink Invalid");
//        }
        else{
            //hide();          
            surveyFilled = checkEmailD(mainPage->getDatabase(), ui->emailEdit->text());

            if(surveyFilled == true)
            {
                ui->drinkEdit->setStyleSheet("QLineEdit { color: black;}");
                ui->emailEdit->setStyleSheet("QLineEdit { color: black;}");
                ui->invalidLabel->hide();

                storeToDatabase(mainPage->getDatabase(), ui->drinkEdit->text(), ui->emailEdit->text(),
                                "Survey Filled");
                mainPage->showFullScreen();
            }
            else {
                ui->emailEdit->setStyleSheet("QLineEdit { color: red;}");
                ui->drinkEdit->setStyleSheet("QLineEdit { color: black;}");
                ui->invalidLabel->setText("Please use another email, entered email already used");
                ui->invalidLabel->show();

                storeToDatabase(mainPage->getDatabase(), ui->drinkEdit->text(), ui->emailEdit->text(),
                                "Email Used");
            }


        }
    }


    //ui->textEdit->setAutoFormatting(QTextEdit::AutoAll);
    drinks = ui->drinkEdit->text();
    email = ui->emailEdit->text();
    //goodStuff = ui->drinkEdit->toPlainText();
    qDebug() << drinks.toLower();
    qDebug() << email.toLower();
}

void survey::on_goBackButton_clicked()
{
    //hide();
    surveyFilled = false;

    storeToDatabase(mainPage->getDatabase(), ui->drinkEdit->text(), ui->emailEdit->text(),
                                      "Go Back Pressed");
    ui->hiddenButton->click();
    ui->drinkEdit->clear();
    ui->emailEdit->clear();
    ui->invalidLabel->hide();
    mainPage->showFullScreen();
}

void survey::showEvent(QShowEvent *event)
{
    ui->hiddenButton->click();
    mainPage->hide();
    QWidget::showEvent(event);
}

void survey::on_ozRB_12_clicked()
{
    radioButtonSelect = "12oz";
}

void survey::on_ozRB_16_clicked()
{
    radioButtonSelect = "16oz";
}

void survey::on_ozRB_24_clicked()
{
    radioButtonSelect = "24oz";
}

void survey::on_ozRB_NA_clicked()
{
    radioButtonSelect = "None of the Above";
}

void survey::on_ozRB_DK_clicked()
{
    radioButtonSelect = "Don't Know the Size";
}


void survey::on_hiddenButton_clicked()
{
    radioButtonSelect = "";
}
