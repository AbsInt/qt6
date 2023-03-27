import QtQuick
import QtQuick.Controls

Item {
    width: 300
    height: 200

    Dialog {
        anchors.centerIn: parent
        height: 100
        width: 100
        visible: true
        enabled: true
        modal: true
        header: Text {
            text: qsTr("dialog 1")
        }
    }
}
