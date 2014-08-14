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

#include <QtCore>
#include <iostream>

#include "drixml.h"

// EnumLangInfo

EnumLangInfo::EnumLangInfo(QXmlStreamReader *xml)
{
    QXmlStreamAttributes attributes = xml->attributes();

    if (attributes.hasAttribute("value")) {
        value = attributes.value("value").toString();
    } else
        throw "Missing value element to enum";

    if (attributes.hasAttribute("text"))
        text = attributes.value("text").toString();
    else
        throw "Missing text element to enum";

    xml->skipCurrentElement();
}

// DescriptionLangInfo

DescriptionLangInfo::DescriptionLangInfo(QXmlStreamReader *xml)
{
    QXmlStreamAttributes attributes = xml->attributes();

    if (attributes.hasAttribute("lang"))
        language = attributes.value("lang").toString();
    else
        throw "Missing lang element to description";

    if (attributes.hasAttribute("text"))
        text = attributes.value("text").toString();
    else
        throw "Missing text element to description";

    enum_info.clear();
    if (!xml->readNextStartElement()) {
        return;
    }
    do {
        if (xml->name() == "enum")
            enum_info.append(new EnumLangInfo(xml));
        else
            xml->skipCurrentElement();
    } while (xml->readNextStartElement());
}

QList<QString> DescriptionLangInfo::GetEnumMenu()
{
    QList<QString> ret;

    for (int i = 0; i < enum_info.length(); i++)
    {
        EnumLangInfo *enu = enum_info.at(i);
        ret.append(enu->value + ": " + enu->text);
    }
    return ret;
}

// DriOptionInfo

DriOptionInfo::DriOptionInfo(QXmlStreamReader *xml)
{
    QXmlStreamAttributes attributes = xml->attributes();

    if (attributes.hasAttribute("name"))
        name = attributes.value("name").toString();
    else
        throw "Missing name element to option info";

    if (attributes.hasAttribute("type")) {
        QString type_string = attributes.value("type").toString();
        if (type_string == "bool")
            type = OptionBool;
        else if (type_string == "int")
            type = OptionInt;
        else if (type_string == "enum")
            type = OptionEnum;
        else if (type_string == "string")
            type = OptionString;
        else
            throw "Unknown type element to option info";
    } else
        throw "Missing type element to option info";

    if (attributes.hasAttribute("default"))
        default_value = attributes.value("default").toString();
    else
        default_value.clear();

    if (type == OptionEnum) {
        if (!attributes.hasAttribute("valid"))
            throw "Missing valid element to option info with enum";

        QString valid_string = attributes.value("valid").toString();
        QStringList valid_string_list = valid_string.split(':');
        bool ok = 0;

        if (!valid_string_list.count() == 2)
            throw "Invalid valid element to option info with enum";

        min_valid = valid_string_list[0].toInt(&ok);
        if (!ok)
            throw "Invalid valid element to option info with enum";
        max_valid = valid_string_list[1].toInt(&ok);
        if (!ok || min_valid > max_valid)
            throw "Invalid valid element to option info with enum";
    }

    descriptions.clear();
    if (!xml->readNextStartElement()) {
        return;
    }
    do {
        if (xml->name() == "description")
            descriptions.append(new DescriptionLangInfo(xml));
        else
            xml->skipCurrentElement();
    } while (xml->readNextStartElement());
}

QString DriOptionInfo::GetName()
{
    return QString(name);
}

QString DriOptionInfo::GetDefaultValue()
{
    return QString(default_value);
}

QString DriOptionInfo::GetDescription(QString lang)
{
    if (descriptions.length() == 0)
        return QString("no description");

    for (int i = 0; i < descriptions.length(); i++)
    {
        DescriptionLangInfo *description = descriptions.at(i);
        if (description->language == lang)
            return QString(description->text);
    }
    for (int i = 0; i < descriptions.length(); i++)
    {
        DescriptionLangInfo *description = descriptions.at(i);
        if (description->language == "en")
            return QString(description->text);
    }
    DescriptionLangInfo *description = descriptions.at(0);
    return QString(description->text);
}

OptionType DriOptionInfo::GetType()
{
    return type;
}

