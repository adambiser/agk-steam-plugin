# AGK Steam Plugin

A plugin to give basic steam_api.dll access to games developed using AppGameKit's Tier 1 system.

## Getting Started

### Prerequisites

* [AppGameKit](https://www.appgamekit.com/) for developing your game.
* [Visual Studio 2015 Community Edition](https://www.visualstudio.com/vs/older-downloads/) was used to compile the plugin.
* [Steamworks SDK](https://partner.steamgames.com) v1.41 was used when compiling.

### Installing

After getting the code for this project, download the Steamworks SDK and place its "public" and "redistributable_bin" folders into the SteamPlugin folder of this project.

### Using the Plugin with AppGameKit

App Game Kit 2\Tier 1\Compiler\Plugins

SteamPlugin/redistributable_bin/steam_api.dll
Note than when developing from the AGK IDE, you might have to mark this dll as Read only so the IDE doesn't remove it.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

* [AppGameKit](https://www.appgamekit.com/) for their wonderful game development engine.
* [Benjamin Moir](https://github.com/DaZombieKiller/steamplugin) for his plugin to use as a guideline.