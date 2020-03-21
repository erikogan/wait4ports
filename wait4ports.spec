Summary: A small utility to wait for network availability
Name: wait4ports
Version: 0.3.1
Release: 1
License: MPL 2.0
Group: Applications/Utilites
URL: https://github.com/erikogan/wait4ports
# https://github.com/erikogan/%{name}/archive/v%{version}.tar.gz
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
*  Sat Mar 21 2020 Erik Ogan <erik@stealthymonkeys.com> - 0.3.1-1
- Add make check target to abstract integration testing

*  Sat Mar 21 2020 Erik Ogan <erik@stealthymonkeys.com> - 0.3.0-1
- Add an optional timeout to wait before exiting in error

*  Sat Mar 21 2020 Erik Ogan <erik@stealthymonkeys.com> - 0.2.4-1
- Fix the SEGV when getaddrinfo does not return an ai_canonname.

* Mon Sep 10 2018  Erik Ogan <erik@stealthymonkeys.com> - 0.2.3-1
- Fix the case where the first argument is not a valid peer, but subsequent
  arguments are valid

* Sun Sep  2 2018 Erik Ogan <erik@stealthymonkeys.com> - 0.2.2-1
- Add 0 to the list of values that will disable verbosity via
  WAIT4PORTS_VERBOSE environment variable

* Sat Sep  1 2018 Erik Ogan <erik@stealthymonkeys.com> - 0.2.1-1
- Add -v for version printing

* Sat Sep  1 2018 Erik Ogan <erik@stealthymonkeys.com> - 0.2.0-1
- Allow verbosity and sleep interval to be set via options or environment variables

* Sat Sep  1 2018 Erik Ogan <erik@stealthymonkeys.com> - 0.1.0-1
- Support IPv6
- Support hostname lookups
- Better error notifications

* Tue Feb 21 2017 Erik Ogan <erik@stealthymonkeys.com> - 0.0.4-1
- Add Alpine APKBUILD
- Mozilla License 2.0

* Sun Jun 28 2015 Erik Ogan <erik@stealthymonkeys.com> - 0.0.3-1
- flush output on write

* Sat Jun 27 2015 Erik Ogan <erik@stealthymonkeys.com> - 0.0.2-1
- Initial build.