QList<QString> DriOptionInfo::GetEnumMenu(QString lang)
{

    for (int i = 0; i < descriptions.length(); i++)
    {
        DescriptionLangInfo *description = descriptions.at(i);
        if (description->language == lang)
            return description->GetEnumMenu();
    }
    for (int i = 0; i < descriptions.length(); i++)
    {
        DescriptionLangInfo *description = descriptions.at(i);
        if (description->language == "en")
            return description->GetEnumMenu();
    }
    DescriptionLangInfo *description = descriptions.at(0);
    return description->GetEnumMenu();
}

// DriOptionsSectionInfo

DriOptionsSectionInfo::DriOptionsSectionInfo(QXmlStreamReader *xml)
{
    options.clear();
    descriptions.clear();

    while (xml->readNextStartElement()) {
        if (xml->name() == "option")
            options.append(new DriOptionInfo(xml)); // perhaps do with insertList
        else if (xml->name() == "description")
            descriptions.append(new DescriptionLangInfo(xml));
        else
            xml->skipCurrentElement();
    }
}

QList<QString> DriOptionsSectionInfo::GetOptionNames()
{
    QList<QString> names;
    for (int i = 0; i < options.length(); i++)
    {
        DriOptionInfo *option = options.at(i);
        names.append(option->GetName());
    }
    return names;
}

QString DriOptionsSectionInfo::GetDescription(QString lang)
{
    if (descriptions.count() == 0)
        return QString("no description");

    for (int i = 0; i < descriptions.length(); i++)
    {
        DescriptionLangInfo *description = descriptions.at(i);
        if (description->language == lang)
            return QString(description->text);
    }
    for (int i = 0; i < descriptions.length(); i++)
    {
        DescriptionLangInfo *description = descriptions.at(i);
        if (description->language == "en")
            return QString(description->text);
    }
    DescriptionLangInfo *description = descriptions.at(0);
    return QString(description->text);
}

bool DriOptionsSectionInfo::FindOption(QString name, DriOptionInfo **option)
{
    for (int i = 0; i < options.length(); i++)
    {
        DriOptionInfo *option_current = options.at(i);
        if (name == option_current->GetName()) {
            *option = option_current;
            return true;
        }
    }
    return false;
}

// DriOption

DriOptionValue::DriOptionValue(QString name, QString value)
{
    this->name = name;
    this->value = value;
}


DriOptionValue::DriOptionValue(QXmlStreamReader *xml)
{
    QXmlStreamAttributes attributes = xml->attributes();

    if (attributes.hasAttribute("name"))
        name = attributes.value("name").toString();
    else
        throw "Missing name element to option";
    if (attributes.hasAttribute("value"))
        value = attributes.value("value").toString(); // We'll have to check after the type
    else
        throw "Missing value element to option";
    xml->skipCurrentElement();
}

void DriOptionValue::save(QXmlStreamWriter *xml)
{
    xml->writeStartElement("option");
    xml->writeAttribute("name", name);
    xml->writeAttribute("value", value);
    xml->writeEndElement();
}

DriOptionValue::DriOptionValue(const DriOptionValue& other)
{
    name = QString(other.name);
    value = QString(other.value);
}

DriOptionValue::~DriOptionValue()
{
    //name.~QString();
    //value.~QString();
}

void DriOptionValue::insertList(QList<DriOptionValue *> *options, DriOptionValue *new_option)
{
    for (int i = 0; i < options->length(); i++)
    {
        DriOptionValue *option = options->at(i);
        if (option->name == new_option->name) {
            if (option->value != new_option->value)
                throw "option defined two times with different value";
            else
                option->value = new_option->value;
            delete new_option;
            return;
        }
    }
    for (int i = 0; i < options->length(); i++)
    {
        DriOptionValue *option = options->at(i);
        if (*new_option < *option) {
            options->insert(i, new_option);
            return;
        }
    }
    options->append(new_option);
}

void DriOptionValue::RemoveFromList(QList<DriOptionValue *> *options, DriOptionValue *option)
{
    options->removeAll(option);
}

void DriOptionValue::RemoveFromList(QList<DriOptionValue *> *options, QString name)
{
    for (int i = 0; i < options->length(); i++)
    {
        DriOptionValue *option = options->at(i);
        if (option->name == name) {
            options->removeAt(i);
            delete option;
            return;
        }
    }
}

void DriOptionValue::MergeLists(QList<DriOptionValue *> *options, QList<DriOptionValue *> *new_options)
{
    for (int i = 0; i < new_options->length(); i++)
    {
        DriOptionValue *new_option = new_options->at(i);
        DriOptionValue::insertList(options, new_option);
    }
}

