#pragma once

#include <window.hpp>

#include <Windows.h>

#include <deque>
#include <optional>

namespace rr {

class WindowsWindow : public Window {
public:
    WindowsWindow(const WindowOptions& options);

    std::optional<ev::Event> poll() const override;
    WindowSize size() const override;

    HINSTANCE hinstance() const;
    HWND hwnd() const;

private:
    static LRESULT CALLBACK windowProc(
        HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    std::optional<ev::Event> createEvent(
        UINT msg, WPARAM wparam, LPARAM lparam) const;

    HINSTANCE _hinstance = NULL;
    HWND _window = NULL;
    mutable std::deque<ev::Event> _events;
};

} // namespace rr