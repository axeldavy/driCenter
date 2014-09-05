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
#include <assert.h>

#include "drimodel.h"

#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#include <GL/glxext.h>

typedef const char * glXGetDriverConfig_t (const char *driverName);
static glXGetDriverConfig_t *GetDriverConfig = NULL;

static const char *getDriverConfig(const char *driver){
     if (GetDriverConfig == NULL)
         GetDriverConfig = (glXGetDriverConfig_t *)glXGetProcAddressARB ((const GLubyte *)"glXGetDriverConfig");
     if (GetDriverConfig == NULL)
         return NULL;
     return (*GetDriverConfig)(driver);
}

DriModel::DriModel(QObject *parent) :
    QAbstractListModel(parent)
{
    drixml = new Drixml();
    QFile *input = new QFile("/etc/drirc");
    input->open(QIODevice::ReadOnly);
    drixml->loadFile(input);
    input->close();
    input = new QFile("/home/axel/.drirc");
    input->open(QIODevice::ReadOnly);
    drixml->loadFile(input);
    input->close();
    driver_name.clear();
    application_name.clear();
    application_executable.clear();
    ApplicationOrDriverModified();
}

void DriModel::ApplicationOrDriverModified()
{
    if (!drixml->getDriver(driver_name, &driver))
        driver = NULL;
    if (driver == NULL || !driver->GetApplication(application_executable, &application))
        application = NULL;
    if (application == NULL) {
        known_application_options = new QList<QString>();
        unknown_application_options = new QList<QString>();
        additional_options = new QList<QString> (drixml->GetOptionInfoNames());
    } else {
        QList<QString> known_options = drixml->GetOptionInfoNames();
        application->GetOptionsMatch(known_options, &known_application_options, &unknown_application_options, &additional_options);
    }
}

void DriModel::save(QString filename)
{
    QFile *output = new QFile(filename);
    output->open(QIODevice::WriteOnly);
    drixml->saveFile(output);
    output->flush();
    output->close();
}

QString DriModel::GetDefaultValueForOption(QString name) const
{
    // Get the value defined for all apps and this driver
    {
        DriApplication *application_all;
        DriOptionValue *option_all;
        if (application_executable != QString() &&
            driver->GetApplication(QString(), &application_all) &&
            application_all->GetOption(name, &option_all)) {
            QString *o_name, *o_value;
            option_all->GetValues(&o_name, &o_value);
            return *o_value;
        }
    }
    // Get the value defined for this apps and all drivers
    {
        DriDriver *driver_all;
        DriApplication *application_driver_all;
        DriOptionValue *option_all;
        if (driver_name != QString() &&
            drixml->getDriver(QString(), &driver_all) &&
            driver_all->GetApplication(application_executable, &application_driver_all) &&
            application_driver_all->GetOption(name, &option_all)) {
            QString *o_name, *o_value;
            option_all->GetValues(&o_name, &o_value);
            return *o_value;
        }
    }
    // Get the value defined for all apps and all drivers
    {
        DriDriver *driver_all;
        DriApplication *application_all;
        DriOptionValue *option_all;
        if (driver_name != QString() &&
            drixml->getDriver(QString(), &driver_all) &&
            application_executable != QString() &&
            driver_all->GetApplication(QString(), &application_all) &&
            application_all->GetOption(name, &option_all)) {
            QString *o_name, *o_value;
            option_all->GetValues(&o_name, &o_value);
            return *o_value;
        }
    }
    // Get the default one from the driver
    DriOptionInfo *option_info;
    if (!drixml->FindOptionInfo(name, &option_info))
        return QString("");
    return option_info->GetDefaultValue();
}

void DriModel::GetOptionData(int index, QString *name, QString *value,
                             DriOptionInfo **option_info,
                             OptionType *option_type,
                             QString *default_value) const
{
    QString *temp_string;
    bool is_in_app = index < known_application_options->length() + unknown_application_options->length();
    bool is_unknown = false;

    if (index < known_application_options->length()) {
        *name = known_application_options->at(index);
    } else if (is_in_app) {
        *name = unknown_application_options->at(index-known_application_options->length());
        is_unknown = true;
    } else {
        *name = additional_options->at(index - known_application_options->length() - unknown_application_options->length());
    }

    if (!is_unknown) {
        assert(drixml->FindOptionInfo(*name, option_info));
        *option_type = (*option_info)->GetType();
    } else {
        *option_info = NULL;
        *option_type = OptionString;
    }

    *default_value = GetDefaultValueForOption(*name);

    if (is_in_app) {
        DriOptionValue *option;
        QString *option_value;
        assert(application->GetOption(*name, &option));
        option->GetValues(&temp_string, &option_value);
        *value = *option_value;
        assert(*temp_string == *name);
    } else {
        *value = *default_value;
    }

}

