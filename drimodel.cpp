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

#include <stdio.h>

#include "drimodel.h"

DriModel::DriModel(QObject *parent) :
    QAbstractListModel(parent)
{
    drixml = new Drixml();
    QFile *input = new QFile("/home/axel/.drirc");
    input->open(QIODevice::ReadOnly);
    drixml->loadFile(input);
    input->close();
    input = new QFile("/home/axel/.drircsection");
    input->open(QIODevice::ReadOnly);
    drixml->loadFile(input);
    input->close();
    driver_name = "i965";
    application_name = "Default";
}

void DriModel::save(QString filename)
{
    QFile *output = new QFile(filename);
    output->open(QIODevice::WriteOnly);
    drixml->saveFile(output);
    output->flush();
    output->close();
}

QVariant DriModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= this->rowCount())
        return QVariant();

    QList<DriOptionValue> *options_list;
    if (role == NameRole) {
        DriDriver *driver;
        DriApplication *application;
        if (!drixml->getDriver(driver_name, &driver) ||
            !driver->GetApplication(application_name, &application))
            return QVariant();
        options_list = application->GetOptions();
        QString *name, *value;
        DriOptionValue option (options_list->at(index.row()));
        option.GetValues(&name, &value);
        return *name;
    } else if (role == ContentRole) {
        DriDriver *driver;
        DriApplication *application;
        if (!drixml->getDriver(driver_name, &driver) ||
            !driver->GetApplication(application_name, &application))
            return QVariant();
        options_list = application->GetOptions();
        QString *name, *value;
        DriOptionValue option (options_list->at(index.row()));
        option.GetValues(&name, &value);
        return *value;
    } else if (role == HelpRole) {
        DriDriver *driver;
        DriApplication *application;
        if (!drixml->getDriver(driver_name, &driver) ||
            !driver->GetApplication(application_name, &application))
            return QString("???");
        options_list = application->GetOptions();
        QString *name, *value;
        DriOptionValue option (options_list->at(index.row()));
        option.GetValues(&name, &value);
        DriOptionInfo *option_info;
        if (!drixml->FindOptionInfo(*name, &option_info))
            return QString("???");
        return option_info->GetDescription("fr");
    } else if (role == StateRole) {
        DriDriver *driver;
        DriApplication *application;
        if (!drixml->getDriver(driver_name, &driver) ||
            !driver->GetApplication(application_name, &application))
            return "OptionString";
        options_list = application->GetOptions();
        QString *name, *value;
        DriOptionValue option (options_list->at(index.row()));
        option.GetValues(&name, &value);
        DriOptionInfo *option_info;
        if (!drixml->FindOptionInfo(*name, &option_info))
            return "OptionString";
        OptionType type = option_info->GetType();
        if (type == OptionBool)
            return "OptionBool";
        else if (type == OptionEnum)
            return "OptionEnum";
        else if (type == OptionInt)
            return "OptionInt";
        else
            return "OptionString";
    } else if (role == EnumRole) {
        DriDriver *driver;
        DriApplication *application;
        if (!drixml->getDriver(driver_name, &driver) ||
            !driver->GetApplication(application_name, &application))
            return QVariant(QList<QString>());
        options_list = application->GetOptions();
        QString *name, *value;
        DriOptionValue option (options_list->at(index.row()));
        option.GetValues(&name, &value);
        DriOptionInfo *option_info;
        if (!drixml->FindOptionInfo(*name, &option_info))
            return QVariant(QList<QString>());
        return QVariant(option_info->GetEnumMenu("fr"));
    } else if (role == DefaultRole) {
        DriDriver *driver;
        DriApplication *application;
        if (!drixml->getDriver(driver_name, &driver) ||
            !driver->GetApplication(application_name, &application))
            return QVariant();
        options_list = application->GetOptions();
        QString *name, *value;
        DriOptionValue option (options_list->at(index.row()));
        option.GetValues(&name, &value);
        DriOptionInfo *option_info;
        if (!drixml->FindOptionInfo(*name, &option_info))
            return QVariant();
        return QVariant(option_info->GetDefaultValue());
    }
    return QVariant();
}

int DriModel::rowCount(const QModelIndex &parent) const
{
    (void) parent;
    DriDriver *driver;
    DriApplication *application;
    if (!drixml->getDriver(driver_name, &driver) ||
        !driver->GetApplication(application_name, &application))
        return 0;
    return application->count();
}

QVariant DriModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

Qt::ItemFlags DriModel::flags(const QModelIndex &index) const
{
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

bool DriModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < 0 || index.row() >= this->rowCount())
        return false;
    if (role != ContentRole)
        return false;

    DriDriver *driver;
    DriApplication *application;
    if (!drixml->getDriver(driver_name, &driver) ||
        !driver->GetApplication(application_name, &application))
        return false;

    QList<DriOptionValue> *options_list = application->GetOptions();
    QString *name, *old_value;
    QString new_value = value.toString();
    DriOptionValue option (options_list->at(index.row()));
    option.GetValues(&name, &old_value);
    if (new_value == old_value)
        return true;
    DriOptionValue *new_option = new DriOptionValue(*name, new_value);
    application->RemoveOption(*name);
    application->AddOption(new_option);
    this->save("/home/axel/.drirctest");
    return true;
}

void DriModel::append(QString name, QString value)
{
    DriDriver *driver;
    DriApplication *application;
    DriOptionValue *option;

    if (!drixml->getDriver(driver_name, &driver)) {
        driver = new DriDriver(driver_name);
        drixml->addDriver(driver);
    }

    if (!driver->GetApplication(application_name, &application)) {
        application = new DriApplication(QString(), application_name);
        driver->AddApplication(application);
    }

    if (application->GetOption(name, &option))
        return;
    DriOptionValue *new_option = new DriOptionValue(name, value);
    beginResetModel();
    application->AddOption(new_option);
    endResetModel();
}

void DriModel::remove(QString name)
{
    DriDriver *driver;
    DriApplication *application;
    DriOptionValue *option;
    if (!drixml->getDriver(driver_name, &driver) ||
        !driver->GetApplication(application_name, &application))
        return;
    if (!application->GetOption(name, &option))
        return;
    beginResetModel();
    application->RemoveOption(name);
    endResetModel();
}

QHash<int, QByteArray> DriModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "optionname";
    roles[ContentRole] = "optioncontent";
    roles[HelpRole] = "optionhelp";
    roles[StateRole] = "optionstate";
    roles[EnumRole] = "optionenum";
    roles[DefaultRole] = "optiondefault";
    return roles;
}

QString DriModel::getDriver()
{
    return driver_name;
}

void DriModel::setDriver(QString driver)
{
    beginResetModel();
    driver_name = driver;
    endResetModel();
}

QString DriModel::getApplication()
{
    return application_name;
}

void DriModel::setApplication(QString application)
{
    beginResetModel();
    application_name = application;
    endResetModel();
}
