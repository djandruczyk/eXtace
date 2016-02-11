Multimediadir = $(datadir)/gnome/apps/Multimedia

INCLUDES = \
	-I$(top_srcdir)/include \
	@PACKAGE_CFLAGS@ \
	-DGTK_DISABLE_COMPAT_H \
	-D_MAJOR_=@EXTACE_MAJOR_VERSION@ \
	-D_MINOR_=@EXTACE_MINOR_VERSION@ \
	-D_MICRO_=@EXTACE_MICRO_VERSION@


noinst_PROGRAMS = sweep

sweep_LDADD = -lm

bin_PROGRAMS = extace

extace_SOURCES = \
	Makefile \
	logo.xpm \
	2d_eq.c \
	buttons.c \
	color_win.c \
	configfile.c \
	convolve.c \
	dir.c \
	draw.c \
	events.c \
	gc_maker.c \
	horiz_specgram.c \
	init.c \
	input.c \
	input_processing.c \
	land3d.c \
	line_eq.c \
	main.c \
	markers.c \
	options.c \
	reducer.c \
	scope.c \
	spike_3d.c \
	stars.c \
	vert_specgram.c \
	vert_specgram2.c \
	win_func.c

extace_LDADD = @EFENCE@ @PACKAGE_LIBS@ @EXTRA_LIBS@ -lm

LIBS= -lpthread

COMMON_FLAGS = @PFLAGS@ @DFLAGS@ 

AM_CFLAGS = $(COMMON_FLAGS) -Wall 

CLEANFILES = *~ core* gmon.out nohup.out 

EXTRA_DIST = extace.desktop Colormap-format

Multimedia_DATA = extace.desktop
