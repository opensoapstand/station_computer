#include "survey.h"
#include "ui_survey.h"

#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include <iostream>

survey::survey(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::survey)
{
    ui->setupUi(this);
    /*background setup*/
    QPixmap background(":/asset/background.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /*label setup*/
    QFont font("Arial", 70, QFont::Bold);
    ui->surveyLabel->setFont(font);
    ui->surveyLabel->setStyleSheet("color: white");
    ui->surveyLabel->setAlignment(Qt::AlignCenter);
    ui->surveyLabel->setText("TELL US ABOUT YOU");

    QFont questionFont("Arial", 40, QFont::Bold);
    ui->drinkLabel->setFont(questionFont);
    ui->drinkLabel->setStyleSheet("color: white");
    ui->drinkLabel->setText("What other drinks would you like to in your mouth?");
    ui->drinkLabel->setWordWrap(true);

    ui->emailLabel->setFont(questionFont);
    ui->emailLabel->setStyleSheet("color: white");
    ui->emailLabel->setText("Join our email list and find out what what our latest mission is?");
    ui->emailLabel->setWordWrap(true);

    QFont invalidFont("Arial", 20, QFont::Bold);
    ui->invalidLabel->setFont(invalidFont);
    ui->invalidLabel->setStyleSheet("color: red");
    ui->invalidLabel->setText("Invalid email format, please try again");
    ui->invalidLabel->setWordWrap(true);
    ui->invalidLabel->setAlignment(Qt::AlignCenter);
    ui->invalidLabel->hide();


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
    ui->saveButton->setStyleSheet("border-image:url(:/asset/rect_button.png); color: white;");


    /*image use for the payment page*/
    QPixmap logo(":/asset/logo-white.png");
    logo = logo.scaled(ui->logoLabel->size(), Qt::IgnoreAspectRatio);
    ui->logoLabel->setPixmap(logo);
}

survey::~survey()
{
    delete ui;
}



void survey::on_saveButton_clicked()
{
    if(!ui->emailEdit->hasAcceptableInput() && !ui->drinkEdit->hasAcceptableInput())
    {
        ui->emailEdit->setStyleSheet("QLineEdit { color: red;}");
        ui->drinkEdit->setStyleSheet("QLineEdit { color: red;}");
        ui->invalidLabel->setText("Invalid formats, please try again");
        ui->invalidLabel->show();
    }
    else if(!ui->emailEdit->hasAcceptableInput()){
        ui->emailEdit->setStyleSheet("QLineEdit { color: red;}");
        ui->invalidLabel->setText("Invalid email format, please try again");
        ui->invalidLabel->show();
    }    
    else if(!ui->drinkEdit->hasAcceptableInput()){
        ui->drinkEdit->setStyleSheet("QLineEdit { color: red;}");
        ui->invalidLabel->setText("Invalid input, more than three characters, please try again");
        ui->invalidLabel->show();
    }
    else{
        ui->drinkEdit->setStyleSheet("QLineEdit { color: black;}");
        ui->emailEdit->setStyleSheet("QLineEdit { color: black;}");
        ui->invalidLabel->hide();
    }


    //ui->textEdit->setAutoFormatting(QTextEdit::AutoAll);
    drinks = ui->drinkEdit->text();
    email = ui->emailEdit->text();
    //goodStuff = ui->drinkEdit->toPlainText();
    qDebug() << drinks.toLower();
    qDebug() << email.toLower();
}
