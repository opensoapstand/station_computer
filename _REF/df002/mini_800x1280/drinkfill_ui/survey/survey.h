#ifndef SURVEY_H
#define SURVEY_H

#include <QDialog>

namespace Ui {
class survey;
}

class survey : public QDialog
{
    Q_OBJECT

public:
    explicit survey(QWidget *parent = nullptr);
    ~survey();

private slots:

    void on_saveButton_clicked();

private:
    Ui::survey *ui;
    QString email;
    QString drinks;
};

#endif // SURVEY_H
