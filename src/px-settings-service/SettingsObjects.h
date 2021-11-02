//
// Created by root on 7/21/19.
//

#ifndef PX_SETTINGS_UI_SETTINGSOBJECTS_H
#define PX_SETTINGS_UI_SETTINGSOBJECTS_H

#include <string>
#include <vector>
#include <stdio.h>
#include <map>
#include <Settings.capnp.h>
#include <stdio.h>
#include <iostream>

using namespace std;
/// @brief A namespace for Settings Objects definitions and some methods for converting these objects.
namespace SettingsObjects {
    // ------------ px-settings-service/src/Plugins/PluginInterface.h
    typedef vector<string> StringList;
    typedef map<string, string> StrStrMap;


    struct Param {
        enum Type {
            Bool=1,
            Numeric,
            Text,
            ListNumeric,
            ListText,
            Enum,
            File,
            ListFile,
            Directory,
            ListDirectory,
            Barcode2D,
            Barcode3D
        };
        enum Attribute {
            Required = 1,
            Protected = 2,
            Readonly = 4,
            List = 8,
            Hidden = 16,
        };
    };

    struct SettingsParam {
        int                 id;
        string              key;
        string              title;
        StringList          value;
        Param::Type         type;
        int                 attributes;
        StringList          enumValues;
        string              description;
    };
    typedef vector<SettingsParam> SettingsParamList;

    struct SettingsSection {
        int                     id;
        string                  name;
        string                  title;
        string                  icon;
        int                     attributes;
        string                  description;
        vector<SettingsSection> subsections;
        SettingsParamList       params;
    };
    typedef vector<SettingsSection> SettingsSectionList;

    struct SettingsWriteResult {
        bool             written;
        StringList       errors;
    };
    // --------------------------------------------------------------
    SettingsObjects::Param::Type rpcType2SettingsType(RPCSettingsParam::Type rpcType);
    SettingsSection rpc2settingsObj(RPCSettingsSection::Reader rpc);

    bool settingsObj2rpc(SettingsSection section, RPCSettingsSection::Builder &rpc);
    RPCSettingsParam::Type settingsType2rpc(SettingsObjects::Param::Type);

    string toString(SettingsSection section);
};


#endif //PX_SETTINGS_UI_SETTINGSOBJECTS_H
