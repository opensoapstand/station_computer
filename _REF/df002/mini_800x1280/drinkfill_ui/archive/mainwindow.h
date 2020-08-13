#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

#include <QToolButton>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
	 static void * listenToPackets(void * pthreadArgs);
    QToolButton * selectedQuantity;

    QPushButton * temp;
    QPushButton * temp2;
    QPushButton * temp3;
    bool quantitySelected = false;



    QPixmap carbonated;
    QPixmap coldBrew;
    QPixmap kombucha;

    ~MainWindow();

private slots:
    void selectDrink();
    void back();
    void confirm();
    void dispense();
    void endDispense();
    void error();
    void updatePBar(int value);
    void backToHome();
    void toThankYou();
    void toogleBackground();
    void dispenseProduct();
    void pageDecision();
    void backToSamePage();
	 void back_2();
	 void updateProgress();
signals:
    void readyRead();
    void updateProgressBar(int value);
public :
    bool isPaused;


private:
    Ui::MainWindow *ui;



};

#endif // MAINWINDOW_H
