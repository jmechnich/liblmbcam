#
# Adjust the PACKAGE... variables to your needs and remake.
#

PACKAGEPREFIX = $(prefix)
PACKAGENAME = liblmbcam
PACKAGEVERSION = $(VERSION)
PACKAGELOCALCFLAGS = -I$(abs_top_srcdir)/src/liblmbcam
PACKAGELOCALLIBDIR = $(abs_top_builddir)/src/liblmbcam/.libs
PACKAGELIBS = -llmbcam
PACKAGELTLIBS = $(libdir)/liblmbcam.la
PACKAGELOCALLTLIBS = $(abs_top_builddir)/src/liblmbcam/liblmbcam.la
PACKAGECFLAGS = -I$(pkgincludedir)
PACKAGEEXTCFLAGS = $(CPPFLAGS) -D_REENTRANT
PACKAGELDFLAGS = $(LDFLAGS) @libdc1394_LIBS@ @libraw1394_LIBS@ -lpthread

abs_top_builddir := $(shell cd $(top_builddir) && pwd)
abs_top_srcdir := $(shell cd $(top_srcdir) && pwd)

#
# 
#

gen_config = sed \
	-e 's,_PACKAGEPREFIX_,$(PACKAGEPREFIX),g' \
	-e 's,_PACKAGENAME_,$(PACKAGENAME),g' \
	-e 's,_PACKAGEVERSION_,$(PACKAGEVERSION),g' \
	-e 's,_PACKAGELTLIBS_,$(PACKAGELTLIBS),g' \
	-e 's,_PACKAGELOCALLTLIBS_,$(PACKAGELOCALLTLIBS),g' \
	-e 's,_PACKAGECFLAGS_,$(PACKAGECFLAGS),g' \
	-e 's,_PACKAGEEXTCFLAGS_,$(PACKAGEEXTCFLAGS),g' \
	-e 's,_PACKAGELDFLAGS_,$(PACKAGELDFLAGS),g' \
	-e 's,_PACKAGELOCALCFLAGS_,$(PACKAGELOCALCFLAGS),g' \
	-e 's,_PACKAGELIBS_,$(PACKAGELIBS),g' \
	-e 's,_PACKAGELOCALLIBDIR_,$(PACKAGELOCALLIBDIR),g'

$(PACKAGENAME)-config: package-config.in Makefile
	$(gen_config) $< > $@
	chmod +x $@

$(PACKAGENAME)-local-config: package-local-config.in Makefile
	$(gen_config) $< > $@
	chmod +x $@

all: $(PACKAGENAME)-config $(PACKAGENAME)-local-config

install-exec-local: all
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	$(INSTALL) $(PACKAGENAME)-config $(DESTDIR)$(bindir)
	$(INSTALL) $(PACKAGENAME)-local-config $(DESTDIR)$(bindir)

uninstall-local:
	rm -f $(DESTDIR)$(bindir)/$(PACKAGENAME)-config
	rm -f $(DESTDIR)$(bindir)/$(PACKAGENAME)-local-config
	-rmdir $(DESTDIR)$(bindir)

MOSTLYCLEANFILES = $(PACKAGENAME)-config $(PACKAGENAME)-local-config

EXTRA_DIST = package-config.in package-local-config.in