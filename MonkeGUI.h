#pragma once

#include "MonkeInput.h"
#include <Windows.h>
#include <cstring>
#include <cwchar>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <string>

namespace MonkeGUI
{
    static const wchar_t* s2wc_safe(const char* c, int& outLen)
    {
        static thread_local wchar_t buf[1024];
        buf[0] = L'\0';
        outLen = 1;

        if (!c) return buf;

        size_t inLen = std::strlen(c);
        if (inLen > 1023) inLen = 1023;

        std::mbstowcs(buf, c, inLen);
        buf[inLen] = L'\0';

        outLen = (int)std::wcslen(buf) + 1;
        return buf;
    }


    static inline float srgb_to_linear(float c)
    {
        if (c <= 0.04045f) return c / 12.92f;
        return powf((c + 0.055f) / 1.055f, 2.4f);
    }

    static inline SDK::FLinearColor SRGB(int r, int g, int b, int a = 255)
    {
        const float rf = (float)r / 255.0f;
        const float gf = (float)g / 255.0f;
        const float bf = (float)b / 255.0f;
        const float af = (float)a / 255.0f;

        SDK::FLinearColor out;
        out.R = srgb_to_linear(rf);
        out.G = srgb_to_linear(gf);
        out.B = srgb_to_linear(bf);
        out.A = af;
        return out;
    }

    namespace Style
    {
        inline float TitleH = 22.f;
        inline FVector2D Pad = FVector2D{ 8.f, 8.f };
        inline float ItemSpacingY = 4.f;
        inline float TextLineH = 16.f;
        inline float FrameH = 18.f;
        inline float FrameR = 0.f;
        inline float BorderT = 1.f;
        inline float FontSize = 13.f;
        inline float TextOffsetY = 2.f;
        inline float RowH = 18.f;

        inline float LabelW = 120.f;

        inline FLinearColor Text = SRGB(230, 230, 230, 255);
        inline FLinearColor TextDisabled = SRGB(150, 150, 150, 255);

        inline FLinearColor WindowBg = SRGB(28, 28, 28, 230);
        inline FLinearColor TitleBg = SRGB(45, 45, 45, 255);
        inline FLinearColor TitleBgActive = SRGB(55, 55, 55, 255);

        inline FLinearColor Tab = SRGB(45, 45, 45, 255);
        inline FLinearColor TabHovered = SRGB(60, 60, 60, 255);
        inline FLinearColor TabActive = SRGB(75, 75, 75, 255);

        inline FLinearColor FrameBg = SRGB(42, 42, 42, 255);
        inline FLinearColor FrameHovered = SRGB(60, 60, 60, 255);
        inline FLinearColor FrameActive = SRGB(75, 75, 75, 255);

        inline FLinearColor Button = SRGB(55, 55, 55, 255);
        inline FLinearColor ButtonHovered = SRGB(70, 70, 70, 255);
        inline FLinearColor ButtonActive = SRGB(90, 90, 90, 255);

        inline FLinearColor Border = SRGB(0, 0, 0, 110);
        inline FLinearColor Separator = SRGB(0, 0, 0, 80);

        inline FLinearColor Accent = SRGB(66, 150, 250, 220);

        inline FLinearColor Shadow = SRGB(0, 0, 0, 60);
        inline FVector2D    ShadowOff = FVector2D{ 1.f, 1.f };
    }

    namespace Post
    {
        enum : int { FILLED_RECT = 1, TEXT_L, TEXT_C, LINE };

        struct Cmd
        {
            int type = -1;
            FVector2D pos{};
            FVector2D size{};
            FLinearColor color{};
            float thickness = 1.f;
            char text[128]{};
            FVector2D a{}, b{};
        };

        static Cmd q[2048];
        static int n = 0;

        static inline void Reset() { n = 0; }

        static inline void RectFilled(const FVector2D& pos, const FVector2D& size, const FLinearColor& c)
        {
            if (n >= 2048) return;
            q[n] = {}; q[n].type = FILLED_RECT; q[n].pos = pos; q[n].size = size; q[n].color = c; n++;
        }

        static inline void Line(const FVector2D& a, const FVector2D& b, float t, const FLinearColor& c)
        {
            if (n >= 2048) return;
            q[n] = {}; q[n].type = LINE; q[n].a = a; q[n].b = b; q[n].thickness = t; q[n].color = c; n++;
        }

        static inline void RectBorder(const FVector2D& pos, const FVector2D& size, float t, const FLinearColor& c)
        {
            RectFilled(pos, FVector2D{ size.X, t }, c);
            RectFilled(FVector2D{ pos.X, pos.Y + size.Y - t }, FVector2D{ size.X, t }, c);
            RectFilled(pos, FVector2D{ t, size.Y }, c);
            RectFilled(FVector2D{ pos.X + size.X - t, pos.Y }, FVector2D{ t, size.Y }, c);
        }

        static inline void TextL(const char* txt, const FVector2D& pos, const FLinearColor& c)
        {
            if (n >= 2048) return;
            q[n] = {};
            q[n].type = TEXT_L;
            q[n].pos = pos;
            q[n].color = c;
            if (txt) { std::snprintf(q[n].text, sizeof(q[n].text), "%s", txt); }
            n++;
        }

        static inline void TextC(const char* txt, const FVector2D& pos, const FLinearColor& c)
        {
            if (n >= 2048) return;
            q[n] = {};
            q[n].type = TEXT_C;
            q[n].pos = pos;
            q[n].color = c;
            if (txt) { std::snprintf(q[n].text, sizeof(q[n].text), "%s", txt); }
            n++;
        }

    }

    static UCanvas* Canvas = nullptr;
    static UTexture* WhiteTex = nullptr;
    static UFont* Font = nullptr;

    static FVector2D Mouse{ 0.f, 0.f };
    static bool MouseDown = false;
    static bool MouseClicked = false;
    static bool MouseReleased = false;

    static int OpenComboId = -1;

    static int Hot = -1;
    static int Active = -1;
    static int IdCounter = 0;

    static FVector2D WinPos{ 200.f, 120.f };
    static FVector2D WinSize{ 360.f, 260.f };
    static FVector2D FullSize{};
    static FVector2D Cursor{ 0.f, 0.f };

    static int OpenColorId = -1;

    static bool Resizing = false;
    static FVector2D ResizeStartMouse{ 0.f, 0.f };
    static FVector2D ResizeStartSize{ 0.f, 0.f };

    static float MinWinW = 220.f;
    static float MinWinH = 120.f;
    static float ResizeGrip = 12.f;

    static bool InWindow = false;

    static bool Dragging = false;
    static FVector2D DragOff{ 0.f, 0.f };

    static inline void SetWhiteTexture(UTexture* T) { WhiteTex = T; }
    static inline void SetFont(UFont* F) { Font = F; }

    struct PopupState
    {
        FVector2D pos{ 0.f, 0.f };
        bool posInit = false;
        bool dragging = false;
        FVector2D dragOff{ 0.f, 0.f };
    };

    static int OpenPopupId = -1;

    struct PopupData
    {
        FVector2D pos{};
        bool posInit = false;
        bool dragging = false;
        FVector2D dragOff{};
        float openT = 0.f;

    };

    static std::unordered_map<int, PopupData> gPopups;

    static PopupState ColorPopup;

    static inline void DrawFilledCircle(const FVector2D& pos, float r, const FLinearColor& color)
    {
        if (!Canvas) return;

        const float PI = 3.14159265359f;
        const int segments = max(12, (int)(r * 6.f));
        const float step = 2.f * PI / segments;

        for (int i = 0; i < segments; i++)
        {
            float a0 = i * step;
            float a1 = (i + 1) * step;

            FVector2D p0{
                pos.X + std::cos(a0) * r,
                pos.Y + std::sin(a0) * r
            };

            FVector2D p1{
                pos.X + std::cos(a1) * r,
                pos.Y + std::sin(a1) * r
            };

            Canvas->K2_DrawLine(pos, p0, 1.f, color);
            Canvas->K2_DrawLine(p0, p1, 1.f, color);
        }
    }

    namespace FX
    {
        struct SnowFlake
        {
            float x, y;
            float vy;
            float vx;
            float size;
            float phase;
        };

        static SnowFlake g_snow[180];
        static bool g_inited = false;

        static inline float frand01()
        {
            return (float)(std::rand() % 10000) / 10000.0f;
        }

        static inline void SnowInit(float w, float h)
        {
            static bool seeded = false;
            if (!seeded) { std::srand((unsigned)std::time(nullptr)); seeded = true; }

            for (auto& f : g_snow)
            {
                f.x = frand01() * w;
                f.y = frand01() * h;
                f.vy = 30.f + frand01() * 90.f;
                f.vx = -10.f + frand01() * 20.f;
                f.size = 0.8f + frand01() * 1.8f;
                f.phase = frand01() * 6.28318f;
            }
            g_inited = true;
        }