bool DriOptionValue::operator<(const DriOptionValue &other)
{
    return name <= other.name;
}

void DriOptionValue::GetValues(QString **name, QString **value)
{
    *name = new QString(this->name);
    *value = new QString(this->value);
}

// DriApplication

DriApplication::DriApplication(QString executable, QString name)
{
    this->executable = executable;
    this->name = name;
    options.clear();
}

DriApplication::DriApplication(QXmlStreamReader *xml)
{
    QXmlStreamAttributes attributes = xml->attributes();

    options.clear();
    if (attributes.hasAttribute("name"))
        name = attributes.value("name").toString();
    else
        name.clear();

    if (attributes.hasAttribute("executable"))
        executable = attributes.value("executable").toString();
    else
        executable.clear();

    while (xml->readNextStartElement()) {
        if (xml->name() == "option")
            DriOptionValue::insertList(&options, new DriOptionValue(xml));
        else
            xml->skipCurrentElement();
    }
}

void DriApplication::save(QXmlStreamWriter *xml)
{
    xml->writeStartElement("application");
    if (!name.isNull())
        xml->writeAttribute("name", name);
    if (!executable.isNull())
        xml->writeAttribute("executable", executable);

    for (int i = 0; i < options.length(); i++)
    {
        DriOptionValue *option = options[i];
        option->save(xml);
    }
    xml->writeEndElement();
}

DriApplication::DriApplication(const DriApplication& other)
{
    name = QString(other.name);
    executable = QString(other.executable);
    options.clear();
    for (int i = 0; i < other.options.length(); i++) {
        DriOptionValue *option = other.options.at(i);
        DriOptionValue *new_option = new DriOptionValue(*option);
        options.append(new_option);
    }
}

DriApplication::~DriApplication()
{
    //name.~QString();
    //executable.~QString();
    for (int i = 0; i < options.length(); i++) {
        DriOptionValue *option = options.at(i);
        delete option;
    }
    //options.~QList();
}

void DriApplication::insertList(QList<DriApplication *> *applications, DriApplication *new_application)
{
    for (int i = 0; i < applications->length(); i++)
    {
        DriApplication *application = applications->at(i);
        if (application->name == new_application->name) {
            if (application->executable != new_application->executable)
                throw "application defined two times with different executable";
            DriOptionValue::MergeLists(&application->options, &new_application->options);
            delete new_application;
            return;
        }
    }
    applications->append(new_application);
}

void DriApplication::RemoveFromList(QList<DriApplication *> *applications, DriApplication *application)
{
    applications->removeAll(application);
}

void DriApplication::RemoveFromList(QList<DriApplication *> *applications, QString name)
{
    for (int i = 0; i < applications->length(); i++)
    {
        DriApplication *application = applications->at(i);
        if (application->name == name) {
            applications->removeAt(i);
            delete application;
            return;
        }
    }
}

void DriApplication::MergeLists(QList<DriApplication *> *applications, QList<DriApplication *> *new_applications)
{
    for (int i = 0; i < new_applications->length(); i++)
    {
        DriApplication *new_application = new_applications->at(i);
        DriApplication::insertList(applications, new_application);
    }
}

bool DriApplication::GetName(QString **name)
{
    if (this->name.isNull())
        return false;
    *name = new QString(this->name);
    return true;
}

bool DriApplication::GetExecutable(QString **executable)
{
    if (this->executable.isNull())
        return false;
    *executable = new QString(this->executable);
    return true;
}

bool DriApplication::GetOption(QString name, DriOptionValue **option)
{
    for (int i = 0; i < options.length(); i++) {
        DriOptionValue *option_current = options.at(i);
        QString *option_name, *option_value;
        option_current->GetValues(&option_name, &option_value);
        if (name == *option_name) {
            *option = option_current;
            return true;
        }
    }
    return false;
}

QList<DriOptionValue> *DriApplication::GetOptions()
{
    QList<DriOptionValue> *ret = new QList<DriOptionValue>();

    for (int i = 0; i < options.length(); i++) {
        DriOptionValue *option = options.at(i);
        DriOptionValue new_option = DriOptionValue(*option);
        ret->append(new_option);
    }

    return ret;
}

