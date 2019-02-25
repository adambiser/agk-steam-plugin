@ECHO OFF
REM This is needed for errorlevel to work in the for loop, also need ! instead of %.
setlocal EnableDelayedExpansion

if [%1] == [] goto :blankparam
if [%2] == [] goto :blankparam

SET PLUGIN_NAME=%~n1

REM Must be run from the batch file's path.
cd /d %~dp0

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

echo Copying plugin into AppGameKit plugin folder.
MkDir "%AGK_PLUGIN_PATH%"
Copy "%1" "AGKPlugin\%PLUGIN_NAME%\%2.dll"
COPY "AGKPlugin\%PLUGIN_NAME%" "%AGK_PLUGIN_PATH%" > nul

echo Compiling example projects.
for /D %%G in ("Examples\*") do (
	REM copy %PLUGIN_FILE% "%%G\Plugins\%PLUGIN_NAME%\Windows.dll" > nul
	REM if !errorlevel! neq 0 echo ERROR: "%%G" plugin copy failed!
	echo    %%G
	pushd "%%G"
	"%AGK_COMPILER_PATH%" -agk "main.agc"
	if !errorlevel! neq 0 echo ERROR: "%%G" compilation failed!
	popd
)

exit /B 0

:blankparam
echo ERROR: This BAT file requires two parameters. 1) Path to the compiled DLL.  2) End file name for DLL in plugins folder (Windows or Windows64).
exit /B 0
