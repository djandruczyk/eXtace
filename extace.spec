%define name     extace
%define title    eXtace
%define mainname %{name}
%define alsaname %{name}-alsa
%define version  1.7.0
%define release  1
%define descr    %{title} - An Extace Waveform Viewer
%define summalsa %{descr} with ALSA support
%define summoss  %{descr} without ALSA support
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

Summary:         %{summoss}
Name:            %{name}
Version:         %{version}
Release:         %{release}
Source:          %{name}-%{version}.tar.gz
Copyright:       GPL
Group:           %{group}
BuildRoot:       %{tmppath_}/%{name}-%{version}-%{release}-buildroot
Requires:        gnome-libs >= 1.0.11, fftw, esound
BuildRequires:   fftw, fftw-devel, alsa-lib, fileutils
Obsoletes:       %{alsaname}

%description
eXtace is a visual sound display/analysis program for the Gnome Desktop
environment (though it works under other environments as long as gnome/esd
or ALSA is installed and used). Requires ESD or ALSA to function. Includes
various fourier transforms of the audio data in real-time. Displays include
3D textured flying landscape, 16-256 channel graphic EQ, scope 3D pointed 
flying landscape, and a Spectragram. All aspects of the display are fully 
configurable, even the axis placement. 

This version is for users who don't use ALSA.

%package alsa
Summary:         %{summalsa}
Group:           %{group}
Requires:        gnome-libs >= 1.0.11, fftw, alsa
Obsoletes:       %{mainname}

%description alsa
eXtace is a visual sound display/analysis program for the Gnome Desktop
environment (though it works under other environments as long as gnome/esd
or ALSA is installed and used). Requires ESD or ALSA to function. Includes
various fourier transforms of the audio data in real-time. Displays include
3D textured flying landscape, 16-256 channel graphic EQ, scope 3D pointed 
flying landscape, and a Spectragram. All aspects of the display are fully 
configurable, even the axis placement. 

This version is for users who use ALSA.

%prep
# remove build directories.  better do it by hand as I later on move
# them around
rm -fr $RPM_BUILD_DIR/%{name}-%{version} $RPM_BUILD_DIR/%{mainname}-alsa

# Unpack main source
%setup -q

# Copy source tree to dir %{alsaname} for later building the alsa version
cp -a $RPM_BUILD_DIR/%{name}-%{version} $RPM_BUILD_DIR/%{alsaname}

%build
# First build the normal/OSS version, and force to ignore ALSA even if present
%configure --disable-alsa
%make

# Now build the ALSA version.  ALSA support is built by default if available
cd $RPM_BUILD_DIR/%{alsaname}
%configure
%make

%install
rm -rf %buildroot

# First install the OSS version
%makeinstall

# Use /etc/alternatives to have it point to the right binary
# The "normal" one is named extace-oss, the alsa bin is called extace-alsa
mv %buildroot%{_bindir}/%{name} %buildroot%{_bindir}/%{name}-oss

# And now install the alsa version
cd $RPM_BUILD_DIR/%{alsaname}
%makeinstall
# Rename the binary so that it doesn't overwrite the pointer to /etc/alternatives
mv %buildroot%{_bindir}/%{name} %buildroot%{_bindir}/%{alsaname}

ln -sf %{_sysconfdir}/alternatives/%{name} %buildroot%{_bindir}/%{name}

# Copy another nice utility.  This one creates a sine-wave.  Turn you phones to
# LOUD when you use this.... :-]
# Or, maybe not....
mkdir -p %buildroot%{bindir_}
cp extace/sine %buildroot%{bindir_}

# Only needed in Mandrake:
# Create menu entry for the package
mkdir -p %buildroot%{menudir_}
cat - << EOF > %buildroot%{menudir_}/%{name}
?package(%{name}):command="%{bindir_}/%{name}" \
                 needs="X11" section="%{section}" title="%{title}" \
                 longtitle="%{summoss}"
EOF

cat - << EOF > %buildroot%{menudir_}/%{alsaname}
?package(%{alsaname}):command="%{bindir_}/%{name}" \
                 needs="X11" section="%{section}" title="%{title}" \
                 longtitle="%{summalsa}"
EOF

%post
# Update /etc/alternatives to point to the right binary file
[ ! -d %{_sysconfdir}/alternatives ] && mkdir -p %{_sysconfdir}/alternatives
ln -sf %{_bindir}/%{name}-oss %{_sysconfdir}/alternatives/%{name}
ln -sf %{_sysconfdir}/alternatives/%{name} %{_bindir}/%{name}
# Only in Mandrake:
# Update menus
#% {update_menus}

%postun
rm -f %{_sysconfdir}/alternatives/%{name}
# Only in Mandrake:
# Remove the menu entry
#% {clean_menus}

%post alsa
# Update /etc/alternatives to point to the right binary file
[ ! -d %{_sysconfdir}/alternatives ] && mkdir -p %{_sysconfdir}/alternatives
ln -sf %{_bindir}/%{name}-alsa %{_sysconfdir}/alternatives/%{name}
ln -sf %{_sysconfdir}/alternatives/%{name} %{_bindir}/%{name}
# Only in Mandrake:
# Update menus
#% {update_menus}

%postun alsa
rm -f %{_sysconfdir}/alternatives/%{name}
# Only in Mandrake:
# Remove the menu entry
#% {clean_menus}

%clean
rm -rf %buildroot
rm -fr $RPM_BUILD_DIR/%{name}-%{version} $RPM_BUILD_DIR/%{alsaname}

%files
%defattr(-,root,root,0755)
%doc TODO AUTHORS CREDITS NEWS ChangeLog README
%{bindir_}/extace
%{bindir_}/extace-oss
%{bindir_}/sine
%{datadir_}/gnome/apps/Multimedia/extace.desktop
# Only Mandrake:
#% {menudir_}/%{name}

%files alsa
%defattr(-,root,root,0755)
%doc TODO AUTHORS CREDITS NEWS ChangeLog README
%{bindir_}/extace
%{bindir_}/extace-alsa
%{bindir_}/sine
%{datadir_}/gnome/apps/Multimedia/extace.desktop
# Only Mandrake:
#% {menudir_}/%{alsaname}

%changelog
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
