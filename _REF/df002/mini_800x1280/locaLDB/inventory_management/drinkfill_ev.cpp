#include "drinkfill_ev.h"
#include "ui_drinkfill_ev.h"

#include <iostream>

drinkfill_ev::drinkfill_ev(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::drinkfill_ev)
{
    ui->setupUi(this);
    ui->statusBar->setSizeGripEnabled(false);
    setWindowTitle("Drinkfill Inventory Management");

    ui->curInventoryView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->curInventoryBox->setTitle("Current Inventory");
    ui->updateInventoryBox->setTitle("Inventroy Management");
    ui->setupBox->setTitle("Setup");
    ui->machineIdEdit->setPlaceholderText("Enter Machine ID");

    /*adjust physical layout*/
    ui->volumeUpdateButton->setFixedHeight(25);
    ui->optionComboBox->setFixedHeight(25);
    ui->volumeEdit->setFixedHeight(25);

    /*text edit setup*/
    QRegularExpression volumeRx("\\b[0-9]{3,7}\\b"); //only allow numeric input
    ui->volumeEdit->setValidator(new QRegularExpressionValidator(volumeRx,this));
    ui->volumeEdit->setPlaceholderText("Enter Volume");

    QRegularExpression optionsRx("\\b[0-9]{1,2}\\b"); //only allow numeric input
    ui->machineIdEdit->setValidator(new QRegularExpressionValidator(optionsRx,this));
    ui->machineIdEdit->setPlaceholderText("Enter #Drinks");

    /*ui style*/
    QPixmap background(":/assets/background.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);
    ui->companyLabel->setPixmap(QPixmap(":/assets/logo-white.png"));
    ui->companyLabel->setScaledContents(true);
    ui->companyLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    QString colorStr = QString("background-color:") + "white" + ";";
    ui->menuBar->setStyleSheet(colorStr);
    ui->statusBar->setStyleSheet(colorStr);
    ui->mainToolBar->setMovable(false);
    ui->mainToolBar->hide();

    /*tab title setup*/
    ui->tabWidget->setTabText(0, "Inventories");
    ui->tabWidget->setTabText(1, "Products");
    ui->tabWidget->setTabText(2, "Activities");
    ui->tabWidget->setTabText(3, "Transactions");
    ui->tabWidget->setTabText(4, "Coupon Code");
    ui->tabWidget->setTabText(5, "Setup");
    ui->tabWidget->setCurrentIndex(5);
    setupTables();

    /*price edit setup*/
    ui->newProductPrice1Edit->setPlaceholderText("Enter Price(Sm) Value");
    QRegularExpression priceRx("\\b[.0-9]{3,7}\\b"); //only allow numeric input
    ui->newProductPrice1Edit->setValidator(new QRegularExpressionValidator(priceRx,this));

    ui->newProductPrice2Edit->setPlaceholderText("Enter Price(Larg) Value");
    ui->newProductPrice2Edit->setValidator(new QRegularExpressionValidator(priceRx,this));

    ui->newProductNameEdit->setPlaceholderText("Drink Name");
    //ui->newProductAmountEdit->setPlaceholderText("12 oz (355ml)");
    ui->newProductAmountEdit->hide();

    //machineID = "Not Set";
    //    machineID = currDatabase.getMachineID(machineID);
    //    NUM_DRINKS = currDatabase.getNumProductLog(machineID);
    updateProductBox();

    NUM_MACHINE = currDatabase.getNumMachine();

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(clearInputs()));

    ui->numDrinksEdit->setText(QString::number(NUM_DRINKS));
    ui->machineIdEdit->setText(machineID);
    qDebug() << "Current ID - " << machineID << ", Current #Drinks - " << NUM_DRINKS;

    int temp = currDatabase.getNumProductLog(machineID);
    qDebug() << "current #Products: " << NUM_DRINKS;

    if (temp < NUM_DRINKS){
        currDatabase.productLogSetup(temp, NUM_DRINKS, machineID);
        qDebug() << "product log set #: " << currDatabase.getNumProductLog(machineID);
    }

    setupOptions();

    ui->machineIdButton->hide();
    ui->numDrinksButton->hide();
}

drinkfill_ev::~drinkfill_ev()
{
    delete ui;
}

void drinkfill_ev::on_volumeUpdateButton_clicked()
{
    QString volume = ui->volumeEdit->text();
    QString option = ui->optionComboBox->currentText();
    int curOption = (option.at(option.size()-1)).digitValue();

    if(ui->volumeEdit->hasAcceptableInput()){
        ui->volumeEdit->setStyleSheet("QLineEdit { color: black;}");
        currDatabase.eventLog(machineID, QDateTime::currentDateTime(), curOption, "Reset - " + option, volume.toInt());

        QStringList temp = inventoryModel->stringList();
        temp[curOption-1] = "option " + QString::number(curOption) + " - " + volume;
        inventoryModel->setStringList(temp);

        ui->statusBar->showMessage("Update Inventory Successfully");
    }
    else {
        ui->volumeEdit->setStyleSheet("QLineEdit { color: red;}");
        currDatabase.eventLog(machineID, QDateTime::currentDateTime(), curOption, "Reset Invalid - " + option, volume.toInt());
        ui->statusBar->showMessage("Inventory is not updated, possibly due to invalid input");
    }

}

