#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QMainWindow>
#include <QWidget>

class testWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit testWindow(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // TESTWINDOW_H