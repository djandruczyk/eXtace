# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.4-p5 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999, 2001 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

srcdir = .
top_srcdir = ..
prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DESTDIR =

pkgdatadir = $(datadir)/extace
pkglibdir = $(libdir)/extace
pkgincludedir = $(includedir)/extace

top_builddir = ..

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /bin/install -c
INSTALL_PROGRAM = ${INSTALL} $(AM_INSTALL_PROGRAM_FLAGS)
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = i686-pc-linux-gnu
host_triplet = i686-pc-linux-gnu
ALSA_LIBS = 
AS = @AS@
CC = gcc
DFLAGS = 
DLLTOOL = @DLLTOOL@
ECHO = echo
EFENCE = 
ESD_CFLAGS = 
ESD_CONFIG = /usr/bin/esd-config
ESD_LIBS = -L/usr/lib -lesd -laudiofile -lm -lasound
EXEEXT = 
EXTACE_DATADIR = NONE/share/eXtace
EXTACE_MAJOR_VERSION = 1
EXTACE_MICRO_VERSION = 04
EXTACE_MINOR_VERSION = 8
EXTACE_VERSION = 1.8.04
EXTRA_LIBS = -lrfftw -lfftw -lm
GLIB_CFLAGS = -I/usr/include/glib-1.2 -I/usr/lib/glib/include -D_REENTRANT
GLIB_CONFIG = /usr/bin/glib-config
GLIB_LIBS = -L/usr/lib -lgthread -lglib -lpthread
GTK_CFLAGS = -I/usr/include/gtk-1.2 -I/usr/include/glib-1.2 -I/usr/lib/glib/include -D_REENTRANT -I/usr/X11R6/include
GTK_CONFIG = /usr/bin/gtk-config
GTK_LIBS = -L/usr/lib -L/usr/X11R6/lib -lgtk -lgdk -rdynamic -lgmodule -lgthread -lglib -lpthread -ldl -lXi -lXext -lX11 -lm
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
MAINT = #
MAKEINFO = makeinfo
OBJDUMP = @OBJDUMP@
OBJEXT = o
PACKAGE = extace
PFLAGS = 
RANLIB = ranlib
STRIP = strip
VERSION = 1.8.04

Multimediadir = $(datadir)/gnome/apps/Multimedia

INCLUDES = -I/usr/include/gtk-1.2 -I/usr/include/glib-1.2 -I/usr/lib/glib/include -D_REENTRANT -I/usr/X11R6/include -DGTK_DISABLE_COMPAT_H  -D_MAJOR_=1 -D_MINOR_=8 -D_MICRO_=04

noinst_PROGRAMS = sine

sine_LDADD = -lm

bin_PROGRAMS = extace

extace_SOURCES =  	Makefile 	globals.h 	enums.h 	defaults.h 	defines.h 	protos.h 	convolve.h 	configfile.h 	logo.xpm    	stars.c 	init.c 	main.c 	draw.c 	land3d.c 	gc_maker.c 	dir.c 	audio_processing.c 	events.c 	configfile.c 	buttons.c 	color_win.c 	reducer.c 	convolve.c 	win_func.c 	2d_eq.c 	scope.c 	markers.c 	spike_3d.c 	sound.c 	vert_specgram.c 	horiz_specgram.c  	options.c 


extace_LDADD =  -L/usr/lib -L/usr/X11R6/lib -lgtk -lgdk -rdynamic -lgmodule -lgthread -lglib -lpthread -ldl -lXi -lXext -lX11 -lm -L/usr/lib -lesd -laudiofile -lm -lasound -lrfftw -lfftw -lm  -lgdk_imlib 

LIBS = -lpthread

COMMON_FLAGS =   

CFLAGS = -O2  $(COMMON_FLAGS) -Wall 

