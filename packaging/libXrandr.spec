Name:           libXrandr
Version:        1.4.0
Release:        3
License:        MIT
Summary:        X
Url:            http://www.x.org
Group:          System Environment/Libraries

Source:         %{name}-%{version}.tar.bz2

BuildRequires:  pkgconfig(randrproto) >= 1.3.0
BuildRequires:  pkgconfig(xext)
BuildRequires:  pkgconfig(xorg-macros)
BuildRequires:  pkgconfig(xproto)
BuildRequires:  pkgconfig(xrender)

%description
X.Org X11 libXrandr runtime library

%package devel
Summary:        X
Group:          Development/Libraries
Requires:       %{name} = %{version}
Provides:       libxrandr-devel

%description devel
X.Org X11 libXrandr development package

%prep
%setup -q

%build
%reconfigure  --disable-static \
	       LDFLAGS="${LDFLAGS} -Wl,--hash-style=both -Wl,--as-needed"
make %{?_smp_mflags}

%install

%make_install

rm -f %{buildroot}%{_libdir}/*.la

%remove_docs

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%doc AUTHORS COPYING ChangeLog
%{_libdir}/libXrandr.so.2
%{_libdir}/libXrandr.so.2.2.0

%files devel
%defattr(-,root,root,-)
%{_includedir}/X11/extensions/Xrandr.h
%{_libdir}/libXrandr.so
%{_libdir}/pkgconfig/xrandr.pc
#%dir %{_mandir}/man3x
#%{_mandir}/man3/*.3*