        static inline void SnowRender(float dt, float alpha = 0.9f)
        {
            if (!Canvas) return;

            const float w = Canvas->SizeX;
            const float h = Canvas->SizeY;

            if (!g_inited) SnowInit(w, h);

            static float t = 0.f;
            t += dt;

            for (auto& f : g_snow)
            {
                const float sway = std::sinf(t * 0.8f + f.phase) * 8.0f;
                f.y += f.vy * dt;
                f.x += (f.vx + sway * 0.05f) * dt;

                if (f.y > h + 5.f)
                {
                    f.y = -5.f - frand01() * 30.f;
                    f.x = frand01() * w;
                }
                if (f.x < -10.f) f.x = w + 10.f;
                if (f.x > w + 10.f) f.x = -10.f;

                DrawFilledCircle(
                    FVector2D{ f.x, f.y },
                    f.size,
                    SRGB(255, 255, 255, (int)(alpha * 255.f))
                );
            }
        }
    }

    struct WidgetAnim
    {
        float hoverT = 0.f;
        float activeT = 0.f;
        float openT = 0.f;
    };

    static std::unordered_map<int, WidgetAnim> gAnim;
    static float gDT = 1.f / 60.f;

    static inline float Clamp01(float x) { return (x < 0.f) ? 0.f : (x > 1.f ? 1.f : x); }

    static inline FLinearColor MulAlpha(FLinearColor c, float aMul)
    {
        c.A *= aMul;
        return c;
    }

    static inline float EaseOutCubic(float t)
    {
        t = Clamp01(t);
        float u = 1.f - t;
        return 1.f - u * u * u;
    }

    static inline float EaseOutBack(float t)
    {
        t = Clamp01(t);
        const float c1 = 1.70158f;
        const float c3 = c1 + 1.f;
        return 1.f + c3 * std::pow(t - 1.f, 3.f) + c1 * std::pow(t - 1.f, 2.f);
    }

    static inline WidgetAnim& Anim(int id) { return gAnim[id]; }

    struct ComboAnimState
    {
        float openT = 0.f;
        float chevronT = 0.f;
    };

    static std::unordered_map<int, ComboAnimState> gComboAnim;

    static inline float ExpApproach(float current, float target, float speed, float dt)
    {
        const float k = 1.f - std::exp(-speed * dt);
        return current + (target - current) * k;
    }

    static inline float Smooth01(float current, float target, float speed)
    {
        return ExpApproach(current, target, speed, gDT);
    }

    static inline FLinearColor LerpColor(const FLinearColor& a, const FLinearColor& b, float t)
    {
        return FLinearColor{
            a.R + (b.R - a.R) * t,
            a.G + (b.G - a.G) * t,
            a.B + (b.B - a.B) * t,
            a.A + (b.A - a.A) * t
        };
    }

    static inline FVector2D GetMousePos_Client()
    {
        POINT p{};
        GetCursorPos(&p);
        HWND hwnd = GetForegroundWindow();
        if (hwnd) ScreenToClient(hwnd, &p);
        return FVector2D{ (float)p.x, (float)p.y };
    }

    static inline float PowfSafe(float x, float p)
    {
        x = Clamp01(x);
        return powf(x, p);
    }


    static inline void UpdateMouse()
    {
        Mouse = GetMousePos_Client();

        bool down = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        MouseClicked = down && !MouseDown;
        MouseReleased = !down && MouseDown;
        MouseDown = down;

        Hot = -1;
    }

    static inline uint32_t Hash32(const char* s)
    {
        uint32_t h = 2166136261u;
        for (; s && *s; ++s) { h ^= (uint8_t)*s; h *= 16777619u; }
        return h;
    }

    static inline int GetID(const char* label, int scope = 0)
    {
        return (int)(Hash32(label) ^ (uint32_t)scope);
    }


    static inline bool MouseInRect(const FVector2D& pos, const FVector2D& size)
    {
        return (Mouse.X >= pos.X && Mouse.Y >= pos.Y &&
            Mouse.X <= pos.X + size.X && Mouse.Y <= pos.Y + size.Y);
    }

    static inline void DrawChevronDown(const FVector2D& center, float s, const FLinearColor& c)
    {
        Post::Line({ center.X - s, center.Y - s * 0.2f }, { center.X, center.Y + s }, 1.f, c);
        Post::Line({ center.X, center.Y + s }, { center.X + s, center.Y - s * 0.2f }, 1.f, c);
    }

    static inline void DrawChevronRight(const FVector2D& center, float s, const FLinearColor& c)
    {
        Post::Line({ center.X - s * 0.2f, center.Y - s }, { center.X + s, center.Y }, 1.f, c);
        Post::Line({ center.X + s, center.Y }, { center.X - s * 0.2f, center.Y + s }, 1.f, c);
    }

    static inline float ContentX() { return WinPos.X + Style::Pad.X; }
    static inline float ControlX() { return ContentX() + Style::LabelW; }
    static inline float TextY() { return Cursor.Y + Style::TextOffsetY; }
    static inline float CenterY(float h) { return Cursor.Y + (Style::RowH - h) * 0.5f; }

    static inline int NextID() { return ++IdCounter; }

    static int IdScope = 0;

    static inline void PushID(const char* s) { IdScope ^= (int)Hash32(s); }
    static inline void PushID(int v) { IdScope ^= (v * 16777619); }
    static inline void PopID(const char* s) { IdScope ^= (int)Hash32(s); }
    static inline void PopID(int v) { IdScope ^= (v * 16777619); }


    static inline float SnapHalfPx(float x) { return floorf(x) + 0.5f; }

    static inline float SnapRadius(float r)
    {
        return SnapHalfPx(r);
    }

    static inline int CalcCircleSegments(float radiusPx, float maxErrorPx = 0.45f)
    {
        radiusPx = max(1.0f, radiusPx);
        maxErrorPx = std::clamp(maxErrorPx, 0.25f, 1.5f);

        float n = 3.14159265f * sqrtf(radiusPx / (2.0f * maxErrorPx));
        int seg = (int)ceilf(n);

        return std::clamp(seg, 32, 160);
    }

    static inline void DrawFilledRect(const FVector2D& pos, const FVector2D& size, const FLinearColor& col)
    {
        if (!Canvas || !WhiteTex) return;
        Canvas->K2_DrawTexture(
            WhiteTex,
            pos,
            size,
            FVector2D{ 0.f, 0.f },
            FVector2D{ 1.f, 1.f },
            col,
            EBlendMode::BLEND_Translucent,
            0.f,
            FVector2D{ 0.f, 0.f }
        );
    }

    static inline void DimBackground(float alpha = 0.45f)
    {
        if (!Canvas) return;

        const float w = Canvas->SizeX;
        const float h = Canvas->SizeY;

        Post::RectFilled(FVector2D{ 0.f, 0.f }, FVector2D{ w, h }, SRGB(0, 0, 0, (int)(alpha * 255.f)));
    }

    static inline void DrawX(const FVector2D& center, float s, const FLinearColor& c)
    {
        Post::Line({ center.X - s, center.Y - s }, { center.X + s, center.Y + s }, 1.f, c);
        Post::Line({ center.X + s, center.Y - s }, { center.X - s, center.Y + s }, 1.f, c);
    }

    static inline void OpenPopup(const char* str_id)
    {
        OpenPopupId = GetID(str_id, IdScope);
    }

    static inline bool BeginPopup(const char* str_id, FVector2D size)
    {
        if (!Canvas) return false;

        int id = GetID(str_id, IdScope);

        if (OpenPopupId != id)
        {
            auto it = gPopups.find(id);
            if (it != gPopups.end())
                it->second.openT = 0.f;
            return false;
        }

        auto& pop = gPopups[id];

        if (!pop.posInit)
        {
            pop.pos = FVector2D{
                Canvas->SizeX * 0.5f - size.X * 0.5f,
                Canvas->SizeY * 0.5f - size.Y * 0.5f
            };
            pop.posInit = true;
            pop.openT = 0.f;
        }

        pop.openT = ExpApproach(pop.openT, 1.f, 14.f, gDT);
        const float t = EaseOutBack(pop.openT);
        const float alpha = Clamp01(pop.openT * 2.f);

        FVector2D pos = pop.pos;

        const float titleH = 20.f;
        FVector2D barPos = pos;
        FVector2D barSz{ size.X, titleH };

        bool hoverBar = MouseInRect(barPos, barSz);

        if (hoverBar && MouseClicked)
        {
            pop.dragging = true;
            pop.dragOff = FVector2D{ Mouse.X - pos.X, Mouse.Y - pos.Y };
        }

        if (!MouseDown)
            pop.dragging = false;

        if (pop.dragging)
        {
            pop.pos = FVector2D{ Mouse.X - pop.dragOff.X, Mouse.Y - pop.dragOff.Y };
            pos = pop.pos;
        }

        if (MouseClicked && !MouseInRect(pos, size))
        {
            OpenPopupId = -1;
            pop.posInit = false;
            pop.openT = 0.f;
            return false;
        }
        const FVector2D center{ pos.X + size.X * 0.5f, pos.Y + size.Y * 0.5f };
        const FVector2D scaledSize{ size.X * t, size.Y * t };
        const FVector2D scaledPos{ center.X - scaledSize.X * 0.5f, center.Y - scaledSize.Y * 0.5f };
        const FVector2D scaledTitleSz{ scaledSize.X, titleH * t };

        DimBackground(0.4f * alpha);

        Post::RectFilled(
            FVector2D{ scaledPos.X + 3.f, scaledPos.Y + 3.f },
            scaledSize,
            MulAlpha(Style::Shadow, alpha * 1.4f));

        Post::RectFilled(scaledPos, scaledSize, MulAlpha(Style::WindowBg, alpha));
        Post::RectBorder(scaledPos, scaledSize, Style::BorderT, MulAlpha(Style::Border, alpha));

        Post::RectFilled(scaledPos, scaledTitleSz, MulAlpha(Style::TitleBgActive, alpha));
        Post::RectBorder(scaledPos, scaledTitleSz, Style::BorderT, MulAlpha(Style::Border, alpha));

        InWindow = true;

        WinPos = scaledPos;
        WinSize = scaledSize;
        Cursor = FVector2D{ scaledPos.X + Style::Pad.X, scaledPos.Y + scaledTitleSz.Y + Style::Pad.Y };

        return true;
    }

