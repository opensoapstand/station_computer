#ifndef PAGE_BUYBOTTLE_H
#define PAGE_BUYBOTTLE_H

#include <QWidget>
#include "df_util.h"
#include "page_select_product.h"
#include "statusbar.h"
#include "page_help.h"

class statusbar;
class page_select_product;
class page_idle;
class page_help;

namespace Ui {
class page_buyBottle;
}

class page_buyBottle : public QWidget
{
    Q_OBJECT

public:
    explicit page_buyBottle(QWidget *parent = 0);
    ~page_buyBottle();
    void setPage(page_idle *pageIdle, page_select_product *p_page_select_product, page_help *pageHelp, statusbar *p_statusbar);
    
    QTimer *userRoleTimeOutTimer;
    int _userRoleTimeOutTimerSec;
    bool areBothBottleButtonsActivated();
    bool isBottleButtonActivated(int bottlePNum);
private slots:
    void onUserRoleTimeOutTimerTick();
    void onProductPageTimeoutTick();
    void on_pushButton_to_idle_clicked();
    void on_pushButton_help_page_clicked();
    void on_pushButton_no_clicked();
private:
    void showEvent(QShowEvent *event);
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    Ui::page_buyBottle *ui;
    page_idle *p_page_idle;
    page_select_product *p_page_select_product;
    page_help* p_page_help;
    statusbar *p_statusbar;

    int _productPageTimeoutSec;
    QTimer* productPageEndTimer;

    QVBoxLayout *statusbarLayout; 
};

#endif // PAGE_BUYBOTTLE_H
