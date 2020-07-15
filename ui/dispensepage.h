#ifndef DISPENSEPAGE_H
#define DISPENSEPAGE_H

#include <QWidget>

class payPage;
class thankYouPage;

namespace Ui {
class dispensePage;
}

class dispensePage : public QWidget
{
    Q_OBJECT

public:
    explicit dispensePage(QWidget *parent = nullptr);
    void setPage(payPage* pagePayment, thankYouPage* pageThankYou);
    ~dispensePage();

private slots:
    void on_finish_Button_clicked();

private:
    Ui::dispensePage *ui;

    payPage* paymentPage;
    thankYouPage* thanksPage;
};

#endif // DISPENSEPAGE_H
