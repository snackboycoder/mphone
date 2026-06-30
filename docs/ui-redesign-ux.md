# MPhone UI Redesign UX Architecture

## Product Direction

MPhone should feel like a production device-operations console for controlling large Android fleets. The new UI should replace the inherited legacy form layout with a purpose-built shell for scanning hundreds of devices, selecting and grouping targets, issuing synchronized controls, running batch scripts, and inspecting individual streams.

The backend behavior should stay intact where possible: keep `qsc::IDeviceManage`, existing ADB command handlers, `VideoForm` rendering/input signals, batch script execution functions, audio/sndcpy actions, boot config persistence, and existing `Config` values. The redesign is primarily a new layout, new widget ownership, and clearer routing of existing controls.

Visual tone: enterprise, dense, calm, device-ops. Use restrained neutrals, clear borders, compact spacing, status color accents, and icon-library icons. Avoid neon, cyber styling, AI-looking gradients, oversized marketing areas, and decorative backgrounds.

## Information Architecture

### Main Shell

Implement a new top-level shell inside `Dialog`, preferably by replacing `initMPhoneView()` with a clean composition rather than reusing the old `.ui` visual hierarchy.

Primary component names:

- `FleetShellWidget`: root widget installed into `Dialog`.
- `AppTopBar`: fixed-height top navigation and global command strip.
- `DevicePoolPanel`: left dock/panel for device inventory, grouping, and logs.
- `FleetWorkspace`: center work area containing toolbar, grid, and expanded views.
- `OperationsPanel`: right dock/panel for settings, scripts, and selected-device details.
- `FleetStatusBar`: bottom status strip for ADB/server/task feedback.

Recommended layout:

```text
FleetShellWidget
  AppTopBar
  MainSplitter
    DevicePoolPanel
    FleetWorkspace
    OperationsPanel
  FleetStatusBar
```

Use `QSplitter` for the three main columns so operators can resize panels. Persist splitter sizes in `Config` if convenient.

### AppTopBar

Purpose: global orientation, fleet-level actions, mode switching.

Contents:

- Left: MPhone title, connected count, running stream count, error/offline count.
- Center: workspace tabs or segmented navigation:
  - `Monitor`: device grid and live control.
  - `Scripts`: batch script workspace.
  - `Logs`: diagnostic log focus.
- Right: global actions:
  - Refresh devices.
  - Start selected / start visible / start all.
  - Stop selected / stop all.
  - Toggle sync control.
  - Open settings panel.

Keep text short. Use icon buttons with tooltips for recurring actions. Use no emoji icons; use the existing FontAwesome helper or another Qt icon library if added later.

### DevicePoolPanel

Purpose: inventory and filtering, not stream rendering.

Sections:

- `DeviceSearchBar`: search by nickname, serial, IP, status, group.
- `DeviceFilterBar`: compact filters for All, Online, Running, Selected, Offline, Error, USB, Wi-Fi.
- `DeviceGroupTree`: optional grouping by user-defined group, connection type, or status.
- `DeviceListView`: dense virtual/list-style rows for all discovered devices.
- `DiagnosticsMiniLog`: collapsible tail of ADB/device events.

Device row fields:

- Selection checkbox.
- Status dot and state text.
- Nickname.
- Serial/IP, smaller muted text.
- Badges for USB/Wi-Fi, running, recording, sync target.
- Small action menu button.

Current migration targets:

- Move `connectedPhoneList` behavior into `DeviceListView`.
- Move `autoUpdatecheckBox`, `updateDevice`, `usbConnectBtn`, `wifiConnectBtn` into panel header actions or top bar actions.
- Move `outEdit`, `adbCommandEdt`, `adbCommandBtn`, `stopAdbBtn`, `clearOut` into `DiagnosticsMiniLog` and the full `Logs` workspace.
- Keep `on_connectedPhoneList_itemDoubleClicked()` behavior as row double-click: start/open that device stream.

### FleetWorkspace

Purpose: the main live control surface.

Subcomponents:

