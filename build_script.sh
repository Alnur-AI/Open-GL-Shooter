#!/bin/bash
BUILD_TYPE=${1:-rel}
CWD=$(pwd)


export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

cd Project
cmake -G Ninja -DBUILD_TYPE=$BUILD_TYPE -B ../Builds/unix/$BUILD_TYPE


cd ../Builds/unix/$BUILD_TYPE 
time -p ninja -j $(nproc)

mv Application ../../Chess/Chess-$BUILD_TYPE.exe

cd ../../..


