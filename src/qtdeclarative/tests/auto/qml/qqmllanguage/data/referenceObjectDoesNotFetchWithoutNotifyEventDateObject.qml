import Test
import QtQml

ReadCounter {
    Component.onCompleted: {
        let reference;

        reference = dateTime, reference.getSeconds(), reference.getMinutes(), reference.getHours();
    }
}
