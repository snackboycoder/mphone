# MPhone UI Redesign: Visual System and Qt Widgets Plan

This is an implementation-ready plan for replacing the inherited legacy mirroring layout with a production device-operations console. The goal is not to tune the old form. Build a new app shell programmatically, keep the working device/control behavior, and reuse existing widgets only as functional controls where that is cheaper and safer than rewriting slots.

## Product Direction

MPhone should feel like a dense enterprise console for Android fleet operations: calm, precise, readable under pressure, and able to scale from 1 phone to hundreds. The UI should favor operational clarity over decoration.

Use these principles:

- One clear primary workspace: the device canvas.
- Controls are grouped by workflow, not by historical source panels.
- Drawers and panels are compact, scrollable, and never allowed to force the center grid to clip.
- Device cards are consistent, compact, and stateful.
- No gradients, no decorative blobs, no marketing hero layout, no emoji.
- Icons are small supporting cues, never the main visual language.

## Visual Language

### Palette

Use a neutral dark enterprise palette with restrained teal/blue accents. Avoid a one-note blue-purple or neon gaming look.

| Role | Token | Hex | Usage |
| --- | --- | --- | --- |
| App background | `--bg-app` | `#0D1117` | Root window, empty page bands |
| Surface | `--bg-surface` | `#131A22` | Command bar, drawers, cards |
| Surface raised | `--bg-raised` | `#18212B` | Device cards, focused panels |
| Field background | `--bg-field` | `#0F151C` | Inputs, editors, lists |
| Border subtle | `--border-subtle` | `#26313D` | Panel borders |
| Border strong | `--border-strong` | `#354454` | Hover/focus borders |
| Text primary | `--text-primary` | `#EEF3F8` | Main labels, device names |
| Text secondary | `--text-secondary` | `#A7B2C0` | Metadata, field labels |
| Text muted | `--text-muted` | `#748293` | Empty states, hints |
| Accent | `--accent` | `#13A89E` | Primary actions, selected state |
| Accent hover | `--accent-hover` | `#18BFB4` | Primary hover |
| Info | `--info` | `#4C9FE8` | Connected/streaming indicators |
| Warning | `--warning` | `#E3B341` | Queued/retrying |
| Danger | `--danger` | `#D75F6A` | Stop/error/destructive |
| Success | `--success` | `#36B37E` | Ready/completed |

Qt style names should use object names and properties rather than many inline styles:

```cpp
card->setObjectName("DeviceCard");
card->setProperty("state", "streaming"); // idle, queued, starting, streaming, error, selected
```

Call `style()->unpolish(widget); style()->polish(widget);` after changing dynamic properties that affect QSS.

### Typography

Use the system UI stack. Do not depend on Inter being installed.

```css
font-family: "SF Pro Text", "Segoe UI", "Inter", "Noto Sans", sans-serif;
```

Recommended sizes:

- App title: 16 px, 700 weight.
- Command bar buttons: 12-13 px, 600 weight.
- Panel section title: 12 px, 700 weight, uppercase optional only for very short labels.
- Device serial/name: 12-13 px, 650 weight.
- Device metadata: 11 px.
- Logs and script console: 12 px monospace, `Menlo`, `Consolas`, `SF Mono`.

Do not scale font sizes with viewport width. Keep letter spacing at `0`.

### Spacing And Density

Use a compact 4 px grid:

- Root shell margin: 0.
- Major regions gap: 8 px.
- Panel padding: 12 px.
- Card padding: 8-10 px.
- Form row gap: 8 px.
- Button height: 30-34 px.
- Icon button: 30 x 30 px or 32 x 32 px.
- Device card radius: 6 px.
- Panel radius: 6 px.

Avoid nested card visuals. A drawer can contain sections, but sections should be flat panels with one border, not cards inside cards.

### Component Style

Buttons:

- Primary: filled teal, used only for high-intent actions such as `Start`, `Run Script`, `Connect`.
- Secondary: neutral raised surface with border.
- Danger: dark red surface with red border/text.
- Icon-only buttons: fixed square, tooltip required.
- Text should not be used where a standard icon is enough: start, stop, refresh, close, expand, save, delete.

Inputs:

- 30-32 px height.
- No hard maximum widths except true compact fields such as port or bitrate unit.
- Use `QSizePolicy::Expanding, Fixed`.
- Placeholder text is muted, not explanatory prose.

Cards:

