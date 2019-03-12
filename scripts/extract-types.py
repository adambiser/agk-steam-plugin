"""
A script to download the apartment prices for Camden Foxcroft and store them in a CSV file.
"""
from bs4 import BeautifulSoup
import os
import re
import requests


OUTPUT_FILENAME = "../Examples/steam_constants.agc"

STEAMWORKS_API_URL_BASE = "https://partner.steamgames.com/doc/api/"

# Manually ignored structs.  Those with descriptions starting with "Deprecated" are ignored automatically.
IGNORED = {
    # "ISteamApps": [
    #     "AppProofOfPurchaseKeyResponse_t",
    #     "RegisterActivationCodeResponse_t",
    # ],
    # "ISteamUser": [
    #     "CallbackMsg_t"
    # ],
    "ISteamMatchmaking": [
        "LobbyKicked_t",  # Currently unused
        "LobbyMatchList_t",  # Plugin returns json array of lobby IDs.
    ]
}

ENUM_VALUE_FIXES = {
    # EHTMLKeyModifiers
    'EHTMLKeyModifier_AltDown': '1',
    'EHTMLKeyModifier_CtrlDown': '2',
    'EHTMLKeyModifier_ShiftDown': '4',
    # EChatSteamIDInstanceFlags
    'EChatInstanceFlagClan': '0x00000800',
    'EChatInstanceFlagLobby': '0x00000400',
    'EChatInstanceFlagMMSLobby': '0x00000200',
    # EMarketingMessageFlags
    'EMarketingMessageFlagsHighPriority': '1',
    'EMarketingMessageFlagsPlatformWindows': '2',
    'EMarketingMessageFlagsPlatformMac': '4',
    'EMarketingMessageFlagsPlatformLinux': '8',
    # ERemoteStoragePlatform
    'ERemoteStoragePlatformWindows': '1',
    'ERemoteStoragePlatformOSX': '2',
    'ERemoteStoragePlatformPS3': '4',
    'ERemoteStoragePlatformLinux': '8',
    'ERemoteStoragePlatformReserved2': '16',
    # ESteamItemFlags
    'ESteamItemNoTrade': '1',
    'ESteamItemRemoved': '0x100',
    'ESteamItemConsumed': '0x200',
}


def check_status_code(response):
    if response.status_code != 200:
        print("! Received status code {}".format(response.status_code))
        exit(response.status_code)


# noinspection PyShadowingNames
def get_url_filename(filename):
    return filename[:filename.index(".")] if "." in filename else filename


def clean_enum_name(name):
    if name.startswith("k_"):
        name = name[2:]
    if name.startswith("e"):
        name = name[0].upper() + name[1:]
    if name.startswith("EEVRHMDType"):
        name = name[1:]
    return name


def clean_member_name(name):
    if name.startswith("m_"):
        name = name[2:]
    if name.startswith("rtime"):
        name = name[1:]
    if name == name.lower() \
            or name.startswith("steam") \
            or name.startswith('game') \
            or name.startswith('num') \
            or name.startswith('item') \
            or name.startswith('time'):
        name = name[0].upper() + name[1:]
    else:
        if re.match('^[a-z]{3,}', name) \
                and not name.startswith("rgch") \
                and not name.startswith("cch") \
                and not name.startswith("rgf") \
                and not name.startswith("pch") \
                and not name.startswith("pub") \
                and not name.startswith("cub") \
                and not name.startswith("rot") \
                and not name.startswith("pos"):
            print("! Potential name truncation: " + name)
        name = re.sub("^[a-z]*", "", name)
    return name


def clean_member_type(mtype, name):
    # print(mtype)
    if mtype.startswith("char") or mtype.startswith("const char"):
        return "string"
    if mtype in ["float", "double"]:
        return "float"
    if "[" in mtype or "\U0000FF3B" in mtype:  # Web pages use unicode brackets.
        return "integer[]"
    if name.endswith("IP"):  # Plugin converts IP address uint32 to string.
        return "string"
    return "integer"


def get_first_sentence(desc):
    # First sentence only.
    match = re.match("^((?![?.][ A-Z]).)*[?.]", desc)
    if match is None:
        # Use the whole thing.
        return desc
    desc = match.group()
    if desc.endswith(" (e.g."):
        desc = desc[:-6] + "."
    if desc.endswith(" ."):
        desc = desc[:-2] + "."
    # print(desc)
    return desc


# noinspection PyShadowingNames
def download_api_page(filename):
    print("Downloading {}...".format(filename))
    url = STEAMWORKS_API_URL_BASE + get_url_filename(filename)
    response = requests.get(url)
    check_status_code(response)
    html = response.text
    with open(filename + '.htm', 'w', encoding='utf-8') as f:
        f.write(html)


