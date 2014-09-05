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
    implicitWidth: 600
    implicitHeight: 400
    anchors.fill: parent

    Component {
        id: appdelegate
        Rectangle {
            width: parent.width
            height: 60
            border.color: "dark gray"
            border.width: 1
            radius: 5
            color: "light gray"
            Text {
                id: appdelegatetext
                x: 0.5 * (parent.width - width)
                y: 3
                text: appname
            }
            ComboBox {
                anchors.top: appdelegatetext.bottom
                model: cardlist
                currentIndex: cardlist.indexOf(appcard)
                onCurrentIndexChanged: {
                    if (currentIndex != 0) {
                        appcard = currentText
                    } else
                        appcard = "unknown";
                }
            }
        }
    }

    Button {
        anchors.left: parent.left
        text: "Detect Running Apps"
        onClicked: {
            appcardchooser.detectRunningApps();
        }
    }

    Button {
        id: begindetect
        anchors.right: parent.right
        text: "Detect Future Running Apps"
        onClicked: {
            begindetect.opacity = 0
            begindetect.enabled = false
            stopdetect.opacity = 1
            stopdetect.enabled = true
            appcardchooser.prepareDetectFutureRunningApps();
        }
    }

    Button {
        id: stopdetect
        anchors.right: parent.right
        text: "Stop Detection"
        enabled: false
        opacity: 0
        onClicked: {
            begindetect.opacity = 1
            begindetect.enabled = true
            stopdetect.opacity = 0
            stopdetect.enabled = false
            appcardchooser.endDetectFutureRunningApps();
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
                anchors.fill: parent
                model: appcardchooser
                delegate: appdelegate
                snapMode: ListView.SnapToItem
                spacing: 1
            }
        }
    }
}

