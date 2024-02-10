#pragma once

#include <cstdint>
#include <ostream>
#include <utility>

namespace rr {

enum class ButtonCode : uint8_t {
    LKM = 1,
    Wheel = 2,
    RKM = 3,
    WheelUp = 4,
    WheelDown = 5,
};

inline std::ostream& operator<<(std::ostream& output, ButtonCode button)
{
    switch (button) {
        case ButtonCode::LKM: return output << "LKM";
        case ButtonCode::Wheel: return output << "Wheel";
        case ButtonCode::RKM: return output << "RKM";
        case ButtonCode::WheelUp: return output << "WheelUp";
        case ButtonCode::WheelDown: return output << "WheelDown";
    }
    return output << "Button" << static_cast<int>(button);
}

enum class ModButtonState : uint16_t {
    None = 0,
    Shift = 1 << 0,
    CapsLock = 1 << 1,
    Ctrl = 1 << 2,
    Alt = 1 << 3,
    NumLock = 1 << 4,
    Mod3 = 1 << 5,
    Mod4 = 1 << 6,
    Mod5 = 1 << 7,
    LMB = 1 << 8,
    Button2 = 1 << 9,
    Button3 = 1 << 10,
    Button4 = 1 << 11,
    Button5 = 1 << 12,
    Button6 = 1 << 13,
    Button7 = 1 << 14,
    Button8 = 1 << 15,
};

inline std::ostream& operator<<(std::ostream& output, ModButtonState mod)
{
    static const char* names[] {
        "Shift", "CapsLock", "Ctrl", "Alt", "NumLock", "Mod3", "Mod4", "Mod5",
        "Button1", "Button2", "Button3", "Button4", "Button5", "Button6",
        "Button7", "Button8",
    };

    bool started = false;
    const char** p = names;
    auto mask = std::to_underlying(mod);
    while (mask > 0) {
        if (mask & 1) {
            if (started) {
                output << "|";
            } else {
                started = true;
            }
            output << *p;
        }

        p++;
        mask >>= 1;
    }

    return output;
}

enum class KeyCode : uint8_t {
    A = 38,
    B = 56,
    C = 54,
    D = 40,
    E = 26,
    F = 41,
    G = 42,
    H = 43,
    I = 31,
    J = 44,
    K = 45,
    L = 46,
    M = 58,
    N = 57,
    O = 32,
    P = 33,
    Q = 24,
    R = 27,
    S = 39,
    T = 28,
    U = 30,
    V = 55,
    W = 25,
    X = 53,
    Y = 29,
    Z = 52,

    Digit1 = 10,
    Digit2 = 11,
    Digit3 = 12,
    Digit4 = 13,
    Digit5 = 14,
    Digit6 = 15,
    Digit7 = 16,
    Digit8 = 17,
    Digit9 = 18,
    Digit0 = 19,

    F1 = 67,
    F2 = 68,
    F3 = 69,
    F4 = 70,
    F5 = 71,
    F6 = 72,
    F7 = 73,
    F8 = 74,
    F9 = 75,
    F10 = 76,
    F11 = 95,
    F12 = 96,

    Minus = 20,
    Plus = 21,
    Escape = 9,
    Backspace = 22,
    //PrintScreen = 0,
    ScrollLock = 78,
    Pause = 127,
    Tilde = 49,
    Tab = 23,
    LeftBracket = 34,
    RightBracket = 35,
    Backslash = 51,
    CapsLock = 66,
    Semicolon = 47,
    SingleQuote = 48,
    Enter = 36,
    LeftShift = 50,
    LeftAngle = 59,
    RightAngle = 60,
    Slash = 61,
    RightShift = 62,
    LeftCtrl = 37,
    RightCtrl = 105,
    LeftAlt = 64,
    Space = 65,
    RightAlt = 108,

    Insert = 118,
    Delete = 119,
    Home = 110,
    End = 115,
    PageUp = 112,
    PageDown = 117,

    ArrowUp = 111,
    ArrowDown = 116,
    ArrowLeft = 113,
    ArrowRight = 114,

    NumLock = 77,
    NumSlash = 106,
    NumStar = 63,
    NumMinus = 82,
    NumPlus = 86,
    NumEnter = 104,
    NumDot = 91,

