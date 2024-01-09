#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QWidget>
#include <QLineEdit>
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
    void registerCallBack(const std::function<void()>& func);
    std::function<void()> callbackFunction;
    // void* callback;
    void initializeKeyboard(bool isVisible, QLineEdit *widget);
private slots:
    void keyboardButtonPressed(int buttonID);
    void onRefreshTimerTick();

private:
    QLineEdit* widgetForTextEdit;
    bool is_keyboard_visible;
    bool hasStartedTyping = false;
    Ui::keyboard *ui;
    page_idle* p_page_idle;
    page_product_overview *p_page_overview;
    
    QTimer *refreshTimer;
    int _refreshTimerTimeoutSec;
};

#endif // KEYBOARD_H
