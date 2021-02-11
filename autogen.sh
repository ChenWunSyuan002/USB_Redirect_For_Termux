#!/bin/sh

set -e

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

autoreconf -fi -Werror "$srcdir"

test -n "$NOCONFIGURE" || "$srcdir/configure" "$@"
