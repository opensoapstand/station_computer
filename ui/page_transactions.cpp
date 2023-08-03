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

        // // set up back button
        // QFont font;
        // font.setFamily(QStringLiteral("Brevia"));
        // font.setPointSize(20);
        // font.setBold(true);
        // font.setWeight(75);

        // ui->pushButton_back->setFont(font);

        // // set up print button

        // font.setFamily(QStringLiteral("Brevia"));
        // font.setPointSize(20);
        // font.setBold(true);
        // font.setWeight(75);
        // ui->pushButton_print->setFont(font);
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
        p_page_idle->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
        QWidget::showEvent(event);

        QString styleSheet = p_page_idle->getCSS(PAGE_TRANSACTIONS_CSS);
        ui->pushButton_back->setStyleSheet(styleSheet);
        ui->pushButton_print->setStyleSheet(styleSheet);
        ui->label_title->setStyleSheet(styleSheet);
        ui->list_transactions->setStyleSheet(styleSheet);

        p_page_idle->setTemplateTextToObject(ui->label_title);
        p_page_idle->setTemplateTextToObject(ui->pushButton_back);
        p_page_idle->setTemplateTextToObject(ui->pushButton_print);

        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TRANSACTIONS_BACKGROUND_PATH);

        idleTimer->start(1000);
        _idleTimeoutSec = 60;
        populateTransactionsTable();

        if (p_page_idle->thisMachine.hasReceiptPrinter())
        {
                ui->pushButton_print->show();
        }
        else
        {
                ui->pushButton_print->hide();
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
        transaction_count = TRANSACTION_HISTORY_COUNT;
        int retrieved_count;
        p_page_idle->g_database->getRecentTransactions(recent_transactions, transaction_count, &retrieved_count);
        transaction_count = retrieved_count;
        populateList();
}

void page_transactions::deleteAllListItems()
{
        ui->list_transactions->clear();
}

void page_transactions::populateList()
{
        deleteAllListItems();

        // header is the first row. (caution: It messes up the indexing a little bit)
        ui->list_transactions->addItem("Dispense time     \t Volume [ml]     Price[$]\t  Name");

        // populate the items of the list
        for (int i = 0; i < transaction_count; i++)
        {
                QString name;
                QString name_ui;
                QString product_type;
                QString description_ui;
                QString features_ui;
                QString ingredients_ui;
                QString rowItem;
                p_page_idle->thisMachine.loadProductPropertiesFromProductsFile(recent_transactions[i][4],
                                                                               &name,
                                                                               &name_ui,
                                                                               &product_type,
                                                                               &description_ui,
                                                                               &features_ui,
                                                                               &ingredients_ui);
                for (int j = 1; j < 4; j++)
                {
                        rowItem += recent_transactions[i][j].rightJustified(8, ' ') + "\t";
                }
                rowItem += name.rightJustified(8, ' ') + "\t";

                // get rid of last tab
                int pos = rowItem.lastIndexOf(QChar('\t'));
                // qDebug() << rowItem.left(pos);

                ui->list_transactions->addItem(rowItem);
        }
}

void page_transactions::on_pushButton_back_clicked()
{
        hideCurrentPageAndShowProvided(p_page_idle);
}

void page_transactions::on_pushButton_print_clicked(bool checked)
{

        // Get the pointer to the currently selected item.
        if (ui->list_transactions->selectedItems().size() != 0)
        {

                QListWidgetItem *item = ui->list_transactions->currentItem();

                // Set the text color and its background color using the pointer to the item.
                // item->setTextColor(Qt::white);
                item->setBackgroundColor(Qt::green);
                _idleTimeoutSec = 60;
                QModelIndex selectedRow = ui->list_transactions->selectionModel()->selectedIndexes()[0];
                int rowIndex = selectedRow.row();

                if (rowIndex != 0)
                {
                        // first row is header

                        QString transactionIndex = recent_transactions[rowIndex - 1][0];
                        qDebug() << "Selected row: " << rowIndex << " with dispense index: " + transactionIndex << ". Send to receipt printer.";

                        p_page_idle->thisMachine.dfUtility->send_command_to_FSM("Printer");
                        usleep(50000);
                        QString command = "t" + transactionIndex;
                        p_page_idle->thisMachine.dfUtility->send_command_to_FSM(command);
                        usleep(50000);
                        p_page_idle->thisMachine.dfUtility->send_command_to_FSM("q");
                }
        }
        else
        {
                qDebug() << "No transaction selected";
        }
}
