#ifndef IDLE_H
#define IDLE_H

#include <QWidget>
#include "productpage_1.h"

namespace Ui {
class idle;
}

class idle : public QWidget
{
    Q_OBJECT

public:
    explicit idle(QWidget *parent = nullptr);
    void setPage(productPage_1 *pageProduct);
    ~idle();

private slots:
    void on_nextPageButton_clicked();

private:
    Ui::idle *ui;
    productPage_1* selection_PageOne;
};

#endif // IDLE_H