    static inline void EndPopup()
    {
        InWindow = false;
    }

    static inline bool PopupCloseButton(const FVector2D& pos, float size = 20.f)
    {
        FVector2D btnPos{ pos.X - size, pos.Y };
        FVector2D btnSz{ size, size };

        bool hovered = MouseInRect(btnPos, btnSz);

        if (hovered && MouseClicked)
        {
            OpenPopupId = -1;
            return true;
        }

        if (hovered)
            Post::RectFilled(btnPos, btnSz, SRGB(255, 255, 255, 12));

        DrawX(FVector2D{ btnPos.X + size * 0.5f, btnPos.Y + size * 0.5f }, 5.f, Style::Text);

        return false;
    }

    static inline void DrawCircleWithLineTex(
        const FVector2D& center,
        float radius,
        float thicknessPx,
        int segments,
        const FLinearColor& col,

        int rings = 4,
        float aaWidthPx = 1.25f,
        float alphaGamma = 1.8f,
        bool featherOutsideOnly = true
    )
    {
        if (!Canvas || !WhiteTex) return;
        if (segments < 8) segments = 8;
        rings = std::clamp(rings, 1, 8);

        const float TwoPi = 6.28318530717958647692f;
        const float Step = TwoPi / (float)segments;

        auto DrawRing = [&](float r, float thick, float aMul)
            {
                if (r <= 0.01f || thick <= 0.01f || aMul <= 0.0001f) return;

                const float segLen = 2.0f * r * sinf(Step * 0.5f);

                FLinearColor c = col;
                c.A = Clamp01(c.A * aMul);

                for (int i = 0; i < segments; i++)
                {
                    const float a0 = Step * i;
                    const float a1 = a0 + Step;
                    const float am = (a0 + a1) * 0.5f;

                    FVector2D mid{
                        center.X + cosf(am) * r,
                        center.Y + sinf(am) * r
                    };

                    FVector2D size{ segLen, thick };
                    FVector2D pos{ mid.X - size.X * 0.5f, mid.Y - size.Y * 0.5f };

                    const float rotDeg = (am * 57.29577951308232f) + 90.0f;
                    const FVector2D pivot{ 0.5f, 0.5f };

                    Canvas->K2_DrawTexture(
                        WhiteTex,
                        pos,
                        size,
                        FVector2D{ 0.f, 0.f },
                        FVector2D{ 1.f, 1.f },
                        c,
                        EBlendMode::BLEND_Translucent,
                        rotDeg,
                        pivot
                    );
                }
            };

        if (rings == 1 || aaWidthPx <= 0.01f)
        {
            DrawRing(radius, thicknessPx, 1.f);
            return;
        }

        const float start = featherOutsideOnly ? radius : (radius - aaWidthPx * 0.5f);
        const float end = featherOutsideOnly ? (radius + aaWidthPx) : (radius + aaWidthPx * 0.5f);

        const float thicknessBoostMax = 0.35f;

        for (int k = 0; k < rings; k++)
        {
            const float t = (float)k / (float)(rings - 1);
            const float r = start + (end - start) * t;

            float w = 1.f;
            if (featherOutsideOnly)
            {
                w = powf(1.0f - t, alphaGamma);
            }
            else
            {
                const float d = fabsf(t - 0.5f) * 2.0f;
                w = powf(1.0f - d, alphaGamma);
            }

            const float thick = thicknessPx + thicknessBoostMax * t;
            DrawRing(r, thick, w);
        }
    }

    static inline void DrawCircleStroke_AA_Final(
        const FVector2D& center,
        float radius,
        float thicknessPx,
        int segments,
        const FLinearColor& col
    )
    {
        if (!Canvas || !WhiteTex) return;

        thicknessPx = max(1.0f, thicknessPx);

        if (segments <= 0)
            segments = CalcCircleSegments(radius, 0.8f);

        DrawCircleWithLineTex(center, radius, thicknessPx, segments, col, 1, 0, 1, true);

        const float halfT = thicknessPx * 0.5f;

        const float featherPx = 1.0f;

        FLinearColor featherCol = col;
        featherCol.A = Clamp01(col.A * 0.32f);

        DrawCircleWithLineTex(
            center,
            radius + halfT + featherPx * 0.5f,
            featherPx,
            segments,
            featherCol,
            1, 0, 1, true
        );

        if (radius - halfT - featherPx * 0.5f > 0.5f)
        {
            DrawCircleWithLineTex(
                center,
                radius - halfT - featherPx * 0.5f,
                featherPx,
                segments,
                featherCol,
                1, 0, 1, true
            );
        }
    }



    static inline void DrawBox(const FVector2D& pos, const FVector2D& size, float thickness, const FLinearColor& col, bool filled = false, float fillAlphaMul = 0.25f)
    {
        if (filled)
        {
            FLinearColor fillCol = col;
            fillCol.A *= fillAlphaMul;

            MonkeGUI::Post::RectFilled(pos, size, fillCol);
        }

        const FVector2D p0{ pos.X,            pos.Y };
        const FVector2D p1{ pos.X + size.X,   pos.Y };
        const FVector2D p2{ pos.X + size.X,   pos.Y + size.Y };
        const FVector2D p3{ pos.X,            pos.Y + size.Y };

        MonkeGUI::Post::Line(p0, p1, thickness, col);
        MonkeGUI::Post::Line(p1, p2, thickness, col);
        MonkeGUI::Post::Line(p2, p3, thickness, col);
        MonkeGUI::Post::Line(p3, p0, thickness, col);
    }

    static inline void DrawLine(const FVector2D& a, const FVector2D& b, float t, const FLinearColor& col)
    {
        if (!Canvas) return;
        Canvas->K2_DrawLine(a, b, t, col);
    }

    static inline void DrawText_Impl(const char* txt, const FVector2D& pos, const FLinearColor& col, bool center)
    {
        if (!Canvas || !Font || !txt) return;

        int len = 0;
        const wchar_t* w = s2wc_safe(txt, len);

        Canvas->K2_DrawText(
            Font,
            FString((const TCHAR*)w),
            pos,
            FVector2D(1.0f, 1.0f),
            col,
            0.0f,
            Style::Shadow,
            Style::ShadowOff,
            center,
            true,
            false,
            SRGB(0, 0, 0, 0)
        );
    }

    static bool TabsActiveRow = false;
    static float TabsX = 0.f;
    static float TabsY = 0.f;

    static inline float EstimateTextW(const char* s)
    {
        if (!s) return 0.f;
        return (float)std::strlen(s) * 7.0f;
    }

    static inline void StampRectLine(const FVector2D& a, const FVector2D& b, float thickness, const FLinearColor& col)
    {
        FVector2D d{ b.X - a.X, b.Y - a.Y };
        float len = std::sqrt(d.X * d.X + d.Y * d.Y);
        if (len <= 0.0001f) return;

        float step = max(1.0f, thickness * 0.5f);
        int steps = (int)std::ceil(len / step);

        const float half = thickness * 0.5f;

        for (int i = 0; i <= steps; i++)
        {
            float t = (steps == 0) ? 0.f : (float)i / (float)steps;
            FVector2D p{ a.X + d.X * t, a.Y + d.Y * t };

            Post::RectFilled(
                FVector2D{ p.X - half, p.Y - half },
                FVector2D{ thickness, thickness },
                col
            );
        }
    }

