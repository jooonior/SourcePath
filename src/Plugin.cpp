#include "stdafx.hpp"

#include "Plugin.hpp"
#include "PathManip.hpp"
#include "ServerPluginCallbacks.hpp"

#include <tier1/tier1.h>
#include <tier2/tier2.h>
#include <filesystem.h>

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
	ConnectTier2Libraries(&interfaceFactory, 1);

	if (!g_pFullFileSystem)
	{
		PluginWarning("Could not connect to the filesystem interface.\n");
		return false;
	}

	ConVar_Register();

	IVEngineClient *engine = (IVEngineClient *)interfaceFactory(VENGINE_CLIENT_INTERFACE_VERSION, NULL);

	if (engine)
	{
		// Plugins are loaded early, so this can be used to modify search paths before
		// other parts of the engine start initializing and loading in thier files.
		engine->ExecuteClientCmd("exec sourcepath.cfg");
	}
	else
	{
		PluginWarning("Unsupported IVEngineClient version, cannot exec plugin CFG.\n");
	}


	return true;
}

void Plugin::Unload(void)
{
	ConVar_Unregister();

	DisconnectTier2Libraries();
	DisconnectTier1Libraries();
}

const char *Plugin::GetPluginDescription(void)
{
	return PLUGIN_NAME " v" PLUGIN_VERSION;
}

// These are what custom/* folders are mounted with.
#define DEFAULT_MOUNT_IDS "game+mod+custom_mod"

CON_COMMAND(path_append, "Add low priority filesystem search path")
{
	if (args.ArgC() < 2 || args.ArgC() > 3)
	{
		Warning("Usage: path_append <path> [ids]\n ");
		return;
	}

	AddSearchPath(args.Arg(1), args.ArgC() == 3 ? args.Arg(2) : DEFAULT_MOUNT_IDS, false);
}

CON_COMMAND(path_prepend, "Add high priority filesystem search path")
{
	if (args.ArgC() < 2 || args.ArgC() > 3)
	{
		Warning("Usage: path_prepend <path> [ids]\n");
		return;
	}

	AddSearchPath(args.Arg(1), args.ArgC() == 3 ? args.Arg(2) : DEFAULT_MOUNT_IDS, true);
}

CON_COMMAND(path_remove, "Remove filesystem search path from given (or all non-explicit) IDs")
{
	if (args.ArgC() < 2 || args.ArgC() > 3)
	{
		Warning("Usage: path_remove <path> [ids]\n");
		return;
	}

	RemoveSearchPath(args.Arg(1), args.ArgC() == 3 ? args.Arg(2) : "");
}

CON_COMMAND(path_clear, "Remove all filesystem search paths under given IDs (or all)")
{
	if (args.ArgC() < 1 || args.ArgC() > 2)
	{
		Warning("Usage: path_clear [ids]\n");
		return;
	}

	RemoveAllSearchPaths(args.ArgC() == 2 ? args.Arg(1) : "");
}
