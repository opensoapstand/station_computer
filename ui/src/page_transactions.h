#ifndef PAGE_TRANSACTIONS_H
#define PAGE_TRANSACTIONS_H

#include <QWidget>
#include "page_idle.h"

class statusbar;

namespace Ui
{
    class page_transactions;
}

class page_transactions : public QWidget
{
    Q_OBJECT

public:
    explicit page_transactions(QWidget *parent = 0);
    ~page_transactions();
    void setPage(page_idle *pageIdle, statusbar *p_statusbar);

    QTimer *idleTimer;
     void populateList();
    void populateTransactionsTable();
    void deleteAllListItems();
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);

private slots:
    void on_pushButton_back_clicked();
    void onIdleTimeoutTick();

    void on_pushButton_print_clicked(bool checked);

private:
   
    Ui::page_transactions *ui;
    void showEvent(QShowEvent *event);
    QString recent_transactions[TRANSACTION_HISTORY_COUNT][5];

    page_idle *p_page_idle;
    statusbar *p_statusbar;

    int _idleTimeoutSec;
    int transaction_count;
    QVBoxLayout *statusbarLayout;

    // QResizeEvent *helpResize;
};

#endif // PAGE_TRANSACTIONS_H