CLEANFILES = *~ core* .deps/* gmon.out nohup.out -r .deps

EXTRA_DIST = extace.desktop README NEWS TODO Colormap-format

Multimedia_DATA = extace.desktop
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../config.h
CONFIG_CLEAN_FILES = 
bin_PROGRAMS =  extace$(EXEEXT)
noinst_PROGRAMS =  sine$(EXEEXT)
PROGRAMS =  $(bin_PROGRAMS) $(noinst_PROGRAMS)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I..
CPPFLAGS = 
LDFLAGS = 
extace_OBJECTS =  stars.$(OBJEXT) init.$(OBJEXT) main.$(OBJEXT) \
draw.$(OBJEXT) land3d.$(OBJEXT) gc_maker.$(OBJEXT) dir.$(OBJEXT) \
audio_processing.$(OBJEXT) events.$(OBJEXT) configfile.$(OBJEXT) \
buttons.$(OBJEXT) color_win.$(OBJEXT) reducer.$(OBJEXT) \
convolve.$(OBJEXT) win_func.$(OBJEXT) 2d_eq.$(OBJEXT) scope.$(OBJEXT) \
markers.$(OBJEXT) spike_3d.$(OBJEXT) sound.$(OBJEXT) \
vert_specgram.$(OBJEXT) horiz_specgram.$(OBJEXT) options.$(OBJEXT)
extace_DEPENDENCIES = 
extace_LDFLAGS = 
sine_SOURCES = sine.c
sine_OBJECTS =  sine.$(OBJEXT)
sine_DEPENDENCIES = 
sine_LDFLAGS = 
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@
DATA =  $(Multimedia_DATA)

DIST_COMMON =  README AUTHORS ChangeLog Makefile.am Makefile.in NEWS \
TODO


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP_ENV = --best
DEP_FILES =  .deps/2d_eq.P .deps/audio_processing.P .deps/buttons.P \
.deps/color_win.P .deps/configfile.P .deps/convolve.P .deps/dir.P \
.deps/draw.P .deps/events.P .deps/gc_maker.P .deps/horiz_specgram.P \
.deps/init.P .deps/land3d.P .deps/main.P .deps/markers.P \
.deps/options.P .deps/reducer.P .deps/scope.P .deps/sine.P \
.deps/sound.P .deps/spike_3d.P .deps/stars.P .deps/vert_specgram.P \
.deps/win_func.P
SOURCES = $(extace_SOURCES) sine.c
OBJECTS = $(extace_OBJECTS) sine.$(OBJEXT)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .S .c .lo .o .obj .s
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu src/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status $(BUILT_SOURCES)
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-binPROGRAMS:

clean-binPROGRAMS:
	-test -z "$(bin_PROGRAMS)" || rm -f $(bin_PROGRAMS)

distclean-binPROGRAMS:

maintainer-clean-binPROGRAMS:

install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(LIBTOOL)  --mode=install $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`"; \
	    $(LIBTOOL)  --mode=install $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`; \
	  else :; fi; \
	done

uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	list='$(bin_PROGRAMS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`; \
	done

mostlyclean-noinstPROGRAMS:

clean-noinstPROGRAMS:
	-test -z "$(noinst_PROGRAMS)" || rm -f $(noinst_PROGRAMS)

distclean-noinstPROGRAMS:

maintainer-clean-noinstPROGRAMS:

# FIXME: We should only use cygpath when building on Windows,
# and only if it is available.
.c.obj:
	$(COMPILE) -c `cygpath -w $<`

.s.o:
	$(COMPILE) -c $<

.S.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	-rm -f *.o core *.core
	-rm -f *.$(OBJEXT)

clean-compile:

distclean-compile:
	-rm -f *.tab.c

maintainer-clean-compile:

.s.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.S.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:

maintainer-clean-libtool:

extace$(EXEEXT): $(extace_OBJECTS) $(extace_DEPENDENCIES)
	@rm -f extace$(EXEEXT)
	$(LINK) $(extace_LDFLAGS) $(extace_OBJECTS) $(extace_LDADD) $(LIBS)

sine$(EXEEXT): $(sine_OBJECTS) $(sine_DEPENDENCIES)
	@rm -f sine$(EXEEXT)
	$(LINK) $(sine_LDFLAGS) $(sine_OBJECTS) $(sine_LDADD) $(LIBS)

install-MultimediaDATA: $(Multimedia_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(Multimediadir)
	@list='$(Multimedia_DATA)'; for p in $$list; do \
	  if test -f $(srcdir)/$$p; then \
	    echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(Multimediadir)/$$p"; \
	    $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(Multimediadir)/$$p; \
	  else if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(DESTDIR)$(Multimediadir)/$$p"; \
	    $(INSTALL_DATA) $$p $(DESTDIR)$(Multimediadir)/$$p; \
	  fi; fi; \
	done

uninstall-MultimediaDATA:
	@$(NORMAL_UNINSTALL)
	list='$(Multimedia_DATA)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(Multimediadir)/$$p; \
	done

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $$unique $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $$unique $(LISP) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = src

distdir: $(DISTFILES)
	here=`cd $(top_builddir) && pwd`; \
	top_distdir=`cd $(top_distdir) && pwd`; \
	distdir=`cd $(distdir) && pwd`; \
	cd $(top_srcdir) \
	  && $(AUTOMAKE) --include-deps --build-dir=$$here --srcdir-name=$(top_srcdir) --output-dir=$$top_distdir --gnu src/Makefile
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pr $$d/$$file $(distdir)/$$file; \
	  else \
	    test -f $(distdir)/$$file \
	    || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	    || cp -p $$d/$$file $(distdir)/$$file || :; \
	  fi; \
	done

DEPS_MAGIC := $(shell mkdir .deps > /dev/null 2>&1 || :)

-include $(DEP_FILES)

mostlyclean-depend:

clean-depend:

distclean-depend:
	-rm -rf .deps

maintainer-clean-depend:

%.o: %.c
	@echo '$(COMPILE) -c $<'; \
	$(COMPILE) -Wp,-MD,.deps/$(*F).pp -c $<
	@-cp .deps/$(*F).pp .deps/$(*F).P; \
	tr ' ' '\012' < .deps/$(*F).pp \
	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
	    >> .deps/$(*F).P; \
	rm .deps/$(*F).pp

%.lo: %.c
	@echo '$(LTCOMPILE) -c $<'; \
	$(LTCOMPILE) -Wp,-MD,.deps/$(*F).pp -c $<
	@-sed -e 's/^\([^:]*\)\.o[ 	]*:/\1.lo \1.o :/' \
	  < .deps/$(*F).pp > .deps/$(*F).P; \
	tr ' ' '\012' < .deps/$(*F).pp \
	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
	    >> .deps/$(*F).P; \
	rm -f .deps/$(*F).pp
info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am: install-binPROGRAMS
install-exec: install-exec-am

install-data-am: install-MultimediaDATA
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am: uninstall-binPROGRAMS uninstall-MultimediaDATA
uninstall: uninstall-am
all-am: Makefile $(PROGRAMS) $(DATA)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:
	$(mkinstalldirs)  $(DESTDIR)$(bindir) $(DESTDIR)$(Multimediadir)


mostlyclean-generic:

clean-generic:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-binPROGRAMS mostlyclean-noinstPROGRAMS \
		mostlyclean-compile mostlyclean-libtool \
		mostlyclean-tags mostlyclean-depend mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-binPROGRAMS clean-noinstPROGRAMS clean-compile \
		clean-libtool clean-tags clean-depend clean-generic \
		mostlyclean-am

clean: clean-am

distclean-am:  distclean-binPROGRAMS distclean-noinstPROGRAMS \
		distclean-compile distclean-libtool distclean-tags \
		distclean-depend distclean-generic clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-binPROGRAMS \
		maintainer-clean-noinstPROGRAMS \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-tags maintainer-clean-depend \
		maintainer-clean-generic distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: mostlyclean-binPROGRAMS distclean-binPROGRAMS clean-binPROGRAMS \
maintainer-clean-binPROGRAMS uninstall-binPROGRAMS install-binPROGRAMS \
mostlyclean-noinstPROGRAMS distclean-noinstPROGRAMS \
clean-noinstPROGRAMS maintainer-clean-noinstPROGRAMS \
mostlyclean-compile distclean-compile clean-compile \
maintainer-clean-compile mostlyclean-libtool distclean-libtool \
clean-libtool maintainer-clean-libtool uninstall-MultimediaDATA \
install-MultimediaDATA tags mostlyclean-tags distclean-tags clean-tags \
maintainer-clean-tags distdir mostlyclean-depend distclean-depend \
clean-depend maintainer-clean-depend info-am info dvi-am dvi check \
check-am installcheck-am installcheck install-exec-am install-exec \
install-data-am install-data install-am install uninstall-am uninstall \
all-redirect all-am all installdirs mostlyclean-generic \
distclean-generic clean-generic maintainer-clean-generic clean \
mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