- Compact state border and a small status pill.
- No giant title blocks.
- Use a stable phone preview area with a fixed aspect ratio.
- Selected cards use accent border plus subtle selected background.
- Error cards use danger border and a short diagnostic line.

Logs and consoles:

- Monospace.
- Keep them visually separate from form controls.
- Provide clear/run/stop icon controls near the editor, not hidden at the bottom.

Icons:

- Existing FontAwesome is acceptable through `:/font/fontawesome-webfont.ttf`.
- Prefer a small helper API that maps semantic names to glyphs: `Icon::Start`, `Icon::Stop`, `Icon::Refresh`, `Icon::Settings`, `Icon::Devices`, `Icon::Search`, `Icon::Terminal`, `Icon::Script`, `Icon::Expand`, `Icon::Close`, `Icon::Check`.
- No emoji icons.
- Icons should be 14-16 px in buttons and 10-12 px in status pills.

## New Layout Skeleton

Replace the old root form with a programmatic shell. The shell should have five stable regions:

```text
┌──────────────────────────────────────────────────────────────────────────────┐
│ Top App Bar: logo/name, fleet counters, global refresh, settings toggle      │
├───────────────┬─────────────────────────────────────────────┬────────────────┤
│ Device Pool   │ Command Bar                                  │ Inspector      │
│ / Log Panel   ├─────────────────────────────────────────────┤ / Settings     │
│               │ Device Canvas                                │ Drawer         │
│               │ scrollable responsive grid                   │                │
│               │ hundreds of cards                            │                │
├───────────────┴─────────────────────────────────────────────┴────────────────┤
│ Script Console / Diagnostics bottom panel, collapsible                       │
└──────────────────────────────────────────────────────────────────────────────┘
```

### 1. App Shell

Root widget:

- `QVBoxLayout` on `Dialog`, margins `0`, spacing `0`.
- `TopBar` fixed height `48`.
- `QSplitter` or central `QHBoxLayout` for left pool, center deck, right drawer.
- Bottom script/log panel either inside a vertical splitter with the main area or as a collapsible fixed-height panel.

Recommended class:

```cpp
class MPhoneShell : public QWidget
```

But to keep the current code smaller, it is acceptable to build this inside:

```cpp
void Dialog::buildMPhoneShell();
void Dialog::buildTopBar();
void Dialog::buildDevicePoolPanel();
void Dialog::buildCommandBar();
void Dialog::buildDeviceCanvas();
void Dialog::buildInspectorDrawer();
void Dialog::buildBottomConsole();
```

### 2. Top App Bar

Purpose: global identity and fleet health.

Content:

- Left: `MPhone` title, small environment/status label such as `ADB ready` or `ADB missing`.
- Center/right: counters `Online`, `Streaming`, `Selected`, `Queued`.
- Actions: refresh devices, start all selected/visible, stop all, toggle inspector, toggle console.

Implementation:

- `QFrame#TopBar`.
- `QHBoxLayout`, margins `14, 0, 14, 0`, spacing `8`.
- Counters as compact `QLabel` chips with object name `MetricChip`.
- Never place large forms in the top bar.

### 3. Command Bar

Purpose: high-frequency operations for the device canvas.

Content:

- Selection group: all, none, invert.
- Stream group: start selected, stop selected, restart failed.
- Sync group: sync input toggle, target mode selector.
- View group: density segmented control, search/filter, sort.

Implementation:

- `QFrame#CommandBar`, fixed or minimum height `44`.
- Use `QToolButton` for icon actions and `QPushButton` for primary text actions.
- Use small separators as 1 px `QFrame::VLine`.
- `m_fleetStatusLabel` can remain but should become a metric chip or short status text.

### 4. Device Canvas

Purpose: the main workspace.

Structure:

- `QScrollArea#DeviceCanvasScroll`, `setWidgetResizable(true)`.
- `QWidget#DeviceGridViewport` as the scroll widget.
- `QGridLayout` or custom `FlowLayout` on the viewport.
- Hundreds of devices must scroll vertically; horizontal scroll should stay off.

Recommended card sizing:

- Compact density: card min `184 x 300`, preview `160 x 250`.
- Comfortable density: card min `220 x 350`, preview `190 x 300`.
- Use aspect-ratio preservation for the video preview.
- Use column calculation based on viewport width minus margins, not window width.

Current `relayoutMPhoneScreens()` is close but should be revised:

