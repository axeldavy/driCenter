/*
 * Copyright © 2014 Axel Davy
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of the
 * copyright holders not be used in advertising or publicity
 * pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.1
import QtPositioning 5.3
import QtQuick.Dialogs 1.2


ApplicationWindow {
    id: applicationWindow1
    visible: true
    width: 640
    height: 480
    title: qsTr("DRI Center")
    property variant currentCard: systemAnalysis.getCard(0)

    menuBar: MenuBar {
        Menu {
            title: "Exit"
            MenuItem {
                text: "Exit"
                onTriggered: Qt.quit()
            }
        }
    }

    ToolBar {
        id: toolbar
        width: parent.width

        ComboBox {
            id: delegateChooser
            model: cardmenu
            width: 150
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            onCurrentIndexChanged: {
                currentCard = systemAnalysis.getCard(currentIndex)
            }
        }
    }

    ListModel {
        id: categories
        ListElement { name: "Configure Options" }
        ListElement { name: "GL Info" }
        ListElement { name: "Card Info" }
        ListElement { name: "System Info" }
    }

    Column {
        id: leftColumn
        x: 0
        y: 0
        width: 200
        anchors.top: toolbar.bottom
        anchors.bottom: parent.bottom
        ListView {
            orientation: ListView.Vertical
            anchors.fill: parent
            focus: true
            highlight: Rectangle { color: "steelblue" }
            highlightMoveVelocity: 9999999
            model: categories
            delegate: CategoryChooser {}
            onCurrentIndexChanged:  {
                if (currentIndex == 0)
                    windowContent.state="Configure"
                else if (currentIndex == 1)
                    windowContent.state="GL"
                else if (currentIndex == 2)
                    windowContent.state="Card"
                else if (currentIndex == 3)
                    windowContent.state="System"
            }
         }
    }

    WindowContent {
        id: windowContent
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 206
        anchors.top: toolbar.bottom
        anchors.topMargin: 0
        state: "Configure"
    }
}
