#!/bin/bash

SCRIPT_DIRNAME=$(dirname "$0")
pushd "$SCRIPT_DIRNAME" > /dev/null
SCRIPT_PATH=`pwd`
popd > /dev/null

"$SCRIPT_PATH/compile_and_launch.sh" "snes9x2010_libretro.dylib" "Roms/SNES (SNES)/Super Mario World (Europe).smc" $*