- Clear old column stretches/minimums before applying new columns.
- Include contents margins in the column calculation.
- Use `qMax(1, (viewportWidth - leftMargin - rightMargin + spacing) / (cardWidth + spacing))`.
- Set all cards to a stable min/max size per density.
- Do not create a large `600 x 420` empty label because it can force narrow layouts to clip.

Empty state:

- A flat centered panel inside the canvas.
- Max width around `420`.
- No hard width larger than the viewport.

### 5. Inspector / Settings Drawer

Purpose: current device and global stream settings.

Width:

- Default `360`.
- Minimum `320`.
- Maximum `420`.
- In very narrow windows, collapse before it steals width from the canvas.

Sections:

- `Selected Device`: serial, alias, connection state, quick actions.
- `Stream`: bitrate, max size, format, orientation, record/background/reverse/fps.
- `Wireless`: IP, port, connect/drop.
- `Audio`: sndcpy install/start/stop.
- `Scripts`: saved script selector and run action, but the full editor belongs in the bottom console.

Implementation:

- Put drawer content inside `QScrollArea`.
- Use `QFormLayout` or two-column `QGridLayout` with fixed label column `88` and expanding field column.
- Avoid `QGroupBox` title-margin hacks if possible. Prefer `QFrame#PanelSection` plus a `QLabel#SectionTitle`; it is easier to style and less prone to clipping.
- Reuse old controls by reparenting them into new rows:
  - `ui->bitRateEdit`, `ui->bitRateBox`, `ui->maxSizeBox`, `ui->formatBox`, `ui->lockOrientationBox`
  - `ui->recordPathEdt`, `ui->selectRecordPathBtn`
  - stream option checkboxes
  - `ui->userNameEdt`, `ui->updateNameBtn`, `ui->serialBox`
  - USB/wireless/audio buttons and fields

### 6. Device Pool / Log Panel

Purpose: online device list and quick ADB diagnostics.

Width:

- Default `280`.
- Minimum `240`.
- Maximum `340`.
- Collapsible.

Content:

- Search/filter field.
- `connectedPhoneList` reused as the pool list.
- Refresh, Wi-Fi, USB connect controls.
- A compact diagnostics command row with `adbCommandEdt`, run, stop, clear.
- Log output below, if the existing output widget exists in `dialog.ui`.

This panel should not use old `leftWidget` directly as the visible app shell. Reparent its useful children into a new `QFrame#DevicePool`.

### 7. Script Console / Diagnostics Panel

Purpose: batch scripts, logs, and command diagnostics without crowding the inspector.

Placement:

- Bottom collapsible panel.
- Default height `220`.
- Resizable by vertical splitter if time allows.

Tabs:

- `Scripts`: selector, new/save/delete/run controls, `m_batchScriptEditor`.
- `Logs`: app/ADB output.
- `ADB`: command input and output if keeping logs separate is too much.

Implementation:

- `QTabWidget#BottomConsole` or custom tab bar.
- `m_batchScriptGroup` should be replaced by flatter widgets; do not bury an editor in the right drawer.
- Existing script functions can remain unchanged.

## Qt Implementation Strategy

### Recommended File Scope

For the first implementation pass, keep edits in:

- `MPhone/ui/dialog.h`
- `MPhone/ui/dialog.cpp`

Later cleanup can split helpers into:

- `MPhone/ui/mphoneuistyle.h/.cpp`
- `MPhone/ui/devicecard.h/.cpp`
- `MPhone/ui/flowlayout.h/.cpp`

### Keep `dialog.ui` As A Widget Factory Temporarily

The safest migration is:

1. Keep `ui->setupUi(this)` so existing object names and auto-connected slots still exist.
2. Immediately detach the old root layout and hide/delete only empty containers after useful widgets are reparented.
3. Build a new root layout programmatically on `Dialog`.
4. Reparent old controls into the new panels.
5. Keep existing slots and signal connections untouched where possible.

This avoids rewriting all `on_*` slots at once.

### Add These Functions

Replace the current partial `initMPhoneView()` with smaller builders:

```cpp
void Dialog::initMPhoneView();
void Dialog::buildMPhoneShell();
QWidget *Dialog::buildTopBar(QWidget *parent);
QWidget *Dialog::buildDevicePoolPanel(QWidget *parent);
QWidget *Dialog::buildCenterDeck(QWidget *parent);
QWidget *Dialog::buildCommandBar(QWidget *parent);
QScrollArea *Dialog::buildDeviceCanvas(QWidget *parent);
QWidget *Dialog::buildInspectorDrawer(QWidget *parent);
QWidget *Dialog::buildBottomConsole(QWidget *parent);
QFrame *Dialog::makeSection(const QString &title, QWidget *parent);
QToolButton *Dialog::makeIconButton(const QString &semanticIcon, const QString &tooltip, QWidget *parent);
QPushButton *Dialog::makeCommandButton(const QString &text, const QString &semanticIcon, QWidget *parent);
void Dialog::adoptWidget(QWidget *widget, QLayout *target, int stretch = 0);
void Dialog::refreshResponsiveLayout();
```

