#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QStateMachine>
#include <QState>

class DrinkStateMachine : public QStateMachine
{
    Q_OBJECT

public:
    DrinkStateMachine(QStateMachine *parent = nullptr);

private slots:
    void doInit();
    void doIdle();
    void doSelected();
    void doPayment();
    void doDispense();
    void doDispensePaused();
    void doDispenseEnded();
    void doDispenseErr();
    void doUpdateInven();
    void doMaintenance();
    void doError();

private:
    DrinkStateMachine *fsm;

    QState *init = new QState();
    QState *idle = new QState();
    QState *selected = new QState();
    QState *payment = new QState();
    QState *dispense = new QState();
    QState *dispensePaused = new QState();
    QState *dispenseEnded = new QState();
    QState *dispenseErr = new QState();
    QState *updateInven = new QState();
    QState *maintenance = new QState();
    QState *error = new QState();
};

#endif // STATEMACHINE_H
