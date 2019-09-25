# discover if this is a 32 or 64 bit machine
ARCH := $(shell getconf LONG_BIT)

SRCROOT = ..
SDK = ../SteamworksSDK
OUTPATH = ../../AGKPlugin/SteamPlugin
#OUTPATH = build

CC = g++
ODIR = build/obj$(ARCH)
INC = -I.. -I../Common -I$(SDK)/public
CFLAGS = -Wall -Wextra -Wno-unknown-pragmas -fvisibility=hidden -fpic -DGNUC -O3
LDFLAGS = -L$(SDK)/redistributable_bin/linux64 -lsteam_api

# list of dependencies so that changes to these files cause a rebuild of source files
DEPS = $(SRCROOT)/Common/stdafx.h \
	$(SRCROOT)/AGKLibraryCommands.h \
	$(SRCROOT)/Common/CCallbacks.h \
	$(SRCROOT)/Common/CCallResultItem.h \
	$(SRCROOT)/Common/CRoomManager.h \
	$(SRCROOT)/Common/CSteamHandleVector.h \
	$(SRCROOT)/Common/SteamApps.h \
	$(SRCROOT)/Common/SteamFriends.h \
	$(SRCROOT)/Common/SteamInput.h \
	$(SRCROOT)/Common/SteamMatchmaking.h \
	$(SRCROOT)/Common/SteamMusic.h \
	$(SRCROOT)/Common/SteamRemoteStorage.h \
	$(SRCROOT)/Common/SteamScreenshots.h \
	$(SRCROOT)/Common/SteamUser.h \
	$(SRCROOT)/Common/SteamUserStats.h \
	$(SRCROOT)/Common/SteamUtils.h \
	$(SRCROOT)/Common/utils.h

# list of object files to compile, they will be placed in the $(ODIR) folder
# matching .cpp files will be searched for below
_OBJS = Common/SteamApps.o \
	Common/SteamAppsList.o \
	Common/SteamFriends.o \
	Common/SteamInput.o \
	Common/SteamMatchmaking.o \
	Common/SteamMusic.o \
	Common/SteamRemoteStorage.o \
	Common/SteamScreenshots.o \
	Common/SteamUser.o \
	Common/SteamUserStats.o \
	Common/SteamUtils.o \
	Common/CSteamHandleVector.o \
	Common/DllMain.o \
	Common/CRoomManager.o \
	Common/CCallResultItem.o \
	Common/CCallbacks.o \
	AGKLibraryCommands.o

OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

# entry point, create folders, compile source files, and link the shared object
all: setup $(OBJS) SharedObject

# make sure the build folders exist
setup:
	mkdir -p build/obj$(ARCH)
	mkdir -p build/obj$(ARCH)/Common

# compile a source file, first search path is the source root.
$(ODIR)/%.o: $(SRCROOT)/%.cpp $(DEPS)
	$(CC) -static -c $(INC) -o $@ $< $(CFLAGS)

# second search path is the Common folder.
$(ODIR)/%.o: $(SRCROOT)/Common/%.cpp $(DEPS)
	$(CC) -static -c $(INC) -o $@ $< $(CFLAGS)

# link the .o files
SharedObject: | $(OBJS)
	$(CC) $(OBJS) -shared -o $(OUTPATH)/Linux$(ARCH).so $(LDFLAGS)

# delete compiled files
clean:
	rm -rf build/obj$(ARCH)/Common/*
	rm -rf build/obj$(ARCH)/*
	rm -f $(OUTPATH)/Linux$(ARCH).so
