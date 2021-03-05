noinst_PROGRAMS = usbredirtestclient

usbredirtestclient_SOURCES = usbredirtestclient.c
usbredirtestclient_LDADD = $(top_builddir)/usbredirparser/libusbredirparser.la
usbredirtestclient_CFLAGS = -I$(top_srcdir)/usbredirparser

EXTRA_DIST =	\
	 meson.build

-include $(top_srcdir)/git.mk
