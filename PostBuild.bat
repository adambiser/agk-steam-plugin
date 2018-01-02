@ECHO OFF
Copy "%1" "..\..\AGKPlugin\SteamPlugin\Windows.dll"
cd ..
cd ..
IF EXIST "UpdateAGKFolder.bat" (
	CALL UpdateAGKFolder.bat
)

@ECHO Copy to each example project.
FOR /D %%G in ("Examples\*") DO (
	Copy "%1" "%%G\Plugins\SteamPlugin\Windows.dll"
)
