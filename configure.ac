dnl Process this file with autoconf to produce a configure script.

AC_INIT(src/main.c)
dnl AC_PREREQ(2.54)

EXTACE_MAJOR_VERSION=1
EXTACE_MINOR_VERSION=9
EXTACE_MICRO_VERSION=4

EXTACE_VERSION=$EXTACE_MAJOR_VERSION.$EXTACE_MINOR_VERSION.${EXTACE_MICRO_VERSION}

AM_INIT_AUTOMAKE(extace, 1.9.4)
AM_CONFIG_HEADER(config.h)

dnl AM_MAINTAINER_MODE

dnl =========================================================================
dnl control variables
dnl =========================================================================

have_fftw3=no
have_fftw2=no
have_alsa=no
have_oss=no
have_esd=no

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_PROG_LIBTOOL

dnl Checks for header files.
AC_CHECK_HEADERS(asm/errno.h drfftw.h esd.h fcntl.h math.h pthread.h  rfftw.h stdio.h stdlib.h string.h sys/ioctl.h sys/poll.h sys/time.h sys/stat.h sys/types.h unistd.h)


dnl 
dnl check for gtk, imlibgdk, gthread
dnl

pkg_modules="gtk+ imlibgdk gthread"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

dnl POSIX.4 threads
dnl ---
AC_CHECK_LIB(pthread,pthread_create, ,
         AC_CHECK_LIB(c_r,pthread_create,LDFLAGS="-pthreads",
                 AC_MSG_ERROR([** POSIX.4 threads not installed or broken **])))


CFLAGS="-O2 "
AC_SUBST(CFLAGS)

EFENCE=""
AC_SUBST(EFENCE)

EXTRA_LIBS=""

dnl
dnl Check for ESOUND library 
dnl
AC_CHECK_LIB(esd, esd_monitor_stream,have_esd=yes,have_esd=no,[`esd-config --libs`]) 
if test "$have_esd" = "yes"
then
	AC_DEFINE(HAVE_ESD,1,[Build with Esound input])
	EXTRA_LIBS="`esd-config --libs` $EXTRA_LIBS "
fi

dnl
dnl Check for ALSA library (can also do OSS under emulation)
dnl
AC_CHECK_LIB(asound, snd_pcm_open, have_alsa=yes, have_alsa=no) 
if test "$have_alsa" = "yes"
then
   AC_DEFINE(HAVE_ALSA,1,[Build with ALSA input]) 
   EXTRA_LIBS="-lasound $EXTRA_LIBS "
fi

dnl
dnl Check for OSS library
dnl
AC_CHECK_HEADER(sys/soundcard.h, have_oss=yes, have_oss=no)
if test "$have_oss" = "yes"
then
	AC_DEFINE(HAVE_OSS,1,[Build with OSS input])
fi

dnl
dnl Check for COMEDI library
dnl
AC_CHECK_LIB(comedi, comedi_open, have_comedi=yes, have_comedi=no,[ -lm])
if test "$have_comedi" = "yes"
then
	AC_DEFINE(HAVE_COMEDI,1,[Build with COMEDI input]) 
	EXTRA_LIBS="-lcomedi $EXTRA_LIBS "
fi


dnl fftw3
dnl
PROGRAMS_EXTACE=""
AC_CHECK_LIB(fftw3, fftw_execute, have_fftw3=yes, have_fftw3=no,[-lfftw3 -lm] )
if test "$have_fftw3" = "yes"
then
 	AC_CHECK_LIB(fftw3, fftw_execute,, )
	AC_DEFINE(USING_FFTW3,1,[Build with FFTW 3.x input])
	PROGRAMS_EXTACE=extace
	EXTRA_LIBS="-lfftw3 $EXTRA_LIBS "	
else
	dnl rfftw
	dnl
	PROGRAMS_EXTACE=""
	AC_CHECK_LIB(rfftw, rfftw_one, have_fftw2=yes, have_fftw2=no,[-lfftw -lm] )
	if test "$have_fftw2" = "yes"
	then
 		AC_CHECK_LIB(rfftw, rfftw_one,, )
		AC_DEFINE(USING_FFTW2,1,[Build with FFTW 2.x input])
		PROGRAMS_EXTACE=extace
		EXTRA_LIBS="-lrfftw -lfftw $EXTRA_LIBS "	
	fi
