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
        text: "0"
        font.pixelSize: 12
        opacity: 0
    }

    ContentDri {
        id: contentDri1
        opacity: 0
        enabled: false
    }

    ContentGLinfo {
        id: contentGLinfo1
        opacity: 0
        enabled: false
    }

    ContentApp {
        id: contentApp1
        opacity: 0
        enabled: false
    }

    states: [
        State {
            name: "Configure"

            PropertyChanges {
                target: contentDri1
                opacity: 1
                enabled: true
            }
        },
        State {
            name: "GL"

            PropertyChanges {
                target: contentGLinfo1
                opacity: 1
                enabled: true
            }
        },
        State {
            name: "Card"
            PropertyChanges {
                target: contentApp1
                opacity: 1
                enabled: true
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
        }
    ]

}