Keep or revise:

- `applyMPhoneStyle()`
- `relayoutMPhoneScreens()`
- `setMPhoneCard()`
- `clearMPhoneGrid()`
- `makeStatusCard()`
- `makeVideoCard()`
- selection and sync functions

### Build Order

Use this order to avoid stale layouts:

```cpp
void Dialog::initMPhoneView()
{
    setWindowTitle("MPhone");
    localizeMPhoneUi();
    prepareLegacyWidgetsForAdoption();
    buildMPhoneShell();
    initBatchScriptPanel(); // revised to place widgets in bottom console
    applyMPhoneStyle();
    refreshResponsiveLayout();
}
```

`prepareLegacyWidgetsForAdoption()` should set sane size policies and remove hard max widths from reusable controls before they are inserted.

### Reparenting Existing Widgets

When moving widgets out of `dialog.ui` layouts:

```cpp
static void detachWidget(QWidget *widget)
{
    if (!widget) {
        return;
    }
    if (QLayout *layout = widget->parentWidget() ? widget->parentWidget()->layout() : nullptr) {
        layout->removeWidget(widget);
    }
    widget->setParent(nullptr);
}
```

Then add the widget to the new layout. Once a widget is in a new layout, Qt will reparent it to the layout parent.

Do not call `delete` on adopted widgets. They are owned by `ui`/the dialog object tree and will be destroyed with the dialog.

### Styling Architecture

Move QSS out of card constructors as much as possible. Set object names and properties in C++ and style centrally in `applyMPhoneStyle()`.

Examples:

```cpp
card->setObjectName("DeviceCard");
card->setProperty("selected", false);
card->setProperty("state", "streaming");
```

```css
QFrame#DeviceCard {
    background: #18212B;
    border: 1px solid #26313D;
    border-radius: 6px;
}
QFrame#DeviceCard[selected="true"] {
    border-color: #13A89E;
    background: #17272B;
}
QLabel#StatusPill[state="error"] {
    color: #FFD7DC;
    background: #3A1F25;
    border: 1px solid #D75F6A;
}
```

### Device Card Structure

Use a dedicated card builder even if it stays inside `dialog.cpp`:

```text
DeviceCard
├─ Header row: select toggle, alias/serial, status pill, action buttons
├─ Preview frame: VideoForm or status placeholder
└─ Footer row: size/fps/connection metadata, optional latency
```

For `VideoForm`:

- Parent it to a stable preview container, not directly to the grid card.
- Give preview container `QSizePolicy::Expanding, Expanding`.
- Avoid fixed card heights that are smaller than header + preview + footer.
- If `VideoForm` has its own minimum size, reset only if safe: `videoForm->setMinimumSize(0, 0)`.

## Pitfalls In Current Code And How To Avoid Them

### 1. Old Root Layout Still Owns The App Shape

Current code inserts a new center widget into `ui->horizontalLayout_11` while still relying on `leftWidget` and `rightWidget` from `dialog.ui`. This keeps inherited constraints alive.

Avoidance:

- Build a new root layout on `Dialog`.
- Adopt useful child widgets from `leftWidget`/`rightWidget`.
- Hide old containers or leave them parentless but not visible.

### 2. `replaceWidgetLayout()` Can Detach Useful Widgets

`replaceWidgetLayout()` calls `clearLayoutItems()`, which sets widget parents to `nullptr`. This is safe only when every detached widget is immediately re-added. If not, controls can become invisible and still alive.

Avoidance:

- For new shell work, explicitly detach/reparent only known widgets.
- Do not clear arbitrary old layouts after some children have already moved.

### 3. Double Delete Risk With Video Cards

`clearMPhoneGrid()` reparents known cards but deletes unknown widgets with `deleteLater()`. `setMPhoneCard()` also deletes replaced cards. This is mostly okay, but becomes risky if a `VideoForm` is also owned by `IDeviceManage` or closed elsewhere.

Avoidance:

