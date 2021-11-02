//
// Created by root on 7/21/19.
//

#include "SettingsObjects.h"

std::map<SettingsObjects::Param::Type, std::string> paramTypeMapString  = {  
                    {SettingsObjects::Param::Type::Bool          , "Bool"            },
                    {SettingsObjects::Param::Type::Numeric       , "Numeric"         },
                    {SettingsObjects::Param::Type::Text          , "Text"            },
                    {SettingsObjects::Param::Type::ListNumeric   , "ListNumeric"     },
                    {SettingsObjects::Param::Type::ListText      , "ListText"        },
                    {SettingsObjects::Param::Type::Enum          , "Enum"            },
                    {SettingsObjects::Param::Type::File          , "File"            },
                    {SettingsObjects::Param::Type::ListFile      , "ListFile"        },
                    {SettingsObjects::Param::Type::Directory     , "Directory"       },
                    {SettingsObjects::Param::Type::ListDirectory , "ListDirectory"   },
                    {SettingsObjects::Param::Type::Barcode2D     , "Barcode2D"       },
                    {SettingsObjects::Param::Type::Barcode3D     , "Barcode3D"       }};

std::map<SettingsObjects::Param::Attribute, std::string> paramAttrMapString = {
                    {SettingsObjects::Param::Attribute::Required    , "Required"    },
                    {SettingsObjects::Param::Attribute::Protected   , "Protected"   },
                    {SettingsObjects::Param::Attribute::Readonly    , "Readonly"    },
                    {SettingsObjects::Param::Attribute::List        , "List"        },
                    {SettingsObjects::Param::Attribute::Hidden      , "Hidden"      }};
                     
SettingsObjects::Param::Type SettingsObjects::rpcType2SettingsType(RPCSettingsParam::Type rpcType){
    if(rpcType==RPCSettingsParam::Type::BOOL)
        return SettingsObjects::Param::Type::Bool;
    else if(rpcType==RPCSettingsParam::Type::ENUM)
        return SettingsObjects::Param::Type::Enum;
    else if(rpcType==RPCSettingsParam::Type::LIST_NUMERIC)
        return SettingsObjects::Param::Type::ListNumeric;
    else if(rpcType==RPCSettingsParam::Type::LIST_TEXT)
        return SettingsObjects::Param::Type::ListText;
    else if(rpcType==RPCSettingsParam::Type::NUMERIC)
        return SettingsObjects::Param::Type::Numeric;
    else if(rpcType==RPCSettingsParam::Type::TEXT)
        return SettingsObjects::Param::Type::Text;
    else if(rpcType==RPCSettingsParam::Type::FILE)
        return SettingsObjects::Param::Type::File;
    else if(rpcType==RPCSettingsParam::Type::LIST_FILE)
        return SettingsObjects::Param::Type::ListFile;
    else if(rpcType==RPCSettingsParam::Type::DIRECTORY)
        return SettingsObjects::Param::Type::Directory;
    else if(rpcType==RPCSettingsParam::Type::LIST_DIRECTORY)
        return SettingsObjects::Param::Type::ListDirectory;
    else if(rpcType==RPCSettingsParam::Type::BARDCODE2D)
        return SettingsObjects::Param::Type::Barcode2D;
    else if(rpcType==RPCSettingsParam::Type::BARDCODE3D)
        return SettingsObjects::Param::Type::Barcode3D;
    return SettingsObjects::Param::Type::Text;
}

SettingsObjects::SettingsSection SettingsObjects::rpc2settingsObj(RPCSettingsSection::Reader rpc) {
    SettingsObjects::SettingsSection section;
    section.id=rpc.getId();
    section.name=rpc.getName().cStr();
    section.title=rpc.getTitle().cStr();
    section.description=rpc.getDescription().cStr();
    section.attributes=rpc.getAttributes();
    section.icon=rpc.getIcon().cStr();
    for (auto par : rpc.getParams()) {
        SettingsObjects::SettingsParam param;
        param.id = par.getId();
        param.attributes = par.getAttributes();
        param.description = par.getDescription().cStr();
        for(auto val : par.getValue())
            param.value.push_back(val);
        param.key = par.getKey().cStr();
        param.title = par.getTitle().cStr();
        param.type = rpcType2SettingsType(par.getType());
        if(param.type == SettingsObjects::Param::Type::Enum)
            for(auto eval : par.getEnumValues())
                param.enumValues.push_back(eval);
        section.params.push_back(param);
    }
    for (auto subsec : rpc.getSubsections()){
        SettingsSection sub;
        sub = rpc2settingsObj(subsec);
        section.subsections.push_back(sub);
    }
    return section;
}

