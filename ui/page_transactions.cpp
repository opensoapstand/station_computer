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

        // set up back button
        QFont font;
        font.setFamily(QStringLiteral("Brevia"));
        font.setPointSize(20);
        font.setBold(true);
        font.setWeight(75);

        ui->back_Button->setStyleSheet("QPushButton { color:#FFFFFF; background-color: transparent; border: 0px }");
        ui->back_Button->setFont(font);
        ui->back_Button->setText("<- Back");

        // set up print button

        font.setFamily(QStringLiteral("Brevia"));
        font.setPointSize(20);
        font.setBold(true);
        font.setWeight(75);

        // ui->print_Button->setStyleSheet("QPushButton { color:#FFFFFF; background-color: transparent; border: 0px }");
        ui->print_Button->setStyleSheet("QPushButton { color:#FFFFFF;background-color: #5E8580; border: 1px solid #3D6675;box-sizing: border-box;border-radius: 20px;}");

        ui->print_Button->setFont(font);
        ui->print_Button->setText("Print Selected Receipt");

        // ui->transactions_List->setStyleSheet("QListWidget{  background:transparent; }QListWidget::item{background:green;}");
        ui->transactions_List->setStyleSheet("QListWidget{  background:transparent; }QListWidget::item{background-color: #5E8580;}");
}

void page_transactions::setPage(page_idle *pageIdle)
{
        this->p_page_idle = pageIdle;
}

page_transactions::~page_transactions()
{
        delete ui;
}

void page_transactions::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
        idleTimer->stop();
        p_page_idle->pageTransition(this, pageToShow);
}

void page_transactions::showEvent(QShowEvent *event)
{
        qDebug() << "<<<<<<< Page Enter: Transactions >>>>>>>>>";
        QWidget::showEvent(event);

        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TRANSACTIONS_BACKGROUND_PATH);

        if (idleTimer == nullptr)
        {
                idleTimer = new QTimer(this);
                idleTimer->setInterval(1000);
                connect(idleTimer, SIGNAL(timeout()), this, SLOT(onIdleTimeoutTick()));
        }

        idleTimer->start(1000);
        _idleTimeoutSec = 60;
        populateTransactionsTable();

        qDebug() << "db for receipt printer check";
        DbManager db(DB_PATH);
        QString paymentMethod = db.getPaymentMethod(p_page_idle->selectedProduct->getSlot());
        bool hasReceiptPrinter = db.hasReceiptPrinter();
        db.closeDB();

        if (hasReceiptPrinter)
        {
                ui->print_Button->show();
        }
        else
        {
                ui->print_Button->show();
                // ui->print_Button->hide();
        }
}

void page_transactions::onIdleTimeoutTick()
{
        if (--_idleTimeoutSec >= 0)
        {
                // qDebug() << "transactions  Tick Down: " << _idleTimeoutSec;
        }
        else
        {
                qDebug() << "transactions Timer Done!" << _idleTimeoutSec;
                hideCurrentPageAndShowProvided(p_page_idle);
        }
}

void page_transactions::populateTransactionsTable()
{
        DbManager db(DB_PATH);

        transaction_count = TRANSACTION_HISTORY_COUNT;
        int retrieved_count;
        db.getRecentTransactions(recent_transactions, transaction_count, &retrieved_count);

        transaction_count = retrieved_count;
        db.closeDB();

        populateList();
}

void page_transactions::deleteAllListItems()
{
        ui->transactions_List->clear();
}

void page_transactions::populateList()
{
        deleteAllListItems();

        // header is the first row. (caution: It messes up the indexing a little bit)
        ui->transactions_List->addItem("Dispense time     \t Volume [ml]     Price     Name");

        // populate the items of the list
        for (int i = 0; i < transaction_count; i++)
        {
                QString rowItem;
                for (int j = 1; j < 5; j++)
                {
                        rowItem += recent_transactions[i][j].rightJustified(8, ' ') + "\t";
                }

                // get rid of last tab
                int pos = rowItem.lastIndexOf(QChar('\t'));
                qDebug() << rowItem.left(pos);

                ui->transactions_List->addItem(rowItem);
        }
}

void page_transactions::on_back_Button_clicked()
{
        hideCurrentPageAndShowProvided(p_page_idle);
}

void page_transactions::on_print_Button_clicked(bool checked)
{

        // Get the pointer to the currently selected item.
        if (ui->transactions_List->selectedItems().size() != 0)
        {

                QListWidgetItem *item = ui->transactions_List->currentItem();

                // Set the text color and its background color using the pointer to the item.
                // item->setTextColor(Qt::white);
                item->setBackgroundColor(Qt::green);
                _idleTimeoutSec = 60;
                QModelIndex selectedRow = ui->transactions_List->selectionModel()->selectedIndexes()[0];
                int rowIndex = selectedRow.row();

                if (rowIndex != 0)
                {
                        // first row is header

                        QString transactionIndex = recent_transactions[rowIndex - 1][0];
                        qDebug() << "Selected row: " << rowIndex << " with dispense index: " + transactionIndex << ". Send to receipt printer.";

                        p_page_idle->dfUtility->send_command_to_FSM("p");
                        usleep(50000);
                        QString command = "t" + transactionIndex;
                        p_page_idle->dfUtility->send_command_to_FSM(command);
                        usleep(50000);
                        p_page_idle->dfUtility->send_command_to_FSM("q");
                }
        }
        else
        {
                qDebug() << "No transaction selected";
        }
}
