#ifndef PAGE_TRANSACTIONS_H
#define PAGE_TRANSACTIONS_H

#include <QWidget>
#include "page_idle.h"

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
    void setPage(page_idle *pageIdle);

    QTimer *idleTimer;
    void onIdleTimeoutTick();
     void exitPage();
     void populateList();
    void populateTransactionsTable();
    void deleteAllListItems();

private slots:
    void on_back_Button_clicked();

    void on_print_Button_clicked(bool checked);

private:
   
    Ui::page_transactions *ui;
    void showEvent(QShowEvent *event);
    QString recent_transactions[10][5];

    page_idle *p_page_idle;

    int _idleTimeoutSec;
    int transaction_count;

    // QResizeEvent *helpResize;
};

#endif // PAGE_TRANSACTIONS_H