- `WorkspaceCommandBar`: selection count, grid density, sort, group, sync mode, visible-device actions.
- `DeviceGridScrollArea`: scrollable center area.
- `DeviceGridView`: adaptive grid of device cards.
- `EmptyFleetState`: compact empty/loading/error states.
- `ExpandedDeviceOverlay` or `ExpandedDevicePane`: focused single-device mode.

Device grid cards should be designed as operational tiles, not decorative cards. Use fixed internal zones so controls do not cause layout shifts.

`DeviceTile` structure:

- Header row:
  - Selection checkbox.
  - Nickname/serial.
  - Status badges.
  - More menu.
- Stream viewport:
  - Embedded `VideoForm`.
  - Loading/offline/error overlay when no video is available.
- Action row:
  - Back, Home, Recents.
  - Power.
  - Screenshot.
  - Restart stream.
  - Expand.

Tile states:

- `idle`: device discovered but stream not running.
- `starting`: server start queued/running.
- `running`: live stream attached.
- `selected`: visible selected outline and checkbox.
- `sync-source`: selected tile receiving local input.
- `sync-target`: selected tile mirroring input.
- `error/offline`: muted viewport with error summary.

Current migration targets:

- Preserve `VideoForm` as the viewport component.
- Preserve `mirrorMouseInput`, `mirrorWheelInput`, and `mirrorKeyInput` connections.
- Replace `makeVideoCard()` with `DeviceTile`.
- Preserve `runDeviceActionOnTargets()` and `runDeviceAction()` behind tile actions.
- Preserve `showExpandedDevice()` but move its presentation into the new expanded pane/overlay.

### OperationsPanel

Purpose: contextual actions and configuration without crowding the grid.

Use tabs inside the right panel:

- `Selection`: details and actions for selected devices.
- `Stream`: stream defaults and recording options.
- `Scripts`: saved batch script editor and run controls.
- `Wireless`: IP/port connect, ADBD, disconnect.
- `Audio`: sndcpy install/start/stop.

`Selection` tab:

- Selected count.
- Group assignment.
- Rename when exactly one device is selected.
- Start/stop/restart/record/screenshot actions.
- Per-device summary list when multiple selected.

`Stream` tab:

- Bitrate value and unit.
- Max size.
- Format.
- Orientation.
- Record save path.
- Toggles: record screen, background/no display, reverse tunnel, FPS, always on top, close screen, frameless, stay awake, show toolbar.

`Scripts` tab:

- Saved script selector.
- New/save/delete/run buttons.
- Script editor.
- Target scope selector: selected devices, visible devices, group, all online.
- Command reference collapsed behind a help/info affordance, not persistent instructional text dominating the panel.

Current migration targets:

- Move `configGroupBox` into `StreamSettingsPanel`.
- Move `usbGroupBox` into `SelectionPanel` and `AudioPanel`.
- Move `wirelessGroupBox` into `WirelessPanel`.
- Move `m_batchScriptGroup`, `m_batchScriptSelector`, `m_batchScriptEditor`, and related buttons into `BatchScriptPanel`.
- Reuse existing slots such as `on_startServerBtn_clicked()`, `on_stopServerBtn_clicked()`, `on_stopAllServerBtn_clicked()`, `on_getIPBtn_clicked()`, `on_startAdbdBtn_clicked()`, `on_wirelessConnectBtn_clicked()`, `on_wirelessDisConnectBtn_clicked()`, `on_startAudioBtn_clicked()`, `on_stopAudioBtn_clicked()`, and `on_installSndcpyBtn_clicked()`.

### FleetStatusBar

Purpose: persistent operational feedback.

Contents:

- ADB status.
- Last command/result.
- Queue activity such as `Starting 4/20`.
- Selection count.
- Sync mode state.
- Log drawer toggle.

Keep it one line, compact, and non-modal. Errors should be clickable or open the Logs workspace.

## Interaction Model

### Device Selection

Selection is a first-class fleet concept.

Required interactions:

- Click checkbox on a device row/tile to toggle selection.
- Click tile body to focus it without changing selection.
- Shift-click selects range in the current sorted/filtered list.
- Ctrl/Cmd-click toggles individual selection.
- Toolbar actions: select all visible, clear, invert.
- Selection persists when switching tabs or filtering, but hidden selected count must remain visible.

Behavior:

- Actions default to selected devices.
- If no devices are selected, global actions should explicitly target visible devices or all devices depending on button label.
- Device detail panel shows single-device controls when one device is focused and batch controls when multiple are selected.

### Sync Control

Sync mode mirrors input from one active source device to selected target devices.

Required states:

- `Sync off`: no mirroring.
- `Sync armed`: selected devices are targets, but no source is active.
- `Sync active`: input from focused running tile is being mirrored.

Rules:

- Sync requires at least two selected running devices.
- The source device must be selected.
- Do not mirror input back to the source.
- Show sync state in `AppTopBar`, `WorkspaceCommandBar`, and `FleetStatusBar`.
- If a selected target disconnects, remove it from active targets and log a concise event.

Reuse existing `m_syncControlEnabled`, `m_selectedDevices`, and mirror input methods, but expose clearer state labels.

### Per-Device Action Menu

Each `DeviceTile` and `DeviceListRow` should expose a compact menu.

Actions:

- Start stream.
- Stop stream.
- Restart stream.
- Expand.
- Rename.
- Assign group.
- Back, Home, Recents.
- Power.
- Volume up/down.
- Wake/sleep screen.
- Screenshot.
- Audio start/stop if available.
- Remove from selection.

Use icon buttons for common actions and a menu for the rest. Keep destructive or disruptive actions visually distinct but not theatrical.

### Expanded Device View

The expanded view is an operational focus mode, not a separate old-style floating phone window by default.

Recommended presentation:

- `ExpandedDevicePane` replaces the grid area or opens as an overlay within `FleetWorkspace`.
- Left/center: large `VideoForm`.
- Right rail: device metadata, primary controls, logs for that device, script run-on-this-device.
- Top row: back to grid, device name/serial, status, sync indicators, pin/float option.

Support an optional "float window" action if the existing `VideoForm` standalone mode is still useful, but the default should keep the operator in the fleet console.

### Search, Filter, Sort, Group

Search/filter must be fast enough for hundreds of devices.

Required controls:

- Search field with debounce.
- Status filters.
- Connection filters USB/Wi-Fi.
- Group filter.
- Sort by name, serial, status, last activity, connection type.
- Grid density selector: compact, standard, large.

Implementation guidance:

- Maintain a lightweight device model separate from live `VideoForm` widgets.
- Only create/render visible or running stream widgets where possible.
- Avoid rebuilding the whole grid on every minor state change.

### Settings Access

Settings should not be a permanent wall of form fields.

Rules:

- Right panel can collapse to an icon rail.
- `Stream` tab holds default start settings.
- Per-device overrides appear in `Selection` only when needed.
- Global app preferences can be a modal dialog later, but current backend settings can remain in the panel during migration.

## Responsive Behavior

### Large Desktop

Width 1440px and above:

- Left panel visible: 260-320px.
- Right panel visible: 340-420px.
- Grid uses 4-8 columns depending on density and viewport.
- Top bar remains single row.

### Standard Desktop

Width 1100-1439px:

- Left panel visible or collapsible.
- Right panel collapses by default when grid density is compact.
- Grid uses 3-5 columns.
- Workspace command bar may wrap only after preserving primary actions.

### Small Desktop

Width below 1100px:

- Left panel collapses to a drawer.
- Right panel collapses to a drawer or tabbed bottom sheet.
- Top bar keeps title, refresh, sync, and panel toggles; secondary actions move into menus.
- Grid uses 2-3 columns.

### Hundreds of Devices

Requirements:

- Grid scroll must remain smooth.
- Device inventory list must not require live stream widgets for every device.
- Provide density control and grouping to reduce visual overload.
- Start-all should use an explicit queue with concurrency limit, continuing existing `m_startQueue`, `m_startAttempts`, `m_activeStarts`, and `m_maxConcurrentStarts`.
- Show queue status in `FleetStatusBar`.

Recommended implementation path:

- Phase 1 can keep `QScrollArea + QGridLayout` if scoped carefully.
- Phase 2 should move inventory/grid metadata to a model/view structure (`QAbstractListModel` plus custom delegates or a virtualized view) if large fleets expose performance limits.

## Migration Plan

### Keep

