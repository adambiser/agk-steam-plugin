@ECHO OFF
REM This is needed for errorlevel to work in the for loop, also need ! instead of %.
setlocal EnableDelayedExpansion
REM Needed for IF DEFINED.
setlocal EnableExtensions

REM Must be run from the batch file's path.
cd /d %~dp0

if [%1] == [] goto :blankparam
if [%2] == [] goto :blankparam

SET PLUGIN_NAME=%~n1

SET DEBUGTEST=%1
if not x%DEBUGTEST:Debug=%==x%DEBUGTEST% SET DEBUG_MODE=1
IF DEFINED DEBUG_MODE ECHO DEBUG MODE!

if "%2" == "Windows" (
	SET CONFIGURATION=release
	IF DEFINED DEBUG_MODE SET CONFIGURATION=debug
	ECHO Building 64-bit plugin: !CONFIGURATION!
	"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" "%PLUGIN_NAME%\Windows\%PLUGIN_NAME%.vcxproj" /p:configuration=!CONFIGURATION! /p:platform=x64
)

REM Determine the path to the compiler.
REM Start by finding the path to the AGK editor using the AGK File type association.
REM This might not work in some setups.
set KEY_NAME="HKCU\Software\Classes\agk_auto_file\shell\open\command"
for /F "usebackq skip=2 tokens=2,*" %%A in (`REG QUERY %KEY_NAME% /ve`) do (
	set AGK_PATH=%%B
)
if defined AGK_PATH (
	REM Registry value looks like this: '"PATH_TO_AGK.EXE" "%1"'
	REM Remove ' "%1"' from the end.
	set AGK_PATH=!AGK_PATH: "%%1"=!
	REM Remove the editor EXE portion of the path.
	set AGK_PATH=!AGK_PATH:\editor\bin\agk.exe=!
	REM Remove quotes
	set AGK_PATH=!AGK_PATH:"=!
	REM Add the path to the compiler.
	set AGK_COMPILER_PATH=!AGK_PATH!\Compiler\AGKCompiler.exe
	set AGK_PLUGIN_PATH=!AGK_PATH!\Compiler\Plugins\%PLUGIN_NAME%
)
REM Verify compiler path.
if not defined AGK_COMPILER_PATH set NO_COMPILER=1
if not exist !AGK_COMPILER_PATH! set NO_COMPILER=1
if defined NO_COMPILER (
	echo ERROR: Could not determine path to AGKCompiler.exe. Is the AGK file type associated with AGK.EXE?
	exit /B 1
)
echo Compiler path: !AGK_COMPILER_PATH!
echo Plugin path: !AGK_PLUGIN_PATH!
echo Plugin name: %PLUGIN_NAME%
echo Plugin OS: %2

echo Creating plugin folders.
if not exist "AGKPlugin" MkDir "AGKPlugin"
if not exist "AGKPlugin\%PLUGIN_NAME%" MkDir "AGKPlugin\%PLUGIN_NAME%"
if not exist "%AGK_PLUGIN_PATH%" MkDir "%AGK_PLUGIN_PATH%"

REM Commands.txt needs to exist.
if not exist "AGKPlugin\%PLUGIN_NAME%\Commands.txt" (
	echo ERROR: "AGKPlugin\%PLUGIN_NAME%\Commands.txt" does not exist.
	exit /B 3
)

echo Copying plugin into the AppGameKit compiler's plugin folder.
Copy /Y "%1" "AGKPlugin\%PLUGIN_NAME%\%2.dll" > nul
Copy /Y "AGKPlugin\%PLUGIN_NAME%\Commands.txt" "%AGK_PLUGIN_PATH%\Commands.txt" > nul
Copy /Y "AGKPlugin\%PLUGIN_NAME%\Windows.dll" "%AGK_PLUGIN_PATH%\Windows.dll" > nul
Copy /Y "AGKPlugin\%PLUGIN_NAME%\Windows64.dll" "%AGK_PLUGIN_PATH%\Windows64.dll" > nul
Copy /Y "AGKPlugin\%PLUGIN_NAME%\Linux64.so" "%AGK_PLUGIN_PATH%\Linux64.so" > nul
REM debug symbols
IF DEFINED DEBUG_MODE (
	Copy /Y "%~dpn1.pdb" "AGKPlugin\%PLUGIN_NAME%\Windows.pdb" > nul
) else (
	if exist "AGKPlugin\%PLUGIN_NAME%\Windows.pdb" (
		Del "AGKPlugin\%PLUGIN_NAME%\Windows.pdb"
	)
)

echo Compiling example projects.
for /D %%G in ("Examples\*") do (
	REM copy %PLUGIN_FILE% "%%G\Plugins\%PLUGIN_NAME%\Windows.dll" > nul
	REM if !errorlevel! neq 0 echo ERROR: "%%G" plugin copy failed!
	echo    %%G
	pushd "%%G"
	"%AGK_COMPILER_PATH%" -agk "main.agc"
	if !errorlevel! neq 0 echo ERROR: "%%G" compilation failed!
	popd
	REM Copy the PDB after compiling.  Compiling will create the Plugins folder if it's not there already.
	IF DEFINED DEBUG_MODE (
		Copy /Y "%~dpn1.pdb" "%%G\Plugins\%PLUGIN_NAME%\Windows.pdb" > nul
	) else (
		if exist "%%G\Plugins\%PLUGIN_NAME%\Windows.pdb" (
			Del "%%G\Plugins\%PLUGIN_NAME%\Windows.pdb"
		)
	)
)

exit /B 0

:blankparam
echo ERROR: This BAT file requires two parameters. 1) Path to the compiled DLL.  2) End file name for DLL in plugins folder (Windows or Windows64).
exit /B 0
