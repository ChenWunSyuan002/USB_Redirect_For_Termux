# Use C++ compiler and linker
AM_DEFAULT_SOURCE_EXT = .cc

if ENABLE_FUZZING

noinst_PROGRAMS = usbredirparserfuzz
noinst_LIBRARIES = libstandalone.a

libstandalone_a_SOURCES = standalone.c

usbredirparserfuzz_SOURCES = usbredirparserfuzz.cc
usbredirparserfuzz_CXXFLAGS = -g -I$(top_srcdir)/usbredirparser
usbredirparserfuzz_LDFLAGS = $(top_builddir)/usbredirparser/libusbredirparser.la
usbredirparserfuzz_LDADD = $(LIB_FUZZING_ENGINE)
usbredirparserfuzz_DEPENDENCIES = libstandalone.a

endif

EXTRA_DIST =	\
	 meson.build

-include $(top_srcdir)/git.mk
