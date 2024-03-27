#include "windows_window.hpp"

#include <error.hpp>

#include <Windows.h>

namespace rr {

namespace {

[[noreturn]] void throwWindowsError()
{
    DWORD error = GetLastError();

    LPTSTR messageBuffer = nullptr;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        messageBuffer,
        0,
        NULL);

    std::string message{ messageBuffer };

    LocalFree(messageBuffer);

    throw Error{} << message;
}

} // namespace

WindowsWindow::WindowsWindow(const WindowOptions& options)
{
    static constexpr const char windowClassName[] = "WeeWee Window Class";

    _hinstance = (HINSTANCE)GetModuleHandle(NULL);

    auto windowClass = WNDCLASSEX{
        .cbSize = sizeof(WNDCLASSEX),
        .style = 0,
        .lpfnWndProc = windowProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = _hinstance,
        .hIcon = NULL,
        .hCursor = NULL,
        .hbrBackground = NULL,
        .lpszMenuName = NULL,
        .lpszClassName = "WeeWee Window Class",
        .hIconSm = NULL,
    };

    if (RegisterClassEx(&windowClass) == 0) {
        throwWindowsError();
    }

    _window = CreateWindowEx(
        0,
        windowClassName,
        "weewee",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        options.x,
        options.y,
        options.w,
        options.h,
        NULL,
        NULL,
        _hinstance,
        this);
    if (_window == NULL) {
        throwWindowsError();
    }
}

std::optional<ev::Event> WindowsWindow::poll() const
{
    for (auto msg = MSG{}; GetMessage(&msg, NULL, 0, 0) > 0; ) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (!_events.empty()) {
        auto event = _events.front();
        _events.pop_front();
        return event;
    }
    return std::nullopt;
}

WindowSize WindowsWindow::size() const
{
    RECT rect;
    if (GetClientRect(_window, &rect) == 0) {
        throw Error{} << "cannot get window rect";
    }

    return WindowSize{
        .width = rect.right - rect.left,
        .height = rect.bottom - rect.top,
    };
}

HINSTANCE WindowsWindow::hinstance() const
{
    return _hinstance;
}

HWND WindowsWindow::hwnd() const
{
    return _window;
}

LRESULT CALLBACK WindowsWindow::windowProc(
    HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    WindowsWindow* window = nullptr;

    if (msg == WM_NCCREATE) {
        CREATESTRUCT* createStruct = (CREATESTRUCT*)lparam;
        window = (WindowsWindow*)createStruct->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
    } else {
        window = (WindowsWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (window) {
        auto event = window->createEvent(msg, wparam, lparam);
        if (event) {
            window->_events.push_back(std::move(*event));
        }
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

std::optional<ev::Event> WindowsWindow::createEvent(
    UINT msg,
    [[maybe_unused]] WPARAM wparam,
    [[maybe_unused]] LPARAM lparam) const
{
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return std::nullopt;

        case WM_LBUTTONDOWN:
            return ev::Event{ev::Button{
                .button = ButtonCode::LKM,
                .press = true,
            }};

        case WM_LBUTTONUP:
            return ev::Event{ev::Button{
                .button = ButtonCode::LKM,
                .press = false,
            }};
    }

    return std::nullopt;
}

} // namespace rr