    static inline void DrawCheckMark(const FVector2D& boxPos, float boxSz, const FLinearColor& col)
    {
        const float thickness = max(2.0f, boxSz * 0.14f);

        FVector2D a{ boxPos.X + boxSz * 0.20f, boxPos.Y + boxSz * 0.55f };
        FVector2D b{ boxPos.X + boxSz * 0.42f, boxPos.Y + boxSz * 0.75f };
        FVector2D c{ boxPos.X + boxSz * 0.80f, boxPos.Y + boxSz * 0.28f };

        StampRectLine(a, b, thickness, col);
        StampRectLine(b, c, thickness, col);
    }

    static inline void TextLeft(const char* txt, const FVector2D& pos, const FLinearColor& col = Style::Text)
    {
        Post::TextL(txt ? txt : "", pos, col);
    }

    static inline void TextCenter(const char* txt, const FVector2D& centerPos, const FLinearColor& col = Style::Text)
    {
        Post::TextC(txt ? txt : "", centerPos, col);
    }

    static inline FLinearColor Lerp(const FLinearColor& a, const FLinearColor& b, float t)
    {
        t = Clamp01(t);
        FLinearColor o;
        o.R = a.R + (b.R - a.R) * t;
        o.G = a.G + (b.G - a.G) * t;
        o.B = a.B + (b.B - a.B) * t;
        o.A = a.A + (b.A - a.A) * t;
        return o;
    }

    static inline FLinearColor HSVtoRGB(float h, float s, float v, float a = 1.f)
    {
        h = h - floorf(h);
        s = Clamp01(s);
        v = Clamp01(v);

        float r = v, g = v, b = v;

        if (s > 0.f)
        {
            float hf = h * 6.f;
            int i = (int)floorf(hf);
            float f = hf - (float)i;
            float p = v * (1.f - s);
            float q = v * (1.f - s * f);
            float t = v * (1.f - s * (1.f - f));

            switch (i % 6)
            {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;
            case 5: r = v; g = p; b = q; break;
            }
        }

        FLinearColor out; out.R = r; out.G = g; out.B = b; out.A = a;
        return out;
    }

    static inline void RGBtoHSV(const FLinearColor& c, float& outH, float& outS, float& outV)
    {
        float r = Clamp01(c.R), g = Clamp01(c.G), b = Clamp01(c.B);
        float mx = max(r, max(g, b));
        float mn = min(r, min(g, b));
        float d = mx - mn;

        outV = mx;
        outS = (mx <= 0.f) ? 0.f : (d / mx);

        if (d <= 0.f)
        {
            outH = 0.f;
            return;
        }

        float h;
        if (mx == r)      h = (g - b) / d + (g < b ? 6.f : 0.f);
        else if (mx == g) h = (b - r) / d + 2.f;
        else              h = (r - g) / d + 4.f;

        outH = (h / 6.f);
    }



    static inline void DrawHueBar(const FVector2D& pos, const FVector2D& sz)
    {
        const int steps = 30;
        const float stepH = sz.Y / (float)steps;

        for (int i = 0; i < steps; i++)
        {
            float h0 = (float)i / (float)steps;
            float h1 = (float)(i + 1) / (float)steps;

            FLinearColor c0 = HSVtoRGB(h0, 1.f, 1.f, 1.f);
            FLinearColor c1 = HSVtoRGB(h1, 1.f, 1.f, 1.f);

            FVector2D p0{ pos.X, pos.Y + i * stepH };
            Post::RectFilled(p0, FVector2D{ sz.X, stepH * 0.5f }, c0);
            Post::RectFilled(FVector2D{ p0.X, p0.Y + stepH * 0.5f }, FVector2D{ sz.X, stepH * 0.5f }, c1);
        }

        Post::RectBorder(pos, sz, Style::BorderT, Style::Border);
    }

    static inline void DrawSVSquare(const FVector2D& pos, const FVector2D& sz, float hue)
    {
        const int stepsX = 40;
        const int stepsY = 40;

        FLinearColor hueCol = HSVtoRGB(hue, 1.f, 1.f, 1.f);

        for (int y = 0; y < stepsY; y++)
        {
            float v = 1.f - (float)y / (float)(stepsY - 1);
            float rowY = pos.Y + (sz.Y * y) / stepsY;
            float rowH = sz.Y / stepsY + 1.f;

            for (int x = 0; x < stepsX; x++)
            {
                float s = (float)x / (float)(stepsX - 1);
                float colX = pos.X + (sz.X * x) / stepsX;
                float colW = sz.X / stepsX + 1.f;

                FLinearColor top = Lerp(FLinearColor{ 1,1,1,1 }, hueCol, s);

                FLinearColor out = top;
                out.R *= v; out.G *= v; out.B *= v;

                Post::RectFilled(FVector2D{ colX, rowY }, FVector2D{ colW, rowH }, out);
            }
        }

        Post::RectBorder(pos, sz, Style::BorderT, Style::Border);
    }


    static inline void DrawCheckerboard(const FVector2D& pos, const FVector2D& size, float cell = 6.f)
    {
        const FLinearColor c0 = SRGB(90, 90, 90, 255);
        const FLinearColor c1 = SRGB(130, 130, 130, 255);

        for (float y = 0; y < size.Y; y += cell)
        {
            for (float x = 0; x < size.X; x += cell)
            {
                bool odd = ((int)(x / cell) + (int)(y / cell)) & 1;
                Post::RectFilled(
                    FVector2D{ pos.X + x, pos.Y + y },
                    FVector2D{ cell, cell },
                    odd ? c0 : c1
                );
            }
        }
    }

    static inline void SetupCanvas(UCanvas* CanvasPtr, float dt)
    {
        Canvas = CanvasPtr;

        gDT = std::clamp(dt, 1.f / 240.f, 1.f / 15.f);

        UpdateMouse();
        IdCounter = 0;
        InWindow = false;
        Post::Reset();
    }

    FLinearColor MenuBg = MonkeGUI::Style::WindowBg;
    FLinearColor TitleBg = MonkeGUI::Style::TitleBgActive;

    static inline bool Begin(const char* title, FVector2D* pos, FVector2D* size, bool open, bool* collapsed = nullptr)
    {
        if (!open || !Canvas || !pos || !size) { InWindow = false; return false; }

        InWindow = true;
        WinPos = *pos;

        const float titleH = Style::TitleH;
        const bool haveCollapse = (collapsed != nullptr);
        const bool isCollapsed = (haveCollapse && *collapsed);

        FullSize = *size;

        FVector2D drawSize = isCollapsed ? FVector2D{ FullSize.X, titleH } : FullSize;
        WinSize = drawSize;

        const float btnSize = titleH;
        FVector2D collapsePos{ WinPos.X + drawSize.X - btnSize, WinPos.Y };
        FVector2D collapseSz{ btnSize, btnSize };
        const bool hoverCollapse = haveCollapse && MouseInRect(collapsePos, collapseSz);

        const bool canResize = !isCollapsed;
        FVector2D gripPos{ WinPos.X + drawSize.X - ResizeGrip, WinPos.Y + drawSize.Y - ResizeGrip };
        FVector2D gripSz{ ResizeGrip, ResizeGrip };
        const bool hoverGrip = canResize && MouseInRect(gripPos, gripSz);

        FVector2D tPos = WinPos;
        FVector2D tSize{ drawSize.X, titleH };
        const bool hoverTitle = MouseInRect(tPos, tSize);

        if (hoverGrip && MouseClicked)
        {
            Resizing = true;
            ResizeStartMouse = Mouse;
            ResizeStartSize = FullSize;
        }
        if (!MouseDown)
            Resizing = false;

        if (Resizing)
        {
            FVector2D delta{ Mouse.X - ResizeStartMouse.X, Mouse.Y - ResizeStartMouse.Y };
            FVector2D newSize{ ResizeStartSize.X + delta.X, ResizeStartSize.Y + delta.Y };

            if (newSize.X < MinWinW) newSize.X = MinWinW;
            if (newSize.Y < MinWinH) newSize.Y = MinWinH;

            *size = newSize;
            FullSize = newSize;

            drawSize = FVector2D{ FullSize.X, FullSize.Y };
            WinSize = drawSize;

            collapsePos = FVector2D{ WinPos.X + drawSize.X - btnSize, WinPos.Y };
            gripPos = FVector2D{ WinPos.X + drawSize.X - ResizeGrip, WinPos.Y + drawSize.Y - ResizeGrip };
            tSize = FVector2D{ drawSize.X, titleH };
        }

        if (hoverTitle && MouseClicked && !hoverCollapse && !hoverGrip && !Resizing)
        {
            Dragging = true;
            DragOff = FVector2D{ Mouse.X - WinPos.X, Mouse.Y - WinPos.Y };
        }
        if (!MouseDown)
            Dragging = false;

        if (Dragging)
        {
            WinPos = FVector2D{ Mouse.X - DragOff.X, Mouse.Y - DragOff.Y };
            *pos = WinPos;

            collapsePos = FVector2D{ WinPos.X + drawSize.X - btnSize, WinPos.Y };
            gripPos = FVector2D{ WinPos.X + drawSize.X - ResizeGrip, WinPos.Y + drawSize.Y - ResizeGrip };
            tPos = WinPos;
        }

        if (haveCollapse && hoverCollapse && MouseClicked)
            *collapsed = !*collapsed;

        Post::RectFilled(WinPos, drawSize, MenuBg);
        Post::RectBorder(WinPos, drawSize, Style::BorderT, Style::Border);

        Post::RectFilled(WinPos, FVector2D{ drawSize.X, titleH }, TitleBg);
        Post::RectBorder(WinPos, FVector2D{ drawSize.X, titleH }, Style::BorderT, Style::Border);

        {
            const float x = WinPos.X + Style::Pad.X;
            const float y = WinPos.Y + titleH * 0.5f;
            Post::TextL(title ? title : "", FVector2D{ x, y }, Style::Text);
        }

        if (haveCollapse)
        {
            if (hoverCollapse)
                Post::RectFilled(collapsePos, collapseSz, SRGB(255, 255, 255, 12));

            FVector2D chevCenter{ collapsePos.X + collapseSz.X * 0.5f, collapsePos.Y + collapseSz.Y * 0.5f };
            const float s = 4.0f;

            if (*collapsed) DrawChevronRight(chevCenter, s, Style::Text);
            else           DrawChevronDown(chevCenter, s, Style::Text);
        }

        if (canResize)
        {
            const FLinearColor gripCol = hoverGrip || Resizing ? Style::TextDisabled : Style::Border;
            const float x = WinPos.X + drawSize.X;
            const float y = WinPos.Y + drawSize.Y;

            Post::Line(FVector2D{ x - 4.f,  y - 1.f }, FVector2D{ x - 1.f,  y - 4.f }, 1.f, gripCol);
            Post::Line(FVector2D{ x - 8.f,  y - 1.f }, FVector2D{ x - 1.f,  y - 8.f }, 1.f, gripCol);
            Post::Line(FVector2D{ x - 12.f, y - 1.f }, FVector2D{ x - 1.f,  y - 12.f }, 1.f, gripCol);
        }

        if (isCollapsed)
            return true;

        Cursor = FVector2D{ WinPos.X + Style::Pad.X, WinPos.Y + titleH + Style::Pad.Y };
        return true;
    }

