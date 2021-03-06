%bcond_with x

Name:           libXrandr
Version:        1.4.2
Release:        3
License:        MIT
Summary:        X Resize and Rotate Extension C Library
Url:            http://www.x.org
Group:          Graphics/X Window System

Source:         %{name}-%{version}.tar.bz2
Source1001: 	libXrandr.manifest

BuildRequires:  pkgconfig(randrproto) >= 1.3.0
BuildRequires:  pkgconfig(xext)
BuildRequires:  pkgconfig(xorg-macros)
BuildRequires:  pkgconfig(xproto)
BuildRequires:  pkgconfig(xrender)

%if !%{with x}
ExclusiveArch:
%endif

%description
X.Org X11 libXrandr runtime library

%package devel
Summary:        X Resize and Rotate Extension C Library
Group:          Development/Libraries
Requires:       %{name} = %{version}

%description devel
X.Org X11 libXrandr development package

%prep
%setup -q
cp %{SOURCE1001} .

%build
%autogen  --disable-static
make %{?_smp_mflags}

%install

%make_install

%remove_docs

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%license COPYING
%{_libdir}/libXrandr.so.2
%{_libdir}/libXrandr.so.2.2.0

%files devel
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_includedir}/X11/extensions/Xrandr.h
%{_libdir}/libXrandr.so
%{_libdir}/pkgconfig/xrandr.pc
