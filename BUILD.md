# MPhone Multi-Platform Build

MPhone is built with CMake and Qt. The supported production targets are:

- macOS arm64: `.app` and `.dmg`
- Linux amd64: `.deb` first, AppImage optional
- Windows x64: deployed folder and `.zip`

## Common Requirements

- CMake 3.19+
- C++ compiler for the target OS
- Qt 6.5.x with Widgets, Network, Multimedia, OpenGL, OpenGLWidgets
- Git submodules initialized

```bash
git submodule update --init --recursive
```

## macOS

Install Qt 6.5.x and set `ENV_QT_PATH` to the Qt version folder.

```bash
export ENV_QT_PATH="$HOME/Qt/6.5.3"
ci/mac/build_for_mac.sh RelWithDebInfo arm64
ci/mac/publish_for_mac.sh ../build arm64
ci/mac/package_for_mac.sh
```

Output:

- `output/arm64/RelWithDebInfo/MPhone.app`
- `ci/build/MPhone.dmg`

## Linux

Ubuntu 22.04 recommended:

```bash
sudo apt update
sudo apt install -y \
  build-essential cmake ninja-build dpkg-dev patchelf \
  qt6-base-dev qt6-base-private-dev qt6-multimedia-dev qt6-tools-dev \
  libqt6opengl6-dev libgl1 libgl1-mesa-dev libx11-dev libxcb1-dev \
  libxkbcommon-dev libxkbcommon-x11-dev libdbus-1-3 libudev1

ci/linux/package_deb.sh Release
```

Output:

- `output/x64/Release/MPhone`
- `output/deb/mphone_<version>_amd64.deb`

Install:

```bash
sudo apt install ./output/deb/mphone_<version>_amd64.deb
mphone
```

## Windows

Requirements:

- Visual Studio 2022 with MSVC x64 toolchain
- Qt 6.5.x `msvc2019_64`

Environment:

```bat
set ENV_QT_PATH=C:\Qt\6.5.3
set ENV_VCVARSALL=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat
set ENV_VCINSTALL=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC
```

Build and package:

```bat
ci\win\build_for_win.bat RelWithDebInfo x64
ci\win\publish_for_win.bat x64 ..\build\MPhone-windows-x64
```

Output:

- `output/x64/RelWithDebInfo/MPhone.exe`
- `ci/build/MPhone-windows-x64/`

## CI

The unified GitHub Actions workflow is:

- `.github/workflows/build.yml`

It builds:

- `MPhone-macos-arm64.dmg`
- Linux `.deb`
- `MPhone-windows-x64.zip`

