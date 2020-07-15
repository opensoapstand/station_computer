#ifndef PAYSELECT_H
#define PAYSELECT_H

#include <QWidget>

class productPage_1;
class payPage;
class idle;

namespace Ui {
class paySelect;
}

class paySelect : public QWidget
{
    Q_OBJECT

public:
    explicit paySelect(QWidget *parent = nullptr);
    void setPage(productPage_1 *pageSelect, payPage *pagePayment, idle* pageIdle);
    ~paySelect();

private slots:
    void on_previousPage_Button_clicked();

    void on_payPage_Button_clicked();

    void on_mainPage_Button_clicked();

private:
    Ui::paySelect *ui;

    productPage_1* firstProductPage;
    payPage* paymentPage;
    idle* idlePage;
};

#endif // PAYSELECT_H
