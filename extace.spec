%define name     extace
%define title    eXtace
%define mainname %{name}
%define version  1.8.06
%define release  1
%define descr    %{title} - An Extace Waveform Viewer
%define summary  %{descr} That utilizes Esound
%define descrfr    %{title} - Un visualisateur de forme d'onde
%define summaryfr  %{descr} le support Esound
%define group    Sound
%define section  Multimedia/%{group}

# Needed for building generic packages
# Not too sure if these are defined in non-Mandrake distributions:
# %{_exec_prefix}/bin -> /usr/bin
%define bindir_  %{_bindir}
# ~/RPM/tmp
%define tmppath_ %{_tmppath}
# %{_prefix}/share -> /usr/share
%define datadir_ %{_datadir}
# %{_libdir}/menu -> %{_exec_prefix}/%{_lib}/menu -> /usr/lib/menu
%define menudir_ %{_menudir}

Summary:         %{summary}
Summary(fr):     %{summaryfr}
Name:            %{name}
Version:         %{version}
Release:         %{release}
Source:          %{name}-%{version}.tar.gz
Copyright:       GPL
Group:           %{group}
BuildRoot:       %{tmppath_}/%{name}-%{version}-%{release}-buildroot
Requires:        gnome-libs >= 1.0.11, fftw, esound
BuildRequires:   fftw, fftw-devel, esound-devel, fileutils

%description
eXtace is a visual sound display/analysis program for the Gnome Desktop
environment (though it works under other environments as long as gnome/esd
is installed and used). Requires ESD to function. Includes
various fourier transforms of the audio data in real-time. Displays include
3D textured flying landscape, 16-256 channel graphic EQ, scope 3D pointed 
flying landscape, and a Spectragram. All aspects of the display are fully 
configurable, even the axis placement. 

This version is for users who use OSS or ALSA with OSS emulation.

%description -l fr
eXtace est un program d'analyse/affichage de son pour l'environnement
GNOME. Il nécessite ESD pour fonctionner. Il inclus différentes
transformations de Fourier des données audio en temps réel. L'affichage
inclus des vues en 3D, un égaliseur graphique 16-256 canaux, et un
spectrogramme. Tous les aspects de l'affichage sont complètement 
configurable, même la position des axes.
 
Cette version est pour les utilisateurs qui n'utilisent pas ALSA.

%prep
# remove build directories.  better do it by hand as I later on move
# them around
rm -fr $RPM_BUILD_DIR/%{name}-%{version}

# Unpack main source
%setup -q


%build
# First build the normal/OSS version,
%configure 
%make
%install
rm -rf %buildroot

%makeinstall


# Copy another nice utility.  This one creates a sine-wave.  Turn you phones to
# LOUD when you use this.... :-]
# Or, maybe not....
mkdir -p %buildroot%{bindir_}
cp $RPM_BUILD_DIR/%{name}-%{version}/src/sine %buildroot%{bindir_}

# Only needed in Mandrake:
# Create menu entry for the package
mkdir -p %buildroot%{menudir_}
cat - << EOF > %buildroot%{menudir_}/%{name}
?package(%{name}):command="%{bindir_}/%{name}" \
                 needs="X11" section="%{section}" title="%{title}" \
                 longtitle="%{summary}"
EOF

%post
# Only in Mandrake:
# Update menus
#% {update_menus}

%postun
# Only in Mandrake:
# Remove the menu entry
#% {clean_menus}

%clean
rm -rf %buildroot
rm -fr $RPM_BUILD_DIR/%{name}-%{version} 

%files
%defattr(-,root,root,0755)
%doc TODO AUTHORS CREDITS NEWS ChangeLog README
%{bindir_}/extace
%{bindir_}/sine
%{datadir_}/gnome/apps/Multimedia/extace.desktop
# Only Mandrake:
#% {menudir_}/%{name}

%changelog
* Wed Jan 29 2003 Dave Andruczyk <djandruczyk@yahoo.com>
- Removed ALSA support as ALSA 0.9.x no longer offers the functionality 
  that eXace requires...

* Wed Nov 21 2001 Eric Lassauge <lassauge@mail.dotcom.fr>
- Added french translations

* Sun Oct 22 2000 Dave Andruczyk <djandruczyk@yahoo.com> 1.3.9-1
- minor tweaks for eXtace 1.3.9

* Sat Oct 14 2000 Alexander Skwar <ASkwar@Linux-Mandrake.com> 1.3.8-2
- Removed unneccessary warning about ALSA in non-ALSA package
- ALSA package does not require esound anymore

* Sat Oct 14 2000 Alexander Skwar <ASkwar@Linux-Mandrake.com> 1.3.8-1
- Used the Mandrake spec file as a template

* Fri Oct 13 2000 Alexander Skwar <ASkwar@Linux-Mandrake.com> 1.3.8-1
- New version
- Split up in a alsa and non-alsa version, ie. it will create two
  binary packages
- Use %{_sysconfdir}/alternatives to have it point to the right binary

* Mon Sep 18 2000 Alexander Skwar <ASkwar@DigitalProjects.com> 1.3.4-1
- New version
- Now with ALSA support
- Hardcoded to use ALSA card 0, Device 0 and Sub Chan 1

* Sun Aug 27 2000 Alexander Skwar <ASkwar@DigitalProjects.com> 1.3.2-1
- New version
- (Build-)requires fftw

* Wed Apr 26 2000 Lenny Cartier <lenny@mandrakesoft.com> 1.2.0-2
- fix group
- spec helper fixes

* Wed Sep 08 1999 Daouda LO <daouda@mandrakesoft.com>
- 1.2.0 

* Tue Jul 20 1999 Chmouel Boudjnah <chmouel@mandrakesoft.com>

- Initalisation of spec file for Mandrake distribution.