void DriApplication::GetOptionsMatch(QList<QString> options_names, QList<DriOptionValue> **matches, QList<DriOptionValue> **non_matches)
{
    *matches = new QList<DriOptionValue>();
    (*matches)->clear();
    *non_matches = new QList<DriOptionValue>();
    (*non_matches)->clear();
    qSort(options_names.begin(), options_names.end());
    for (int option_name_index = 0, current_name_index = 0; option_name_index < options_names.count() && current_name_index < options.count();) {
        QString option_name = options_names[option_name_index];
        QString *current_name, *current_value;
        DriOptionValue *option = options[current_name_index];
        option->GetValues(&current_name, &current_value);
        if (*current_name == option_name) {
            (*matches)->append(DriOptionValue(*option));
            current_name_index++;
            option_name_index++;
        } else if (*current_name < option_name) {
            (*non_matches)->append(DriOptionValue(*option));
            current_name_index++;
        } else {
            option_name_index++;
        }

    }
}

int DriApplication::count()
{
    return options.count();
}

bool DriApplication::AddOption(DriOptionValue *option)
{
    if (options.count(option) >= 1)
        return false;

    DriOptionValue::insertList(&options, option);
    return true;
}

bool DriApplication::RemoveOption(DriOptionValue *option)
{
    if (options.count(option) == 0)
        return false;
    DriOptionValue::RemoveFromList(&options, option);
    return true;
}

bool DriApplication::RemoveOption(QString name)
{
    DriOptionValue *temp;

    if (!this->GetOption(name, &temp))
        return false;
    DriOptionValue::RemoveFromList(&options, name);
    return true;
}

// DriDriver

DriDriver::DriDriver(QString driver, int screen)
{
    applications.clear();
    this->driver = driver;
    this->screen = screen;
}

DriDriver::DriDriver(QXmlStreamReader *xml)
{
    QXmlStreamAttributes attributes = xml->attributes();

    applications.clear();

    if (attributes.hasAttribute("driver"))
        driver = attributes.value("driver").toString();
    else
        driver.clear();

    if (attributes.hasAttribute("screen")) {
        bool ok = 0;
        screen = attributes.value("screen").toInt(&ok);
    } else
        screen = -1;

    while (xml->readNextStartElement()) {
        if (xml->name() == "application")
            DriApplication::insertList(&applications, new DriApplication(xml));
        else
            xml->skipCurrentElement();
    }
}

void DriDriver::save(QXmlStreamWriter *xml)
{
    xml->writeStartElement("device");
    if (!driver.isNull())
        xml->writeAttribute("driver", driver);
    if (screen != -1)
        xml->writeAttribute("screen", QString::number(screen));

    for (int i = 0; i < applications.length(); i++)
    {
        DriApplication *application = applications[i];
        application->save(xml);
    }
    xml->writeEndElement();
}

DriDriver::DriDriver(const DriDriver& other)
{
    driver = QString(other.driver);
    screen = other.screen;
    applications.clear();
    for (int i = 0; i < other.applications.length(); i++) {
        DriApplication *application = other.applications.at(i);
        DriApplication *new_application = new DriApplication(*application);
        applications.append(new_application);
    }
}

DriDriver::~DriDriver()
{
    //driver.~QString();
    for (int i = 0; i < applications.length(); i++) {
        DriApplication *application = applications.at(i);
        delete application;
    }
    //applications.~QList();
}

void DriDriver::insertList(QList<DriDriver *> *drivers, DriDriver *new_driver)
{
    for (int i = 0; i < drivers->length(); i++)
    {
        DriDriver *driver = drivers->at(i);
        if (driver->driver == new_driver->driver) {
            if (driver->screen != new_driver->screen)
                continue;
            DriApplication::MergeLists(&driver->applications, &new_driver->applications);
            delete new_driver;
            return;
        }
    }
    drivers->append(new_driver);
}

void DriDriver::Merge(DriDriver * driver1, DriDriver *driver2)
{
    DriApplication::MergeLists(&driver1->applications, &driver2->applications);
    delete driver2;
}

bool DriDriver::GetName(QString **name)
{
    if (this->driver.isNull())
        return false;
    *name = new QString(this->driver);
    return true;
}

bool DriDriver::GetScreen(int *screen)
{
    if (this->screen == -1)
        return false;
    *screen = this->screen;
    return true;
}

bool DriDriver::GetApplication(QString name, DriApplication **application)
{
    for (int i = 0; i < applications.length(); i++) {
        DriApplication *application_current = applications.at(i);
        QString *application_name;
        if ((!application_current->GetName(&application_name) &&
            name.isNull()) ||
            (name == *application_name)) {
            *application = application_current;
            return true;
        }
    }
    return false;
}

