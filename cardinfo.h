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

/* CardInfo is the class containing
 * all the specific information
 * about a gpu.
 */

class CardInfo : public QObject
{
    Q_OBJECT
public:
    CardInfo(QObject* parent = 0);
    void fillInfo(const QString &pci_id);
    bool getPlaforms(QString* &platforms, char &numPlatforms);
    Q_INVOKABLE QString getName();
    Q_INVOKABLE QString getPciId();
    Q_INVOKABLE QString getEGLInfo();
private:
    void fillGLXInfo();
    void fillEGLInfo();
    QString m_glx_info;
    QString m_egl_info;
    QString m_pci_id;
    QString m_name;

};
Q_DECLARE_METATYPE(CardInfo*)

/* CardModel is used to define a model
 * for ListView so we can select between
 * different gpus
 */

class CardModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum CardRoles {
        NameRole = Qt::UserRole + 1
    };

    CardModel(QObject *parent = 0);
    virtual ~CardModel();

    void addCard(CardInfo* card);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<CardInfo*> m_cards;

};

#endif // CARDINFO_H
