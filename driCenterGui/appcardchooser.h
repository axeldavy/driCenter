#ifndef APPCARDCHOOSER_H
#define APPCARDCHOOSER_H

#include <QAbstractItemModel>

#include "drixml.h"

class AppCardChooser : public QAbstractListModel
{
    Q_OBJECT
    Drixml *drixml;
    QList<QString> AppExecs;
    QList<QString> AppCards;
    QList<QString> AppDetected;
public:
    enum CardRoles {
        NameRole = Qt::UserRole + 1,
        CardRole
    };

    AppCardChooser(Drixml *drixml, QObject *parent = 0);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);

    Q_INVOKABLE void detectRunningApps();
    Q_INVOKABLE void prepareDetectFutureRunningApps();
    Q_INVOKABLE void endDetectFutureRunningApps();
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    void DetectCards();
    void fillAppCards();
};

#endif // APPCARDCHOOSER_H
