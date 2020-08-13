//***************************************
//
// mainwindow.cpp
//
// starting point of the UI
//
// created: 01-04-2019
// by: Phur Tenzin Sherpa
//
// copyright 2019 by drinkfill
// all rights reserved

#include "mainwindow.h"
#include "df_sockets.h"
#include <stdio.h>
#include <stdlib.h>
#include <QApplication>

int main(int argc, char *argv[]) {



  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}
