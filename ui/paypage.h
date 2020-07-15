#ifndef PAYPAGE_H
#define PAYPAGE_H

#include <QWidget>

class paySelect;
class dispensePage;
class idle;

namespace Ui {
class payPage;
}

class payPage : public QWidget
{
    Q_OBJECT

public:
    explicit payPage(QWidget *parent = nullptr);
    void setPage(paySelect* pageSizeSelect, dispensePage* pageDispense, idle* pageIdle);
    ~payPage();

private slots:
    void on_previousPage_Button_clicked();

    void on_passPayment_Button_clicked();

    void on_mainPage_Button_clicked();

private:
    Ui::payPage *ui;

    paySelect* paySelectPage;
    dispensePage* dispensingPage;
    idle* idlePage;

};

#endif // PAYPAGE_H
