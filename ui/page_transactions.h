#ifndef PAGE_TRANSACTIONS_H
#define PAGE_TRANSACTIONS_H

#include <QWidget>

namespace Ui {
class page_transactions;
}

class page_transactions : public QWidget
{
    Q_OBJECT

public:
    explicit page_transactions(QWidget *parent = 0);
    ~page_transactions();

private:
    Ui::page_transactions *ui;
};

#endif // PAGE_TRANSACTIONS_H
