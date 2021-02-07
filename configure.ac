AC_PREREQ(2.63)
AC_INIT([usbredir], [0.8.0])
AC_CONFIG_SRCDIR([configure.ac])

AM_CONFIG_HEADER([config.h])

dnl libtool versioning
# +1 : 0 : +1  == adds new functions to the interface
# +1 : 0 : 0   == changes or removes functions (changes include both
#                 changes to the signature and the semantic)
#  ? :+1 : ?   == just internal changes
# CURRENT : REVISION : AGE
LIBUSBREDIRHOST_SO_VERSION=1:1:0
AC_SUBST(LIBUSBREDIRHOST_SO_VERSION)

LIBUSBREDIRPARSER_SO_VERSION=1:1:0
AC_SUBST(LIBUSBREDIRPARSER_SO_VERSION)

AM_INIT_AUTOMAKE([foreign dist-bzip2 no-dist-gzip])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_PROG_CC
AC_PROG_CXX
AC_HEADER_STDC
AC_PROG_INSTALL
AC_PROG_LN_S
LT_INIT
dnl AC_DEFINE(_GNU_SOURCE, [1], [Enable GNU extensions])

AC_MSG_CHECKING([for native Win32])
echo "host is $host"
case "$host" in
     *mingw*|*cygwin*)
        os_win32=yes
        ;;
     *)
        os_win32=no
        ;;
esac
AC_MSG_RESULT([$os_win32])
AM_CONDITIONAL([OS_WIN32],[test "$os_win32" = "yes"])

AC_ARG_ENABLE([fuzzing],
  AS_HELP_STRING([--enable-fuzzing@<:@=MODE@:>@],
    [Enable libFuzzer integration]))

AS_CASE(["$enable_fuzzing"], [yes], [
  AC_REQUIRE([AC_PROG_CXX])

  AC_LANG_PUSH([C++])
  AC_CHECK_HEADER([fuzzer/FuzzedDataProvider.h], [], [
    AC_MSG_FAILURE([libFuzzer headers are required])
  ])

  save_LDFLAGS="$LDFLAGS"
  LDFLAGS="${LDFLAGS} ${LIB_FUZZING_ENGINE}"
  AC_MSG_CHECKING([for fuzzing engine])
  # Test whether linker already provides a main function, as is the case with
  # fuzzing-enabled compilers.
  AC_LINK_IFELSE([
    AC_LANG_SOURCE([[
      #include <cstddef>
      #include <cstdint>
      extern "C" int LLVMFuzzerTestOneInput(const uint8_t *, size_t) {
        return 0;
      }
    ]])], [
    AC_MSG_RESULT(yes)
    ], [
    AC_MSG_RESULT([not found, enabling standalone replacement])
    AS_VAR_SET([LIB_FUZZING_ENGINE], [libstandalone.a])
    ])
  LDFLAGS="$save_LDFLAGS"

  AC_LANG_POP([C++])
])

AM_CONDITIONAL([ENABLE_FUZZING], [test "x$enable_fuzzing" = "xyes"])
AC_ARG_VAR([LIB_FUZZING_ENGINE], [Location of prebuilt fuzzing engine library])
AC_SUBST([LIB_FUZZING_ENGINE])

# Set some sane default CFLAGS, avoid having to do another release like 0.4.1
if test "$ac_test_CFLAGS" != set; then
  DEFAULT_CFLAGS="-Wall -Werror -Wp,-D_FORTIFY_SOURCE=2 -fstack-protector --param=ssp-buffer-size=4"
  for F in $DEFAULT_CFLAGS; do
    AC_MSG_CHECKING([whether $CC supports $F])
    save_CFLAGS="$CFLAGS"
    CFLAGS="$CFLAGS $F"
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([ ])], [cc_flag=yes], [cc_flag=no])
    if test "x$cc_flag" != "xyes"; then
      CFLAGS="$save_CFLAGS"
    fi
    AC_MSG_RESULT([$cc_flag])
  done
fi

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES(LIBUSB, [libusb-1.0 >= 1.0.9])

AC_CONFIG_FILES([
Makefile
docs/Makefile
usbredirhost/Makefile
usbredirhost/libusbredirhost.pc
usbredirparser/Makefile
usbredirparser/libusbredirparser-0.5.pc
usbredirserver/Makefile
usbredirtestclient/Makefile
fuzzing/Makefile
])
AC_OUTPUT
