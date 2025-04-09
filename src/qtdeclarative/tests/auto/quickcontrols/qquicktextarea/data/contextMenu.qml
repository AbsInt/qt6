import QtQuick.Controls

Pane {
    width: 400
    height: 400

    property alias textArea: textArea

    TextArea {
        id: textArea
        text: "Memento mori"
        width: parent.width
        anchors.centerIn: parent
    }
}
