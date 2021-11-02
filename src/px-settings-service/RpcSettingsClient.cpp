#include "RpcSettingsClient.h"

RPCSettingsClient::RPCSettingsClient()
{

}

bool RPCSettingsClient::getModulesList(vector<string> &modules, vector<string> &errors)
{
    try{
        auto _rpcClient = new RPCClient<RPCSettingsService, RPCSettingsService::Client>(SETTINGS_SERVER_ADDRESS);
        _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSettingsService::Client &client) {
            auto listReq = client.getModulesListRequest();
            auto listRes = listReq.send().wait(ctx.waitScope);
            for (auto module : listRes.getModules()){
                modules.push_back(module.getTitle().cStr());
                // Module.getDescription().cStr();
                // Module.getIcon().cStr();
            }
        },errors);
        if(errors.size()) return false;
    }
    catch(exception& e)
    {
        errors.emplace_back(e.what());
        return false;
    }
    return true;
}

bool RPCSettingsClient::getModuleSections(const string &module, SettingsObjects::SettingsSectionList &sections,
                                          vector<string> &errors) {
    try{
        auto _rpcClient = new RPCClient<RPCSettingsService, RPCSettingsService::Client>(SETTINGS_SERVER_ADDRESS);
        _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSettingsService::Client &client) {
            auto sectionReq = client.getModuleSectionsRequest();
            sectionReq.setModule(module);
            auto sectionRes = sectionReq.send().wait(ctx.waitScope);
            for (const auto &sec : sectionRes.getSections())
                sections.push_back(SettingsObjects::rpc2settingsObj(sec));
        },errors);
        if(errors.size()) return false;
    } catch(exception& e) {
        errors.emplace_back(e.what());
        return false;
    }
    return true;
}

bool RPCSettingsClient::getSection(const string &module, const string &section,
                                   SettingsObjects::SettingsSectionList &sections,
                                   vector<string> &errors) {
    try{
        auto _rpcClient = new RPCClient<RPCSettingsService, RPCSettingsService::Client>(SETTINGS_SERVER_ADDRESS);
        _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSettingsService::Client &client) {
            auto sectionReq = client.getSectionRequest();
            sectionReq.setModule(module);
            sectionReq.setSection(section);
            auto sectionRes = sectionReq.send().wait(ctx.waitScope);
            for (const auto &sec : sectionRes.getSections())
                sections.push_back(SettingsObjects::rpc2settingsObj(sec));
        },errors);
        if(errors.size()) return false;
    } catch(exception& e) {
        errors.emplace_back(e.what());
        return false;
    }
    return true;
}

SettingsObjects::SettingsWriteResult
RPCSettingsClient::addToSection(const string &module, const string &section,
                                const SettingsObjects::SettingsSection &sectionValue) {
    SettingsObjects::SettingsWriteResult result;
    capnp::MallocMessageBuilder sectionBuilder;
    RPCSettingsSection::Builder rpcSettingsSection = sectionBuilder.initRoot<RPCSettingsSection>();
    if(!SettingsObjects::settingsObj2rpc(sectionValue,rpcSettingsSection));
    // TODO error handling
    try{
        auto _rpcClient = new RPCClient<RPCSettingsService, RPCSettingsService::Client>(SETTINGS_SERVER_ADDRESS);
        _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSettingsService::Client &client) {
            auto addSectionReq = client.addSectionRequest();
            addSectionReq.setModule(module);
            addSectionReq.setSection(section);
            addSectionReq.setSectionValue(rpcSettingsSection);
            auto addSectionRes = addSectionReq.send().wait(ctx.waitScope);
            if(addSectionRes.getResult().getResult())
                result.written = true;
            else {
                result.written = false;
                for(auto err : addSectionRes.getResult().getErrors())
                    result.errors.push_back(err);
            }
        },result.errors);
    }
    catch(exception& e)
    {
        result.errors.emplace_back(e.what());
    }
    return result;
}

SettingsObjects::SettingsWriteResult
RPCSettingsClient::editInSection(const string &module, const string &section, const int &id,
                                 const SettingsObjects::SettingsSection &sectionValue) {
    SettingsObjects::SettingsWriteResult result;
    capnp::MallocMessageBuilder sectionBuilder;
    RPCSettingsSection::Builder rpcSettingsSection = sectionBuilder.initRoot<RPCSettingsSection>();
    if(!SettingsObjects::settingsObj2rpc(sectionValue,rpcSettingsSection));
        // TODO error handling
    try{
        auto _rpcClient = new RPCClient<RPCSettingsService, RPCSettingsService::Client>(SETTINGS_SERVER_ADDRESS);
        _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSettingsService::Client &client) {
            auto editSectionReq = client.editSectionRequest();
            editSectionReq.setModule(module);
            editSectionReq.setSection(section);
            editSectionReq.setId(id);
            editSectionReq.setSectionValue(rpcSettingsSection);
            auto editSectionRes = editSectionReq.send().wait(ctx.waitScope);
            if(editSectionRes.getResult().getResult())
                result.written = true;
            else{
                result.written = false;
                for(auto err : editSectionRes.getResult().getErrors())
                    result.errors.push_back(err);
            }
        },result.errors);
    }
    catch(exception& e)
    {
        result.errors.emplace_back(e.what());
    }
    return result;
}

SettingsObjects::SettingsWriteResult RPCSettingsClient::removeFromSection(const string &module, const string &section,
                                                                          const int &id) {
    SettingsObjects::SettingsWriteResult result;
    try{
        auto _rpcClient = new RPCClient<RPCSettingsService, RPCSettingsService::Client>(SETTINGS_SERVER_ADDRESS);
        _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSettingsService::Client &client) {
            auto removeSectionReq = client.removeSectionRequest();
            removeSectionReq.setModule(module);
            removeSectionReq.setSection(section);
            removeSectionReq.setId(id);
            auto removeSectionRes = removeSectionReq.send().wait(ctx.waitScope);
            if(removeSectionRes.getResult().getResult())
                result.written = true;
            else{
                result.written = false;
                for(auto err : removeSectionRes.getResult().getErrors())
                    result.errors.push_back(err);
            }
        },result.errors);
    }
    catch(exception& e)
    {
        result.errors.emplace_back(e.what());
    }
    return result;
}

bool RPCSettingsClient::apply(string &module, vector<string> &errors) {
    try{
        auto _rpcClient = new RPCClient<RPCSettingsService, RPCSettingsService::Client>(SETTINGS_SERVER_ADDRESS);
        _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSettingsService::Client &client) {
            auto applyReq = client.applyRequest();
            applyReq.setModule(module);
            auto applyRes = applyReq.send().wait(ctx.waitScope);
            if(applyRes.getResult()==0) return true;
            else return false;
        },errors);
        if(errors.size()) return false;
    }
    catch(exception& e)
    {
        errors.emplace_back(e.what());
        return false;
    }
    return true;
}