- `Dialog` as the main window entry point for now.
- `VideoForm` rendering and input forwarding.
- `ToolForm` functionality, but do not show it as the primary embedded control surface; fold its actions into `DeviceTile` and expanded view.
- `qsc::IDeviceManage` signals and device lifecycle calls.
- Existing slots for ADB, wireless, stream start/stop, audio, script, record path, and config persistence.
- Existing batch script parser/executor functions.

### Hide Or Reparent

- Hide the old `leftWidget` and `rightWidget` only as a temporary compatibility measure.
- Reparent individual existing controls into new panels during Phase 1 to avoid breaking signals.
- After migration, stop relying on `dialog.ui` for visible layout. It may remain as a hidden control container only briefly.

Specific reparent targets:

- `connectedPhoneList` -> replace with `DeviceListView`; retain double-click behavior.
- `serialBox` -> keep as hidden/current selection backing during Phase 1; later replace with selected-device model access.
- `outEdit` -> `DiagnosticsLogView`.
- `adbCommandEdt` and buttons -> `LogsWorkspaceCommandBar`.
- `configGroupBox` controls -> `StreamSettingsPanel`.
- `usbGroupBox` controls -> split across `SelectionPanel` and `AudioPanel`.
- `wirelessGroupBox` controls -> `WirelessPanel`.
- `m_batchScriptGroup` controls -> `BatchScriptPanel`.
- `m_mphoneScroll`, `m_mphoneGrid`, `m_mphoneCards` -> replace with `DeviceGridScrollArea`, `DeviceGridView`, and `DeviceTileRegistry`.

### Replace

- Replace the old horizontal form composition with `FleetShellWidget`.
- Replace text-heavy push buttons in dense toolbars with icon-library buttons and tooltips.
- Replace fixed-width side columns with splitter-managed panels.
- Replace one-off card stylesheet mutations with state-driven properties and central stylesheet rules.
- Replace "simple mode" concept with panel collapse/density controls.

## Implementation-Ready Component List

Create or refactor toward these classes/object names:

- `FleetShellWidget`
- `AppTopBar`
- `MainFleetSplitter`
- `DevicePoolPanel`
- `DeviceSearchBar`
- `DeviceFilterBar`
- `DeviceGroupTree`
- `DeviceListView`
- `DiagnosticsLogView`
- `FleetWorkspace`
- `WorkspaceCommandBar`
- `DeviceGridScrollArea`
- `DeviceGridView`
- `DeviceTile`
- `DeviceTileHeader`
- `DeviceTileViewport`
- `DeviceTileActionBar`
- `ExpandedDevicePane`
- `OperationsPanel`
- `SelectionPanel`
- `StreamSettingsPanel`
- `BatchScriptPanel`
- `WirelessPanel`
- `AudioPanel`
- `FleetStatusBar`
- `DeviceUiState` lightweight struct for serial, nickname, connection type, status, selected, running, group, error text.
- `DeviceTileRegistry` map for serial-to-widget lifecycle during Phase 1.

## Priority List

1. Build `FleetShellWidget` with top bar, splitter, workspace, operations panel, and status bar.
2. Move existing controls into new panels without changing backend slot behavior.
3. Replace `makeVideoCard()` with `DeviceTile` and centralize tile states.
4. Implement search/filter/selection model for device rows and grid tiles.
5. Rework sync mode labels and source/target visual states while keeping existing mirror methods.
6. Move batch scripts into the `Scripts` workspace/tab with target scope control.
7. Add expanded device pane as the default focus mode.
8. Add persisted panel sizes, collapsed states, and grid density.
9. Optimize for hundreds of devices by separating device metadata from live stream widgets.
10. Remove dependency on visible legacy `.ui` layout once the new shell covers all workflows.

## Acceptance Criteria

- The first screen is the fleet operations console, not a legacy settings form.
- Operators can discover, filter, select, start, stop, sync, script, and inspect devices without opening separate legacy panels.
- Hundreds of devices remain navigable through search, filters, grouping, density, and scroll behavior.
- Existing backend actions continue to work through the new controls.
- The right panel is contextual and collapsible.
- The left panel is inventory/log focused, not a dumping ground for controls.
- The visual system is compact, professional, and stable under resize.
