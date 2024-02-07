#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include "page_idle.h"
#include "page_productOverview.h"

class page_product_overview;

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
    void setPage(page_idle *pageIdle, page_product_overview *page_Overview);
    void showEvent(QShowEvent *event);
    void refresh();
    QString roleTimeOutTrailingText;
    // checking if its a QLineEdit or QTextEdit
    bool lineEdit;
    void registerCallBack(const std::function<void()>& func);
    std::function<void()> callbackFunction;
    void registerCancelCallBack(const std::function<void()>& func);
    std::function<void()> cancelCallbackFunction;
    void initializeKeyboard(bool isVisible, QLineEdit *widget);
    void initializeKeyboard(bool isVisible, QTextEdit *widget);
    // checking if the keyboard needs CAPS button
    void needCAPS(bool capsYorN);
    void setTimeoutSec(int* seconds, bool timeoutYorN);
    void adjustTimeoutSec(int seconds);
    void keyboardButtonDefaultAllInCAPS();
private slots:
    void keyboardButtonPressed(int buttonID);
    void onRefreshTimerTick();
private:
    int* timeoutSec;
    bool timeout = false;
    bool needCAPSbutton;
    QLineEdit* widgetForLineEdit;
    QTextEdit* widgetForTextEdit;
    bool is_keyboard_visible;
    bool hasStartedTyping = false;
    Ui::keyboard *ui;
    page_idle* p_page_idle;
    page_product_overview *p_page_overview;
    
    QTimer *refreshTimer;
    int _refreshTimerTimeoutSec;
};

#endif // KEYBOARD_H
