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
    anchors.leftMargin: 5
    property var displaydrm: applicationWindow1.currentCard.drm
    property var displayx11: applicationWindow1.currentCard.x11
    property var displayWayland: applicationWindow1.currentCard.Wayland
    property int displayindex: 1
    property var display: (displayindex == 0) ? displaydrm : ((displayindex == 1) ? displayx11 : displayWayland)


    ListModel {
        id: displayServers
        ListElement { text: "Drm" }
        ListElement { text: "X11" }
        ListElement { text: "Wayland" }
    }

    ComboBox {
        id: comboServers
        anchors.right: parent.right
        anchors.rightMargin: 3
        model: displayServers
        onCurrentIndexChanged: {
           displayindex = currentIndex
        }
    }

    ColumnLayout {
        anchors.top: comboServers.bottom
        anchors.topMargin: 5

        Text {
            id: text3
            text: "card id : " + applicationWindow1.currentCard.id_path_tag
            font.pixelSize: 12
        }

        Text {
            id: text4
            anchors.left: parent.left
            anchors.right: parent.right
            text: "card driver : " + applicationWindow1.currentCard.driver_name
            font.pixelSize: 12
        }

        Row {
            Text {
                text: "glx : "
            }

            CheckBox {
                checked: display !== undefined && display.has_glx !== undefined && display.has_glx
                enabled: false
            }
        }

        Row {
            Text {
                text: "egl : "
            }

            CheckBox {
                checked: display !== undefined && display.has_egl !== undefined && display.has_egl
                enabled: false
            }
        }
    }
}
