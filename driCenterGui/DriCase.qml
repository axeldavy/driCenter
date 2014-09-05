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
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.1

Rectangle {
    id: dricaserect
    implicitWidth: 400
    width: parent.width
    height: 60
    border.color: "dark gray"
    border.width: 1
    radius: 5
    color: "light gray"
    property string name: "???"
    property string content: "???"
    property string help: "???"
    property var enum_menu: []
    property string default_value: ""

    Text {
        id: text1
        x: 5
        y: 30 - text1.height / 2
        width: 0.6 * parent.width
        text: name
        font.pixelSize: 12
    }

    Button {
        property var subwindowhelp
        property int hassubwindowhelp: 0
        id: button
        x: 0.85 * parent.width
        y: 30 - button.height / 2
        width: 0.10 * parent.width
        text: qsTr("Help")
        opacity: 0
        enabled: false
        onClicked: {
            optionhelpwindow.opacity = 1
            optionhelpwindowtext.text = help
        }
    }

    Rectangle {
        id: textEdit1
        x: 0.6 * parent.width
        y: 30 - textEdit1_content.height / 2 - 3
        width: 0.15 * parent.width > textEdit1_content.contentWidth + 6 ? 0.15 * parent.width : textEdit1_content.contentWidth + 6
        height: textEdit1_content.height + 6
        TextEdit {
            id: textEdit1_content
            x: 3
            y: 3
            width: textEdit1.width - 6
            text: content
            font.pixelSize: 12
            verticalAlignment: TextEdit.AlignVCenter
            horizontalAlignment: TextEdit.AlignHCenter
            onTextChanged: content=textEdit1_content.text
        }
        border.color: "dark gray"
        border.width: 1
        radius: 5
        color: "white"
        opacity: 0
    }

    ComboBox {
        id: comboBox
        model: enum_menu
        currentIndex: content
        enabled: false
        x: 0.6 * parent.width
        y: 30 - comboBox.height / 2
        width: 0.18 * parent.width
        height: 39
        opacity: 0
        onCurrentIndexChanged: {
            if (enabled)
                content = currentIndex.toString()
        }
    }

    Button {
        id: deletebutton
        text: "X"
        x: parent.width - deletebutton.width - 2
        y: 3
        width: 12
        height: 12
        onClicked: drimodel.remove(name)
    }

    Button {
        id: resetbutton
        text: "Reset"
        x: parent.width - resetbutton.width - 2
        y: parent.height - resetbutton.height - 3
        width: 40
        height: 12
        onClicked: content = default_value
    }

    Button {
        id: addOptionButton
        text: "Add Custom"
        x: 0.6 * parent.width
        y: 30 - button.height / 2
        width: 0.20 * parent.width
        opacity: 0
        enabled: false
        onClicked: {
            drimodel.append(name, default_value)
        }
    }

    CheckBox {
        id: checkBox
        checked: (content == "true")
        enabled: false
        x: 0.6 * parent.width
        y: 30 - checkBox.height / 4
        height: 39
        opacity: 0
        onClicked: {
            if (enabled) {
                if (checked)
                    content = "true"
                else
                    content = "false"
            }
        }
    }

    onContentChanged: {
        checkBox.checked = (content == "true")
    }

    states: [
        State {
            name: "OptionInt"

            PropertyChanges {
                target: button
                enabled: (help == "???") ? false : true
                opacity: 1
            }

            PropertyChanges {
                target: textEdit1
                opacity: 1
            }

            PropertyChanges {
                target: textEdit1_content
                inputMethodHints: Qt.ImhDigitsOnly
            }
        },
        State {
            name: "OptionString"

            PropertyChanges {
                target: button
                enabled: (help == "???") ? false : true
                opacity: 1
            }

            PropertyChanges {
                target: textEdit1
                opacity: 1
            }
        },
        State {
            name: "OptionEnum"

            PropertyChanges {
                target: button
                enabled: (help == "???") ? false : true
                opacity: 1
            }

            PropertyChanges {
                target: comboBox
                enabled: true
                opacity: 1
            }
        },
        State {
            name: "OptionBool"

            PropertyChanges {
                target: button
                enabled: (help == "???") ? false : true
                opacity: 1
            }

            PropertyChanges {
                target: checkBox
                enabled: true
                opacity: 1
            }
        },
        State {
            name: "UnEditable"

            PropertyChanges {
                target: button
                enabled: (help == "???") ? false : true
                opacity: 1
            }

            PropertyChanges {
                target: addOptionButton
                enabled: true
                opacity: 1
            }

            PropertyChanges {
                target: resetbutton
                enabled: false
                opacity: 0
            }

            PropertyChanges {
                target: deletebutton
                enabled: false
                opacity: 0
            }
        }
    ]
}