QVariant DriModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= this->rowCount())
        return QVariant();

    QString name, value, default_value;
    OptionType option_type;
    DriOptionInfo *option_info;
    GetOptionData(index.row(), &name, &value, &option_info, &option_type, &default_value);

    if (role == NameRole) {
        return name;
    } else if (role == ContentRole) {
        return value;
    } else if (role == HelpRole) {
        if (option_info == NULL)
            return QString("???");
        QString help = option_info->GetDescription("fr");
        help.append("\nDefault Value : ");
        if (option_type == OptionBool ||
            option_type == OptionInt ||
            (option_type == OptionString && !default_value.isEmpty()))
            help.append(QString(default_value));
        else if (option_type == OptionEnum) {
            int default_value_int = default_value.toInt();
            help.append(option_info->GetEnumMenu("fr")[default_value_int]);
        } else
            help.append("unknown");
        return help;
    } else if (role == StateRole) {
        if (option_type == OptionBool)
            return "OptionBool";
        else if (option_type == OptionEnum)
            return "OptionEnum";
        else if (option_type == OptionInt)
            return "OptionInt";
        else
            return "OptionString";
    } else if (role == EnumRole) {
        if (option_info == NULL)
            return QVariant(QList<QString>());
        return QVariant(option_info->GetEnumMenu("fr"));
    } else if (role == DefaultRole) {
        return default_value;
    } else if (role == SectionRole) {
        if (index.row() < known_application_options->length())
            return "App specific known options";
        else if (index.row() < known_application_options->length() + unknown_application_options->length())
            return "App specific unknown options";
        else
            return "Driver options";
    }
    return QVariant();
}

int DriModel::rowCount(const QModelIndex &parent) const
{
    (void) parent;
    return known_application_options->length() +
            unknown_application_options->length() +
            additional_options->length();
}

QVariant DriModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    (void) section;
    (void) orientation;
    (void) role;
    return QVariant();
}

Qt::ItemFlags DriModel::flags(const QModelIndex &index) const
{
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

bool DriModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < 0 || index.row() >= this->known_application_options->length() + this->unknown_application_options->length())
        return false;
    if (role != ContentRole)
        return false;

    QString option_name;
    if (index.row() < known_application_options->length()) {
        option_name = known_application_options->at(index.row());
    } else  {
        option_name = unknown_application_options->at(index.row()-known_application_options->length());
    }

    DriOptionValue *option;
    QString *temp_string, *old_value;
    QString new_value = value.toString();
    assert(application->GetOption(option_name, &option));
    option->GetValues(&temp_string, &old_value);
    assert(*temp_string == option_name);
    if (new_value == *old_value)
        return true;
    DriOptionValue *new_option = new DriOptionValue(option_name, new_value);
    application->RemoveOption(option_name);
    application->AddOption(new_option);
    this->save("/home/axel/.drirctest");
    emit dataChanged(index, index, QVector<int>(1, ContentRole));
    return true;
}

void DriModel::append(QString name, QString value)
{
    DriOptionValue *option;

    if (driver == NULL) {
        driver = new DriDriver(driver_name);
        drixml->addDriver(driver);
    }

    if (application == NULL) {
        application = new DriApplication(application_executable, application_name);
        driver->AddApplication(application);
    }

    if (application->GetOption(name, &option))
        return;
    DriOptionValue *new_option = new DriOptionValue(name, value);
    beginResetModel();
    application->AddOption(new_option);
    this->save("/home/axel/.drirctest");
    ApplicationOrDriverModified();
    endResetModel();
}

void DriModel::remove(QString name)
{
    DriOptionValue *option;
    if (!application->GetOption(name, &option))
        return;
    beginResetModel();
    application->RemoveOption(name);
    ApplicationOrDriverModified();
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
    roles[SectionRole] = "optionsection";
    return roles;
}

QString DriModel::getDriver()
{
    if (driver_name.isNull())
        return QString("All");
    return driver_name;
}

void DriModel::setDriver(QString driver)
{
    beginResetModel();
    if (driver == "All") {
        driver_name = QString();
        for (int i = 0; i < driver_list.length(); i++) {
            QString info = getDriverConfig(driver_list.at(i).toUtf8().data());
            QXmlStreamReader xml(info);
            drixml->loadXML(&xml);
        }
    }
    else {
        driver_name = driver;
        drixml->forgetInfo();
        QString info = getDriverConfig(driver_name.toUtf8().data());
        QXmlStreamReader xml(info);
        drixml->loadXML(&xml);
    }
    ApplicationOrDriverModified();
    endResetModel();
}

QString DriModel::getApplicationName()
{
    if (application_name.isNull())
        return QString("All Apps");
    return application_name;
}

void DriModel::setApplicationName(QString applicationName)
{
    if (applicationName == "All Apps")
        application_name = QString();
    else
        application_name = applicationName;
}

QString DriModel::getApplicationExec()
{
    if (application_executable.isNull())
        return QString("All Apps");
    return application_executable;
}

void DriModel::setApplicationExec(QString applicationExec)
{
    beginResetModel();
    if (applicationExec == "All Apps")
        application_executable = QString();
    else
        application_executable = applicationExec;
    ApplicationOrDriverModified();
    endResetModel();
}

QList<QString> DriModel::getApplicationNames() {
    QList<QString> ret;
    if (driver == NULL)
        return ret;
    ret = driver->GetApplicationsNames();
    return ret;
}

QList<QString> DriModel::getApplicationExecs()
{
    QList<QString> ret;
    if (driver == NULL)
        return ret;
    ret = driver->GetApplicationsExecutables();
    return ret;
}

void DriModel::setDriverList(QList<QString> driver_list)
{
    this->driver_list = driver_list;
}
