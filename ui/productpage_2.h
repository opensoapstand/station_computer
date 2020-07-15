#ifndef PRODUCTPAGE_2_H
#define PRODUCTPAGE_2_H

#include <QWidget>

class productPage_1;
class paySelect;

namespace Ui {
class productPage_2;
}

class productPage_2 : public QWidget
{
    Q_OBJECT

public:
    explicit productPage_2(QWidget *parent = nullptr);
    void setPage(productPage_1 *pageOneProducts, paySelect *pageSizeSelect);
    ~productPage_2();

private slots:
    void on_previousPage_Button_clicked();

    void on_selection7_Button_clicked();

private:
    Ui::productPage_2 *ui;

    productPage_1* firstProductPage;
    paySelect* paySelectPage;
};

#endif // PRODUCTPAGE_2_H