bool DriDriver::AddApplication(DriApplication *application)
{
    if (applications.count(application) >= 1)
        return false;
    DriApplication::insertList(&applications, application);
    return true;
}

bool DriDriver::RemoveApplication(DriApplication *application)
{
    if (applications.count(application) == 0)
        return false;
    DriApplication::RemoveFromList(&applications, application);
    return true;
}

bool DriDriver::RemoveApplication(QString name) //TODO
{
    DriApplication::RemoveFromList(&applications, name);
    return true;
}


// DriConf

bool Drixml::getDriver(QString name, DriDriver **driver)
{
    for (int i = 0; i < drivers.length(); i++) {
        DriDriver *driver_current = drivers.at(i);
        QString *driver_name;
        if ((!driver_current->GetName(&driver_name) &&
            name.isNull()) ||
            (name == *driver_name)) {
            *driver = driver_current;
            return true;
        }
    }
    return false;
}

void Drixml::addDriver(DriDriver *driver)
{
    QString *name;
    bool hasname = driver->GetName(&name);

    if (hasname && *name == "loader") {
        if (driver_loader)
            DriDriver::Merge(driver_loader, driver);
        else
            driver_loader = driver;
        return;
    }
    DriDriver::insertList(&drivers, driver);
}

bool Drixml::loadFile(QFile *file)
{
    QXmlStreamReader xml;
    xml.setDevice(file);

    return this->loadXML(&xml);
}

bool Drixml::loadXML(QXmlStreamReader *xml)
{
    if (xml->readNextStartElement()) {
        if (xml->name() == "driconf" || xml->name() == "driinfo")
            loadDriConf(xml);
        else
            xml->raiseError(QObject::tr("not reading a driconf file"));
    }

    return !xml->error();
}

void Drixml::loadDriConf(QXmlStreamReader *xml)
{
    while (xml->readNextStartElement()) {
        if (xml->name() == "device" &&
            xml->attributes().hasAttribute("driver") &&
            xml->attributes().value("driver") == "loader") {
            DriDriver *driver = new DriDriver(xml);
            if (driver_loader)
                DriDriver::Merge(driver_loader, driver);
            else
                driver_loader = driver;
        }
        else if (xml->name() == "device")
            DriDriver::insertList(&drivers, new DriDriver(xml));
        else if (xml->name() == "section")
            infos.append(new DriOptionsSectionInfo(xml));//perhaps do with insertList
        else
            xml->skipCurrentElement();
    }
}

void Drixml::RemoveDriver(DriDriver *driver)
{
    if (driver_loader == driver) {
        driver_loader = NULL;
        delete driver_loader;
        return;
    }
    drivers.removeAll(driver);
    return;
}

void Drixml::RemoveDriver(QString name)
{
    DriDriver *driver;

    if (name == "loader") {
        RemoveDriver(driver_loader);
        return;
    }
    if (Drixml::getDriver(name, &driver)) {
        RemoveDriver(driver);
        return;
    }
}

void Drixml::saveFile(QFile *file)
{
    QXmlStreamWriter xml(file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("driconf");
    if (driver_loader)
        driver_loader->save(&xml);
    for (int i = 0; i < drivers.length(); i++)
    {
        DriDriver *driver = drivers[i];
        driver->save(&xml);
    }
    xml.writeEndElement();
    xml.writeEndDocument();
}

bool Drixml::FindOptionInfo(QString name, DriOptionInfo **option)
{
    for (int i = 0; i < infos.length(); i++)
    {
        DriOptionsSectionInfo *section = infos.at(i);
        if (section->FindOption(name, option))
            return true;
    }
    return false;
}


Drixml::Drixml()
{
    driver_loader = NULL;
    drivers.clear();
}

Drixml::~Drixml()
{
    for (int i = 0; i < drivers.length(); i++)
    {
        DriDriver *driver = drivers[i];
        delete driver;
    }
    //drivers.~QList();
}


/*int main(int argc, char *argv[])
{
    Drixml drixml = Drixml();
    QFile *input = new QFile("/home/axel/.drirc");
    QFile *output = new QFile("/home/axel/.drirc_copy");
    input->open(QIODevice::ReadOnly);
    output->open(QIODevice::WriteOnly);
    drixml.loadFile(input);
    drixml.saveFile(output);
    output->flush();
    return 0;
}*/