    static inline void DrawPopupTitleBar(const FVector2D& pos, const FVector2D& sz, const char* title)
    {
        const float titleH = 20.f;
        Post::RectFilled(pos, FVector2D{ sz.X, titleH }, Style::TitleBgActive);
        Post::RectBorder(pos, FVector2D{ sz.X, titleH }, Style::BorderT, Style::Border);
        Post::TextL(title ? title : "", FVector2D{ pos.X + 6.f, pos.Y + titleH * 0.5f }, Style::Text);
    }

    static inline void End()
    {
        InWindow = false;
    }

    static inline void Separator()
    {
        if (!InWindow) return;
        FVector2D a{ WinPos.X + Style::Pad.X, Cursor.Y + 4.f };
        FVector2D b{ WinPos.X + WinSize.X - Style::Pad.X, Cursor.Y + 4.f };
        Post::Line(a, b, 1.f, Style::Separator);
        Cursor.Y += 8.f;
    }

    static inline void Text(const char* txt, bool disabled = false)
    {
        if (!InWindow) return;

        Post::TextL(
            txt,
            FVector2D{ ContentX(), TextY() },
            disabled ? Style::TextDisabled : Style::Text
        );

        Cursor.Y += Style::RowH + Style::ItemSpacingY;
    }

    static inline bool Button(const char* label, float w = 96.f, float h = Style::FrameH)
    {
        if (!InWindow) return false;

        int id = NextID();
        auto& a = Anim(id);

        FVector2D pos{ ContentX(), Cursor.Y };
        FVector2D sz{ w, h };

        bool hovered = MouseInRect(pos, sz);
        if (hovered) Hot = id;
        if (hovered && MouseClicked) Active = id;

        bool pressed = (Active == id && hovered && MouseReleased);

        a.hoverT = ExpApproach(a.hoverT, hovered ? 1.f : 0.f, 18.f, gDT);
        a.activeT = ExpApproach(a.activeT, (Active == id && MouseDown) ? 1.f : 0.f, 28.f, gDT);

        FLinearColor base = Style::Button;
        FLinearColor hov = Style::ButtonHovered;
        FLinearColor act = Style::ButtonActive;

        FLinearColor col = LerpColor(base, hov, EaseOutCubic(a.hoverT));
        col = LerpColor(col, act, EaseOutCubic(a.activeT));

        float pop = 1.f + 0.03f * EaseOutBack(a.activeT);
        FVector2D drawSz = FVector2D{ sz.X * pop, sz.Y * pop };
        FVector2D drawPos = FVector2D{ pos.X + (sz.X - drawSz.X) * 0.5f, pos.Y + (sz.Y - drawSz.Y) * 0.5f };

        Post::RectFilled(drawPos, drawSz, col);
        Post::RectBorder(drawPos, drawSz, Style::BorderT, Style::Border);

        Post::TextC(label ? label : "", FVector2D{ pos.X + sz.X * 0.5f, pos.Y + sz.Y * 0.5f }, Style::Text);

        Cursor.Y += h + Style::ItemSpacingY;
        return pressed;
    }


    static inline bool Checkbox(const char* label, bool* v)
    {
        if (!InWindow || !v) return false;

        const int id = (int)(Hash32(label ? label : "") ^ (uint32_t)IdScope);
        const float box = 16.f;

        FVector2D rowPos{ ContentX(), Cursor.Y };
        FVector2D rowSz{ WinSize.X - 2.f * Style::Pad.X, Style::RowH };

        bool hovered = MouseInRect(rowPos, rowSz);
        if (hovered) Hot = id;
        if (hovered && MouseClicked) Active = id;

        bool toggled = false;
        if (Active == id && hovered && MouseReleased)
        {
            *v = !*v;
            toggled = true;
        }

        auto& a = Anim(id);
        a.hoverT = ExpApproach(a.hoverT, hovered ? 1.f : 0.f, 18.f, gDT);
        a.activeT = ExpApproach(a.activeT, (Active == id && MouseDown) ? 1.f : 0.f, 28.f, gDT);
        a.openT = ExpApproach(a.openT, (*v) ? 1.f : 0.f, 20.f, gDT);

        FVector2D boxPos{ ContentX(), CenterY(box) };
        FVector2D boxSz{ box, box };

        FLinearColor frame = LerpColor(Style::FrameBg, Style::FrameHovered, EaseOutCubic(a.hoverT));
        frame = LerpColor(frame, Style::FrameActive, EaseOutCubic(a.activeT));

        Post::RectFilled(boxPos, boxSz, frame);
        Post::RectBorder(boxPos, boxSz, Style::BorderT, Style::Border);

        if (a.openT > 0.001f)
        {
            float t = EaseOutCubic(a.openT);
            DrawCheckMark(boxPos, box, MulAlpha(Style::Accent, t));
        }

        const float labelX = boxPos.X + box + 6.f;
        const float textY = rowPos.Y + rowSz.Y * 0.5f;

        Post::TextL(label ? label : "", FVector2D{ labelX, textY }, Style::Text);

        Cursor.Y += Style::RowH + Style::ItemSpacingY;

        return toggled;
    }


