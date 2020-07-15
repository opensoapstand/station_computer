#ifndef PRODUCTPAGE_1_H
#define PRODUCTPAGE_1_H

#include <QWidget>

class productPage_2;
class paySelect;

namespace Ui {
class productPage_1;
}

class productPage_1 : public QWidget
{
    Q_OBJECT

public:
    explicit productPage_1(QWidget *parent = nullptr);
    void setPage(productPage_2 *pageTwoProducts, paySelect *pageSizeSelect);
    ~productPage_1();

private slots:
    void on_nextPageButton_clicked();
    void on_selection1_Button_clicked();

private:
    Ui::productPage_1 *ui;
    productPage_2 *selection_PageTwo;
    paySelect *paymentSelectPage;
};

#endif // PRODUCTPAGE_1_H
