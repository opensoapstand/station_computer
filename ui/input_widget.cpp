#include "input_widget.h"
#include "ui_input_widget.h"
#include "page_idle.h"

input_widget::input_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::input_widget)
{
    ui->setupUi(this);
}

input_widget::~input_widget()
{
    delete ui;
}

void input_widget::setPage(page_idle *pageIdle)
{
    this->p_page_idle = pageIdle;
    qDebug() << "Input Widget set page. ";
}

void input_widget::showEvent(QShowEvent *event)
{
    qDebug() << "Input Widget show.";

    QWidget::showEvent(event);
    QString styleSheet = p_page_idle->thisMachine->getCSS(INPUT_WIDGET_CSS);
    ui->label_input_background->setStyleSheet(styleSheet);
    ui->label_input_info->setStyleSheet(styleSheet);
    ui->lineEdit_input->setStyleSheet(styleSheet);

    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_input_info);

    refresh();
}

void input_widget::toggleInputWidget(bool isVisible)
{
    is_input_wdiget_visible = isVisible;
    refresh();
}

void input_widget::refresh()
{
    if (is_input_wdiget_visible)
    {
        this->show();
    }
    else
    {
        this->hide();
    }
}
