#include "stdafx.hpp"

#include "PathManip.hpp"
#include "Plugin.hpp"

#include <filesystem.h>

static void SplitPathIDs(const char *ids, CUtlStringList &out)
{
	V_SplitString(ids, "+", out);

	FOR_EACH_VEC(out, i)
	{
		Q_StripPrecedingAndTrailingWhitespace(out[i]);
	}
}

template<typename F>
static bool MatchPath(const char *path, F &&callback)
{
	FileFindHandle_t handle;
	const char *filename = g_pFullFileSystem->FindFirstEx(path, "BASE_PATH", &handle);

	if (!filename)
		return false;

	do
	{
		if (filename[0] == '.')
			continue;

		char absolute_path[MAX_PATH];
		V_ExtractFilePath(path, absolute_path, sizeof(absolute_path));
		g_pFullFileSystem->RelativePathToFullPath_safe(absolute_path, 0, absolute_path);
		V_AppendSlash(absolute_path, sizeof(absolute_path));
		V_strcat_safe(absolute_path, filename);

		if (!g_pFullFileSystem->FindIsDirectory(handle) && !V_strEndsWith(filename, ".vpk"))
			continue;

		callback(absolute_path);

	} while (filename = g_pFullFileSystem->FindNext(handle));

	g_pFullFileSystem->FindClose(handle);

	return true;
}

void AddSearchPath(const char *path, const char *id, bool should_override)
{
	CUtlStringList ids;
	SplitPathIDs(id, ids);

	CUtlStringList paths;

	if (!MatchPath(path, [&](const char *path) { paths.CopyAndAddToTail(path); }))
	{
		PluginWarning("%s : no such file or directory.\n", path);
		return;
	}

	auto mode = should_override ? PATH_ADD_TO_HEAD : PATH_ADD_TO_TAIL;

	FOR_EACH_VEC(paths, path_idx)
	{
		// If prepending, do so in reverse order, so that relative ordering is the same.
		int actual_path_idx = should_override ? paths.Size() - path_idx - 1 : path_idx;

		FOR_EACH_VEC(ids, id_idx)
		{
			g_pFullFileSystem->AddSearchPath(paths[actual_path_idx], ids[id_idx], mode);
		}
	}

	FOR_EACH_VEC(paths, path_idx)
	{
		DevMsg("%s : mounted under %s\n", paths[path_idx], id);
	}
}

void RemoveSearchPath(const char *path, const char *id)
{
	CUtlStringList ids;
	SplitPathIDs(id, ids);

	auto callback = [&](const char *path) {
		bool removed = false;

		if (ids.IsEmpty())
		{
			removed = g_pFullFileSystem->RemoveSearchPath(path, nullptr);
			id = "any non-explicit IDs";
		}
		else
		{
			FOR_EACH_VEC(ids, i)
			{
				removed |= g_pFullFileSystem->RemoveSearchPath(path, ids[i]);
			}
		}

		if (removed)
		{
			DevMsg("%s : removed from %s\n", path, id);
		}
		else
		{
			DevWarning("%s : not mounted under %s\n", path, id );
		}
	};

	if (!MatchPath(path, callback))
	{
		PluginWarning("%s : no such file or directory.\n", path);
	}
}

void RemoveAllSearchPaths(const char* id)
{
	CUtlStringList ids;
	SplitPathIDs(id, ids);

	if (ids.IsEmpty())
	{
		g_pFullFileSystem->RemoveAllSearchPaths();
		PluginWarning("Removed all search paths.\n");
	}
	else
	{
		FOR_EACH_VEC(ids, i)
		{
			g_pFullFileSystem->RemoveSearchPaths(ids[i]);
		}
		PluginMsg("Removed all search paths under %s.\n", id);
	}

}