# noinspection PyShadowingNames
def append_text_file(filename, add_header=True, outfile=None):
    if not os.path.exists(filename + ".txt"):
        return False
    print("+ Appending {}...".format(filename))
    with open(filename + '.txt', 'r', encoding='utf-8') as f:
        text = f.read()
    f = open(OUTPUT_FILENAME, 'a') if outfile is None else outfile
    if add_header:
        f.write('/' * 80 + '\n')
        f.write('// {}\n'.format(filename))
        f.write('// {}\n'.format(STEAMWORKS_API_URL_BASE + get_url_filename(filename)))
        f.write('/' * 80 + '\n')
        f.write('\n')
    f.write(text)
    f.write('\n')
    if outfile is None:
        f.close()
    return True


# noinspection PyShadowingNames
def parse_api_page(filename):
    if append_text_file(filename):
        return
    print("Parsing {}...".format(filename))
    with open(filename + '.htm', 'r', encoding='utf-8') as f:
        html = f.read()
    soup = BeautifulSoup(html, "html5lib")
    structs = []
    enums = []
    for section in soup.find_all("h2", "bb_section"):
        # print(section.text)
        if section.text in ['Callbacks', 'Structs']:
            node = section
            struct = {}
            while node.nextSibling is not None:
                node = node.nextSibling
                if node.name == "h2" and "bb_section" in node["class"]:
                    # Found the next section.  Quit.
                    break
                if node.name == "h2" and "bb_subsection" in node["class"]:
                    # Found a subsection. This contains the struct name.  Also grab any description.
                    struct = {}
                    structs.append(struct)
                    struct["name"] = node.text
                    struct["desc"] = ""
                    while node is not None and (node.nextSibling.name is None or node.nextSibling.name == "a"):
                        node = node.nextSibling
                        if node.name is None:
                            struct['desc'] += node.strip() + " "
                        else:
                            struct['desc'] += node.text.strip() + " "
                    struct["desc"] = struct["desc"].strip()
                elif node.name == "table" and "members" not in struct:
                    # Parse the table of member variables.
                    struct["members"] = []
                    for row in node.find_all("tr")[1:]:  # First row is the header.  Skip it.
                        cols = row.find_all("td")
                        # if struct['name'] == 'FriendsEnumerateFollowingList_t':
                        #     print(cols[1].text)
                        struct["members"].append({
                            "name": cols[0].text,
                            "type": cols[1].text,
                            "desc": cols[2].text,
                        })
                elif node.name == "strong" and node.text == "Associated Functions:":
                    # Keep a list of associated functions.
                    struct["functions"] = []
                    node = node.nextSibling
                    while node is not None and node.nextSibling.name == "a" \
                            and "bb_apilink" in node.nextSibling["class"]:
                        node = node.nextSibling
                        struct["functions"].append(node.text)
        if section.text in ['Enums']:
            # print("enums")
            node = section
            enum = {}
            while node.nextSibling is not None:
                node = node.nextSibling
                if node.name == "h2" and "bb_section" in node["class"]:
                    # Found the next section.  Quit.
                    break
                if node.name == "h2" and "bb_subsection" in node["class"]:
                    # Found a subsection. This contains the struct name.  Also grab any description.
                    enum = {}
                    enums.append(enum)
                    enum["name"] = node.text
                    enum["desc"] = ""
                    while node is not None and (node.nextSibling.name is None or node.nextSibling.name == "a"):
                        node = node.nextSibling
                        if node.name is None:
                            enum['desc'] += node.strip() + " "
                        else:
                            enum['desc'] += node.text.strip() + " "
                    enum["desc"] = enum["desc"].strip()
                elif node.name == "table" and "members" not in enum:
                    # Parse the table of member variables.
                    enum["members"] = []
                    for row in node.find_all("tr")[1:]:  # First row is the header.  Skip it.
                        cols = row.find_all("td")
                        # if struct['name'] == 'FriendsEnumerateFollowingList_t':
                        #     print(cols[1].text)
                        enum["members"].append({
                            "name": cols[0].text,
                            "value": cols[1].text,
                            "desc": cols[2].text,
                        })

    # print([t for t in structs if t['name'] == 'FriendsEnumerateFollowingList_t'])

    # noinspection PyShadowingNames
    def exclude_struct(struct):
        if 'members' not in struct:
            print("! Type had no members: {name}".format(**struct))
            return True
        if filename in IGNORED and struct['name'] in IGNORED[filename]:
            return True
        if 'desc' in struct:
            desc_lower = struct['desc'].lower()
            if 'used internally' in desc_lower:
                return True
            return desc_lower.startswith('deprecated')
        return False

    # noinspection PyShadowingNames
    def exclude_enum(enum):
        if 'desc' in enum:
            desc_lower = enum['desc'].lower()
            if 'used internally' in desc_lower:
                return True
            if 'steam internal usage only' in desc_lower:
                return True
            return desc_lower.startswith('deprecated')
        return False

    def is_deprecated_enum_member(enum_member):
        desc_lower = enum_member['desc'].lower()
        if 'used internally' in desc_lower:
            if enum_member['name'] != 'EUGCQuery_RankedByTotalVotesAsc':
                return True
        if 'deprecated' in desc_lower:
            return True
        if 'deprecated' in enum_member['name'].lower():
            return True
        if enum_member['name'] in ['EFriendRelationshipMax', 'EPersonaStateMax']:
            return True
        return False

    # noinspection PyShadowingNames
    def cleanup_struct(struct):
        struct['desc'] = get_first_sentence(struct["desc"].strip())
        for x in range(len(struct['members'])):
            member = struct['members'][x]
            member['name'] = clean_member_name(member['name'])
            member['desc'] = get_first_sentence(member['desc'])
            if member['type'].startswith('E'):
                member['desc'] = member['type'] + ". " + member['desc']
            member['type'] = clean_member_type(member['type'], member['name'])

    # noinspection PyShadowingNames
    def cleanup_enum(enum):
        enum['desc'] = get_first_sentence(enum["desc"].strip())
        for x in range(len(enum['members'])):
            member = enum['members'][x]
            member['name'] = clean_enum_name(member['name'])
            member['desc'] = get_first_sentence(member['desc'])
            if member['name'] in ENUM_VALUE_FIXES:
                member['desc'] += "\t// {}".format(member['value'])
                member['value'] = ENUM_VALUE_FIXES[member['name']]

    # Output to file.
    with open(OUTPUT_FILENAME, 'a') as f:
        f.write('/' * 80 + '\n')
        f.write('// {}\n'.format(filename))
        f.write('// {}\n'.format(STEAMWORKS_API_URL_BASE + get_url_filename(filename)))
        f.write('/' * 80 + '\n')
        f.write('\n')
        structs[:] = [struct for struct in structs if not exclude_struct(struct)]
        enums[:] = [enum for enum in enums if not exclude_enum(enum)]
        if structs or os.path.exists(filename + "-structs-append.txt"):
            f.write('//' + '-' * 78 + '\n')
            f.write('// Structs\n')
            f.write('//' + '-' * 78 + '\n')
            f.write('\n')
            for struct in structs:
                if append_text_file(struct['name'] + '-replace', False, outfile=f):
                    continue
                cleanup_struct(struct)
                if struct['desc']:
                    f.write('// {desc}\n'.format(**struct))
                if 'functions' in struct:
                    f.write('// Associated Functions: {0}\n'.format(' '.join(struct['functions'])))
                f.write('Type {name}\n'.format(**struct))
                for member in struct['members']:  # type: dict
                    f.write('\t{name} as {type}'.format(**member))
                    if member['desc'] != '':
                        f.write('\t// {desc}'.format(**member))
                    f.write("\n")
                f.write("EndType\n")
                f.write("\n")
            append_text_file(filename + "-structs-append", False, outfile=f)
        if enums or os.path.exists(filename + "-enums-append.txt"):
            f.write('//' + '-' * 78 + '\n')
            f.write('// Enums and Constants\n')
            f.write('//' + '-' * 78 + '\n')
            f.write('\n')
            for enum in enums:
                if append_text_file(enum['name'] + '-replace', False, outfile=f):
                    continue
                cleanup_enum(enum)
                f.write('// {name}\n'.format(**enum))
                if enum['desc']:
                    f.write('// {desc}\n'.format(**enum))
                for member in enum['members']:  # type: dict
                    if is_deprecated_enum_member(member):
                        f.write('//')
                    f.write('#constant {name}\t{value}'.format(**member))
                    if member['desc'] != '':
                        f.write('\t// {desc}'.format(**member))
                    f.write('\n')
                append_text_file(enum['name'] + '-append', False, outfile=f)
                f.write("\n")
            append_text_file(filename + "-enums-append",False,  outfile=f)