    static inline bool ColorPicker(const char* label, FLinearColor* col, bool withAlpha = true)
    {
        if (!InWindow || !col) return false;
        int id = NextID();

        const float btn = 18.f;
        FVector2D rowPos{ ContentX(), Cursor.Y };
        FVector2D rowSz{ WinSize.X - 2.f * Style::Pad.X, Style::RowH };

        FVector2D btnPos{ ContentX(), CenterY(btn) };
        FVector2D btnSz{ btn, btn };

        const float labelX = btnPos.X + btn + 6.f;
        const float centerY = rowPos.Y + rowSz.Y * 0.5f;

        Post::TextL(label ? label : "", FVector2D{ labelX, centerY }, Style::Text);


        bool hovered = MouseInRect(btnPos, { btn,btn });
        if (hovered) Hot = id;
        if (hovered && MouseClicked) Active = id;

        if (Active == id && hovered && MouseReleased)
        {
            OpenColorId = (OpenColorId == id) ? -1 : id;

            if (OpenColorId == -1)
            {
                ColorPopup.posInit = false;
                ColorPopup.dragging = false;
            }
            else
            {
                ColorPopup.posInit = false;
            }
        }

        DrawCheckerboard(btnPos, { btn,btn });
        Post::RectFilled(btnPos, { btn,btn }, *col);
        Post::RectBorder(btnPos, { btn,btn }, 1.f, Style::Border);

        Cursor.Y += Style::RowH + Style::ItemSpacingY;

        if (OpenColorId != id)
            return false;

        const FVector2D svSz{ 180.f, 150.f };
        const FVector2D hueSz{ 14.f, svSz.Y };
        const float pad = 8.f;
        const float titleH = 20.f;

        FVector2D popupSz{
            svSz.X + hueSz.X + pad * 3.f,
            titleH + pad + svSz.Y + (withAlpha ? 28.f : 0.f) + pad
        };

        if (!ColorPopup.posInit)
        {
            ColorPopup.pos = FVector2D{
                WinPos.X + (WinSize.X - popupSz.X) * 0.5f,
                WinPos.Y + (WinSize.Y - popupSz.Y) * 0.5f
            };
            ColorPopup.posInit = true;
        }

        FVector2D popupPos = ColorPopup.pos;

        FVector2D barPos{ popupPos.X, popupPos.Y };
        FVector2D barSz{ popupSz.X, titleH };

        const float xBtn = titleH;
        FVector2D xPos{ popupPos.X + popupSz.X - xBtn, popupPos.Y };
        FVector2D xSz{ xBtn, xBtn };

        bool hoverX = MouseInRect(xPos, xSz);
        bool hoverBar = MouseInRect(barPos, barSz) && !hoverX;

        if (MouseClicked && !MouseInRect(popupPos, popupSz) && !hovered)
        {
            OpenColorId = -1;
            ColorPopup.posInit = false;
            ColorPopup.dragging = false;
            return false;
        }

        if (hoverX && MouseClicked)
        {
            OpenColorId = -1;
            ColorPopup.posInit = false;
            ColorPopup.dragging = false;
            return false;
        }

        if (hoverBar && MouseClicked)
        {
            ColorPopup.dragging = true;
            ColorPopup.dragOff = FVector2D{ Mouse.X - popupPos.X, Mouse.Y - popupPos.Y };
        }
        if (!MouseDown)
            ColorPopup.dragging = false;

        if (ColorPopup.dragging)
        {
            ColorPopup.pos = FVector2D{ Mouse.X - ColorPopup.dragOff.X, Mouse.Y - ColorPopup.dragOff.Y };
            popupPos = ColorPopup.pos;

            barPos = popupPos;
            xPos = FVector2D{ popupPos.X + popupSz.X - xBtn, popupPos.Y };
        }

        Post::RectFilled(popupPos, popupSz, Style::WindowBg);
        Post::RectBorder(popupPos, popupSz, Style::BorderT, Style::Border);

        Post::RectFilled(popupPos, FVector2D{ popupSz.X, titleH }, Style::TitleBgActive);
        Post::RectBorder(popupPos, FVector2D{ popupSz.X, titleH }, Style::BorderT, Style::Border);
        Post::TextL(label ? label : "", FVector2D{ popupPos.X + 6.f, popupPos.Y + titleH * 0.5f }, Style::Text);

        if (hoverX) Post::RectFilled(xPos, xSz, SRGB(255, 255, 255, 12));
        FVector2D xCenter{ xPos.X + xSz.X * 0.5f, xPos.Y + xSz.Y * 0.5f };
        DrawX(xCenter, 5.f, Style::Text);

        float h, s, v;
        RGBtoHSV(*col, h, s, v);

        FVector2D svPos{ popupPos.X + pad, popupPos.Y + titleH + pad };
        FVector2D huePos{ svPos.X + svSz.X + pad, svPos.Y };

        DrawSVSquare(svPos, svSz, h);
        DrawHueBar(huePos, hueSz);

        bool changed = false;

        if (MouseDown && MouseInRect(svPos, svSz))
        {
            s = Clamp01((Mouse.X - svPos.X) / svSz.X);
            v = Clamp01(1.f - (Mouse.Y - svPos.Y) / svSz.Y);
            changed = true;
        }

        if (MouseDown && MouseInRect(huePos, hueSz))
        {
            h = Clamp01((Mouse.Y - huePos.Y) / hueSz.Y);
            changed = true;
        }

        FVector2D p{
            svPos.X + s * svSz.X,
            svPos.Y + (1.f - v) * svSz.Y
        };
        Post::Line({ p.X - 5.f, p.Y }, { p.X + 5.f, p.Y }, 1.f, SRGB(0, 0, 0, 220));
        Post::Line({ p.X, p.Y - 5.f }, { p.X, p.Y + 5.f }, 1.f, SRGB(0, 0, 0, 220));

        float a = col->A;
        if (withAlpha)
        {
            FVector2D aPos{ svPos.X, svPos.Y + svSz.Y + 8.f };
            FVector2D aSz{ svSz.X + pad + hueSz.X, 12.f };

            DrawCheckerboard(aPos, aSz);
            Post::RectFilled(aPos, { aSz.X * a, aSz.Y }, Style::Accent);
            Post::RectBorder(aPos, aSz, 1.f, Style::Border);

            if (MouseDown && MouseInRect(aPos, aSz))
            {
                a = Clamp01((Mouse.X - aPos.X) / aSz.X);
                changed = true;
            }
        }

        if (changed)
            *col = HSVtoRGB(h, s, v, a);

        return changed;
    }

    static inline FVector2D LerpV2(const FVector2D& a, const FVector2D& b, float t)
    {
        t = Clamp01(t);
        return FVector2D{ a.X + (b.X - a.X) * t, a.Y + (b.Y - a.Y) * t };
    }

    static inline void DrawChevronMorph(const FVector2D& center, float s, float t, const FLinearColor& c)
    {
        t = EaseOutCubic(Clamp01(t));

        FVector2D r0a{ center.X - s * 0.2f, center.Y - s };
        FVector2D r0b{ center.X + s,        center.Y };
        FVector2D r1a{ center.X + s,        center.Y };
        FVector2D r1b{ center.X - s * 0.2f, center.Y + s };

        FVector2D d0a{ center.X - s, center.Y - s * 0.2f };
        FVector2D d0b{ center.X,     center.Y + s };
        FVector2D d1a{ center.X,     center.Y + s };
        FVector2D d1b{ center.X + s, center.Y - s * 0.2f };

        FVector2D a0 = LerpV2(r0a, d0a, t);
        FVector2D b0 = LerpV2(r0b, d0b, t);
        FVector2D a1 = LerpV2(r1a, d1a, t);
        FVector2D b1 = LerpV2(r1b, d1b, t);

        Post::Line(a0, b0, 1.f, c);
        Post::Line(a1, b1, 1.f, c);
    }


