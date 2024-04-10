@echo off
set BUILD_TYPE=%1

cd Builds/Chess
Chess-%BUILD_TYPE%.exe
cd ../..