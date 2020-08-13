//***************************************
//
// mainwindow.cpp
//
// this class represents the and creates the UI
//
// created: 01-04-2019
// by: Phur Tenzin Sherpa
//
// copyright 2019 by drinkfill
// all rights reserved


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QDebug>
#include <QTimer>
#include <df_sockets.h>
#include <QToolButton>
#include <QPolygon>
#include <QColorDialog>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <QGraphicsScene>
#include <unistd.h>
#include "progressbar.h"
#include "globalvariables.h"

int progressData;
int selectedDrink = 10;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    QMainWindow::showFullScreen();
    ui->label_2->setVisible(false);


    ui->pages->setCurrentIndex(0);
	//	ui->back_2->setVisible(false);
    /**
     * @brief coffeeBtn
     * page one buttons
     * @button 1- coffee
     * @button 2- kombucha
     * @button 3- water
     */
    connect(ui->coffee, SIGNAL(clicked()), this, SLOT(selectDrink()));
    connect(ui->kombucha, SIGNAL(clicked()), this, SLOT(selectDrink()));
    connect(ui->sparklingwater, SIGNAL(clicked()), this, SLOT(selectDrink()));

    /*
     * apply rounded button properties for three above the buttons
     */
    ui->kombucha->setStyleSheet("border-radius: 25px;background-color:rgba(0, 0, 0, 0)");
    ui->coffee->setStyleSheet("border-radius: 25px;background-color:rgba(0, 0, 0, 0)");
    ui->sparklingwater->setStyleSheet("border-radius: 25px;background-color:rgba(0, 0, 0, 0)");
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

   /**
    * @brief connect quantity buttons to dispenseProduct slot
    */
   connect(ui->pushButton_1,SIGNAL(clicked()),this,SLOT(dispenseProduct()));
   connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(dispenseProduct()));
   connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(dispenseProduct()));

   /**
     * @brief connect back button
     */
    connect(ui->back, SIGNAL(clicked()), this, SLOT(back()));
	// connect(ui->back_2,SIGNAL(clicked()),this,SLOT(back_2()));

    ui->productToDispense->setVisible(false);




}


void MainWindow::updateProgress()
{
	qDebug()<<"shold update progress bar"<<endl;
	std::cout<<progressData;
	ui->progressBar->setValue(473-progressData);
}
void MainWindow::back_2()
{
	ui->pages->setCurrentIndex(1);
}



