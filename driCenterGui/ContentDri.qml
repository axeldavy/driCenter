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
import QtQuick.Layouts 1.0
import QtPositioning 5.3
import QtQuick.Dialogs 1.2

Rectangle {
    id: rectangle1
    implicitWidth: 600
    implicitHeight: 400
    anchors.fill: parent

    ListModel {
        id: appNames
        ListElement { text: "All Apps" }
        ListElement { text: "Add New" }
    }

    ListModel {
        id: appExecs
        ListElement { text: "All Apps" }
        ListElement { text: "Add New" }
    }

    ComboBox {
        id: comboBox2
        anchors.left: parent.left
        anchors.leftMargin: 3
        model: appNames
        y: 18
        onCurrentIndexChanged: {
            if (currentIndex == count - 1) {
                var newComponent = Qt.createComponent("AddAppWindow.qml")
                var windownewapp = newComponent.createObject()
                currentIndex = 0
            }
            drimodel.application_name = appNames.get(currentIndex).text
            drimodel.application_exec = appExecs.get(currentIndex).text
        }
    }

    ComboBox {
        id: comboBox1
        anchors.right: parent.right
        anchors.rightMargin: 3
        model: {
            var list = ["All"]
            var i;
            for (i = 0; i < cardmenu.numCards(); i++)
                list.push(cardmenu.get(i).driver_name)
            return list
        }
        y: 18
        onCurrentIndexChanged: {
            drimodel.driver = model[currentIndex]
            var app_names = drimodel.getApplicationNames();
            var app_execs = drimodel.getApplicationExecs();
            var i;
            if (appNames.count > 2) {
                appNames.clear()
                appExecs.clear()
                appNames.insert(0, {"text": "Add New"})
                appExecs.insert(0, {"text": "Add New"})
                appNames.insert(0, {"text": "All Apps"})
                appExecs.insert(0, {"text": "All Apps"})
                comboBox2.currentIndex = 0
            }
            for (i = 0; i < app_names.length; i++) {
                appNames.insert(i+1, {"text": app_names[i]})
                appExecs.insert(i+1, {"text": app_execs[i]})
            }
        }
    }

    Item {
        x: 29
        y: 58
        anchors.fill: parent
        anchors.bottomMargin: 15
        anchors.leftMargin: 17
        anchors.rightMargin: 8
        anchors.topMargin: 64
        ScrollView {
            anchors.fill: parent
            ListView {
                id: optionlist
                anchors.fill: parent
                Component {
                    id: dridelegate
                    Item {
                        height: dricase.height
                        width: parent.width
                        DriCase {
                            id: dricase
                            state: optionsection != "Driver options" ? optionstate : "UnEditable"
                            name: optionname
                            content: optioncontent
                            help: optionhelp
                            enum_menu: optionenum
                            default_value: optiondefault
                            onContentChanged: optioncontent = content
                        }
                    }
                }
                model: drimodel
                delegate: dridelegate
                snapMode: ListView.SnapToItem
                spacing: 1
                footer: Rectangle {
                    id: additemrect
                    height: 60
                    width: optionlist.width
                    Rectangle {
                        id: newnamerect
                        x: 3
                        y: 30 - newnametext.height / 2 - 3
                        width: 0.4 * additemrect.width
                        height: newnametext.height + 6
                        TextEdit {
                            id: newnametext
                            x: 3
                            y: 3
                            text: "new_name"
                            font.pixelSize: 12
                            inputMethodHints: Qt.ImhPreferLowercase
                        }
                        border.color: "dark gray"
                        border.width: 1
                        radius: 5
                        color: "white"
                    }
                    Rectangle {
                        id: newcontentrect
                        x: 0.6 * parent.width
                        y: 30 - newcontenttext.height / 2 - 3
                        width: newcontenttext.width + 6
                        height: newcontenttext.height + 6
                        TextEdit {
                            id: newcontenttext
                            x: 3
                            y: 3
                            text: "new_value"
                            font.pixelSize: 12
                            onWidthChanged:
                                newcontentrect.width = width + 6
                        }
                        border.color: "dark gray"
                        border.width: 1
                        radius: 5
                        color: "white"
                    }
                    Button {
                        id: addbutton
                        x: 0.85 * parent.width
                        y: 30 - addbutton.height / 2
                        width: 0.10 * parent.width
                        text: qsTr("Add")
                        onClicked: {
                            drimodel.append(newnametext.text, newcontenttext.text)
                        }
                    }
                }
                section.property: "optionsection"
                section.criteria: ViewSection.FullString
                section.delegate:
                    Rectangle {
                        id: sectionrect
                        height: 60
                        width: optionlist.width
                        Text {
                            id: sectiontext
                            x: (sectionrect.width - sectiontext.width)/2
                            y: (sectionrect.height - sectiontext.height)/2
                            text: section
                        }
                    }
                section.labelPositioning: ViewSection.InlineLabels
            }
        }
    }
}
