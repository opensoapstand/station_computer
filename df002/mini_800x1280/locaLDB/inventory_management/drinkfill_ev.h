#ifndef DRINKFILL_EV_H
#define DRINKFILL_EV_H

#include <QMainWindow>
#include <QStringListModel>

#include <QListView>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDateTime>
#include <QDebug>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QFileDialog>
#include <QSortFilterProxyModel>
#include <QItemDelegate>
#include <QSqlError>
#include <QStandardItem>
#include <QList>

#include "database_logger.h"

#define NUM_DRINKS 2
#define machineID "mini-3"

namespace Ui {
class drinkfill_ev;
}

class drinkfill_ev : public QMainWindow
{
    Q_OBJECT

public:
    explicit drinkfill_ev(QWidget *parent = nullptr);
    ~drinkfill_ev();

private slots:
    void on_volumeUpdateButton_clicked();

    void on_activityButton_clicked();

    void on_transactionButton_clicked();

    void on_newImageButton_clicked();

    void clearInputs();

    void on_machineIdButton_clicked();

    void on_numDrinksButton_clicked();

    void on_clearProductDBButton_clicked();

private:
    Ui::drinkfill_ev *ui;
    database_logger currDatabase;

    QStringListModel *inventoryModel;
    QStringListModel *optionModel;
    QStringListModel *tagModel;
    QStringListModel *machineIdModel;

    QString fileName;
    QByteArray readByteArray;

//    int NUM_DRINKS;
    int NUM_MACHINE;
//    QString machineID;

    //QSqlQueryModel *productModel = new QSqlQueryModel(this);
   // QSqlTableModel *productModel = new QSqlTableModel(this);
    QStandardItemModel *productModel = new QStandardItemModel(this);

    void setupTables();
    void updateProductBox();
    void setupOptions();
    void setupMachineID();
};

#endif // DRINKFILL_EV_H
