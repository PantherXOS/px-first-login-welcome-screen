#ifndef RPCSETTINGSCLIENT_H
#define RPCSETTINGSCLIENT_H

#include <string>
#include <vector>
#include <stdio.h>
#include "RPCclient.h"
#include "Settings.capnp.h"
#include "SettingsObjects.h"

using namespace std;
/// @brief All rpc request to px-settings-service implemented in this class.
class RPCSettingsClient
{

    #define  SETTINGS_SERVER_ADDRESS  string("unix:") + "/tmp/" + string(getenv("USER")) + "/rpc/settings"

public:
    RPCSettingsClient();
	/// @brief get list of modules from settings server
    /// @param modules a string vector contain the modules - fill in the method
    /// @param errors the error list
	/// @return true/false
    bool getModulesList(vector<string> &modules, vector<string> &errors);
	
	/// @brief get the all sections of module
	/// @param module the module name
	/// @param sections Section list of module - fill in the method
	/// @param errors the error list
	/// @return true/false
    bool getModuleSections(const string &module, SettingsObjects::SettingsSectionList &sections,
                           vector<string> &errors);
	
	/// @brief get the section of module
	/// @param module the module name
	/// @param section the section name
    /// @param sections Section list of section - fill in the method
    /// @param errors the error list
    /// @return true/false
    bool getSection(const string &module, const string &section, SettingsObjects::SettingsSectionList &sections,
                    vector<string> &errors);
	
	/// @brief add new section
	/// @param module the module name
	/// @param section the section name
	/// @param sectionValue the structure that included the parameters, values and ...
	/// @return the result included result and error list
    SettingsObjects::SettingsWriteResult addToSection(const string &module, const string &section,
                                                      const SettingsObjects::SettingsSection &sectionValue);
    
	/// @brief edit the section
	/// @param module the module name
	/// @param section the section name
	/// @param id the index
	/// @param sectionValue the structure that included the parameters, values and ...
	/// @return the result included result and error list
	SettingsObjects::SettingsWriteResult editInSection(const string &module, const string &section, const int &id,
                                                       const SettingsObjects::SettingsSection &sectionValue);

	/// @brief remove section
	/// @param module the module name
	/// @param section the section name
	/// @param id the index
	/// @return the result included result and error list
    SettingsObjects::SettingsWriteResult removeFromSection(const string &module, const string &section, const int &id);

	/// @brief apply the changes
    /// @param module the module name
    /// @param errors the error list
    /// @return get the result as a true/false
    bool apply(string &module, vector<string> &errors);
};

#endif // RPCSETTINGSCLIENT_H
