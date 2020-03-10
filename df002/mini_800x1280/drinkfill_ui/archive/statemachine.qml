import QtQuick 2.12
import QtQml.StateMachine 1.12

StateMachine {
    id: stateMachine
    // set the initial state
    initialState: s1

    // start the state machine
    running: true

    State {
        id: s1
        // create a transition from s1 to s2 when the button is clicked
        SignalTransition {
            targetState: s2
            signal: button.clicked
        }
        // do something when the state enters/exits
        onEntered: console.log("s1 entered")
        onExited: console.log("s1 exited")
    }

    State {
        id: s2
        // create a transition from s2 to s3 when the button is clicked
        SignalTransition {
            targetState: s3
            signal: button.clicked
        }
        // do something when the state enters/exits
        onEntered: console.log("s2 entered")
        onExited: console.log("s2 exited")
    }
    State {
        id: s3
        // create a transition from s3 to s1 when the button is clicked
        SignalTransition {
            targetState: s1
            signal: button.clicked
        }
        // do something when the state enters/exits
        onEntered: console.log("s3 entered")
        onExited: console.log("s3 exited")
    }
}
