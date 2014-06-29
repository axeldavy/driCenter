import QtQuick 2.0

Item {
    id: delegate

    width: delegate.ListView.view.width; height: delegate.ListView.view.height / delegate.ListView.view.count

    Text {
        text: name
        color: delegate.ListView.isCurrentItem ? "white" : "black"
        font { family: "Arial"; pixelSize: 16; bold: true }
        anchors {
            left: parent.left; leftMargin: 15
            verticalCenter: parent.verticalCenter
        }
    }

    Rectangle {
        width: delegate.width; height: 1; color: "#cccccc"
        anchors.bottom: delegate.bottom
        visible: delegate.ListView.isCurrentItem ? false : true
    }
    Rectangle {
        width: delegate.width; height: 1; color: "white"
        visible: delegate.ListView.isCurrentItem ? false : true
    }
    Rectangle {
        height: delegate.height; width: 1; color: "white"
        anchors.right: delegate.right
        visible: true
    }
    Rectangle {
        height: delegate.height; width: 1; color: "white"
        anchors.left: delegate.left
        visible: true
    }

    MouseArea {
        anchors.fill: delegate
        onClicked: {
            delegate.ListView.view.currentIndex = index
            window.currentFeed = feed
        }
    }

}
