@echo off
set BUILD_TYPE=%1


set CWD=%cd%


cd Project
cmake -G Ninja  -DBUILD_TYPE=%BUILD_TYPE% -B ../Builds/win/%BUILD_TYPE%
cd ../Builds/win/%BUILD_TYPE% 

"../../../BuildTools/Ninja/ninja.exe"

set binPath=..\..\Chess
IF not exist %binPath% (mkdir %binPath%)

ren Chess.exe Chess-%BUILD_TYPE%.exe
move Chess-%BUILD_TYPE%.exe %binPath%

cd ../../..