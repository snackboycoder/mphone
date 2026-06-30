#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
SDK_DIR="${ANDROID_HOME:-${ANDROID_SDK_ROOT:-}}"
if [[ -z "${SDK_DIR}" ]]; then
  for candidate in \
    "${HOME}/Android/Sdk" \
    "${HOME}/Library/Android/sdk" \
    "/opt/android-sdk" \
    "/usr/lib/android-sdk"; do
    if [[ -d "${candidate}" ]]; then
      SDK_DIR="${candidate}"
      break
    fi
  done
fi
if [[ -z "${SDK_DIR}" || ! -d "${SDK_DIR}" ]]; then
  echo "error: Android SDK not found. Set ANDROID_HOME or ANDROID_SDK_ROOT." >&2
  exit 1
fi

BUILD_TOOLS="${ANDROID_BUILD_TOOLS:-}"
if [[ -z "${BUILD_TOOLS}" ]]; then
  BUILD_TOOLS="$(find "${SDK_DIR}/build-tools" -mindepth 1 -maxdepth 1 -type d 2>/dev/null | sort -V | tail -1)"
fi
PLATFORM="${ANDROID_PLATFORM:-}"
if [[ -z "${PLATFORM}" ]]; then
  PLATFORM="$(find "${SDK_DIR}/platforms" -mindepth 2 -maxdepth 2 -name android.jar 2>/dev/null | sort -V | tail -1)"
fi
if [[ -z "${BUILD_TOOLS}" || ! -x "${BUILD_TOOLS}/aapt2" || ! -x "${BUILD_TOOLS}/d8" ]]; then
  echo "error: Android build-tools with aapt2/d8 not found under ${SDK_DIR}/build-tools." >&2
  exit 1
fi
if [[ -z "${PLATFORM}" || ! -f "${PLATFORM}" ]]; then
  echo "error: Android platform android.jar not found under ${SDK_DIR}/platforms." >&2
  exit 1
fi
OUT_DIR="${ROOT_DIR}/build"
KEYSTORE="${ROOT_DIR}/mphone-agent-debug.keystore"
APK_UNSIGNED="${OUT_DIR}/mphone-agent-unsigned.apk"
APK_ALIGNED="${OUT_DIR}/mphone-agent-aligned.apk"
APK_FINAL="${OUT_DIR}/mphone-agent.apk"

rm -rf "${OUT_DIR}"
mkdir -p "${OUT_DIR}/classes" "${OUT_DIR}/dex"

"${BUILD_TOOLS}/aapt2" link \
  -o "${APK_UNSIGNED}" \
  -I "${PLATFORM}" \
  --manifest "${ROOT_DIR}/AndroidManifest.xml" \
  --min-sdk-version 23 \
  --target-sdk-version 28 \
  --version-code 1 \
  --version-name 0.1.0

javac -source 8 -target 8 \
  -bootclasspath "${PLATFORM}" \
  -d "${OUT_DIR}/classes" \
  $(find "${ROOT_DIR}/src" -name '*.java')

"${BUILD_TOOLS}/d8" \
  --min-api 23 \
  --lib "${PLATFORM}" \
  --output "${OUT_DIR}/dex" \
  $(find "${OUT_DIR}/classes" -name '*.class')

(cd "${OUT_DIR}/dex" && zip -q -u "${APK_UNSIGNED}" classes.dex)

if [[ ! -f "${KEYSTORE}" ]]; then
  keytool -genkeypair -v \
    -keystore "${KEYSTORE}" \
    -storepass mphone-agent \
    -alias mphone-agent \
    -keypass mphone-agent \
    -keyalg RSA \
    -keysize 2048 \
    -validity 10000 \
    -dname "CN=MPhone Agent,O=MPhone,C=VN" >/dev/null
fi

"${BUILD_TOOLS}/zipalign" -f 4 "${APK_UNSIGNED}" "${APK_ALIGNED}"
"${BUILD_TOOLS}/apksigner" sign \
  --ks "${KEYSTORE}" \
  --ks-pass pass:mphone-agent \
  --key-pass pass:mphone-agent \
  --out "${APK_FINAL}" \
  "${APK_ALIGNED}"

"${BUILD_TOOLS}/apksigner" verify "${APK_FINAL}"
echo "${APK_FINAL}"
