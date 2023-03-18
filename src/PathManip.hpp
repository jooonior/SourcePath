#pragma once

// Requires tier2 to be connected.
void AddSearchPath(const char* path, const char* id, bool should_override);

// Requires tier2 to be connected.
void RemoveSearchPath(const char* path, const char* id);

// Requires tier2 to be connected.
void RemoveAllSearchPaths(const char* id);
