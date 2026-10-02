Name:           koverlay
Version:        1.1.0
Release:        1%{?dist}
Summary:        Click-through overlay for Wayland (Qt 6 + LayerShellQt)

License:        MIT
URL:            https://github.com/MallarDev/koverlay
# Point to a release tarball or use git archive (see below)
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  extra-cmake-modules
BuildRequires:  gcc-c++
BuildRequires:  make
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtdeclarative-devel
BuildRequires:  qt6-qtwayland-devel
BuildRequires:  layer-shell-qt-devel
BuildRequires:  wayland-devel
BuildRequires:  pkgconfig(xkbcommon)

# Helpful runtime hints (autodeps will also catch .so):
Requires:       layer-shell-qt
Requires:       qt6-qtbase
Requires:       qt6-qtdeclarative
Requires:       qt6-qtwayland
Requires:       libksysguard
Requires:       kf6-kquickcharts
Requires:       ksystemstats

%description
KOverlay is a click-through, always-on-top overlay for KDE/Wayland (Qt 6 + QML + LayerShellQt).
Useful for sticky notes, cheat sheets, and keybindings. Supports hot-reloading config.
Includes CPU, GPU, memory and disk history charts backed by KDE sensors.

%prep
%autosetup -n %{name}-%{version}

%build
%cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo
%cmake_build

%install
%cmake_install


%files
%license LICENSE
%doc README.md
%{_bindir}/koverlay
%{_datadir}/applications/koverlay.desktop

%changelog
* Sat Oct 25 2025 Erik - 1.0.1-1
- New: configurable overlay position (top-left, top-right, bottom-left, bottom-right, custom x/y)
- New: hot-reload when using `textFile=` (watches file and parent dir; handles atomic saves)
- Tweak: trim a single trailing newline when reading `textFile` to avoid extra blank line in UI
- Build: bump project to 1.0.1 (CMake, CPack/RPM)
* Sat Oct 04 2025 Erik - 1.0.0-1
- Initial build
