#!/usr/bin/env bash
set -euo pipefail

build_mode="${1:-Release}"
package_name="${PACKAGE_NAME:-mphone}"
app_name="MPhone"
display_name="MPhone"

if [[ "$build_mode" != "Release" && "$build_mode" != "Debug" && "$build_mode" != "MinSizeRel" && "$build_mode" != "RelWithDebInfo" ]]; then
    echo "error: unknown build mode: $build_mode"
    exit 1
fi

script_dir="$(cd "$(dirname "$0")" && pwd)"
project_root="$(cd "$script_dir/../.." && pwd)"
arch="$(dpkg --print-architecture)"
version="$(cat "$project_root/MPhone/appversion" 2>/dev/null || echo "0.1.0")"
version="${PACKAGE_VERSION:-$version}"

build_dir="$project_root/build-linux-deb"
output_dir="$project_root/output/x64/$build_mode"
package_root="$project_root/output/deb/${package_name}_${version}_${arch}"
deb_out_dir="$project_root/output/deb"

echo "Building $display_name $version for $arch ($build_mode)"

agent_apk="$project_root/agent/android/build/mphone-agent.apk"
if [[ -x "$project_root/agent/android/build-agent.sh" ]]; then
    if "$project_root/agent/android/build-agent.sh"; then
        echo "Built MPhone Agent APK: $agent_apk"
    else
        echo "warning: MPhone Agent APK build failed"
    fi
fi
if [[ ! -f "$agent_apk" ]]; then
    echo "error: missing $agent_apk"
    echo "Install Android SDK build-tools/platforms or copy a prebuilt mphone-agent.apk to that path."
    exit 1
fi

cmake -S "$project_root" -B "$build_dir" \
    -DCMAKE_BUILD_TYPE="$build_mode" \
    -DQT_DESIRED_VERSION="${QT_DESIRED_VERSION:-6}"
cmake --build "$build_dir" -j"$(nproc)"

binary="$output_dir/$app_name"
if [[ ! -x "$binary" ]]; then
    echo "error: executable not found: $binary"
    exit 1
fi

rm -rf "$package_root"
mkdir -p \
    "$package_root/DEBIAN" \
    "$package_root/opt/mphone/bin" \
    "$package_root/opt/mphone/lib" \
    "$package_root/opt/mphone/plugins" \
    "$package_root/opt/mphone/config" \
    "$package_root/usr/bin" \
    "$package_root/usr/share/applications" \
    "$package_root/usr/share/icons/hicolor/256x256/apps"

cp "$binary" "$package_root/opt/mphone/bin/$app_name"
chmod +x "$package_root/opt/mphone/bin/$app_name"

for file in adb scrcpy-server sndcpy.sh sndcpy.apk; do
    if [[ -f "$output_dir/$file" ]]; then
        cp "$output_dir/$file" "$package_root/opt/mphone/bin/$file"
    fi
done
if [[ -f "$output_dir/mphone-agent.apk" ]]; then
    cp "$output_dir/mphone-agent.apk" "$package_root/opt/mphone/bin/mphone-agent.apk"
elif [[ -f "$agent_apk" ]]; then
    cp "$agent_apk" "$package_root/opt/mphone/bin/mphone-agent.apk"
fi
chmod +x "$package_root/opt/mphone/bin/adb" "$package_root/opt/mphone/bin/sndcpy.sh" 2>/dev/null || true

if [[ -d "$project_root/config" ]]; then
    cp -R "$project_root/config/." "$package_root/opt/mphone/config/"
fi
cat > "$package_root/usr/bin/mphone" << 'EOF'
#!/usr/bin/env bash
set -e
APP_HOME="/opt/mphone"
export LD_LIBRARY_PATH="$APP_HOME/lib:${LD_LIBRARY_PATH:-}"
export QT_PLUGIN_PATH="$APP_HOME/plugins:${QT_PLUGIN_PATH:-}"
export QT_QPA_PLATFORM_PLUGIN_PATH="$APP_HOME/plugins/platforms"
export MPHONE_ADB_PATH="$APP_HOME/bin/adb"
export MPHONE_SERVER_PATH="$APP_HOME/bin/scrcpy-server"
export MPHONE_AGENT_APK="$APP_HOME/bin/mphone-agent.apk"
export MPHONE_CONFIG_PATH="$APP_HOME/config"
exec "$APP_HOME/bin/MPhone" "$@"
EOF
chmod +x "$package_root/usr/bin/mphone"

