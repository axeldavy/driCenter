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

#include <QApplication>
#include <QQmlApplicationEngine>

#include <QGuiApplication>
#include <qqmlengine.h>
#include <qqmlcontext.h>
#include <qqml.h>
#include <QtQuick/qquickitem.h>
#include <QtQuick/qquickview.h>

#include "cardinfo.h"
#include "drimodel.h"
#include "appcardchooser.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    CardModel model;
    QList<QString> card_id_list = model.getIDList();
    card_id_list.insert(0, "undefined");

    DriModel drimodel;
    drimodel.setDriverList(model.getDriverList());

    AppCardChooser appcardchooser (drimodel.getDrixml());
    QQmlApplicationEngine engine;
    QQmlContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("cardmenu", &model);
    ctxt->setContextProperty("drimodel", &drimodel);
    ctxt->setContextProperty("appcardchooser", &appcardchooser);
    ctxt->setContextProperty("cardlist", QVariant(card_id_list));
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    return app.exec();
}
