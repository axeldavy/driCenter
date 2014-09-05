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
    QList<QString> driver_list;
    QString application_name;
    QString application_executable;
    DriDriver *driver;
    DriApplication *application;
    Drixml *drixml;
    QList<QString> *known_application_options;
    QList<QString> *unknown_application_options;
    QList<QString> *additional_options;
private:
    void ApplicationOrDriverModified();
    QString GetDefaultValueForOption(QString name) const;
    void GetOptionData(int index, QString *name, QString *value, DriOptionInfo **option_info, OptionType *option_type, QString *default_value) const;
public:
    enum DriRoles {
        NameRole = Qt::UserRole + 1,
        ContentRole,
        HelpRole,
        StateRole,
        EnumRole,
        DefaultRole,
        SectionRole
    };
    explicit DriModel(QObject *parent = 0);
    Drixml *getDrixml() {
        return drixml;
    }
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
    Q_INVOKABLE QList<QString> getApplicationNames();
    Q_INVOKABLE QList<QString> getApplicationExecs();
    void setDriverList(QList<QString> driver_list);
    QString getDriver();
    void setDriver(QString driver);
    QString getApplicationName();
    void setApplicationName(QString applicationName);
    QString getApplicationExec();
    void setApplicationExec(QString applicationExec);
    Q_PROPERTY(QString driver READ getDriver WRITE setDriver)
    Q_PROPERTY(QString application_name READ getApplicationName WRITE setApplicationName)
    Q_PROPERTY(QString application_exec READ getApplicationExec WRITE setApplicationExec)

protected:
    QHash<int, QByteArray> roleNames() const;

signals:

public slots:

};

#endif // DRIMODEL_H
