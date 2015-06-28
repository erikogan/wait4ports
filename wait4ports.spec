Summary: A small utility to wait for network availability
Name: wait4ports
Version: 0.0.3
Release: 1
License: GPL
Group: Applications/Utilites
URL: https://github.com/erikogan/wait4ports
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
A utility to wait until a set of network peers are running. It polls each
address and port, sleeping between runs.

Designed to be used in docker, specifically docker-compose, containers that
need to wait for linked containers to be ready.

%prep
%setup -n wait4ports

%build
make

%install
rm -rf $RPM_BUILD_ROOT
make PREFIX=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
/usr/bin/wait4ports
%doc


%changelog
* Sun Jun 28 2015 Erik Ogan <erik@stealthymonkeys.com> - 0.0.3-1
- flush output on write

* Sat Jun 27 2015 Erik Ogan <erik@stealthymonkeys.com> - 0.0.2-1
- Initial build.
