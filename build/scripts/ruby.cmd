@echo off

setlocal

set GEM_HOME=
set GEM_PATH=

set FILEDIR=%~dp0
set TOOLSDIR=%FILEDIR%..

set RUBYVER=
pushd %TOOLSDIR%\crew\ruby
for /f "delims=" %%a in ('type active_version.txt') do @set RUBYVER=%%a
popd
set RUBYDIR=%TOOLSDIR%\crew\ruby\%RUBYVER%\bin

%RUBYDIR%\ruby.exe %*

endlocal

exit /b %errorlevel%
