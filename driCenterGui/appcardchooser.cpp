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

#include <QProcess>

#include "appcardchooser.h"

AppCardChooser::AppCardChooser(Drixml *drixml, QObject *parent) :
    QAbstractListModel(parent)
{
    this->drixml = drixml;
}


int AppCardChooser::rowCount(const QModelIndex &parent) const
{
    (void) parent;
    return AppExecs.length();
}

QVariant AppCardChooser::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= AppExecs.length())
        return QVariant();
    if (role == NameRole)
        return AppExecs[index.row()];
    else if (role == CardRole)
        return AppCards[index.row()];
    return QVariant();
}

Qt::ItemFlags AppCardChooser::flags(const QModelIndex &index) const
{
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

bool AppCardChooser::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < 0 || index.row() >= AppExecs.length())
        return false;
    if (role != CardRole)
        return false;

    QString AppExec = AppExecs[index.row()];
    DriDriver *driver;
    DriApplication *application;
    DriOptionValue *option;

    if (!drixml->getDriver("loader", &driver)) {
        driver = new DriDriver("loader");
        drixml->addDriver(driver);
    }

    AppCards.replace(index.row(), value.toString());
    if (driver->GetApplication(AppExec, &application)) {
        if (application->GetOption("device_id", &option)) {
            QString *temp_name, *temp_value;
            option->GetValues(&temp_name, &temp_value);
            if (*temp_value == value.toString())
                return true;
            application->RemoveOption("device_id");
        }
        if (value.toString() == "unknown")
            return true;
        option = new DriOptionValue("device_id", value.toString());
        application->AddOption(option);
        emit dataChanged(index, index, QVector<int>(1, CardRole));
        return true;
    }
    if (value.toString() == "unknown")
        return false;
    option = new DriOptionValue("device_id", value.toString());
    application = new DriApplication(AppExec, AppExec);
    application->AddOption(option);
    driver->AddApplication(application);
    emit dataChanged(index, index, QVector<int>(1, CardRole));
    return true;
}

void AppCardChooser::detectRunningApps()
{
    beginResetModel();
    DetectCards();
    AppExecs = AppDetected;
    fillAppCards();
    endResetModel();
}

void AppCardChooser::prepareDetectFutureRunningApps()
{
    DetectCards();
}

void AppCardChooser::endDetectFutureRunningApps()
{
    QList<QString> backup = AppDetected;
    beginResetModel();
    DetectCards();
    AppExecs.clear();

    int index_b=0, index_d=0;

    while (index_b < backup.length() && index_d < AppDetected.length()) {
        QString App_b = backup[index_b];
        QString App_d = AppDetected[index_d];
        if (App_b == App_d) {
            index_b ++;
            index_d ++;
        } else if (App_b < App_d) {
            index_b ++;
        } else {
            AppExecs.append(App_d);
            index_d ++;
        }
    }

    for (;index_d < AppDetected.length(); index_d++)
        AppExecs.append(AppDetected[index_d]);

    fillAppCards();
    endResetModel();
}

QHash<int, QByteArray> AppCardChooser::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "appname";
    roles[CardRole] = "appcard";
    return roles;
}

void AppCardChooser::DetectCards()
{
    AppDetected.clear();
    QString program_name = "lsof";
    QStringList program_args;
    QProcess program;
    program_args << "-F0pc" << "+d" << "/dev/dri";
    program.start(program_name, program_args);
    if (!program.waitForFinished(1000))
        return;
    QByteArray ret = program.readAllStandardOutput();
    int index = 0;
    while (index < ret.length()) {
        if (ret[index] == 'c') {
            QString newDetected;
            int last = index + 1;
            while (ret[last] != (char)0) {
                newDetected.append(QChar(ret[last]));
                last ++;
            }
            AppDetected.append(newDetected);
            index = last;
        } else
            index += 1;
    }
    qSort(AppDetected.begin(), AppDetected.end());
    for (int i = AppDetected.length() - 1; i > 0; i--) {
        if (AppDetected[i] == AppDetected[i-1])
            AppDetected.removeAt(i);
    }
}

void AppCardChooser::fillAppCards()
{
    AppCards.clear();
    DriDriver *driver;

    if (!drixml->getDriver("loader", &driver)) {
        for (int i=0; i<AppExecs.length(); i++) {
            AppCards.append("unknown");
        }
        return;
    }

    for (int i=0; i<AppExecs.length(); i++) {
        DriApplication *application;
        DriOptionValue *option;
        if (!driver->GetApplication(AppExecs[i], &application)) {
            AppCards.append("unknown");
            continue;
        }
        if (!application->GetOption("device_id", &option)) {
            AppCards.append("unknown");
            continue;
        }
        QString *temp_name, *value;
        option->GetValues(&temp_name, &value);
        AppCards.append(*value);
    }
    return;
}


