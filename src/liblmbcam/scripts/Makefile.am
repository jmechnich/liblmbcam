#
# Adjust the PACKAGE... variables to your needs and remake.
#

PACKAGEPREFIX = $(prefix)
PACKAGENAME = liblmbcam
PACKAGEVERSION = $(VERSION)
PACKAGELIBS = -llmbcam
PACKAGELTLIBS = $(libdir)/liblmbcam.la
PACKAGECFLAGS = -I$(pkgincludedir)
PACKAGEEXTCFLAGS = $(CPPFLAGS) -D_REENTRANT
PACKAGELDFLAGS = $(LDFLAGS) @LIBS@ @libdc1394_LIBS@ @libraw1394_LIBS@ @libv4l2_LIBS@ @libudev_LIBS@ -lpthread

#
# 
#

gen_config = sed \
	-e 's,_PACKAGEPREFIX_,$(PACKAGEPREFIX),g' \
	-e 's,_PACKAGENAME_,$(PACKAGENAME),g' \
	-e 's,_PACKAGEVERSION_,$(PACKAGEVERSION),g' \
	-e 's,_PACKAGELTLIBS_,$(PACKAGELTLIBS),g' \
	-e "s,_PACKAGELOCALLTLIBS_,`cd $(top_builddir) && pwd`/src/liblmbcam/liblmbcam.la,g" \
	-e 's,_PACKAGECFLAGS_,$(PACKAGECFLAGS),g' \
	-e 's,_PACKAGEEXTCFLAGS_,$(PACKAGEEXTCFLAGS),g' \
	-e 's,_PACKAGELDFLAGS_,$(PACKAGELDFLAGS),g' \
	-e "s,_PACKAGELOCALCFLAGS_,-I`cd $(top_srcdir) && pwd`/src/liblmbcam,g" \
	-e 's,_PACKAGELIBS_,$(PACKAGELIBS),g' \
	-e "s,_PACKAGELOCALLIBDIR_,`cd $(top_builddir) && pwd`/src/liblmbcam/.libs,g"

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