void drinkfill_ev::setupTables()
{
    ui->activityView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->transactionView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //ui->productView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->activityView->setSortingEnabled(true);
    ui->transactionView->setSortingEnabled(true);


    ui->activityView->setStyleSheet(QString("background-color:") + "white" + ";");
    ui->transactionView->setStyleSheet(QString("background-color:") + "white" + ";");
    ui->productView->setStyleSheet(QString("background-color:") + "white" + ";");

    ui->activityBox->setTitle("Events From Database");
    ui->transactionBox->setTitle("Transactions From Database");
    ui->productBox->setTitle("Products From Database");


}

void drinkfill_ev::updateProductBox()
{
    QSqlQuery* qry = new QSqlQuery(currDatabase.getDB());
    qry->prepare("SELECT * FROM `products` WHERE `machine_id` = '" + QString(machineID) + "'");

    //QSqlQueryModel *productModel = new QSqlQueryModel(this);
    //QSqlTableModel *model = new QSqlTableModel(this, currDatabase.getDB());

    if(qry->exec())
    {
        //productModel->setQuery(*qry);
        //productModel->setTable("products");
        productModel->clear();
        int idx = 0;
        while(qry->next())
        {
            QList<QStandardItem*> listOfItems;
            for (int i = 0; i < 7; i++){
                if(i != 4)
                    listOfItems.append(new QStandardItem(qry->value(i).toString()));
                else {

                    if (currDatabase.getImgArray(idx, machineID) == QByteArray(NULL))
                    {
                        listOfItems.append(new QStandardItem(qry->value(i).toString()));
                    }
                    else{
                        QPixmap image;
                        image.loadFromData(currDatabase.getImgArray(idx,machineID));
                        image = image.scaled(100,100, Qt::KeepAspectRatio);

                        QStandardItem *item = new QStandardItem();
                        item->setData(QVariant(image), Qt::DecorationRole);
                        listOfItems.append(item);
                    }
                }
               }
               productModel->appendRow(listOfItems);

               idx++;
           }
           ui->productView->setModel(productModel);
//        QSortFilterProxyModel *sortedModel = new QSortFilterProxyModel(this);

//        sortedModel->setDynamicSortFilter(true);
//        sortedModel->setSourceModel(model);

//        ui->productView->setModel(sortedModel);
//        ui->productView->sortByColumn(0,Qt::SortOrder::AscendingOrder);
        productModel->sort(0, Qt::SortOrder::AscendingOrder);
        ui->productView->setSortingEnabled(true);
        ui->productView->resizeColumnsToContents();
        ui->productView->resizeRowsToContents();
    }
    else {
        qDebug() << qry->lastError();
    }

}

void drinkfill_ev::setupOptions()
{
    inventoryModel = new QStringListModel(this);
    optionModel = new QStringListModel(this);
    tagModel = new QStringListModel(this);

    //make data
    QStringList inventoryList;
    QStringList optionList;
    int temp;
    for (int i = 1; i <= NUM_DRINKS; i++)
    {
        temp = currDatabase.getInventory(i, machineID);
        if(temp != int(NULL))
        {
            inventoryList << "option " + QString::number(i) + " - " + QString::number(temp);
            optionList << "option " + QString::number(i);
        }
        else
        {
            inventoryList << "option " + QString::number(i) + " - " + "no data available";
            optionList << "option " + QString::number(i);
        }
    }

    //populate the model
    inventoryModel->setStringList(inventoryList);
    optionModel->setStringList(optionList);

    ui->curInventoryView->setModel(inventoryModel);
    ui->optionComboBox->setModel(optionModel);
    ui->newProductSelectBox->setModel(optionModel);

    setupMachineID();
}

void drinkfill_ev::setupMachineID()
{
    machineIdModel = new QStringListModel(this);

    //make data
    QStringList machineIdList;

    QString temp;
    for (int i = 0; i <= NUM_MACHINE; i++)
    {
        temp = currDatabase.getAllMachineID(i);
        if(temp != QString(""))
        {
            machineIdList << temp;

            qDebug() << temp;
        }
    }

    //populate the model
    machineIdModel->setStringList(machineIdList);
    ui->machineIDComboBox->setModel(machineIdModel);
}

void drinkfill_ev::on_activityButton_clicked()
{
    QSqlQuery* qry = new QSqlQuery(currDatabase.getDB());
    qry->prepare("select * from event");

    QSqlQueryModel *model = new QSqlQueryModel();
    if(qry->exec())
    {
        model->setQuery(*qry);
        ui->activityView->setModel(model);
        ui->activityView->resizeColumnsToContents();
    }
}

