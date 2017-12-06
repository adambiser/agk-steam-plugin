@ECHO OFF
Copy "%1" "..\..\AGKPlugin\SteamPlugin\Windows.dll"
cd ..
cd ..
IF EXIST "UpdateAGKFolder.bat" (
	CALL UpdateAGKFolder.bat
)

ECHO Copying plugin to AGK sample folder.

Copy "AGKPlugin\SteamPlugin\Windows.dll" "AGK\Plugins\SteamPlugin\Windows.dll"