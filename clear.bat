FOR /D /R %%X IN (*.tlog) DO RD /S /Q "%%X"

rmdir %~dp0\prj\Debug /s /q
rmdir %~dp0\prj\Release /s /q
rmdir %~dp0\prj\x64 /s /q

rmdir %~dp0\prj\.vs\ /s /q



