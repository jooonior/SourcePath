#include "stdafx.hpp"

#include "Plugin.hpp"

#include "ServerPluginCallbacks.hpp"

#include <tier1/tier1.h>

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)

#define PLUGIN_VERSION STR(PLUGIN_VERSION_MAJOR) "." STR(PLUGIN_VERSION_MINOR) "." STR(PLUGIN_VERSION_PATCH)

class Plugin : public ServerPluginCallbacks
{
public:
	Plugin();
	~Plugin();

	virtual bool Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);
	virtual void Unload(void);
	virtual const char *GetPluginDescription(void);
};

EXPOSE_SINGLE_INTERFACE(Plugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS);

Plugin::Plugin() {}
Plugin::~Plugin() {}

bool Plugin::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	PluginMsg("Loading version %s\n", PLUGIN_VERSION);

	ConnectTier1Libraries(&interfaceFactory, 1);

	ConVar_Register();

	return true;
}

void Plugin::Unload(void)
{
	ConVar_Unregister();

	DisconnectTier1Libraries();
}

const char *Plugin::GetPluginDescription(void)
{
	return PLUGIN_NAME " v" PLUGIN_VERSION;
}
