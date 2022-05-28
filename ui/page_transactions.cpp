#include "page_transactions.h"
#include "ui_page_transactions.h"

page_transactions::page_transactions(QWidget *parent) : QWidget(parent),
                                                        ui(new Ui::page_transactions)
{
    ui->setupUi(this);
    idleTimer = new QTimer(this);
    idleTimer->setInterval(1000);
    connect(idleTimer, SIGNAL(timeout()), this, SLOT(onIdleTimeoutTick()));
    transaction_count = TRANSACTION_HISTORY_COUNT;

// ui->transactions_List->setStyleSheet("QListWidget{  background:transparent; }QListWidget::item{background:green;}");
ui->transactions_List->setStyleSheet("QListWidget{  background:transparent; }QListWidget::item{background-color: #5E8580;}");

// ui->transactions_List->setStyleSheet(
//     "QlistWidget::item:selected {
//     background:
//     transparent; }
//     QListWidget::item:selected {
//     border:
//         1px solid red; }"
// )

// It paints the border, but renders the text foreground color black. If I change it to:
// self.listWidgetRepo.setStyleSheet("""
// QListWidget::item:selected {
//     foreground:
//         green;
//         QListWidget::item : selected
//         {
//         background:
//             transparent;
//         }
//         QListWidget::item : selected
//         {
//         border:
//             1px solid red;
//         }
//         ""
//         ")
}

void page_transactions::setPage(page_idle *pageIdle)
{
        this->p_page_idle = pageIdle;
}

page_transactions::~page_transactions()
{
        delete ui;
}

void page_transactions::showEvent(QShowEvent *event)
{
        qDebug() << "<<<<<<< Page Enter: Transactions >>>>>>>>>";
        QWidget::showEvent(event);

        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TRANSACTIONS_BACKGROUND_PATH);

        idleTimer->start(1000);
        _idleTimeoutSec = 60;
        // ui->refreshLabel->hide();
        populateTransactionsTable();
}

void page_transactions::onIdleTimeoutTick()
{
        if (--_idleTimeoutSec >= 0)
        {
            qDebug() << "transactions  Tick Down: " << _idleTimeoutSec;
        }
        else
        {
            qDebug() << "transactions Timer Done!" << _idleTimeoutSec;

            exitPage();
        }
}

void page_transactions::populateTransactionsTable()
{
        DbManager db(DB_PATH);

        db.getRecentTransactions(recent_transactions, transaction_count);
        // for (uint8_t i = 0; i < count; i++)
        // {
        //     qDebug() << recent_transactions[i][0];
        // }

        //     if (checked != db.getPumpRampingEnabled())
        //     {
        //         qDebug() << "Write to db: Pump ramping enabled?" << checked;
        //         db.setPumpRampingEnabled(checked);
        //         ui->enable_pump_ramping_checkBox->setChecked(db.getPumpRampingEnabled());
        //     }
        db.closeDB();

        populateList();

        // table
        // ui->transactions_Table->setRowCount(10);
        // ui->transactions_Table->setColumnCount(4);

        // QTableWidgetItem *itemTitle0 = new QTableWidgetItem(tr("%1").arg("Dispense Time"));
        // QTableWidgetItem *itemTitle1 = new QTableWidgetItem(tr("%1").arg("Name"));
        // QTableWidgetItem *itemTitle2 = new QTableWidgetItem(tr("%1").arg("Volume [ml]"));
        // QTableWidgetItem *itemTitle3 = new QTableWidgetItem(tr("%1").arg("Price"));
        // ui->transactions_Table->setItem(0, 0, itemTitle0);
        // ui->transactions_Table->setItem(0, 1, itemTitle1);
        // ui->transactions_Table->setItem(0, 2, itemTitle2);
        // ui->transactions_Table->setItem(0, 3, itemTitle3);

        // // populate the items of the list
        // for (int i = 1; i < (count+1); i++)
        // {
        //     for (int j = 0; j < 4; j++)
        //     {
        //         QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(recent_transactions[i][j]));

        //         ui->transactions_Table->setItem(i, j, newItem);
        //     }
        // }

        // QHeaderView* header =  ui->transactions_Table->horizontalHeader();
        // header->setSectionResizeMode(QtWidgets.QHeaderView.ResizeToContents);

        // list
}

void page_transactions::deleteAllListItems()
{
        // for (uint16_t i = 0; i < ui->transactions_List->count(); i++)
        // {
        //     QListWidgetItem *it = ui->transactions_List->takeItem(ui->transactions_List->currentRow());
        //     delete it;
        // }
        ui->transactions_List->clear();
}

void page_transactions::populateList()
{
        deleteAllListItems();

        // DO NOT ADD HEADER IN TABLE. It can be selected and causes a crash.
        // ui->transactions_List->addItem("Dispense time     \t Volume [ml]      \t Price \t Name");
        // populate the items of the list
        for (int i = 0; i < transaction_count; i++)
        {
            QString rowItem;
            // paddedString = originalString.leftJustified(4, ' ');
            for (int j = 1; j < 5; j++)
            {
                rowItem += recent_transactions[i][j].rightJustified(8, ' ') + "\t";
            }

            // get rid of last tab
            int pos = rowItem.lastIndexOf(QChar('\t'));
            qDebug() << rowItem.left(pos);

            ui->transactions_List->addItem(rowItem);
            // ui->transactions_List->addItem( recent_transactions[i][0] + "\t:\t" +  recent_transactions[i][2] + "\t" + recent_transactions[i][3] + "\t" + recent_transactions[i][1]);
        }
}
void page_transactions::exitPage()
{
        idleTimer->stop();
        p_page_idle->showFullScreen();
        this->hide();
}
void page_transactions::on_back_Button_clicked()
{
        exitPage();
}

void page_transactions::on_print_Button_clicked(bool checked)
{
        // Get the pointer to the currently selected item.
        QListWidgetItem *item = ui->transactions_List->currentItem();

        // Set the text color and its background color using the pointer to the item.
        // item->setTextColor(Qt::white);
        item->setBackgroundColor(Qt::green);
        _idleTimeoutSec = 60;
        QModelIndex selectedRow = ui->transactions_List->selectionModel()->selectedIndexes()[0];
        int rowIndex = selectedRow.row();
        qDebug() << selectedRow.row();
        // rowIndex = (selectedRow.row()).toInt();

        QString transactionIndex = recent_transactions[rowIndex][0];
        qDebug() << "selected row: " << rowIndex << " has dispense index: " + transactionIndex;
}