- Keep one ownership rule: card owns preview container; device manager owns device/session; `VideoForm` parent follows card unless expanded.
- When expanding, remove the `VideoForm` from card layout before changing parent.
- On card replacement, disconnect signals tied to old card actions if they capture old `VideoForm`.

### 4. Column Stretch Stale State

`relayoutMPhoneScreens()` sets column minimums/stretches for the current column count but does not reset columns from a previous wider layout.

Avoidance:

- Track previous column count or reset a reasonable range before applying new columns.
- Set `m_mphoneGrid->setColumnMinimumWidth(i, 0)` and stretch `0` for stale columns.

### 5. Empty State Forces Clipping

The current empty label uses `setMinimumSize(600, 420)`. In narrower layouts, this can force the scroll area wider than its viewport.

Avoidance:

- Use `setMaximumWidth(420)`, `setMinimumHeight(220)`, no large minimum width.
- Center it using layout stretch or an inner wrapper.

### 6. Hard Maximum Widths Cause Text Clipping

Current `localizeMPhoneUi()` and `rebuildMPhoneConfigPanel()` set many max widths. This helped the old drawer but will fight a responsive shell.

Avoidance:

- Remove max widths except tiny fields/buttons.
- Use layout column constraints instead.
- For long text labels, set tooltips and elide via a custom label or `QFontMetrics`.

### 7. QGroupBox Title Styling Is Fragile

`QGroupBox` title margins and padding vary across platforms and font metrics. This has likely contributed to clipping.

Avoidance:

- Prefer `QFrame` sections with explicit `QVBoxLayout`.
- Use a normal `QLabel` for the title.

### 8. Global QSS Can Accidentally Restyle Video Widgets

`qApp->setStyleSheet()` affects all widgets including `VideoForm`, OpenGL/video surfaces, dialogs, and menus.

Avoidance:

- Keep global rules shallow.
- Use object names for MPhone shell widgets.
- Do not style generic `QWidget` backgrounds too aggressively if OpenGL surfaces are affected.

### 9. `resizeEvent()` Relayout Can Thrash

If `resizeEvent()` directly calls `relayoutMPhoneScreens()` on every resize, hundreds of cards can be re-added repeatedly while dragging.

Avoidance:

- Use a single-shot timer, e.g. `m_relayoutTimer->start(50)`.
- Relayout only when the computed column count changes.

### 10. `QScrollArea::setWidgetResizable(true)` Needs Cooperative Children

If the scroll widget or its children have oversized minimum widths, the viewport cannot shrink cleanly.

Avoidance:

- `m_mphoneContainer->setMinimumWidth(0)`.
- Cards use bounded minimums per density.
- Empty state has no large minimum width.
- Drawer and pool panels collapse before the canvas drops below one card.

## Implementation Checklist

1. Add shell member pointers in `dialog.h`:
   - `QFrame *m_topBar`
   - `QFrame *m_devicePoolPanel`
   - `QFrame *m_centerDeck`
   - `QFrame *m_inspectorDrawer`
   - `QTabWidget *m_bottomConsole`
   - optional `QSplitter *m_mainSplitter`, `QSplitter *m_verticalSplitter`
2. Replace current `initMPhoneView()` body with `buildMPhoneShell()` flow.
3. Move reusable controls from `ui->leftWidget` and `ui->rightWidget` into the new panels.
4. Replace `QGroupBox`-heavy inspector with `QFrame#PanelSection` sections.
5. Move batch script editor to bottom console.
6. Revise `makeVideoCard()` and `makeStatusCard()` to use shared object names/properties and no inline QSS.
7. Revise `relayoutMPhoneScreens()` for margin-aware columns and stale-column reset.
8. Make drawer/pool collapse buttons change visibility and splitter sizes, not widget max widths.
9. Apply centralized QSS and dynamic state properties.
10. Test these viewports:
    - 1280 x 720
    - 1440 x 900
    - 1920 x 1080
    - narrow window with both side panels collapsed
    - 100+ fake/status cards

## Acceptance Criteria

- The first screen is the operational console, not a form or landing page.
- Device canvas remains scrollable and usable with hundreds of devices.
- Side panels can collapse without leaving blank layout gaps.
- No controls overlap or clip at 1280 x 720.
- No device card has changing height when selection/status changes.
- Existing start/stop/connect/settings/script functions still work through reused widgets or preserved slots.
- `dialog.ui` no longer determines the visible layout after `initMPhoneView()`.
- Styling is centralized enough that future card states do not require inline style strings.
