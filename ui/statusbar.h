#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QWidget>
#include "page_idle.h"

namespace Ui
{
    class statusbar;
}

class statusbar : public QWidget
{
    Q_OBJECT

public:
    explicit statusbar(QWidget *parent = 0);
    ~statusbar();
    void setPage(page_idle *pageIdle);
    void showEvent(QShowEvent *event);
    void exit_page();
    // void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    // void hide();
    void refresh();
    void setVisibility(bool isVisible);


private slots:
    void on_pushButton_hide_clicked();
    void onRefreshTimerTick();

private:
    bool is_statusbar_visible;
    Ui::statusbar *ui;
    page_idle* p_page_idle;
    
    QTimer *refreshTimer;
    int _refreshTimerTimeoutSec;
};

#endif // STATUSBAR_H
