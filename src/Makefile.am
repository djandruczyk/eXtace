Multimediadir = $(datadir)/gnome/apps/Multimedia

INCLUDES = \
	-I$(top_srcdir)/include \
	@GTK_CFLAGS@ \
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
	comedi_input.c \
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
	main.c \
	markers.c \
	options.c \
	reducer.c \
	scope.c \
	spike_3d.c \
	stars.c \
	vert_specgram.c \
	win_func.c

extace_LDADD = @EFENCE@ @GTK_LIBS@ @ESD_LIBS@ @EXTRA_LIBS@ @ALSA_LIBS@ -lgdk_imlib 



LIBS= -lpthread

COMMON_FLAGS = @PFLAGS@ @DFLAGS@ 

CFLAGS += $(COMMON_FLAGS) -Wall 

CLEANFILES = *~ core* .deps/* gmon.out nohup.out -r .deps

EXTRA_DIST = extace.desktop Colormap-format

Multimedia_DATA = extace.desktop
