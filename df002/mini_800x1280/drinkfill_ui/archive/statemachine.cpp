#include "statemachine.h"
#include <iostream>

using namespace std;

DrinkStateMachine::DrinkStateMachine(QStateMachine *parent) :
    QStateMachine(parent) {

    fsm = new DrinkStateMachine();

    fsm->addState(init);
    fsm->addState(idle);
    fsm->addState(selected);
    fsm->addState(payment);
    fsm->addState(dispense);
    fsm->addState(dispensePaused);
    fsm->addState(dispenseEnded);
    fsm->addState(dispenseErr);
    fsm->addState(updateInven);
    fsm->addState(maintenance);
    fsm->addState(error);

    fsm->setInitialState(idle);
    fsm->start();

};

// Initialize application
void DrinkStateMachine::doInit() {
    cout << "Initialization.\n";
    // Check internet connection
    // Check inventory status
    // Check payment machine
    // Check controllers

    // Placeholder for function results
    bool startupSuccess = true;

    // Go to next state depending on startup status

    if (startupSuccess) {
        cout << "Initialization successful. Enter idle state.\n\n";
        fsm->doIdle();
    } else {
        cout << "Initialization failed. Enter error state.\n";
        fsm->doError();
    }
}

// Idle (no user interaction - idle on start screen)
void DrinkStateMachine::doIdle() {
    cout << "Idling...\n\n";
    // Wait for user selection

    // Once user has selected, go to selected state
    cout << "User has selected a product. Enter selected state.\n";
    fsm->doSelected();
}

// User has selected product
// If payment system exists, take payment
// Otherwise go straight to dispense
void DrinkStateMachine::doSelected() {
    // Grab product information from database and display
    cout << "Display product information here...\n";
    cout << "Would you like to continue? (Y/N) ";

    // Placeholder for user confirmation and payment system info
    bool takePayment = true;
    bool confirm = true;

    // If there is payment system, go to payment
    if (takePayment && confirm) {
        cout << "Go to payment.\n";
        fsm->doPayment();
    } else if (!takePayment && confirm) {
        // If there is no payment system, go to dispense state
        cout << "Go to dispense.\n";
        fsm->doDispense();
    }

    // If user cancels or timeout, go back to idle state
    bool cancel = false;
    bool timeout = false;
    if (cancel || timeout) {
        fsm->doIdle();
    }
}

// Take payment from customer
// On success, go to dispense
// On failure, retry payment for a max. of two times
// Once tries are maxed, return to idle
void DrinkStateMachine::doPayment() {
    cout << "Payment\n";

    // Write to payment device
    // Wait for response
    // Placeholder for payment status
    bool paymentSuccess = false;

    // Go to next state depending on payment status

    if (paymentSuccess) {
        fsm->doDispense();
    } else {
        fsm->doError();
    }
}

// Dispense product
// Check if coming back from mid-transaction or new transaction
void DrinkStateMachine::doDispense() {
    // Read physical button input and dispense
    // Place holder for button click input
    bool clicked = false;
    bool dispensing = false;

    if (clicked && !dispensing) {
        // Write to dispense selected product
        // On dispense complete, go to dispenseEnded state
        fsm->doDispenseEnded();
    } else if (!clicked && dispensing) {
        // If user releases button while dispensing, go to dispensePaused state
        fsm->doDispensePaused();
    }
}

// Dispensing paused
// User can resume dispense
// Or transaction will end after a set timeout
void DrinkStateMachine::doDispensePaused() {
    // Read button input
    // If button input, go back to dispense
    // Else stay in this state
    bool clicked = false;

    if (clicked) {
        fsm->doDispense();
    } else {
        fsm->doDispensePaused();
    }

    // On timeout, end transaction
    bool timeout = false;
    if (timeout) {
        fsm->doDispenseEnded();
    }
}

// Log dispense error and end transaction
void DrinkStateMachine::doDispenseErr() {
    // Save error to logs

    // End transaction completely;
    return fsm->doDispenseEnded();
}

// End transaction and turn off all controllers/parts that are still on
void DrinkStateMachine::doDispenseEnded() {
    // Turn off any running dispense parts
    // Update inventory
    fsm->doUpdateInven();
}

// Update inventory after transaction
// On success, go to idle state
// On failure, go to error state
void DrinkStateMachine::doUpdateInven() {
    cout << "Updating inventory...";
    // Update inventory here

    // Placeholder for function results
    bool updateSuccess = true;

    // Go to next state depending on update status
    if (updateSuccess) {
        fsm->doIdle();
    } else {
        fsm->doError();
    }
}

// Scheduled routine maintenance of the machine
// On success, go to idle state
// On failure, go to error state
void DrinkStateMachine::doMaintenance() {
    cout << "Scheduled maintenance.\n";
    // Check internet connection
    // Check inventory status
    // Check payment machine
    // Check controllers

    // Placeholder for function results
    bool maintainSuccess = true;

    // Go to next state depending on status

    if (maintainSuccess) {
        fsm->doIdle();
    } else {
        fsm->doError();
    }
}

// Application has entered error state
// Handle error and restart
void DrinkStateMachine::doError() {
    // Save error to logs
    cout << "Error has occurred.\n";
    // ENDING STATE
    // Display error on screen
}