void drinkfill_ev::on_transactionButton_clicked()
{
    QSqlQuery* qry = new QSqlQuery(currDatabase.getDB());
    qry->prepare("select * from customer");

    QSqlQueryModel *model = new QSqlQueryModel();
    if(qry->exec())
    {
        model->setQuery(*qry);
        ui->transactionView->setModel(model);
        ui->transactionView->resizeColumnsToContents();
    }
}

void drinkfill_ev::on_newImageButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
    dialog.setViewMode(QFileDialog::Detail);
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                            "home/drinkfill",
                                            tr("Images (*.png *.xpm *.jpg)"));
    qDebug() << fileName;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        QString option = ui->newProductSelectBox->currentText();
        int curOption = (option.at(option.size()-1)).digitValue();

        readByteArray = file.readAll();
        ui->statusBar->showMessage(fileName + " is uploaded");
        if(!ui->newProductPrice1Edit->text().isEmpty())
        {
            if(!ui->newProductNameEdit->text().isEmpty()){
                currDatabase.productLog(curOption, ui->newProductPrice1Edit->text(),
                                        ui->newProductNameEdit->text(),fileName, readByteArray, true, true, machineID);

            }

            else {
                currDatabase.productLog(curOption, ui->newProductPrice1Edit->text(),
                                        "", fileName, readByteArray, true, false, machineID);
            }

        }
        else {
            if(!ui->newProductNameEdit->text().isEmpty())
            {
                if(!ui->newProductNameEdit->text().isEmpty())
                    currDatabase.productLog(curOption, "",
                                            ui->newProductNameEdit->text(),fileName, readByteArray, false, true, machineID);
            }
            else {
                currDatabase.productLog(curOption, "",
                                        "", fileName, readByteArray, false, false, machineID);
            }
        }

        if(!ui->newProductAmountEdit->text().isEmpty())
        {
            currDatabase.productLog(curOption, ui->newProductAmountEdit->text(), machineID);
        }


        if(!ui->newProductPrice2Edit->text().isEmpty())
        {
            currDatabase.price2Update(curOption, ui->newProductPrice2Edit->text(), machineID);
        }







        updateProductBox();

        //QImage image(readByteArray);
//        QImage image((const unsigned char*)readByteArray.data(),446,446,QImage::Format_RGB32);
//        image.save("test","PNG");
        //QStandardItem *item = new QStandardItem();
        //item->setData(QVariant(QPixmap::fromImage(image)), Qt::DecorationRole);


//        const QModelIndex idx = ui->productView->indexAt(QPoint(0,0));
//        QStandardItem *item = new QStandardItem();
//        item->setData(QVariant(QPixmap::fromImage(image)), Qt::DecorationRole);
//        qDebug() << productModel->setData(idx, QVariant(QPixmap::fromImage(image)), Qt::DecorationRole);
//        qDebug() << ui->productView->indexAt(QPoint(0,0));
//        qDebug() << productModel->data(idx, Qt::DecorationRole);

    }
    else {
        ui->statusBar->showMessage(fileName + " is invalid");
        qDebug() << "Invalid choice of file";
    }

    clearInputs();
}

void drinkfill_ev::clearInputs()
{
    ui->newProductPrice1Edit->clear();
    ui->newProductPrice2Edit->clear();
    ui->volumeEdit->clear();
    ui->newProductNameEdit->clear();
    ui->newProductAmountEdit->clear();
    fileName = nullptr;
    readByteArray = nullptr;
    updateProductBox();
}

void drinkfill_ev::on_machineIdButton_clicked()
{
//    if(!ui->machineIdEdit->text().isEmpty())
//    {
//        machineID = ui->machineIdEdit->text();
//        int temp = currDatabase.getNumProductLog(machineID);

//        NUM_DRINKS = temp;
//        qDebug() << "current #Products: " << NUM_DRINKS;

//        //currDatabase.machineIdSetup(machineID, NUM_DRINKS);

//        if (temp < defaultDrinkNum){
//            currDatabase.productLogSetup(temp, defaultDrinkNum, machineID);
//            qDebug() << "product log set #: " << currDatabase.getNumProductLog(machineID);
//        }
//        ui->statusBar->showMessage("Machine ID Updated");
//        updateProductBox();
//    }
}

void drinkfill_ev::on_numDrinksButton_clicked()
{
//    if(!ui->machineIdEdit->text().isEmpty())
//    {HiVE-mini
//        //currDatabase.resetProductLog();
//        int temp = currDatabase.getNumProductLog(machineID);
//        NUM_DRINKS = ui->numDrinksEdit->text().toInt();

//        if (temp < NUM_DRINKS)
//            currDatabase.productLogSetup(temp, NUM_DRINKS, machineID);

//        setupOptions();
//        updateProductBox();
//        ui->statusBar->showMessage("Number of Drinks Updated");
//    }
}

void drinkfill_ev::on_clearProductDBButton_clicked()
{
//   currDatabase.resetProductLog(machineID);
//    updateProductBox();
//    setupTables();
//    ui->statusBar->showMessage("All Product Logs Has Been Erased");

    ui->tabWidget->setCurrentIndex(0);
}

