# MPhone Edition Plan

This fork keeps MPhone as the core mirroring and Android control engine, then adds an operations dashboard for managing large Android phone clusters.

## Base

- Upstream project: `mphone/MPhone`
- License: Apache-2.0
- Core dependency: `MPhone/MPhoneCore` submodule
- Target platform: Ubuntu first, while keeping upstream cross-platform structure intact where practical

## Why Build On MPhone

MPhone already provides the hard parts:

- Low-latency Android screen mirroring
- Mouse and keyboard control
- Multi-device service management
- Group control
- APK install and file transfer path
- ADB command execution
- Qt UI and OpenGL rendering

The MPhone work should extend device operations, not rewrite streaming/control.

## MPhone Features To Add

- Full device inventory table with serial, state, model, Android version, battery, IP, group, note, and last seen.
- Device groups for mphone, USB hub, campaign, or account pool.
- Batch actions: start mirror, stop mirror, reboot, wake, sleep, install APK, launch package, force-stop package, clear app data.
- Health checks: offline detection, low battery, unauthorized state, missing package, high temperature where available.
- Persistent device metadata in a local config file.
- Search and filters for online/offline/group/model/package status.
- Optional compact multi-screen layout for many devices.
- Safer ADB command shortcuts with saved presets.

## Suggested Implementation Path

1. Add a `mphone` module with a device table model and metadata store.
2. Reuse `qsc::AdbProcess` for inventory and batch shell commands.
3. Reuse `qsc::IDeviceManage` for start/stop service and active-device state.
4. Add a MPhone tab/panel to `Dialog` instead of replacing existing controls.
5. Keep MPhone upstream mergeable by isolating new code under `MPhone/mphone`.

## Ubuntu Build Notes

Install Qt, FFmpeg, OpenGL, ADB, and private Qt headers:

```bash
sudo apt update
sudo apt install -y build-essential cmake git \
  qt6-base-dev qt6-base-private-dev qt6-multimedia-dev \
  libavformat-dev libavcodec-dev libavutil-dev libswscale-dev \
  libgl1-mesa-dev android-tools-adb
```

Clone with submodules:

```bash
git clone --recursive <repo-url>
```

Build:

```bash
cmake -S . -B build -DQT_DESIRED_VERSION=6
cmake --build build
```

## Ubuntu DEB Package

On an Ubuntu x86_64 build machine:

```bash
sudo apt update
sudo apt install -y build-essential cmake git dpkg-dev \
  qt6-base-dev qt6-base-private-dev qt6-multimedia-dev qt6-tools-dev-tools \
  libavformat-dev libavcodec-dev libavutil-dev libswscale-dev \
  libgl1-mesa-dev libx11-dev libxcb1-dev libxkbcommon-x11-0 libdbus-1-3

git submodule update --init --recursive
./ci/linux/package_deb.sh Release
```

The package will be created under:

```bash
output/deb/mphone_<version>_amd64.deb
```

Install on the target Ubuntu machine:

```bash
sudo apt install ./mphone_<version>_amd64.deb
mphone
```
