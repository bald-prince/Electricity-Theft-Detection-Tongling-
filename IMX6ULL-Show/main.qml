import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.VirtualKeyboard 2.2

ApplicationWindow {
    visible: true
    width: 600
    height: 400
    title: "Virtual Keyboard Example"

    Rectangle {
        width: 400
        height: 200

        MouseArea {
            anchors.fill: parent
            onClicked: {
                inputField.forceActiveFocus()
                virtualKeyboard.visible = true
            }

            TextInput {
                id: inputField
                anchors.centerIn: parent
                width: parent.width - 20
                height: 30
                //placeholderText: "Click to open keyboard"
            }
        }
    }

    InputPanel {
        id: virtualKeyboard
        visible: false
        width: 874
        height: 480
    }
}
