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

import QtQuick 2.0
import QtQuick.Controls 1.2

Rectangle {
    anchors.fill: parent

    Text {
        id: text1
        x: 84
        y: 51
        text: systemAnalysis.getCardsNumber()
        font.pixelSize: 12
        opacity: 0
    }

    ComboBox {
        id: comboBox1
        anchors.right: parent.right
        anchors.rightMargin: 3
        model: ["i965", "radeonsi"]
        y: 18
        opacity: 0
        onCurrentIndexChanged: {
            drimodel.driver = model[currentIndex]
        }
    }

    ComboBox {
        id: comboBox2
        anchors.left: parent.left
        anchors.leftMargin: 3
        model: ListModel {
            id: comboBox2items
            ListElement { text: "All Apps" }
            ListElement { text: "Add New" }
        }
        y: 18
        enabled: false
        opacity: 0
        onCurrentIndexChanged: {
            if (currentIndex == count - 1) {
                var newComponent = Qt.createComponent("AddAppWindow.qml")
                var windownewapp = newComponent.createObject()
                currentIndex = 0
            }
            drimodel.application = comboBox2items.get(currentIndex).text
        }
    }

    Text {
        id: text2
        x: 86
        y: 249
        text: qsTr(height.toString())
        font.pixelSize: 12
        opacity: 0
    }

    Text {
        id: text3
        x: 30
        y: 229
        text: applicationWindow1.currentCard.getPciId()
        font.pixelSize: 12
        opacity: 0
    }

    TextArea {
        id: text4
        y: 318
        anchors.left: parent.left
        anchors.right: parent.right
        text: applicationWindow1.currentCard.getEGLInfo()
        font.pixelSize: 12
        opacity: 0
    }

    ContentDri {
        id: contentDri1
        opacity: 0
    }

    states: [
        State {
            name: "Configure"

            PropertyChanges {
                target: comboBox1
                opacity: 1
            }

            PropertyChanges {
                target: comboBox2
                opacity: 1
                enabled: true
            }

            PropertyChanges {
                target: contentDri1
                x: 29
                y: 58
                anchors.fill: parent
                anchors.bottomMargin: 15
                anchors.leftMargin: 17
                anchors.rightMargin: 8
                anchors.topMargin: 64
                opacity: 1
            }
        },
        State {
            name: "GL"

            PropertyChanges {
                target: text1
                x: 29
                y: 58
                width: 543
                height: 146
                opacity: 1
            }

            PropertyChanges {
                target: comboBox1
                opacity: 1
            }

            PropertyChanges {
                target: text2
                x: 29
                y: 31
                opacity: 1
            }

            PropertyChanges {
                target: text3
                opacity: 1
            }

            PropertyChanges {
                target: text4
                x: 30
                y: 258
                width: 542
                height: 123
                anchors.leftMargin: 29
                wrapMode: Text.WordWrap
                opacity: 1
            }

        },
        State {
            name: "Card"
            PropertyChanges {
                target: text1
                x: 29
                y: 58
                width: 543
                height: 322
                opacity: 1
            }

            PropertyChanges {
                target: comboBox1
                opacity: 1
            }
        },
        State {
            name: "System"
            PropertyChanges {
                target: text1
                x: 29
                y: 58
                width: 543
                height: 322
                opacity: 1
            }

            PropertyChanges {
                target: comboBox1
                opacity: 1
            }
        }
    ]

}
