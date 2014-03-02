
ACLOCAL_AMFLAGS = -I build-aux

if DEMO
DEMODIR = demo
endif

EXTRA_HEADERS =

nobase_include_HEADERS = @INCFILES@

EXTRA_DIST = bootstrap devconf.sh conf.sh

SUBDIRS = src test $(DEMODIR)
