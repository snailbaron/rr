#pragma once

#include <window.hpp>

#include <xcb/xcb.h>

#include <memory>

namespace rr {

class XcbConnection {
public:
    XcbConnection() = default;
    XcbConnection(const char* displayName);

    void connect(const char* displayName);
    void disconnect();

    xcb_connection_t* ptr() const;
    int preferredScreen() const;

private:
    std::unique_ptr<xcb_connection_t, void(*)(xcb_connection_t*)> _ptr
        {nullptr, xcb_disconnect};
    int _preferredScreen = 0;
};

class XcbWindow : public Window {
public:
    XcbWindow(const WindowOptions& options);

    xcb_connection_t* connection() const;
    xcb_window_t window() const;

    WindowSize size() const;

    virtual std::optional<ev::Event> poll() const override;

private:
    XcbConnection _connection;
    const xcb_screen_t* _screen = nullptr;
    xcb_window_t _window {};
};

} // namespace rr