    static inline bool Combo(const char* label, int* currentIndex, const char** items, int itemsCount, float w = 140.f, int maxVisible = 6)
    {
        if (!InWindow || !currentIndex || !items || itemsCount <= 0) return false;

        const int id = NextID();

        Post::TextL(label ? label : "", FVector2D{ ContentX(), TextY() }, Style::Text);
        Cursor.Y += Style::TextLineH + 2.f;

        const float h = Style::FrameH;
        FVector2D framePos{ ContentX(), Cursor.Y };
        FVector2D frameSz{ w, h };

        bool hovered = MouseInRect(framePos, frameSz);
        if (hovered) Hot = id;
        if (hovered && MouseClicked) Active = id;

        bool changed = false;

        if (Active == id && hovered && MouseReleased)
            OpenComboId = (OpenComboId == id) ? -1 : id;

        const bool openTarget = (OpenComboId == id);

        auto& an = gComboAnim[id];

        an.openT = ExpApproach(an.openT, openTarget ? 1.f : 0.f, 18.f, gDT);

        an.chevronT = ExpApproach(an.chevronT, openTarget ? 1.f : 0.f, 22.f, gDT);

        const float tOpen = EaseOutCubic(an.openT);

        auto& wa = Anim(id);
        wa.hoverT = ExpApproach(wa.hoverT, hovered ? 1.f : 0.f, 18.f, gDT);
        wa.activeT = ExpApproach(wa.activeT, (Active == id && MouseDown) ? 1.f : 0.f, 28.f, gDT);

        FLinearColor frameCol = LerpColor(Style::FrameBg, Style::FrameHovered, EaseOutCubic(wa.hoverT));
        frameCol = LerpColor(frameCol, Style::FrameActive, EaseOutCubic(max(tOpen, wa.activeT)));

        float pop = 1.f + 0.02f * EaseOutBack(wa.activeT);
        FVector2D drawSz{ frameSz.X * pop, frameSz.Y * pop };
        FVector2D drawPos{ framePos.X + (frameSz.X - drawSz.X) * 0.5f, framePos.Y + (frameSz.Y - drawSz.Y) * 0.5f };

        Post::RectFilled(drawPos, drawSz, frameCol);
        Post::RectBorder(drawPos, drawSz, Style::BorderT, Style::Border);

        const char* cur = (*currentIndex >= 0 && *currentIndex < itemsCount) ? items[*currentIndex] : "";
        Post::TextL(cur ? cur : "", FVector2D{ framePos.X + 6.f, framePos.Y + h * 0.5f }, Style::Text);

        {
            const float cx = framePos.X + frameSz.X - 10.f;
            const float cy = framePos.Y + frameSz.Y * 0.5f - 1.f;
            FVector2D c{ cx, cy };

            DrawChevronMorph(c, 3.0f, an.chevronT, Style::TextDisabled);
        }


        Cursor.Y += h + Style::ItemSpacingY + 4.f;

        if (tOpen <= 0.001f)
            return false;

        const float itemH = Style::RowH;
        const int visible = (itemsCount < maxVisible) ? itemsCount : maxVisible;

        const float fullH = visible * itemH + 2.f;

        const float slide = (1.f - tOpen) * 6.f;

        FVector2D listPos{ framePos.X, framePos.Y + h + 2.f - slide };
        FVector2D listFullSz{ frameSz.X, fullH };

        FVector2D listSz{ frameSz.X, fullH * tOpen };
        const float alpha = tOpen;

        bool hoverList = MouseInRect(listPos, listFullSz);

        if (MouseClicked && !hovered && !hoverList)
            OpenComboId = -1;

        Post::RectFilled(FVector2D{ listPos.X + Style::ShadowOff.X, listPos.Y + Style::ShadowOff.Y }, listSz, MulAlpha(Style::Shadow, 0.8f * alpha));
        Post::RectFilled(listPos, listSz, MulAlpha(Style::WindowBg, alpha));
        Post::RectBorder(listPos, listSz, Style::BorderT, MulAlpha(Style::Border, alpha));

        const float visH = max(0.f, listSz.Y - 2.f);

        const float stagger = 0.07f;
        const float revealSoft = 0.35f;

        for (int i = 0; i < visible; i++)
        {
            const int idx = i;

            FVector2D itemPos{ listPos.X + 1.f, listPos.Y + 1.f + i * itemH };
            FVector2D itemSz{ frameSz.X - 2.f, itemH };

            const float rowTop = i * itemH;
            const float rowBottom = (i + 1) * itemH;
            if (rowTop >= visH) break;

            float rowVisH = Clamp01((visH - rowTop) / itemH);

            float tRowStart = i * stagger;
            float tRow = (tOpen - tRowStart) / max(0.0001f, revealSoft);
            tRow = EaseOutCubic(Clamp01(tRow));

            const float aRow = alpha * rowVisH * tRow;
            if (aRow <= 0.001f) continue;

            const bool rowClickable = (rowVisH > 0.95f);
            const bool itemHover = rowClickable && MouseInRect(itemPos, itemSz);

            if (itemHover)
                Post::RectFilled(itemPos, itemSz, MulAlpha(Style::FrameHovered, aRow));

            bool selected = (*currentIndex == idx);
            if (selected)
                Post::RectFilled(itemPos, itemSz, MulAlpha(Style::FrameActive, aRow));

            FVector2D textPos{
                itemPos.X + 6.f + (1.f - tRow) * 6.f,
                itemPos.Y + itemH * 0.5f
            };

            Post::TextL(items[idx] ? items[idx] : "", textPos, MulAlpha(Style::Text, aRow));

            if (itemHover && MouseClicked)
            {
                *currentIndex = idx;
                changed = true;
                OpenComboId = -1;
            }
        }

        Cursor.Y += (fullH * tOpen) + Style::ItemSpacingY;

        return changed;
    }

    static inline void BeginTabs()
    {
        if (!InWindow) return;

        TabsActiveRow = true;
        TabsX = ContentX();
        TabsY = Cursor.Y;
    }

    static inline bool TabItem(const char* label, int index, int* currentIndex)
    {
        if (!InWindow || !TabsActiveRow || !currentIndex) return false;

        const int id = NextID();
        auto& an = Anim(id);

        const float h = Style::RowH;
        const float padX = 10.f;
        const float w = EstimateTextW(label) + padX * 2.f;

        FVector2D pos{ TabsX, TabsY };
        FVector2D sz{ w, h };

        bool hovered = MouseInRect(pos, sz);
        if (hovered) Hot = id;
        if (hovered && MouseClicked) Active = id;

        bool pressed = (Active == id && hovered && MouseReleased);
        if (pressed) *currentIndex = index;

        bool isActive = (*currentIndex == index);

        an.hoverT = Smooth01(an.hoverT, hovered ? 1.f : 0.f, 16.f);
        an.openT = Smooth01(an.openT, isActive ? 1.f : 0.f, 16.f);

        FLinearColor bg = LerpColor(Style::Tab, Style::TabHovered, EaseOutCubic(an.hoverT));
        bg = LerpColor(bg, Style::TabActive, EaseOutCubic(an.openT));

        float lift = -2.f * EaseOutCubic(an.openT);
        FVector2D dpos{ pos.X, pos.Y + lift };

        Post::RectFilled(dpos, sz, bg);
        Post::RectBorder(dpos, sz, Style::BorderT, Style::Border);

        Post::TextC(label ? label : "", FVector2D{ pos.X + sz.X * 0.5f, pos.Y + sz.Y * 0.5f + lift }, Style::Text);

        TabsX += w + 4.f;
        return pressed;
    }

    static inline void EndTabs()
    {
        if (!InWindow || !TabsActiveRow) return;

        TabsActiveRow = false;

        Cursor.Y = TabsY + Style::RowH + Style::ItemSpacingY;

        FVector2D a{ WinPos.X + Style::Pad.X, Cursor.Y };
        FVector2D b{ WinPos.X + WinSize.X - Style::Pad.X, Cursor.Y };
        Post::Line(a, b, 1.f, Style::Separator);

        Cursor.Y += Style::ItemSpacingY;
    }

    struct SearchBarState
    {
        char  buf[128]{};
        int   len = 0; 
        bool  focused = false;
        float focusT = 0.f;
        float cursorBlink = 0.f;

        int   heldKey = 0;
        float heldTimer = 0.f;
        float heldRepeat = 0.f;
    };

    static std::unordered_map<int, SearchBarState> gSearchBar;

