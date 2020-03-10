import QtQuick 2.0
import QtQuick.VirtualKeyboard 2.2
import QtQuick.VirtualKeyboard.Styles 2.13

QtObject{
    property Component keyboardBackground: null
    keyboardBackground: Rectangle {
        color: "red"
    }

}
