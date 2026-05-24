# MonkeGUI Documentation

Lightweight animated immediate-mode GUI framework for Unreal Engine canvas rendering.

---

## Table of Contents

1. [Overview](#overview)
2. [Setup](#setup)
3. [Core Concepts](#core-concepts)
4. [Windows](#windows)
5. [Widgets](#widgets)
   - [Text](#text)
   - [Button](#button)
   - [Checkbox](#checkbox)
   - [SliderFloat](#sliderfloat)
   - [Combo](#combo)
   - [ColorPicker](#colorpicker)
   - [Separator](#separator)
6. [Tabs](#tabs)
7. [Search Bar](#search-bar)
8. [Popups](#popups)
9. [Effects](#effects)
10. [Watermark](#watermark)
11. [Styling](#styling)
12. [Drawing Utilities](#drawing-utilities)
13. [ID System](#id-system)
14. [Render Loop](#render-loop)

---

## Overview

MonkeGUI is an immediate-mode GUI library inspired by [Dear ImGui](https://github.com/ocornut/imgui), built specifically for rendering on top of Unreal Engine's `UCanvas`. It requires no external GUI dependencies and is compatible with UE4 and UE5.

All widgets are drawn via a deferred command queue and flushed in a single `Render()` call at the end of each frame. Animations are driven by exponential smoothing for a polished, responsive feel.

---

## Setup

### Per-frame initialization

Call these at the start of your render function, before any widgets:

```cpp
MonkeGUI::SetWhiteTexture(myWhiteTexture); // a 1x1 white UTexture*
MonkeGUI::SetFont(myFont);                 // a UFont*
MonkeGUI::SetupCanvas(Canvas, DeltaTime);  // UCanvas* and frame delta time
```

`SetupCanvas` resets the draw queue, updates mouse state, and clamps `DeltaTime` to a safe range (1/240 – 1/15 seconds).

### End of frame

After all widgets have been submitted, flush the draw queue:

```cpp
MonkeGUI::Render();
```

This is the only point at which actual draw calls are issued to `UCanvas`.

---

## Core Concepts

### Immediate mode

MonkeGUI follows the immediate-mode pattern: you call widget functions every frame, and they both handle input and queue their own draw commands. There is no retained widget tree.

### The draw queue

All drawing goes through `MonkeGUI::Post::`. Rectangles, lines, and text are enqueued and then issued to `UCanvas` in `Render()`. This ensures consistent draw order regardless of widget layout.

The queue holds up to **2048 commands**. Complex layouts with many widgets (e.g. large color pickers and open combos simultaneously) can approach this limit. Keep this in mind if you add many simultaneous widgets.

### Delta time

`gDT` drives all animations. It is set by `SetupCanvas` and clamped so animations behave correctly at both very high and very low frame rates.

---

## Windows

A window is the root container for all widgets. You must call `Begin` / `End` around your widget code.

```cpp
FVector2D pos  = { 200.f, 150.f };
FVector2D size = { 400.f, 300.f };
static bool collapsed = false;

if (MonkeGUI::Begin("My Window", &pos, &size, true, &collapsed))
{
    // widgets go here
    MonkeGUI::End();
}
```

### `Begin`

```cpp
bool Begin(const char* title, FVector2D* pos, FVector2D* size, bool open, bool* collapsed = nullptr);
```

| Parameter   | Description |
|-------------|-------------|
| `title`     | Title bar text. |
| `pos`       | Window position; updated in-place when dragged. |
| `size`      | Window size; updated in-place when resized. |
| `open`      | Pass `false` to skip rendering entirely. |
| `collapsed` | Optional pointer to a bool; enables the collapse button. Pass `nullptr` to disable. |

Returns `true` when the window is visible and widgets should be submitted. Returns `true` even when collapsed (so you can still call `End()`).

The window supports:
- **Dragging** via the title bar.
- **Resizing** via the grip in the bottom-right corner (minimum size: 220 × 120).
- **Collapsing** via the chevron button in the top-right (when `collapsed` is provided).

### `End`

```cpp
void End();
```

Must be called once for every successful `Begin`.

---

## Widgets

All widgets must be called between `Begin` and `End`. They advance `Cursor.Y` automatically.

---

### Text

Draws a single line of left-aligned text.

```cpp
void Text(const char* txt, bool disabled = false);
```

Pass `disabled = true` to render in the dimmed `Style::TextDisabled` color.

```cpp
MonkeGUI::Text("Hello, world!");
MonkeGUI::Text("This is greyed out", true);
```

---

### Button

```cpp
bool Button(const char* label, float w = 96.f, float h = Style::FrameH);
```

Returns `true` on the frame the button is released (clicked).

```cpp
if (MonkeGUI::Button("Click Me", 120.f))
{
    // handle click
}
```

Buttons animate on hover and press, including a subtle pop scale on activation.

---

### Checkbox

```cpp
bool Checkbox(const char* label, bool* v);
```

Returns `true` on the frame the value is toggled. The check mark animates in and out smoothly.

```cpp
static bool enabled = false;
MonkeGUI::Checkbox("Enable Feature", &enabled);
```

---

### SliderFloat

```cpp
bool SliderFloat(const char* label, float* v, float v_min, float v_max, float w = 220.f);
```

Returns `true` while the value is being changed. Displays a label above the track and the current value to the right.

```cpp
static float speed = 50.f;
MonkeGUI::SliderFloat("Speed", &speed, 0.f, 100.f);
```

The filled portion of the track animates toward the current value using exponential smoothing.

---

### Combo

A drop-down selector.

```cpp
bool Combo(const char* label, int* currentIndex, const char** items, int itemsCount,
           float w = 140.f, int maxVisible = 6);
```

Returns `true` on the frame the selection changes.

```cpp
static const char* options[] = { "Option A", "Option B", "Option C" };
static int selected = 0;
MonkeGUI::Combo("Mode", &selected, options, 3);
```

The drop-down animates open with a sliding reveal and staggered row fade-in. The chevron morphs between right (closed) and down (open).

Only one combo can be open at a time. Clicking outside closes it.

---

### ColorPicker

An inline color swatch that opens a draggable floating picker.

```cpp
bool ColorPicker(const char* label, FLinearColor* col, bool withAlpha = true);
```

Returns `true` while the color is being modified.

```cpp
static FLinearColor myColor = { 1.f, 0.5f, 0.f, 1.f };
MonkeGUI::ColorPicker("Tint", &myColor);
MonkeGUI::ColorPicker("Solid Color", &myColor, false); // no alpha bar
```

The picker includes:
- An SV (saturation/value) square.
- A vertical hue bar.
- An optional alpha bar (checkerboard background).
- A draggable title bar.
- A close button (X) or click-outside-to-close.

---

### Separator

Draws a horizontal rule and adds a small vertical gap.

```cpp
void Separator();
```

```cpp
MonkeGUI::Text("Section A");
MonkeGUI::Separator();
MonkeGUI::Text("Section B");
```

---

## Tabs

Tabs must be placed between `BeginTabs` / `EndTabs`, inside a window.

```cpp
static int tab = 0;

MonkeGUI::BeginTabs();
MonkeGUI::TabItem("General",  0, &tab);
MonkeGUI::TabItem("Advanced", 1, &tab);
MonkeGUI::TabItem("About",    2, &tab);
MonkeGUI::EndTabs();

if (tab == 0) { /* general widgets */ }
if (tab == 1) { /* advanced widgets */ }
if (tab == 2) { /* about widgets */ }
```

### `BeginTabs`
```cpp
void BeginTabs();
```
Begins a horizontal tab row at the current cursor position.

### `TabItem`
```cpp
bool TabItem(const char* label, int index, int* currentIndex);
```
Adds one tab button. Returns `true` on the frame it is clicked. Sets `*currentIndex = index` when pressed.

Tab width is estimated from the label length. The active tab lifts slightly and brightens.

### `EndTabs`
```cpp
void EndTabs();
```
Closes the tab row and draws a separator line beneath it.

---

## Search Bar

An interactive text input with a magnifier icon, blinking cursor, placeholder text, and key-repeat support.

```cpp
bool SearchBar(const char* label, char* buf, int bufSize, float w = 220.f);
```

Returns `true` on every frame the text changes.

```cpp
static char query[128]{};
MonkeGUI::SearchBar("##search", query, sizeof(query));
```

Use `"##id"` style labels (double hash prefix) to suppress the visible label while still providing a unique identifier.

### Filtering with `SearchMatches`

```cpp
bool SearchMatches(const char* item, const char* query);
```

Case-insensitive substring match. Returns `true` if `item` contains `query`, or if `query` is empty (show all).

```cpp
static char search[128]{};
MonkeGUI::SearchBar("##s", search, sizeof(search));

for (int i = 0; i < itemCount; i++)
    if (MonkeGUI::SearchMatches(itemNames[i], search))
        MonkeGUI::Checkbox(itemNames[i], &itemEnabled[i]);
```

### Keyboard behavior

| Key        | Action |
|------------|--------|
| Any letter / digit / symbol | Appends character (US layout) |
| Shift      | Uppercase / shifted symbol |
| Backspace  | Deletes last character (hold for repeat) |
| Escape     | Clears text if non-empty, otherwise unfocuses |

---

## Popups

Popups are centered, draggable, animated modal windows. Only one popup can be open at a time.

### Opening a popup

```cpp
MonkeGUI::OpenPopup("my_popup");
```

Call this in response to a button press or any other event.

### Rendering a popup

```cpp
if (MonkeGUI::BeginPopup("my_popup", { 240.f, 120.f }))
{
    MonkeGUI::Text("Hello from a popup!");

    if (MonkeGUI::Button("Close", 80.f))
        MonkeGUI::OpenPopupId = -1; // close programmatically

    MonkeGUI::EndPopup();
}
```

```cpp
bool BeginPopup(const char* str_id, FVector2D size);
void EndPopup();
```

`BeginPopup` returns `true` only while the popup with `str_id` is open. It renders a dimmed background overlay, a shadow, an animated scale-in with `EaseOutBack` bounce, and a draggable title bar.

Clicking outside the popup closes it automatically.

### `PopupCloseButton`

An optional X button you can place manually:

```cpp
bool PopupCloseButton(const FVector2D& pos, float size = 20.f);
```

Returns `true` when clicked, and closes the popup.

---

## Effects

### Snow

A purely cosmetic particle overlay. 180 snowflakes fall across the canvas with sinusoidal sway.

```cpp
// Call once per frame, outside Begin/End, before Render()
MonkeGUI::FX::SnowRender(deltaTime, 0.85f); // second arg = opacity 0..1
```

Snow resets and re-seeds automatically on first call. It draws directly to the canvas (bypasses the post queue), so call it before `Render()`.

### `DimBackground`

Fills the entire canvas with a semi-transparent black rectangle. Useful to draw focus to an open menu or popup.

```cpp
MonkeGUI::DimBackground(0.40f); // alpha 0..1
```

---

## Watermark

Draws a small overlay bar in the top-left corner showing custom text and optionally a live FPS counter.

```cpp
MonkeGUI::Watermark("My App v1.0");
```

### FPS callback

To display FPS, provide a callback before your first `Watermark` call (typically at init time):

```cpp
MonkeGUI::SetFPSCallback([]() -> float {
    const float dt = /* your delta time source */;
    return (dt > 0.00001f) ? 1.0f / dt : 0.f;
});
```

Without a callback, only the text is shown. With one, the output becomes `"My App v1.0 | 144 FPS"`.

---

## Styling

All style values live in the `MonkeGUI::Style` namespace as `inline` globals and can be changed at any time:

```cpp
// Sizes
MonkeGUI::Style::TitleH       = 22.f;   // title bar height
MonkeGUI::Style::RowH         = 18.f;   // widget row height
MonkeGUI::Style::FrameH       = 18.f;   // input frame height
MonkeGUI::Style::FontSize     = 13.f;   // informational; not enforced
MonkeGUI::Style::ItemSpacingY = 4.f;    // vertical gap between widgets
MonkeGUI::Style::Pad          = { 8.f, 8.f }; // window content padding

// Colors  (FLinearColor)
MonkeGUI::Style::Accent       = SRGB(66, 150, 250, 220); // highlight color
MonkeGUI::Style::Text         = SRGB(230, 230, 230, 255);
MonkeGUI::Style::WindowBg     = SRGB(28, 28, 28, 230);
// ... and more (see Style namespace in MonkeGUI.h)
```

Two top-level colors control the active window's background and title bar and can be changed at runtime (e.g. via `ColorPicker`):

```cpp
MonkeGUI::MenuBg  = someColor; // window background
MonkeGUI::TitleBg = someColor; // title bar
```

Colors are stored as `FLinearColor` (linear space). The `SRGB(r, g, b, a)` helper converts 0–255 sRGB values to linear automatically.

---

## Drawing Utilities

These are available for custom rendering outside of the widget system.

```cpp
// Filled axis-aligned rectangle
void DrawFilledRect(const FVector2D& pos, const FVector2D& size, const FLinearColor& col);

// Line segment
void DrawLine(const FVector2D& a, const FVector2D& b, float thickness, const FLinearColor& col);

// Outlined box, with optional filled interior
void DrawBox(const FVector2D& pos, const FVector2D& size, float thickness,
             const FLinearColor& col, bool filled = false, float fillAlphaMul = 0.25f);

// Anti-aliased circle stroke
void DrawCircleStroke_AA_Final(const FVector2D& center, float radius, float thicknessPx,
                                int segments, const FLinearColor& col);

// Checkerboard pattern (useful for alpha visualization)
void DrawCheckerboard(const FVector2D& pos, const FVector2D& size, float cell = 6.f);
```

All of these except `DrawLine` and `DrawCircleStroke_AA_Final` go through the Post queue and are flushed in `Render()`.

---

## ID System

Widget IDs are derived from their label strings using FNV-1a hashing. This means two widgets with the same label in the same scope will collide. Use `PushID` / `PopID` to scope IDs when rendering repeated or dynamic widget lists:

```cpp
for (int i = 0; i < count; i++)
{
    MonkeGUI::PushID(i);
    MonkeGUI::Checkbox("Enable", &items[i].enabled);
    MonkeGUI::PopID(i);
}
```

Both `const char*` and `int` overloads are available:

```cpp
void PushID(const char* s);
void PushID(int v);
void PopID(const char* s);
void PopID(int v);
```

Push and Pop must always be matched. The scope value is XOR'd into all IDs while active.

---

## Render Loop

A complete minimal render loop:

```cpp
void MyHUD::PostRender(UCanvas* Canvas)
{
    float dt = /* delta time */;

    MonkeGUI::SetWhiteTexture(myWhiteTex);
    MonkeGUI::SetFont(myFont);
    MonkeGUI::SetupCanvas(Canvas, dt);

    // Optional effects (before Begin)
    MonkeGUI::DimBackground(0.35f);
    MonkeGUI::FX::SnowRender(dt);

    // Watermark
    MonkeGUI::Watermark("MyApp");

    // Main window
    static FVector2D pos  = { 200.f, 150.f };
    static FVector2D size = { 380.f, 280.f };
    static bool collapsed = false;

    if (MonkeGUI::Begin("My Window", &pos, &size, true, &collapsed))
    {
        if (!collapsed)
        {
            static bool opt = false;
            MonkeGUI::Checkbox("Some Option", &opt);

            if (MonkeGUI::Button("Do Thing"))
                DoThing();
        }
        MonkeGUI::End();
    }

    // Popups (after main window)
    if (MonkeGUI::BeginPopup("my_popup", { 200.f, 100.f }))
    {
        MonkeGUI::Text("A popup!");
        if (MonkeGUI::Button("OK", 60.f))
            MonkeGUI::OpenPopupId = -1;
        MonkeGUI::EndPopup();
    }

    // Always last
    MonkeGUI::Render();
}
```
