# MonkeGUI
Lightweight animated immediate-mode GUI framework for Unreal Engine canvas rendering.

## Features

- Immediate-mode design
- Fully rendered using `UCanvas`
- Animated widgets
- Tabs
- Combo boxes
- Color picker
- Popup system
- Window dragging/resizing
- Custom styling
- Minimal dependencies
- UE4/UE5 compatible

## Preview

![preview](screenshots/menu.png)

---

# Widgets

- Button
- Checkbox
- Combo
- Tabs
- Image Tabs
- Color Picker
- Popup
- Text
- Separators
- Search Bar
- Animated Components

---

# Example

```cpp
if (MonkeGUI::Begin("Example", &menuPos, &menuSize, true))
{
    static bool enabled = false;

    MonkeGUI::Checkbox("Enable Feature", &enabled);

    if (MonkeGUI::Button("Execute"))
    {
        // action
    }

    MonkeGUI::End();
}
```

---

# Setup

```cpp
MonkeGUI::SetupCanvas(Canvas, DeltaTime);
MonkeGUI::SetWhiteTexture(Texture);
MonkeGUI::SetFont(Font);
```

---

# Requirements

- Unreal Engine 4 / 5
- `UCanvas`
- C++

---

# Design Goals

MonkeGUI was designed to provide:

- Clean visuals
- Smooth animations
- Easy integration
- Zero external GUI dependencies
- Lightweight rendering pipeline

---

# Credits

Inspired by Dear ImGui and custom UE canvas renderers.

---

# License

MIT License
