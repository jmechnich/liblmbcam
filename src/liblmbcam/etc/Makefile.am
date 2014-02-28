CAMCONFFILES = cam_PYRO.conf

EXTRA_DIST = $(CAMCONFFILES)

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(sysconfdir)/lmbcam
	$(INSTALL_DATA) $(top_srcdir)/src/liblmbcam/etc/*.conf \
		$(DESTDIR)$(sysconfdir)/lmbcam

uninstall-local:
	rm -rf $(DESTDIR)$(sysconfdir)/lmbcam/*.conf
