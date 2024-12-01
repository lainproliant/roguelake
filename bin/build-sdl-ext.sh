#!/bin/bash
#
# build-ext.sh
#
# Author: Lain Musgrove (lain.proliant@gmail.com)
# Date: Monday November 25, 2024
#

set -e

if [ -z "$1" ]; then
    echo "Missing SDL extension name."
    exit 1
fi

ext="$1"
shift

mkdir -p ./exts
mkdir -p ./pfx
git clone "https://github.com/libsdl-org/$ext" "./exts/$ext"
mkdir -p "./exts/$ext/build"
cmake -S "./exts/$ext" -B "./exts/$ext/build"
cmake --build "./exts/$ext/build" -j $(nproc --all)
cmake --install "./exts/$ext/build" --prefix ./pfx
