#!/bin/bash
#

# The program location
TOP=$(cd "${0%/*}" && echo ${PWD})

# Change LinuxPlayer64 to the name of your Linux executable.
PROGRAM="${TOP}/LinuxPlayer64"

# Steam sets this environment variable when launching games
if [ -z "$STEAM_RUNTIME" ]; then
	# Note: Need to use $HOME because setting LD_PRELOAD treats ~ like a string and preloading gameoverlayrenderer.so fails.
	export STEAM_RUNTIME="$HOME/.local/share/Steam/ubuntu12_32/steam-runtime/i386"
	if [ ! -d "${STEAM_RUNTIME}" ]; then
		export STEAM_RUNTIME="$HOME/.steam/ubuntu12_32/steam-runtime/i386"
	fi
	export LD_LIBRARY_PATH="${STEAM_RUNTIME}/lib/i386-linux-gnu:${STEAM_RUNTIME}/lib:${STEAM_RUNTIME}/usr/lib/i386-linux-gnu:${STEAM_RUNTIME}/usr/lib:${LD_LIBRARY_PATH}"
fi

# Show the Steam Overlay - https://partner.steamgames.com/doc/store/application/platforms/linux#FAQ
export LD_PRELOAD="${STEAM_RUNTIME}/../../../ubuntu12_64/gameoverlayrenderer.so"
# Add the current directory to the library path to pick up libsteam_api.so
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:."

exec "${PROGRAM}" "$@"
