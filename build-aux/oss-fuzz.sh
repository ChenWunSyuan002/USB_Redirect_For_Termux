#!/usr/bin/env bash
# SPDX-License-Identifier: LGPL-2.1-or-later
# Original from: https://github.com/systemd/systemd/blob/main/tools/oss-fuzz.sh


set -ex

export LC_CTYPE=C.UTF-8

export CC=${CC:-clang}
export CXX=${CXX:-clang++}
clang_version="$($CC --version | sed -nr 's/.*version ([^ ]+?) .*/\1/p' | sed -r 's/-$//')"

SANITIZER=${SANITIZER:-address -fsanitize-address-use-after-scope}
flags="-O1 -fno-omit-frame-pointer -gline-tables-only -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION -fsanitize=$SANITIZER"

clang_lib="/usr/lib64/clang/${clang_version}/lib/linux"
[ -d "$clang_lib" ] || clang_lib="/usr/lib/clang/${clang_version}/lib/linux"

export CFLAGS=${CFLAGS:-$flags}
export CXXFLAGS=${CXXFLAGS:-$flags}
export LDFLAGS=${LDFLAGS:--L${clang_lib}}

export WORK=${WORK:-$(pwd)}
export OUT=${OUT:-$(pwd)/out}
mkdir -p $OUT

build=$WORK/build
rm -rf $build
mkdir -p $build

if [ -z "$FUZZING_ENGINE" ]; then
    fuzzflag="llvm-fuzz=enabled"
fi

if ! meson $build -D$fuzzflag ; then
    cat $build/meson-logs/meson-log.txt
    exit 1
fi

ninja -v -C $build
