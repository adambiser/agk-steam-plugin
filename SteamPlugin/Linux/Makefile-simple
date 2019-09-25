SRCROOT = ..
SDK = ../SteamworksSDK
OUTPATH = ../../AGKPlugin/SteamPlugin

all: 
	g++ -Wall -Wextra -Wno-unknown-pragmas \
		-fvisibility=hidden -fpic \
		-DGNUC \
		-I$(SRCROOT) -I$(SRCROOT)/Common -I$(SDK)/public \
		-shared -o $(OUTPATH)/Linux64.so $(SRCROOT)/AGKLibraryCommands.cpp $(SRCROOT)/Common/*.cpp \
		-L$(SDK)/redistributable_bin/linux64 -lsteam_api
