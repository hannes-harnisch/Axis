@echo off

if exist "Vitro.sln" (
	del "Vitro.sln"
)

call premake5 vs2022
if %ERRORLEVEL% neq 0 (pause)