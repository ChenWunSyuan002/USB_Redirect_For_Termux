if HAVE_TOOLS
bin_PROGRAMS = usbredirect

usbredirect_SOURCES = \
	usbredirect.c
usbredirect_LDADD = \
	$(LIBUSB_LIBS) \
	$(GIO_UNIX_LIBS) \
	$(GIO_WIN_LIBS) \
	$(top_builddir)/usbredirhost/libusbredirhost.la

usbredirect_CFLAGS = \
	$(LIBUSB_CFLAGS) \
	$(GIO_UNIX_CFLAGS) \
	$(GIO_WIN_CFLAGS) \
	-I$(top_srcdir)/usbredirhost \
	-I$(top_srcdir)/usbredirparser

dist_man_MANS = usbredirect.1
endif
