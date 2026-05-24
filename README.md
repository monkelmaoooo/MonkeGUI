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

# Preview

## Menu
![Menu](screenshots/image.png)

## Widgets
![Widgets](screenshots/image2.png)

## Color Picker
![Color Picker](screenshots/image3.png)

## Tabs / Combo
![Tabs](screenshots/image4.png)

---

# Video Showcase

<p align="center">
  <a href="https://youtu.be/XlFDaLO_2MQ">
    <img src="https://img.youtube.com/vi/XlFDaLO_2MQ/maxresdefault.jpg" width="800">
  </a>
</p>

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

Inspired by ImGui.

---

# License

MIT License
