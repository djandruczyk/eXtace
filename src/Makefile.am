Multimediadir = $(datadir)/gnome/apps/Multimedia

INCLUDES = @GTK_CFLAGS@ -DGTK_DISABLE_COMPAT_H  -D_MAJOR_=@EXTACE_MAJOR_VERSION@ -D_MINOR_=@EXTACE_MINOR_VERSION@ -D_MICRO_=@EXTACE_MICRO_VERSION@


noinst_PROGRAMS = sine

sine_LDADD = -lm

bin_PROGRAMS = extace

extace_SOURCES = \
	Makefile \
	globals.h \
	enums.h \
	defaults.h \
	defines.h \
	protos.h \
	convolve.h \
	configfile.h \
	stars.h \
	logo.xpm \
   	stars.c \
	init.c \
	main.c \
	draw.c \
	land3d.c \
	gc_maker.c \
	dir.c \
	audio_processing.c \
	events.c \
	configfile.c \
	buttons.c \
	color_win.c \
	reducer.c \
	convolve.c \
	win_func.c \
	2d_eq.c \
	scope.c \
	markers.c \
	spike_3d.c \
	sound.c \
	vert_specgram.c \
	horiz_specgram.c  \
	options.c 

extace_LDADD = @EFENCE@ @GTK_LIBS@ @ESD_LIBS@ @EXTRA_LIBS@ @ALSA_LIBS@ -lgdk_imlib 



LIBS= -lpthread

COMMON_FLAGS = @PFLAGS@ @DFLAGS@ 

CFLAGS += $(COMMON_FLAGS) -Wall 
	
CLEANFILES = *~ core* .deps/* gmon.out nohup.out -r .deps

EXTRA_DIST = extace.desktop README NEWS TODO Colormap-format

Multimedia_DATA = extace.desktop
