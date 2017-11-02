# AGK Steam Plugin

A plugin to give basic steam_api.dll access to games developed using AppGameKit's Tier 1 system.

## Getting Started

### Prerequisites

* [AppGameKit](https://www.appgamekit.com/) for developing your game.
* [Visual Studio 2015 Community Edition](https://www.visualstudio.com/vs/older-downloads/) was used to compile the plugin.
* [Steamworks SDK](https://partner.steamgames.com) v1.41 was used when compiling.

## Compiling the Plugin

The Steamworks SDK is required to compile the plugin DLL.
After getting the code for this project, download the Steamworks SDK and place its "public" and "redistributable_bin" folders into the SteamPlugin folder of this project.

## Adding the Plugin to the AGK IDE

To allow AGK to use the plugin, copy the SteamPlugin folder from inside the AGKPlugin folder of this project into the "Tier 1\Compiler\Plugins" folder where App Game Kit 2 is installed on your system.

## Important Notes

When running a project from within the AGK IDE, the IDE will delete the "steam_api.dll" and "steam_appid.txt" files.
To prevent this, set both as Read-only in their file properties.
This does not happen when running the game directly from the project's EXE file.

Also, when running from within the IDE, the plugin might grab the appid for AppGameKit instead of the project itself.
Renaming the "steam_api.dll" and "steam_appid.txt" files found in "App Game Kit 2\Tier 1\Editor\bin" is one way around this.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

* [AppGameKit](https://www.appgamekit.com/) for their wonderful game development engine.
* [Benjamin Moir](https://github.com/DaZombieKiller/steamplugin) for his plugin to use as a guideline.