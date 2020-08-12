#include <iostream>
#include <string>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

// Define states
enum class State {
    init,
    idle,
    selected,
    payment,
    dispense,
    dispensePaused,
    dispenseErr,
    dispenseEnded,
    updateInven,
    maintenance,
    error
};

// Initialize application
State init() {
    cout << "Initialization.\n";
    // Check internet connection
    // Check inventory status
    // Check payment machine
    // Check controllers

    // Placeholder for function results
    bool startupSuccess = true;

    // Go to next state depending on startup status
        
        // Default state
        State ret = State::error;
    
    if (startupSuccess) {
        cout << "Initialization successful. Enter idle state.\n\n";
        ret = State::idle;
    } else {
        cout << "Initialization failed. Enter error state.\n";
    }

    return ret;
}

// Idle (no user interaction - idle on start screen)
State idle() {
    cout << "Idling...\n\n";
    // Wait for user selection

    // Once user has selected, go to selected state
    cout << "User has selected a product. Enter selected state.\n";

    return State::selected;
}

// User has selected product
// If payment system exists, take payment
// Otherwise go straight to dispense
State selected() {
    // Grab product information from database and display
    cout << "Display product information here...\n";
    cout << "Would you like to continue? (Y/N) ";  

    // Placeholder for user confirmation and payment system info
    bool takePayment = true;
    bool confirm = true;

        // Default state
        State ret = State::error;
    
    // If there is payment system, go to payment
    if (takePayment && confirm) {
        ret = State::payment;
    } else if (!takePayment && confirm) {
        // If there is no payment system, go to dispense state
        ret = State::dispense;
    }

    // If user cancels or timeout, go back to idle state
    bool cancel = false;
    bool timeout = false;
    if (cancel || timeout) {
        ret = State::idle;
    }

    return ret;
}

// Take payment from customer
// On success, go to dispense
// On failure, retry payment for a max. of two times
// Once tries are maxed, return to idle
State payment() {
    cout << "Payment\n";

    // Write to payment device
    // Wait for response
    // Placeholder for payment status
    bool paymentSuccess = false;

    // Go to next state depending on payment status
    
        // Default state
        State ret = State::error;
    
    if (paymentSuccess) {
        ret = State::dispense;
    }

    return ret;
}

// Dispense product
// Check if coming back from mid-transaction or new transaction
State dispense() {
    // Read physical button input and dispense
    // Place holder for button click input
    bool clicked = false;
    bool dispensing = false;

        // Default state
        State ret = State::error;
    
    if (clicked && !dispensing) {
        // Write to dispense selected product
        // On dispense complete, go to dispenseEnded state
        ret = State::dispenseEnded;
    } else if (!clicked && dispensing) {
        // If user releases button while dispensing, go to dispensePaused state
        ret = State::dispensePaused;
    }

    return ret;
}

// Dispensing paused
// User can resume dispense
// Or transaction will end after a set timeout
State dispensePaused() {
    // Read button input
    // If button input, go back to dispense
    // Else stay in this state
    bool clicked = false;

        // Default state
        State ret = State::error;
   
    if (clicked) {
        ret = State::dispense;
    } else {
        ret = State::dispensePaused;
    }

    // On timeout, end transaction
    bool timeout = false;
    if (timeout) {
        ret = State::dispenseEnded;
    }

    return ret;
}

// Log dispense error and end transaction
State dispenseErr() {
    // Save error to logs

    // End transaction completely;
    return State::dispenseEnded;
}

// End transaction and turn off all controllers/parts that are still on
State dispenseEnded() {
    // Turn off any running dispense parts
    // Update inventory 
    return State::updateInven;
}

// Update inventory after transaction
// On success, go to idle state
// On failure, go to error state
State updateInven() {
    cout << "Updating inventory...";
    // Update inventory here

    // Placeholder for function results
    bool updateSuccess = true;

    // Go to next state depending on update status
        
        // Default state
        State ret = State::error;

    if (updateSuccess) {
        ret = State::idle;
    }

    return ret;
}

// Scheduled routine maintenance of the machine
// On success, go to idle state
// On failure, go to error state
State maintenance() {
    cout << "Scheduled maintenance.\n";
    // Check internet connection
    // Check inventory status
    // Check payment machine
    // Check controllers

    // Placeholder for function results
    bool maintainSuccess = true;

    // Go to next state depending on status
        
        // Default state
        State ret = State::error;
    
    if (maintainSuccess) {
        return State::idle;
    }
    
    return ret;
}

// Application has entered error state
// Handle error and restart
State error() {
    // Save error to logs
    cout << "Error has occurred.\n";
    // ENDING STATE
    // Display error on screen
    return State::error;
}

// Run FSM on start up
int main() {

    // Initial starting state
    State s = State::init;

    // Run FSM
    int run = 1;
    while (run) {
        // FSM state switches
        switch (s) {
        case State::init:
            s = init();
            break;
        case State::idle:
            s = idle();
            break;
        case State::selected:
            s = selected();
            break;
        case State::payment:
            s = payment();
            break;
        case State::dispense:
            s = dispense();
            break;
        case State::dispensePaused:
            s = dispensePaused();
            break;
        case State::dispenseErr:
            s = dispenseErr();
            break;
        case State::dispenseEnded:
            s = dispenseEnded();
            break;
        case State::updateInven:
            s = updateInven();
            break;
        case State::maintenance:
            s = maintenance();
            break;
        case State::error:
        default:
            s = error();
            cout << "Exit FSM.\n";
            run = 0;
            break;
        }

        // Break in between states
        sleep_for(milliseconds(100));

    }
}