bool SettingsObjects::settingsObj2rpc(SettingsObjects::SettingsSection section, RPCSettingsSection::Builder &rpc) {
    rpc.setAttributes(section.attributes);
    rpc.setDescription(section.description);
    rpc.setIcon(section.icon);
    rpc.setId(section.id);
    rpc.setName(section.name);
    rpc.setTitle(section.title);

    ::capnp::List<RPCSettingsSection>::Builder rpcSubsections = rpc.initSubsections(section.subsections.size());
    for(int s=0; s<section.subsections.size(); s++){
        RPCSettingsSection::Builder subsec = rpcSubsections[s];
        settingsObj2rpc(section.subsections.at(s),subsec);
    }

    ::capnp::List<RPCSettingsParam>::Builder rpcParams = rpc.initParams(section.params.size());
    int i = 0;
    for(auto par:section.params){
        rpcParams[i].setId(par.id);
        rpcParams[i].setDescription(par.description);
        rpcParams[i].setAttributes(par.attributes);
        ::capnp::List<::capnp::Text>::Builder rpcParamValues = rpcParams[i].initValue(par.value.size());
        for(int v=0; v<par.value.size(); v++)
            rpcParamValues.set(v, par.value.at(v));

        ::capnp::List<::capnp::Text>::Builder rpcParamEnumValues = rpcParams[i].initEnumValues(par.enumValues.size());
        for(int v=0; v<par.enumValues.size(); v++)
            rpcParamEnumValues.set(v, par.enumValues.at(v));
        rpcParams[i].setKey(par.key);
        rpcParams[i].setTitle(par.title);
        rpcParams[i].setType(settingsType2rpc(par.type));

        i++;
    }
    return true;
}

RPCSettingsParam::Type SettingsObjects::settingsType2rpc(SettingsObjects::Param::Type type) {
    if(type == SettingsObjects::Param::Type::Bool)
        return RPCSettingsParam::Type::BOOL;
    else if(type == SettingsObjects::Param::Type::Text)
        return RPCSettingsParam::Type::TEXT;
    if(type == SettingsObjects::Param::Type::Numeric)
        return RPCSettingsParam::Type::NUMERIC;
    if(type == SettingsObjects::Param::Type::Enum)
        return RPCSettingsParam::Type::ENUM;
    if(type == SettingsObjects::Param::Type::ListNumeric)
        return RPCSettingsParam::Type::LIST_NUMERIC;
    if(type == SettingsObjects::Param::Type::ListText)
        return RPCSettingsParam::Type::LIST_TEXT;
    if(type == SettingsObjects::Param::Type::File)
        return RPCSettingsParam::Type::FILE;
    if(type == SettingsObjects::Param::Type::ListFile)
        return RPCSettingsParam::Type::LIST_FILE;
    if(type == SettingsObjects::Param::Type::Directory)
        return RPCSettingsParam::Type::DIRECTORY;
    if(type == SettingsObjects::Param::Type::ListDirectory)
        return RPCSettingsParam::Type::LIST_DIRECTORY;
    if(type == SettingsObjects::Param::Type::Barcode2D)
        return RPCSettingsParam::Type::BARDCODE2D;
    if(type == SettingsObjects::Param::Type::Barcode3D)
        return RPCSettingsParam::Type::BARDCODE3D;
    return RPCSettingsParam::Type::TEXT;
}

string paramAttributeToString (int attr){
    string result = "";
    for (int i = 1; i <= SettingsObjects::Param::Attribute::Hidden; i*=2){
        if(attr & i)
            result += paramAttrMapString[(SettingsObjects::Param::Attribute)i] + ", ";
    }
    return "{ " + result + " }";
}

string SettingsObjects::toString(SettingsSection section){
    string str =  " +" + to_string(section.id) + ", " +
                  section.name + " ," + section.title + " ," + section.icon + " ," + paramAttributeToString(section.attributes) +
                  ", " + section.description + "\n";
    for(auto par : section.params){
        string val;
        for(auto v:par.value)
            val += v + ",";
        string eval;
        for(auto v:par.enumValues)
            eval += v + ",";
        str+= "   - " + to_string(par.id) + " ," + par.key + " ," + par.title + ",{"+ val + "/" + eval + "} ," + paramTypeMapString[par.type] + " ," + paramAttributeToString(par.attributes) + " ," + par.description + "\n";
    }
    for(auto subsec: section.subsections)
        str += " "+toString(subsec);
    return str;
}