fi

if test "$have_fftw2" = "no"
then
	if test "$have_fftw3" = "no"
	then
		echo 
		echo "!!! FFTW libraries NOT found. eXtace will NOT compile without them!!!"
		echo
		echo "Please install the FFTW 3.x library (Fastest FFT in the West), it is available from http://www.fftw.org or from your most convienient .RPM .DEB or Gentoo mirror"
		echo
		echo " - Gentoo users can get it via \"emerge fftw\" as root"
		echo " - Debian users can get it via \"apt-get fftw\" or \"apt-get fftw-lib\" as root"
		echo " - RPM users (mandrake, suse, redhat) can install it via \"urpmi fftw\" or \"urpmi fftw-lib\", or via http://RPMfind.net as root"
		echo
		exit 1
	fi

fi

dnl ===============
dnl Debugging stuff
dnl ===============

AC_ARG_ENABLE(debug,
[  --enable-debug	  enable debugging [default=no]],, enable_debug=no)

if test "$enable_debug" = yes; then
DFLAGS="$DFLAGS -g -DDEBUG"
fi
AC_SUBST(DFLAGS)

dnl Experimental Features support code
AC_ARG_ENABLE(experimental,
[  --enable-experimental	  enable experimental [default=no]],, enable_experimental=no)

if test "$enable_experimental" = yes; then
DFLAGS="$DFLAGS -DEXPERIMENTAL"
fi
AC_SUBST(DFLAGS)

dnl Drag'n Drop routines Debugging code
AC_ARG_ENABLE(dnd-debug,
[  --enable-dnd-debug	  enable DND debugging ],, enable_dnd_debug=no)

if test "$enable_dnd_debug" = yes; then
DFLAGS="$DFLAGS -g -DDND_DEBUG"
fi
AC_SUBST(DFLAGS)

dnl Window function Debugging code
AC_ARG_ENABLE(window-debug,
[  --enable-window-debug	  enable window function debugging ],, enable_window_debug=no)

if test "$enable_window_debug" = yes; then
DFLAGS="$DFLAGS -g -DWINDOW_DEBUG"
fi
AC_SUBST(DFLAGS)

dnl Use electric fence (malloc debugger)
AC_ARG_ENABLE(malloc-debug,
[  --enable-malloc-debug	  enable Malloc code debugging via ElectricFence ],, enable_malloc_debug=no)

if test "$enable_malloc_debug" = yes; then
	AC_CHECK_LIB(efence, malloc,, )
EFENCE="$EFENCE -lefence"
fi
AC_SUBST(EFENCE)

dnl ==============
dnl profiling
dnl =============
AC_ARG_ENABLE(profiling,
[  --enable-profiling	  enable profiling ],, enable_profiling=no)

if test "$enable_profiling" = yes; then
PFLAGS="-pg"
fi
AC_SUBST(PFLAGS)

AC_SUBST(EXTRA_LIBS)
EXTACE_DATADIR="${prefix}/share/eXtace"
AC_SUBST(EXTACE_DATADIR)
AC_SUBST(EXTACE_MAJOR_VERSION)
AC_SUBST(EXTACE_MINOR_VERSION)
AC_SUBST(EXTACE_MICRO_VERSION)
AC_SUBST(EXTACE_VERSION)
AC_OUTPUT([
Makefile 
src/Makefile
include/Makefile
extace.spec
VERSION
])

echo "
Configuration:

    Install Path:			${prefix}
    FFTW 3.x Support:			${have_fftw3}
    FFTW 2.x Support:			${have_fftw2}
    Experimental Features:		${enable_experimental}
    General Debugging:			${enable_debug}
    Drag'n Drop Debugging:		${enable_dnd_debug}
    Window Function Debugging:		${enable_window_debug}
    Memory Allocation Debugging:	${enable_malloc_debug}
    Profiling:				${enable_profiling}
    ESD Input Support:			${have_esd}
    ALSA Input Support:			${have_alsa}
    OSS Input Support:			${have_oss}
    COMEDI Input Support:		${have_comedi}
"

