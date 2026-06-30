# MPhone Android Agent

MPhone Agent is a tiny Android foreground service for phone boxes. It has no normal UI.

What it can do on a regular Android build:
- keep a partial wake lock while the service is alive
- restart after boot/package update
- run as a foreground service so Android is less likely to kill it
- expose a visible heartbeat in `logcat` as `MPhoneAgent`
- request battery optimization whitelist where the ROM allows it
- pulse the screen awake when `SCREEN_OFF` is observed
- apply an ADB hardening profile from MPhone: stay-on, device-idle disable, ADB persistence settings, and common power-button behavior settings

What needs a modded ROM, root, device-owner, or priv-app install:
- fully block hardware power-button shutdown on ROMs that do not expose power-key settings
- guarantee ADB never restarts or disconnects across USB controller resets
- silently grant every protected system permission
- restart itself after force-stop from Settings

Build:

```bash
./agent/android/build-agent.sh
```

Install/start manually:

```bash
adb -s SERIAL install -r -g agent/android/build/mphone-agent.apk
adb -s SERIAL shell am start-foreground-service -n com.mphone.agent/.MPhoneAgentService
adb -s SERIAL shell pidof com.mphone.agent
```

In MPhone, right-click a device and use:
- `Install keepalive agent`
- `Apply phonebox hardening`
- `Agent status`

For production phone boxes with your modded ROM, keep the ADB hardening profile as the default install flow. If a specific ROM ignores `power_button_long_press` / `long_press_power_button_behavior`, add its vendor-specific settings key to `applyMPhoneAgentHardening()`.
