@echo off
set executingDir=%~dp0

psexec -i 0 %executingDir%x64\Release\AmpSandbox.exe

pause