icon_source=""
if [[ -f "$project_root/MPhone/res/image/tray/logo.png" ]]; then
    icon_source="$project_root/MPhone/res/image/tray/logo.png"
elif [[ -f "$project_root/backup/logo.png" ]]; then
    icon_source="$project_root/backup/logo.png"
fi
if [[ -n "$icon_source" ]]; then
    cp "$icon_source" "$package_root/usr/share/icons/hicolor/256x256/apps/mphone.png"
fi

cat > "$package_root/usr/share/applications/mphone.desktop" << EOF
[Desktop Entry]
Type=Application
Name=MPhone
Comment=Manage and control Android device clusters
Exec=mphone
Icon=mphone
Categories=Utility;Development;
Terminal=false
StartupNotify=true
EOF

copy_ldd_libs() {
    local target="$1"
    ldd "$target" | sed -n \
        -e 's/.*=> \([^ ]*\) .*/\1/p' \
        -e 's/^[[:space:]]*\(\/[^ ]*\) .*/\1/p' \
        | while read -r lib; do
        [[ -f "$lib" ]] || continue
        case "$lib" in
            /lib/x86_64-linux-gnu/libc.so.*|\
            /lib/x86_64-linux-gnu/libpthread.so.*|\
            /lib/x86_64-linux-gnu/libdl.so.*|\
            /lib/x86_64-linux-gnu/librt.so.*|\
            /lib/x86_64-linux-gnu/libm.so.*|\
            /lib/x86_64-linux-gnu/ld-linux-*.so.*)
                continue
                ;;
        esac
        cp -n "$lib" "$package_root/opt/mphone/lib/" 2>/dev/null || true
    done
}

copy_ldd_libs "$binary"
for so in "$output_dir"/*.so*; do
    [[ -f "$so" ]] || continue
    cp -n "$so" "$package_root/opt/mphone/lib/" 2>/dev/null || true
    copy_ldd_libs "$so" || true
done

qt_plugin_source=""
if command -v qtpaths6 >/dev/null 2>&1; then
    qt_plugin_source="$(qtpaths6 --plugin-dir 2>/dev/null || true)"
elif command -v qtpaths >/dev/null 2>&1; then
    qt_plugin_source="$(qtpaths --plugin-dir 2>/dev/null || true)"
fi

if [[ -z "$qt_plugin_source" || ! -d "$qt_plugin_source" ]]; then
    for candidate in \
        /usr/lib/x86_64-linux-gnu/qt6/plugins \
        /usr/lib/x86_64-linux-gnu/qt5/plugins \
        /usr/lib/qt6/plugins \
        /usr/lib/qt5/plugins; do
        if [[ -d "$candidate" ]]; then
            qt_plugin_source="$candidate"
            break
        fi
    done
fi

if [[ -n "$qt_plugin_source" && -d "$qt_plugin_source" ]]; then
    for plugin_dir in platforms imageformats platformthemes xcbglintegrations iconengines tls; do
        if [[ -d "$qt_plugin_source/$plugin_dir" ]]; then
            mkdir -p "$package_root/opt/mphone/plugins/$plugin_dir"
            cp -R "$qt_plugin_source/$plugin_dir/." "$package_root/opt/mphone/plugins/$plugin_dir/"
            find "$package_root/opt/mphone/plugins/$plugin_dir" -type f -name "*.so" -print0 | while IFS= read -r -d '' plugin; do
                copy_ldd_libs "$plugin" || true
            done
        fi
    done
fi

installed_size="$(du -sk "$package_root" | awk '{print $1}')"
cat > "$package_root/DEBIAN/control" << EOF
Package: $package_name
Version: $version
Section: utils
Priority: optional
Architecture: $arch
Maintainer: MPhone <support@mphone.local>
Installed-Size: $installed_size
Depends: libgl1, libx11-6, libxcb1, libxkbcommon-x11-0, libdbus-1-3, libudev1
Description: MPhone Android device cluster manager
 MPhone displays, controls, and automates multiple Android devices.
EOF

cat > "$package_root/DEBIAN/postinst" << 'EOF'
#!/usr/bin/env bash
set -e
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database /usr/share/applications || true
fi
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -q /usr/share/icons/hicolor || true
fi
exit 0
EOF
chmod 755 "$package_root/DEBIAN/postinst"

mkdir -p "$deb_out_dir"
deb_path="$deb_out_dir/${package_name}_${version}_${arch}.deb"
dpkg-deb --build "$package_root" "$deb_path"

echo "Created: $deb_path"
echo "Install with: sudo apt install ./$(basename "$deb_path")"