api_files = [
    'ISteamApps',
    # 'ISteamAppTicket',  # This should never be needed in most cases.
    # 'ISteamClient',  # You'll only ever need to use these APIs if you have a more complex versioning scheme...
    # 'ISteamController',
    'ISteamFriends',
    # 'ISteamGameCoordinator',  # Deprecated
    'ISteamGameServer',
    'ISteamGameServerStats',
    'ISteamHTMLSurface',
    'ISteamHTTP',
    'ISteamInput',  # Steamworks SDK documentation doesn't have this yet!
    'ISteamInventory',
    'ISteamMatchmaking',
    'ISteamMatchmakingServers',
    'ISteamMusic',
    'ISteamMusicRemote',
    'ISteamNetworking',
    'ISteamNetworkingSockets',
    # 'ISteamNetworkingUtils',  # IN BETA
    'ISteamParties',
    'ISteamRemoteStorage',
    'ISteamScreenshots',
    'ISteamUGC',
    # 'ISteamUnifiedMessages',  # Deprecated
    'ISteamUser',
    'ISteamUserStats',
    'ISteamUtils',
    'ISteamVideo',
    'SteamEncryptedAppTicket',
    'steam_api.h',
    'steam_gameserver.h',
    ]

# for filename in api_files:
#     download_api_page(filename)

if os.path.exists(OUTPUT_FILENAME):
    os.remove(OUTPUT_FILENAME)
append_text_file('header', False)
for filename in api_files:
    if not os.path.exists(filename + ".htm"):
        download_api_page(filename)
    parse_api_page(filename)
