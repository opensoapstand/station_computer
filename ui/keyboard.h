#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QWidget>
#include "page_idle.h"

namespace Ui
{
    class keyboard;
}

class keyboard : public QWidget
{
    Q_OBJECT

public:
    explicit keyboard(QWidget *parent = 0);
    ~keyboard();
    void setPage(page_idle *pageIdle);
    void showEvent(QShowEvent *event);
    // void exit_page();
    // void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    // void hide();
    void refresh();
    // void setRoleTimeOutTrailingText(QString text);
    QString roleTimeOutTrailingText;
    
    void autoSetVisibility();
    void setVisibility(bool isVisible, QWidget *widget);
    QWidget target;
private slots:
    void keyboardButtonPressed(int buttonID);
    void on_pushButton_hide_clicked();
    void onRefreshTimerTick();

    // void on_pushButton_active_role_clicked();

private:
    bool is_keyboard_visible;
    Ui::keyboard *ui;
    page_idle* p_page_idle;
    
    QTimer *refreshTimer;
    int _refreshTimerTimeoutSec;
};

#endif // KEYBOARD_H
