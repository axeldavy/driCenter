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

#ifndef DRIXML_H
#define DRIXML_H

#include <QtCore>
#include <QFile>

class DriApplication;
class DriDriver;
class Drixml;

class EnumLangInfo {
public:
    QString value;
    QString text;
    EnumLangInfo(QXmlStreamReader *xml);
};

class DescriptionLangInfo {
public:
    QString language;
    QString text;
    QList<EnumLangInfo *> enum_info;
    DescriptionLangInfo(QXmlStreamReader *xml);
    QList<QString> GetEnumMenu();
};

enum OptionType { OptionBool, OptionEnum, OptionString, OptionInt };

class DriOptionInfo {
private:
    QList<DescriptionLangInfo *> descriptions;
    QString name;
    OptionType type;
    QString default_value;
    int min_valid;
    int max_valid;
public:
    DriOptionInfo(QXmlStreamReader *xml);
    QString GetName();
    QString GetDefaultValue();
    QString GetDescription(QString lang);
    OptionType GetType();
    QList<QString> GetEnumMenu(QString lang);
};

class DriOptionsSectionInfo {
private:
    QList<DescriptionLangInfo *> descriptions;
    QList<DriOptionInfo *> options;
public:
    DriOptionsSectionInfo(QXmlStreamReader *xml);
    QList<QString> GetOptionNames();
    QString GetDescription(QString lang);
    bool FindOption(QString name, DriOptionInfo **option);
};

class DriOptionValue {
private:
    DriApplication *parent;
    QString name;
    QString value;

public:
    // Creation/Save
    DriOptionValue(QXmlStreamReader *xml);
    DriOptionValue(QString name, QString value);
    void save(QXmlStreamWriter *xml);
    // Internals
    DriOptionValue(const DriOptionValue& other);
    ~DriOptionValue();
    static void insertList(QList<DriOptionValue *> *options, DriOptionValue *new_option);
    static void RemoveFromList(QList<DriOptionValue *> *options, DriOptionValue *option);
    static void RemoveFromList(QList<DriOptionValue *> *options, QString name);
    static void MergeLists(QList<DriOptionValue *> *options, QList<DriOptionValue *> *new_options);
    bool operator<(const DriOptionValue &other);
    // Manipulation
    void GetValues(QString **name, QString **value);
};

class DriApplication {
private:
    DriDriver *parent;
    QString name;
    QString executable;
    QList<DriOptionValue *> options;

public:
    // Creation/Save
    DriApplication(QXmlStreamReader *xml);
    DriApplication(QString executable, QString name);
    void save(QXmlStreamWriter *xml);
    // Internals
    DriApplication(const DriApplication& other);
    ~DriApplication();
    static void insertList(QList<DriApplication *> *applications, DriApplication *application);
    static void RemoveFromList(QList<DriApplication *> *applications, DriApplication *application);
    static void RemoveFromList(QList<DriApplication *> *applications, QString name);
    static void MergeLists(QList<DriApplication *> *applications, QList<DriApplication *> *new_applications);
    // Manipulation
    bool GetName(QString **name);
    bool GetExecutable(QString **executable);
    bool GetOption(QString name, DriOptionValue **option);
    QList<DriOptionValue> *GetOptions();
    void GetOptionsMatch(QList<QString> options_names, QList<DriOptionValue> **matches, QList<DriOptionValue> **non_matches);
    int count();
    bool AddOption(DriOptionValue *option);
    bool RemoveOption(DriOptionValue *option);
    bool RemoveOption(QString name);
};

class DriDriver {
private:
    Drixml *parent;
    QString driver;
    int screen;
    QList<DriApplication *> applications;

public:
    // Creation/Save
    DriDriver(QString driver, int screen = -1);
    DriDriver(QXmlStreamReader *xml);
    void save(QXmlStreamWriter *xml);
    // Internals
    DriDriver(const DriDriver& other);
    ~DriDriver();
    static void insertList(QList<DriDriver *> *drivers, DriDriver *new_driver);
    static void Merge(DriDriver * driver1, DriDriver *driver2);
    // Manipulation
    bool GetName(QString **name);
    bool GetScreen(int *screen);
    bool GetApplication(QString name, DriApplication **application);
    bool AddApplication(DriApplication *application);
    bool RemoveApplication(DriApplication *application);
    bool RemoveApplication(QString name);

};

class Drixml
{
private:
    QList<DriDriver *> drivers;
    DriDriver * driver_loader;
    QList<DriOptionsSectionInfo *> infos;
    void loadDriConf(QXmlStreamReader *xml);

public:
    Drixml();
    ~Drixml();
    bool getDriver(QString name, DriDriver **driver);
    void addDriver(DriDriver *driver);
    bool loadFile(QFile *file);
    bool loadXML(QXmlStreamReader *xml);
    void RemoveDriver(DriDriver *driver);
    void RemoveDriver(QString name);
    void saveFile(QFile *file);
    bool FindOptionInfo(QString name, DriOptionInfo **option);
};

#endif // DRIXML_H