void MainWindow::selectDrink() {


    ui->productToDispense->setVisible(false);
    ui->productToDispense->setText(((QPushButton*)sender())->text());
    ui->pages->setCurrentIndex(1);

    QPixmap coldbrew("/home/soapstand/Desktop/drinkfilldemo-master/images/page1/Pg2-Coldbrew.png");
               ui->productImage->setPixmap(coldbrew);

    if(ui->productToDispense->text()=="COLD COFFEE")
        {  QPixmap coldbrew("/home/soapstand/Desktop/drinkfilldemo-master/images/page1/Pg2-Coldbrew.png");
           ui->productImage->setPixmap(coldbrew);
			selectedDrink = 1;

        }

    else if(ui->productToDispense->text()=="KOMBUCHA")
        {
            QPixmap kombucha("/home/soapstand/Desktop/drinkfilldemo-master/images/page1/Pg2-Kombucha.png");
            ui->productImage->setPixmap(kombucha);
			 selectedDrink = 2;
        }

    else if(ui->productToDispense->text()=="WATER")
        {

        QPixmap carbonated("/home/soapstand/Desktop/drinkfilldemo-master/images/page1/pg2-carbonated.png");
        ui->productImage->setPixmap(carbonated);
			  selectedDrink = 3;
        }

    qDebug()<<ui->productImage->text();
    qDebug()<<ui->productToDispense->text()<<endl;

}
void MainWindow::dispenseProduct()
    {
	  int dispenseId = 10;
     qDebug()<<"dispense product called"<<endl;

     QString amountSelected = ((QPushButton*)sender())->objectName();


     ui->buttonGroup_2->setStyleSheet("   color: #333;"
                                    "border: 0px solid #555;"
                                    "border-radius: 125px;"
                                    "background: white;"
                                    "border-style: outset;");

     /*
          ui->pushButton_1->setStyleSheet("   color: #333;"
                      "border: 0px solid #555;"
                      "border-radius: 125px;"
                      "background: white;"
                      "border-style: outset;");

      ui->pushButton_1->setStyleSheet("   color: #333;"
                      "border: 0px solid #555;"
                      "border-radius: 125px;"
                      "background: white;"
                      "border-style: outset;");
      ui->pushButton_1->setStyleSheet("   color: #333;"
                          "border: 0px solid #555;"
                          "border-radius: 125px;"
                          "background: white;"
                          "border-style: outset;");
*/

      ui->pages->setCurrentIndex(2);
     qDebug()<<"dispenseProduct";

     if(amountSelected == "pushButton_1")
     {
				//*begin = 473;


     }

     else if(amountSelected == "pushButton_2")
     {
			  //  *begin = 709;
     }

      else if(amountSelected == "pushButton_3")
     {
				 //*begin = 946;
     }

	  qDebug()<<"progess bar max should be "<<473;
	  ui->progressBar->setMaximum(473);
	  ui->progressBar->setValue(473);

     quantitySelected = true;


	  packet tempHeader ;
	  tempHeader.version= 1;
	  tempHeader.id = 1;
	  tempHeader.body = selectedDrink;
	  tempHeader.query = query_types::s_dispense;
	  tempHeader.stateToSet = df_State::selected;


	  pthread_t reader_thread;
	  if(pthread_create(&reader_thread,NULL,&df_sockets::listenToPackets, nullptr))
	  {
		  qDebug("error creating the reader thread");
	  }

	  df_sockets writer;
	  writer.writePackets(tempHeader);


	  QTimer *timer = new QTimer(this);
	  connect(timer, SIGNAL(timeout()), this, SLOT(updateProgress()));
	  timer->start(5);
	  timer->setInterval(5);

  }



 void MainWindow::pageDecision()
 {
     if(quantitySelected)
     {
        // confirm();
           //ui->pages->setCurrentIndex(2);
     }
     else {
         ui->pages->setCurrentIndex(0);
     }

 }

 // Go back to previous page
 void MainWindow::back() {

   //  ui->toolButton_3->setStyleSheet("QToolButton {background-color:white;}");

	  qDebug()<<"back called by timer?";


     // change the state to FSM state to selected state

	//  df_sockets writer;
	 // writer.writePackets("test",&tempHeader);
   //  ui->pages->setCurrentIndex(0);
     int currentIndex = ui->pages->currentIndex();
    // qDebug()<<currentIndex<<endl;
     switch (currentIndex)
     {
         case 1:
                ui->pages->setCurrentIndex(0);
            //    timerPageOneZero->stop();
                break;
         case 2:
                ui->pages->setCurrentIndex(1);
            //    timerPageTwoOne->stop();   //unnecessary ?
                //*letterToArduino = 'x';
                //writeToArduino()
                break;
         case 3:
                ui->pages->setCurrentIndex(2);
            //    timerToResume->stop();

                break;
         case 0:
                ui->pages->setCurrentIndex(0);
                break;
         default:
                ui->pages->setCurrentIndex(2);
                break;
     }
 }

 // Confirm user choice and proceed to dispense page

 void MainWindow::confirm() {

 }


 void MainWindow::dispense() {


 }

 // End dispense
 void MainWindow::endDispense() {
		ui->progressBar->setValue(473);

   //  ui->dispenselabel->setText("Dispense complete!");
   //  ui->dispensebtn->setText("Dispense");
     ui->pages->setCurrentIndex(3);
     QTimer::singleShot(3000,this, SLOT(backToHome()));
 }

 // Show error page if an error has occured
 void MainWindow::error() {
     ui->pages->setCurrentIndex(4);
 }



 /**
  * @brief MainWindow::arduinoInit
  * function to initialize the arduino port
  */
/*
 void MainWindow::arduinoInit() {

         arduinoPort    = new QSerialPort();
         char data = 'z';
         selectedProduct = &data;
         serial.setPortName("ttyACM0");
         serial.setBaudRate(QSerialPort::Baud9600, QSerialPort::Output);
         QByteArray buffer = QByteArray::number(123) + "\n";
         serial.open(QIODevice::ReadWrite);
         qDebug()<<serial.errorString();
         qDebug()<<*selectedProduct;
          if (serial.isOpen()){

              qDebug()<<"port is open"<<endl;
            //serial.close();
          }
          else {
               qDebug()<<"port is not open error"<<endl;
          }

 }
 /**
  * @brief MainWindow::writeToArduino
  * write data to arduino
  */