    static inline bool SearchBar(const char* label, char* buf, int bufSize, float w = 220.f)
    {
        if (!InWindow || !buf || bufSize <= 1) return false;

        const int id = NextID();
        SearchBarState& s = gSearchBar[id];
        auto& an = Anim(id);

        if (label && label[0] && !(label[0] == '#' && label[1] == '#'))
        {
            Post::TextL(label, FVector2D{ ContentX(), TextY() }, Style::Text);
            Cursor.Y += Style::TextLineH + 2.f;
        }

        const float h = Style::FrameH;
        FVector2D pos{ ContentX(), Cursor.Y };
        FVector2D sz{ w, h };

        bool hovered = MouseInRect(pos, sz);
        if (hovered) Hot = id;
        if (hovered && MouseClicked)
        {
            s.focused = true;
            Active = id;
        }
        else if (MouseClicked && !hovered)
        {
            s.focused = false;
        }

        bool changed = false;
        if (s.focused)
        {

            auto TryAppend = [&](char c)
                {
                    if (s.len < bufSize - 1)
                    {
                        s.buf[s.len++] = c;
                        s.buf[s.len] = '\0';
                        changed = true;
                    }
                };

            auto TryBackspace = [&]()
                {
                    if (s.len > 0)
                    {
                        s.buf[--s.len] = '\0';
                        changed = true;
                    }
                };

            bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

            struct VKMap { int vk; char lo; char hi; };
            static const VKMap kmap[] = {
                {'A','a','A'},{'B','b','B'},{'C','c','C'},{'D','d','D'},
                {'E','e','E'},{'F','f','F'},{'G','g','G'},{'H','h','H'},
                {'I','i','I'},{'J','j','J'},{'K','k','K'},{'L','l','L'},
                {'M','m','M'},{'N','n','N'},{'O','o','O'},{'P','p','P'},
                {'Q','q','Q'},{'R','r','R'},{'S','s','S'},{'T','t','T'},
                {'U','u','U'},{'V','v','V'},{'W','w','W'},{'X','x','X'},
                {'Y','y','Y'},{'Z','z','Z'},
                {'0','0',')'},{'1','1','!'},{'2','2','@'},{'3','3','#'},
                {'4','4','$'},{'5','5','%'},{'6','6','^'},{'7','7','&'},
                {'8','8','*'},{'9','9','('},
                {VK_SPACE,' ',' '},{VK_OEM_MINUS,'-','_'},
                {VK_OEM_PERIOD,'.','>'},{VK_OEM_COMMA,',','<'},
            };

            const float kInitDelay = 0.40f;
            const float kRepeatRate = 0.05f;
            s.cursorBlink += gDT;

            {
                bool bsDown = (GetAsyncKeyState(VK_BACK) & 0x8000) != 0;
                if (bsDown)
                {
                    if (s.heldKey != VK_BACK)
                    {
                        s.heldKey = VK_BACK;
                        s.heldTimer = 0.f;
                        s.heldRepeat = 0.f;
                        TryBackspace();
                    }
                    else
                    {
                        s.heldTimer += gDT;
                        if (s.heldTimer >= kInitDelay)
                        {
                            s.heldRepeat += gDT;
                            while (s.heldRepeat >= kRepeatRate)
                            {
                                TryBackspace();
                                s.heldRepeat -= kRepeatRate;
                            }
                        }
                    }
                }
                else if (s.heldKey == VK_BACK)
                {
                    s.heldKey = 0;
                }
            }

            for (auto& km : kmap)
            {
                bool down = (GetAsyncKeyState(km.vk) & 0x8000) != 0;
                if (down)
                {
                    char c = shift ? km.hi : km.lo;
                    if (s.heldKey != km.vk)
                    {
                        s.heldKey = km.vk;
                        s.heldTimer = 0.f;
                        s.heldRepeat = 0.f;
                        TryAppend(c);
                    }
                    else if (s.heldKey == km.vk)
                    {
                        s.heldTimer += gDT;
                        if (s.heldTimer >= kInitDelay)
                        {
                            s.heldRepeat += gDT;
                            while (s.heldRepeat >= kRepeatRate)
                            {
                                TryAppend(c);
                                s.heldRepeat -= kRepeatRate;
                            }
                        }
                    }
                    break;
                }
                else if (s.heldKey == km.vk)
                {
                    s.heldKey = 0;
                }
            }

            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            {
                if (s.len > 0) { s.len = 0; s.buf[0] = '\0'; changed = true; }
                else s.focused = false;
            }
        }

        if (changed)
            std::snprintf(buf, bufSize, "%s", s.buf);

        an.hoverT = ExpApproach(an.hoverT, hovered ? 1.f : 0.f, 18.f, gDT);
        an.activeT = ExpApproach(an.activeT, s.focused ? 1.f : 0.f, 22.f, gDT);
        s.focusT = an.activeT;

        FLinearColor frame = LerpColor(Style::FrameBg, Style::FrameHovered, EaseOutCubic(an.hoverT));
        frame = LerpColor(frame, Style::FrameActive, EaseOutCubic(an.activeT));

        Post::RectFilled(pos, sz, frame);
        Post::RectBorder(pos, sz, Style::BorderT,
            LerpColor(Style::Border, Style::Accent, EaseOutCubic(an.activeT)));

        {
            const float cx = pos.X + 10.f;
            const float cy = pos.Y + h * 0.5f;
            const float r = 4.f;
            const FLinearColor iconCol = LerpColor(Style::TextDisabled, Style::Text, EaseOutCubic(an.activeT));

            const int segs = 8;
            const float pi2 = 6.28318f;
            for (int i = 0; i < segs; i++)
            {
                float a0 = (float)i / segs * pi2;
                float a1 = (float)(i + 1) / segs * pi2;
                Post::Line(
                    FVector2D{ cx + std::cos(a0) * r, cy + std::sin(a0) * r },
                    FVector2D{ cx + std::cos(a1) * r, cy + std::sin(a1) * r },
                    1.f, iconCol);
            }
            Post::Line(
                FVector2D{ cx + r * 0.7f, cy + r * 0.7f },
                FVector2D{ cx + r * 1.8f, cy + r * 1.8f },
                1.5f, iconCol);
        }

        const float textX = pos.X + 20.f;
        const float textY2 = pos.Y + h * 0.5f;

        if (s.len > 0)
        {
            Post::TextL(s.buf, FVector2D{ textX, textY2 }, Style::Text);
        }
        else if (!s.focused)
        {
            const char* ph = "Search...";
            if (label && label[0] && !(label[0] == '#' && label[1] == '#'))
                ph = label;
            Post::TextL(ph, FVector2D{ textX, textY2 }, Style::TextDisabled);
        }

        if (s.focused)
        {
            float cursorX = textX + (float)s.len * 7.0f;
            float blinkAlpha = (std::sin(s.cursorBlink * 5.5f) > 0.f) ? 1.f : 0.f;
            if (blinkAlpha > 0.5f)
                Post::RectFilled(
                    FVector2D{ cursorX + 1.f, pos.Y + 3.f },
                    FVector2D{ 1.5f, h - 6.f },
                    MulAlpha(Style::Text, 0.85f));
        }

        Cursor.Y += h + Style::ItemSpacingY;
        return changed;
    }

    static inline bool SearchMatches(const char* item, const char* query)
    {
        if (!query || query[0] == '\0') return true;
        if (!item)                      return false;

        for (const char* p = item; *p; ++p)
        {
            const char* i = p;
            const char* q = query;
            while (*i && *q && (std::tolower((unsigned char)*i) == std::tolower((unsigned char)*q)))
            {
                ++i; ++q;
            }
            if (*q == '\0') return true;
        }
        return false;
    }

    static inline bool SliderFloat(const char* label, float* v, float v_min, float v_max, float w = 220.f)
    {
        if (!InWindow || !v) return false;

        const int id = NextID();
        auto& an = Anim(id);

        Post::TextL(label ? label : "", FVector2D{ ContentX(), TextY() }, Style::Text);
        Cursor.Y += Style::TextLineH + 2.f;

        FVector2D pos{ ContentX(), Cursor.Y };
        FVector2D sz{ w, 14.f };

        bool hovered = MouseInRect(pos, sz);
        if (hovered) Hot = id;
        if (hovered && MouseClicked) Active = id;

        bool changed = false;
        if (Active == id && MouseDown)
        {
            float t = (Mouse.X - pos.X) / sz.X;
            t = Clamp01(t);

            float nv = v_min + (v_max - v_min) * t;
            if (nv != *v) { *v = nv; changed = true; }
        }

        an.hoverT = Smooth01(an.hoverT, hovered ? 1.f : 0.f, 16.f);
        an.activeT = Smooth01(an.activeT, (Active == id && MouseDown) ? 1.f : 0.f, 24.f);

        static std::unordered_map<int, float> gSliderVis;
        float& vis = gSliderVis[id];
        if (vis == 0.f && *v != 0.f) vis = *v;
        vis = ExpApproach(vis, *v, 20.f, gDT);

        float tFill = (vis - v_min) / (v_max - v_min);
        tFill = Clamp01(tFill);

        FLinearColor frame = LerpColor(Style::FrameBg, Style::FrameHovered, EaseOutCubic(an.hoverT));
        frame = LerpColor(frame, Style::FrameActive, EaseOutCubic(an.activeT));

        Post::RectFilled(pos, sz, frame);
        Post::RectBorder(pos, sz, Style::BorderT, Style::Border);

        Post::RectFilled(pos, FVector2D{ sz.X * tFill, sz.Y }, Style::Accent);

        const float grabW = 6.f;
        float grabX = pos.X + (sz.X * tFill) - grabW * 0.5f;
        grabX = std::clamp(grabX, pos.X, pos.X + sz.X - grabW);

        float grabAlpha = 0.5f + 0.5f * EaseOutCubic(an.activeT);
        Post::RectFilled(FVector2D{ grabX, pos.Y }, FVector2D{ grabW, sz.Y }, SRGB(0, 0, 0, (int)(120.f * grabAlpha)));

        char buf[64]{};
        std::snprintf(buf, sizeof(buf), "%.3f", (double)*v);

        Post::TextL(buf, FVector2D{ pos.X + sz.X + 8.f, pos.Y + sz.Y * 0.5f }, Style::TextDisabled);

        Cursor.Y += sz.Y + Style::ItemSpacingY + 4.f;
        return changed;
    }

    static float (*gFPSCallback)() = nullptr;

    static inline void SetFPSCallback(float (*cb)()) { gFPSCallback = cb; }

    static inline void Watermark(const char* text)
    {
        if (!Canvas || !Font) return;

        char full[160]{};

        if (gFPSCallback)
            std::snprintf(full, sizeof(full), "%s | %d FPS", text ? text : "", (int)(gFPSCallback() + 0.5f));
        else
            std::snprintf(full, sizeof(full), "%s", text ? text : "");

        const FVector2D pos{ 10.f, 10.f };
        const float h = 18.f;
        const float padX = 8.f;
        const float totalW = 240.f;

        Post::RectFilled(pos, FVector2D{ totalW, h }, SRGB(20, 20, 20, 200));
        Post::RectBorder(pos, FVector2D{ totalW, h }, 1.f, Style::Border);
        Post::TextL(full, FVector2D{ pos.X + padX, pos.Y + h * 0.5f }, Style::Text);
    }

    static inline void Render()
    {
        if (!Canvas) return;

        for (int i = 0; i < Post::n; i++)
        {
            const auto& c = Post::q[i];

            switch (c.type)
            {
            case Post::FILLED_RECT: DrawFilledRect(c.pos, c.size, c.color); break;
            case Post::LINE:        DrawLine(c.a, c.b, c.thickness, c.color); break;
            case Post::TEXT_L:      DrawText_Impl(c.text, c.pos, c.color, false); break;
            case Post::TEXT_C:      DrawText_Impl(c.text, c.pos, c.color, true);  break;
            default: break;
            }

        }

        if (MouseReleased)
            Active = -1;

        Post::Reset();
    }
}