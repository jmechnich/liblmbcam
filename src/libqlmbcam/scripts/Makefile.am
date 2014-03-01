#
# Adjust the PACKAGE... variables to your needs and remake.
#

PACKAGEPREFIX = $(prefix)
PACKAGENAME = libqlmbcam
PACKAGEVERSION = $(VERSION)
PACKAGELIBS = -lqlmbcam
PACKAGELTLIBS = $(libdir)/libqlmbcam.la
PACKAGECFLAGS = -I$(includedir)/libqlmbcam
PACKAGEEXTCFLAGS = $(CPPFLAGS) $(LIBFIDWID_CFLAGS) @QtGui_CFLAGS@
PACKAGELDFLAGS = $(LDFLAGS) $(LIBFIDWID_LIBS) @QtGui_LIBS@

#
# 
#

gen_config = sed \
	-e 's,_PACKAGEPREFIX_,$(PACKAGEPREFIX),g' \
	-e 's,_PACKAGENAME_,$(PACKAGENAME),g' \
	-e 's,_PACKAGEVERSION_,$(PACKAGEVERSION),g' \
	-e 's,_PACKAGELTLIBS_,$(PACKAGELTLIBS),g' \
	-e 's,_PACKAGELOCALLTLIBS_,`cd $(top_builddir) && pwd`/src/libqlmbcam/libqlmbcam.la,g' \
	-e 's,_PACKAGECFLAGS_,$(PACKAGECFLAGS),g' \
	-e 's,_PACKAGEEXTCFLAGS_,$(PACKAGEEXTCFLAGS),g' \
	-e 's,_PACKAGELDFLAGS_,$(PACKAGELDFLAGS),g' \
	-e 's,_PACKAGELOCALCFLAGS_,-I`cd $(top_srcdir) && pwd`/src/libqlmbcam,g' \
	-e 's,_PACKAGELIBS_,$(PACKAGELIBS),g' \
	-e 's,_PACKAGELOCALLIBDIR_,`cd $(top_builddir) && pwd`/src/libqlmbcam/.libs,g'

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