/*
 void MainWindow::writeToArduino() {
  //  timerPageTwoOne->stop();
   // timerPageThreeTwo->start(15000);

    qDebug()<<"selected product" <<*selectedProduct;
    qDebug()<<"selected amount "<<*begin;
    qDebug() <<"letter to arduino"<<*letterToArduino;
     if(serial.isOpen())
     {
         serial.write(letterToArduino);
         serial.waitForBytesWritten(20);
         serial.flush();


         //serial.close();
     }
     else {

          qDebug()<<"Port not open";

     }

 }

 */
 /**
  * @brief MainWindow::readFromArduino
  * read data from arduino
  */
/*
 void MainWindow::readFromArduino() {
   //  timerPageTwoOne->stop();     // check if triggered on read ready ?

     float previousData = *begin;
     QString fs2;
     qDebug()<<"read called"<<endl;
		dummy= dummy-1;
   if(serial.isOpen())
     {

       qDebug()<< dummy;
        while(serial.bytesAvailable()>0)
         {


         QByteArray data = serial.readLine();

          fs2 = tr(data);

          if(fs2.toFloat() > data2 && fs2.toFloat() !=0.0 )
          {
              dummy= 5000;
               qDebug()<<"read:";
              data2= fs2.toFloat();
          }

           qDebug()<<data2;


/*

           if(*dispensePaused == previousData-data2 && *dispensePaused!= 0.0)
           {
               timerPageThreeTwo->stop();
             //  timerToResume->start(15000);
           //    qDebug()<<"dispensing paused, should go to thank you page in 15s"<<endl;
            //   qDebug()<<*dispensePaused;
               //fire a timer
           }


            if(dummy ==0)
            {
                timerToResume->stop();
                timerPageThreeTwo->stop();
                qDebug()<<"dispenseing complete";
                *letterToArduino = 'x';
					 //writeToArduino();
               QTimer::singleShot(0,this, SLOT(toThankYou()));

            }

          if(previousData>(data2))
          {



              qDebug()<<data2;
            //  dummy= dummy--;
             // qDebug()<<*dispensePaused;
             // qDebug()<<previousData-data2;


              previousData = previousData-data2;
              ui->progressBar->setValue((previousData));
               *dispensePaused = previousData;

              if(previousData <=2 )        // process completed
              {
                  timerToResume->stop();
                //  timerPageThreeTwo->stop();
                  qDebug()<<"dispenseing complete";

                  QTimer::singleShot(2000,this, SLOT(toThankYou()));

              }

          }

        }

     }

 }
*/
 void MainWindow::backToHome()
 {


	 packet tempHeader ;
	 tempHeader.version= 1;
	 tempHeader.id = 1;
	 tempHeader.body = 4;
	 tempHeader.query = query_types::s_dispense;
	 tempHeader.stateToSet = df_State::selected;


	 df_sockets writer;
	 writer.writePackets(tempHeader);
	 ui->pages->setCurrentIndex(0);
	 ui->progressBar->setValue(0);

}


 void MainWindow::toThankYou()
 {


     ui->pages->setCurrentIndex(3);
     QTimer::singleShot(3000,this, SLOT(backToHome()));
 }


 /*
  * method to change the UI componenets when a user selects are drink
  * this method also sends a packet to the state machine telling it to change the state from \
  * idle to selected
  */

void MainWindow::toogleBackground()
{
    //ui->toolButton_3->setStyleSheet("QToolButton {background-color:white }");

}

void MainWindow::backToSamePage()
{
	// timerToResume->stop();
    ui->pages->setCurrentIndex(2);
   // ui->lcdNumber->setVisible(true);
   // ui->lcdNumber->display(timerToResume->remainingTime());
	//forceEndDispensing->start(5000);
	//*letterToArduino ='x';
 //  writeToArduino();


}


void MainWindow::updatePBar(int i)
{

}




 MainWindow::~MainWindow() {
     delete ui;
 }
