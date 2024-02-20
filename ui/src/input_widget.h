#ifndef INPUT_WIDGET_H
#define INPUT_WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include "page_idle.h"

class page_idle;

namespace Ui {
class input_widget;
}

class input_widget : public QWidget
{
    Q_OBJECT

public:
    explicit input_widget(QWidget *parent = 0);
    ~input_widget();
    void setPage(page_idle *pageIdle);
    void showEvent(QShowEvent *event);
    void toggleInputWidget(bool isVisible);
    void refresh();
private:
    Ui::input_widget *ui;
    page_idle* p_page_idle;
    bool is_input_wdiget_visible;

};

#endif // INPUT_WIDGET_H
