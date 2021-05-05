#!/usr/bin/env bash
# SPDX-License-Identifier: LGPL-2.1-or-later

set -eux -o pipefail

export LC_CTYPE=C.UTF-8
export WORK="${WORK:-${PWD}/build}"

config=(
    --default-library=static
    -Dprefix="${OUT:?}"

    -Dfuzzing=enabled
    -Dfuzzing-engine="${LIB_FUZZING_ENGINE:?}"
    -Dfuzzing-install-dir="${OUT:?}"

    # Fails to build on Ubuntu 16.04
    -Dtools=disabled

    # Don't use "-Wl,--no-undefined"
    -Db_lundef=false
    )

# Meson doesn't apply changed options without "--reconfigure" or "--wipe", but
# those fail when the build directory is not already configured. This issue can
# be avoided by checking for a configured build directory first.
#
# https://github.com/mesonbuild/meson/issues/7261
if [[ -d "$WORK" ]] && meson introspect --projectinfo -- "$WORK"; then
    config+=( --reconfigure )
fi

if ! meson setup "${config[@]}" -- "$WORK"; then
    cat "${WORK}/meson-logs/meson-log.txt" >&2 || :
    exit 1
fi

meson compile -C "$WORK" -v

meson install -C "$WORK"

exit 0
