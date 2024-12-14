#!/bin/bash

build(){
  if [[ $DEBUG ]]; then
    cmake -S . -B build -G "Unix Makefiles" -D CMAKE_EXPORT_COMPILE_COMMANDS=1 -D CMAKE_BUILD_TYPE=Debug
  else
    cmake -S . -B build -G "Unix Makefiles" -D CMAKE_EXPORT_COMPILE_COMMANDS=1
  fi
}

compile(){
  if [[ -n ./build ]] ; then
    build
  fi
  cd build
  make
}

test(){
  if [[ -e ./build ]] ; then
    compile
  else
    build
    compile
  fi
  ctest
}

install(){
  if [[ -e ./build ]] ; then
    compile
  else
    build
    compile
  fi
  make install
}

run(){
  if [[ -e ./build ]] ; then
    compile && exec ./resmon-widget.exe
  else
    build
    compile && exec ./resmon-widget.exe
  fi
}

clean(){
  if [[ -e ./build ]] ; then
    rm -r ./build
  fi
}

if [[ -n $1 ]] ; then
  $1
else
 run
fi

