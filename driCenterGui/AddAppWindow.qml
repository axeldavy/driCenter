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

Window {
    id: helpwindow
    visible: true;
    flags: Qt.Dialog;
    width: 400
    height: 200
    title: "Describe the Application to add"
    Text {
        id: nameenter
        text: "Name : "
        anchors.left: parent.left
        anchors.leftMargin: 6
        anchors.top: parent.top
        anchors.topMargin: 9
    }
    Text {
        id: execenter
        text: "Executable name : "
        anchors.left: parent.left
        anchors.leftMargin: 6
        anchors.top: nameenter.bottom
        anchors.topMargin: 6
    }
    Rectangle {
        id: namerect
        border.width: 1
        border.color: "black"
        radius: 5
        width: (nametext.contentWidth + 6 > 100) ? nametext.contentWidth + 6 : 100
        height: nametext.height + 6
        anchors.left: execenter.right
        y: nameenter.y - 3
        TextEdit {
            id: nametext
            x: 3
            y: 3
            text: "Name"
            width: namerect.width - 6
        }
    }
    Rectangle {
        id: execrect
        border.width: 1
        border.color: "black"
        radius: 5
        width: (exectext.contentWidth + 6 > 100) ? exectext.contentWidth + 6 : 100
        height: exectext.height + 6
        anchors.left: execenter.right
        y: execenter.y - 3
        TextEdit {
            id: exectext
            x: 3
            y: 3
            text: "executable_name"
            width: execrect.width - 6
        }
    }
    Button {
        text: "Add"
        anchors.top: execrect.bottom
        anchors.left: execenter.right
        onClicked: {
            var index = comboBox2.count-1
            appNames.insert(index, {"text": nametext.text})
            appExecs.insert(index, {"text": exectext.text})
            comboBox2.currentIndex = index
            helpwindow.destroy()
        }
    }
}