    Num0 = 90,
    Num1 = 87,
    Num2 = 88,
    Num3 = 89,
    Num4 = 83,
    Num5 = 84,
    Num6 = 85,
    Num7 = 79,
    Num8 = 80,
    Num9 = 81,
};

inline std::ostream& operator<<(std::ostream& output, KeyCode keyCode)
{
    switch (keyCode) {
        case KeyCode::A: return output << "A";
        case KeyCode::B: return output << "B";
        case KeyCode::C: return output << "C";
        case KeyCode::D: return output << "D";
        case KeyCode::E: return output << "E";
        case KeyCode::F: return output << "F";
        case KeyCode::G: return output << "G";
        case KeyCode::H: return output << "H";
        case KeyCode::I: return output << "I";
        case KeyCode::J: return output << "J";
        case KeyCode::K: return output << "K";
        case KeyCode::L: return output << "L";
        case KeyCode::M: return output << "M";
        case KeyCode::N: return output << "N";
        case KeyCode::O: return output << "O";
        case KeyCode::P: return output << "P";
        case KeyCode::Q: return output << "Q";
        case KeyCode::R: return output << "R";
        case KeyCode::S: return output << "S";
        case KeyCode::T: return output << "T";
        case KeyCode::U: return output << "U";
        case KeyCode::V: return output << "V";
        case KeyCode::W: return output << "W";
        case KeyCode::X: return output << "X";
        case KeyCode::Y: return output << "Y";
        case KeyCode::Z: return output << "Z";

        case KeyCode::Digit1: return output << "Digit1";
        case KeyCode::Digit2: return output << "Digit2";
        case KeyCode::Digit3: return output << "Digit3";
        case KeyCode::Digit4: return output << "Digit4";
        case KeyCode::Digit5: return output << "Digit5";
        case KeyCode::Digit6: return output << "Digit6";
        case KeyCode::Digit7: return output << "Digit7";
        case KeyCode::Digit8: return output << "Digit8";
        case KeyCode::Digit9: return output << "Digit9";
        case KeyCode::Digit0: return output << "Digit0";

        case KeyCode::F1: return output << "F1";
        case KeyCode::F2: return output << "F2";
        case KeyCode::F3: return output << "F3";
        case KeyCode::F4: return output << "F4";
        case KeyCode::F5: return output << "F5";
        case KeyCode::F6: return output << "F6";
        case KeyCode::F7: return output << "F7";
        case KeyCode::F8: return output << "F8";
        case KeyCode::F9: return output << "F9";
        case KeyCode::F10: return output << "F10";
        case KeyCode::F11: return output << "F11";
        case KeyCode::F12: return output << "F12";

        case KeyCode::Minus: return output << "Minus";
        case KeyCode::Plus: return output << "Plus";
        case KeyCode::Backspace: return output << "Backspace";
        case KeyCode::Escape: return output << "Escape";
        //PrintScreen
        case KeyCode::ScrollLock: return output << "ScrollLock";
        case KeyCode::Pause: return output << "Pause";
        case KeyCode::Tilde: return output << "Tilde";
        case KeyCode::Tab: return output << "Tab";
        case KeyCode::LeftBracket: return output << "LeftBracket";
        case KeyCode::RightBracket: return output << "RightBracket";
        case KeyCode::Backslash: return output << "Backslash";
        case KeyCode::CapsLock: return output << "CapsLock";
        case KeyCode::Semicolon: return output << "Semicolon";
        case KeyCode::SingleQuote: return output << "SingleQuote";
        case KeyCode::Enter: return output << "Enter";
        case KeyCode::LeftShift: return output << "LeftShift";
        case KeyCode::LeftAngle: return output << "LeftAngle";
        case KeyCode::RightAngle: return output << "RightAngle";
        case KeyCode::Slash: return output << "Slash";
        case KeyCode::RightShift: return output << "RightShift";
        case KeyCode::LeftCtrl: return output << "LeftCtrl";
        case KeyCode::RightCtrl: return output << "RightCtrl";
        case KeyCode::LeftAlt: return output << "LeftAlt";
        case KeyCode::Space: return output << "Space";
        case KeyCode::RightAlt: return output << "RightAlt";

        case KeyCode::Insert: return output << "Insert";
        case KeyCode::Delete: return output << "Delete";
        case KeyCode::Home: return output << "Home";
        case KeyCode::End: return output << "End";
        case KeyCode::PageUp: return output << "PageUp";
        case KeyCode::PageDown: return output << "PageDown";

        case KeyCode::ArrowUp: return output << "ArrowUp";
        case KeyCode::ArrowDown: return output << "ArrowDown";
        case KeyCode::ArrowLeft: return output << "ArrowLeft";
        case KeyCode::ArrowRight: return output << "ArrowRight";

        case KeyCode::NumLock: return output << "NumLock";
        case KeyCode::NumSlash: return output << "NumSlash";
        case KeyCode::NumStar: return output << "NumStar";
        case KeyCode::NumMinus: return output << "NumMinus";
        case KeyCode::NumPlus: return output << "NumPlus";
        case KeyCode::NumEnter: return output << "NumEnter";
        case KeyCode::NumDot: return output << "NumDot";

        case KeyCode::Num0: return output << "Num0";
        case KeyCode::Num1: return output << "Num1";
        case KeyCode::Num2: return output << "Num2";
        case KeyCode::Num3: return output << "Num3";
        case KeyCode::Num4: return output << "Num4";
        case KeyCode::Num5: return output << "Num5";
        case KeyCode::Num6: return output << "Num6";
        case KeyCode::Num7: return output << "Num7";
        case KeyCode::Num8: return output << "Num8";
        case KeyCode::Num9: return output << "Num9";
    }
    return output << "KeyCode(" << static_cast<int>(keyCode) << ")";
}

} // namespace rr
