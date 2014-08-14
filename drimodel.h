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

#ifndef DRIMODEL_H
#define DRIMODEL_H

#include <QAbstractItemModel>

#include "drixml.h"

class DriModel : public QAbstractListModel
{
    Q_OBJECT
    QString driver_name;
    QString application_name;
    Drixml *drixml;
public:
    enum DriRoles {
        NameRole = Qt::UserRole + 1,
        ContentRole,
        HelpRole,
        StateRole,
        EnumRole,
        DefaultRole
    };
    explicit DriModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);
    Q_INVOKABLE void append(QString name, QString value);
    Q_INVOKABLE void remove(QString name);
    void save(QString filename);
    QString getDriver();
    void setDriver(QString driver);
    QString getApplication();
    void setApplication(QString application);
    Q_PROPERTY(QString driver READ getDriver WRITE setDriver)
    Q_PROPERTY(QString application READ getApplication WRITE setApplication)

protected:
    QHash<int, QByteArray> roleNames() const;

signals:

public slots:

};

#endif // DRIMODEL_H
