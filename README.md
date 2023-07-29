# SourcePath

Source SDK 2013 plugin for modifying engine search paths. Enables you to have
multiple separate configuration profiles without having to copy files around.

## Preface

Source engine maintains a list of directories where it looks for files.
When it needs to access a file, lets say `cfg/autoexec.cfg`, it goes though
this list from top to bottom, looking into each directory for the desired file.
Once a matching file is found, the search stops.
For example, if `tf/custom/config/cfg/autoexec.cfg` existed, it would
be chosen over `tf/cfg/autoexec.cfg`, because `tf/custom/*` directories are
above `tf/` in the search path list.

Search paths are further categorized by so called IDs, but all you need to know
is this: Using the `game+mod+custom_mod` set of IDs will work as if you dropped
the files into TF2's `tf/custom/*` directory.

If you want to know more, check your game's `gameinfo.txt` file. It will
contain a `SearchPaths` section listing what paths are mounted and under what
IDs. In-game, the `path` command can be used to print this information.

## Download

Here's a direct link to the
[latest version](https://github.com/jooonior/SourcePath/releases/latest/download/SourcePath.zip).
Older versions can be found on the
[releases page](https://github.com/jooonior/SourcePath/releases).

## Commands

### `path_append <path> [ids]`

Adds `path` to the **end** of the search path list, meaning it will have the
**lowest priority**. If `ids` are not given, uses `game+mod+custom_mod` as IDs.
`path` may contain wildcards.

### `path_prepend <path> [ids]`

Adds `path` to the **front** of the search path list, meaning it will have the
**highest priority**. If `ids` are not given, uses `game+mod+custom_mod` as IDs.
`path` may contain wildcards.

### `path_remove <path> [ids]`

Removes `path` from the search path list. If `ids` are given, only removes
entries under those IDs. `path` may contain wildcards.

### `path_clear [ids]`

Removes **all** paths from the search path list. If `ids` are given, only
removes entries under those IDs.

## `sourcepath.cfg`

When the plugin loads, it tries to `exec` this file. Because plugin
initialization happens quite early, you can modify search paths from here
and it will work as you would want it to. In contrast, using `autoexec.cfg`
to mount a custom HUD would not work, because HUD files are loaded before CFGs.

## Configuration Profiles

Isolated profiles can be created such that their configuration does not leak
into the default game installation. This is achieved by replacing paths used
for writing files, while keeping the read-only paths where game files are
located.

The following example shows the file structure and `sourcepath.cfg` required
to create a casting profile that does not interfere with your regular game
configuration. Once the `tf_casting` folder is set up, the profile is loaded
by including `-insecure -insert_search_path tf_casting` in your launch options. 
You can create a shortcut with these for maximum convenience.

```
Team Fortress 2/
├── tf/
└── tf_casting/
    ├── addons/
    │   ├── SourcePath.dll
    │   └── SourcePath.vdf
    ├── cfg/
    │   └── sourcepath.cfg
    └── custom/
        ├── CastingEssentials/
        └── mastercomfig-ultra-preset.vpk
```

```
// Redirect writes to our directory.
path_clear mod_write+game_write+default_write_path
path_append tf_casting mod_write+game_write+default_write_path

// Unmount tf/custom subdirectories.
path_remove tf/custom/* game+mod+custom_mod

// Mount our custom subdirectories.
path_prepend tf_casting/custom/* game+mod+custom_mod

// Note that `tf/cfg` was not unmounted. If you have CFGs there that you don't
// want executed, override them by creating matching ones in `tf_casting/cfg`.
```
