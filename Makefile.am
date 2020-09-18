SUBDIRS = usbredirparser usbredirhost docs fuzzing tools
if ! OS_WIN32
SUBDIRS += usbredirserver  usbredirtestclient
endif

EXTRA_DIST =			\
	 README.md		\
	 ChangeLog.md

MAINTAINERCLEANFILES =					\
	$(GITIGNORE_MAINTAINERCLEANFILES_TOPLEVEL)	\
	$(GITIGNORE_MAINTAINERCLEANFILES_MAKEFILE_IN)	\
	$(GITIGNORE_MAINTAINERCLEANFILES_M4_LIBTOOL)

-include $(top_srcdir)/git.mk
