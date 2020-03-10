#ifndef SURVEY_H
#define SURVEY_H

#include <QDialog>
#include "database_logger.h"

class MainWindow;
class payment;

namespace Ui {
class survey;
}

class survey : public QDialog
{
    Q_OBJECT

public:
    explicit survey(QWidget *parent = nullptr);
    void setPages(MainWindow* main);
    bool getSurveyFilled();
    void resetSurveyFilled();
    void storeToDatabase(database_logger logger,QString drink,QString email,QString stat);
    ~survey();

    bool checkEmailD(database_logger logger, QString email);

private slots:

    void on_saveButton_clicked();

    void on_goBackButton_clicked();

    void on_ozRB_12_clicked();

    void on_ozRB_16_clicked();

    void on_ozRB_24_clicked();

    void on_ozRB_NA_clicked();

    void on_ozRB_DK_clicked();

    void on_hiddenButton_clicked();

private:
    Ui::survey *ui;
    QString email;
    QString drinks;
    bool surveyFilled = false;
    QString radioButtonSelect;

    payment* payPage;
    MainWindow * mainPage;

protected:
    void showEvent(QShowEvent *event);
};

#endif // SURVEY_H
