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

#ifndef CARDINFO_H
#define CARDINFO_H

#include <QObject>
#include <QAbstractListModel>
#include <QStringList>
extern "C" {
#include "system.h"
}
/* CardModel is used to define a model
 * for ListView so we can select between
 * different gpus
 */

class CardModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum CardRoles {
        NameRole = Qt::UserRole + 1,
        IDRole,
        DriverNameRole,
        DrmRole,
        X11Role,
        WaylandRole
    };

    CardModel(QObject *parent = 0);
    virtual ~CardModel();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    Q_INVOKABLE int numCards() const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE QVariant get(int row);
    QList<QString> getIDList();
    QList<QString> getDriverList();

protected:
    QHash<int, QByteArray> roleNames() const;
private:
    struct system_info *system_info;

    QVariant GetTableForDisplay(display_system_info *display_info) const;
};

#endif // CARDINFO_H
