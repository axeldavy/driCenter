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

    Item {
        anchors.fill: parent
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
                            state: optionstate
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
            }
        }
    }
}
