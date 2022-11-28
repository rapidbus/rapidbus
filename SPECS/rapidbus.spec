Name:		RapidBus
Version:	0.1.2
Release:	1%{?dist}
Summary:	MODBUS RTU to MQTT bridge

Group:		Development/Tools
License:	dual-licensed under GPL 2.0 and GPL 3.0
URL:		https://github.com/rapidbus/rapidbus
Source0:	https://github.com/rapidbus/rapidbus

BuildRequires:	gcc
BuildRequires:	make
#Requires:	

%description
Software for reading MODBUS data over serial (RTU) and forwarding interpreted data to MQTT

%prep
%setup -q


%build
%configure
make %{?_smp_mflags}


%install
make install DESTDIR=%{buildroot}


%files
%doc



%changelog

