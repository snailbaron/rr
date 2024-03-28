#include "xcb_window.hpp"

#include <error.hpp>

#include <cstring>

#define LEN_AND_STRING(STR) std::strlen(STR), STR

namespace rr {

XcbConnection::XcbConnection(const char* displayName)
{
    connect(displayName);
}

void XcbConnection::connect(const char* displayName)
{
    _ptr.reset(xcb_connect(displayName, &_preferredScreen));
    if (int err = xcb_connection_has_error(_ptr.get())) {
        switch (err) {
            case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:
                throw Error{} << "XCB_CONN_CLOSED_EXT_NOTSUPPORTED: " <<
                    "extension not supported";
            case XCB_CONN_CLOSED_MEM_INSUFFICIENT:
                throw Error{} << "XCB_CONN_CLOSED_MEM_INSUFFICIENT: " <<
                    "memory not available";
            case XCB_CONN_CLOSED_REQ_LEN_EXCEED:
                throw Error{} << "XCB_CONN_CLOSED_REQ_LEN_EXCEED: " <<
                    "exceeding request length that server accepts";
            case XCB_CONN_CLOSED_PARSE_ERR:
                throw Error{} << "XCB_CONN_CLOSED_PARSE_ERR: " <<
                    "error during parsing display string";
            case XCB_CONN_CLOSED_INVALID_SCREEN:
                throw Error{} << "XCB_CONN_CLOSED_INVALID_SCREEN: " <<
                    "the server does not have a screen matching the display";
            default:
                throw Error{} << "xcb_connect failed with error " << err;
        }
    }
}

void XcbConnection::disconnect()
{
    _ptr.reset();
    _preferredScreen = 0;
}

xcb_connection_t* XcbConnection::ptr() const
{
    return _ptr.get();
}

int XcbConnection::preferredScreen() const
{
    return _preferredScreen;
}

XcbWindow::XcbWindow(const WindowOptions& options)
{
    _connection.connect(options.displayName);

    const xcb_setup_t* const setup = xcb_get_setup(_connection.ptr());
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    for (int i = 0; i < _connection.preferredScreen(); i++) {
        xcb_screen_next(&it);
    }
    _screen = it.data;

    _window = xcb_generate_id(_connection.ptr());
    static constexpr uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    const uint32_t values[] {
        _screen->black_pixel,

        XCB_EVENT_MASK_EXPOSURE |
        XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_BUTTON_RELEASE |
        XCB_EVENT_MASK_POINTER_MOTION |
        XCB_EVENT_MASK_BUTTON_MOTION |
        XCB_EVENT_MASK_ENTER_WINDOW |
        XCB_EVENT_MASK_LEAVE_WINDOW |
        XCB_EVENT_MASK_KEY_PRESS |
        XCB_EVENT_MASK_KEY_RELEASE
    };


    [[maybe_unused]] xcb_void_cookie_t cookie = xcb_create_window(
        _connection.ptr(),
        XCB_COPY_FROM_PARENT, // depth
        _window,
        _screen->root, // parent window
        (int16_t)options.x,
        (int16_t)options.y,
        (uint16_t)options.w,
        (uint16_t)options.h,
        (uint16_t)options.borderWidth,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        _screen->root_visual,
        mask,
        values);

    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom(
        _connection.ptr(), 1 /*only_if_exists*/, LEN_AND_STRING("ATOM"));
    xcb_intern_atom_cookie_t protocolsCookie = xcb_intern_atom(
        _connection.ptr(), 1 /*only_if_exists*/, LEN_AND_STRING("WM_PROTOCOLS"));
    xcb_intern_atom_cookie_t deleteWindowCookie = xcb_intern_atom(
        _connection.ptr(), 1 /*only_if_exists*/, LEN_AND_STRING("WM_DELETE_WINDOW"));

    xcb_intern_atom_reply_t* atom = xcb_intern_atom_reply(
        _connection.ptr(), atomCookie, nullptr);
    xcb_intern_atom_reply_t* deleteWindow = xcb_intern_atom_reply(
        _connection.ptr(), deleteWindowCookie, nullptr /*error*/);
    xcb_intern_atom_reply_t* protocols = xcb_intern_atom_reply(
        _connection.ptr(), protocolsCookie, nullptr /*error*/);

    xcb_change_property(
        _connection.ptr(),
        XCB_PROP_MODE_REPLACE,
        _window,
        protocols->atom,
        atom->atom,
        32, // format for client messages
        1,
        &deleteWindow->atom);

    xcb_map_window(_connection.ptr(), _window);
    xcb_flush(_connection.ptr());
}

WindowSize XcbWindow::size() const
{
    auto cookie = xcb_get_geometry(_connection.ptr(), _window);
    auto* reply = xcb_get_geometry_reply(_connection.ptr(), cookie, nullptr);
    return {.width = reply->width, .height = reply->height};
}

std::optional<ev::Event> XcbWindow::poll() const
{
    xcb_generic_event_t* e = xcb_poll_for_event(_connection.ptr());
    if (e == nullptr) {
        return std::nullopt;
    }

    const uint8_t eventType = e->response_type & ~0x80;
    switch (eventType) {
        case XCB_BUTTON_PRESS:
        case XCB_BUTTON_RELEASE:
        {
            auto* bp = reinterpret_cast<xcb_button_press_event_t*>(e);
            return ev::Event{ev::Button{
                .button = ButtonCode{bp->detail},
                .press = (eventType == XCB_BUTTON_PRESS),
            }};
        }
        case XCB_KEY_PRESS:
        case XCB_KEY_RELEASE:
        {
            auto* kp = reinterpret_cast<xcb_key_press_event_t*>(e);
            return ev::Event{ev::Key{
                .keyCode = KeyCode{kp->detail},
                .mod = ModButtonState{kp->state},
                .press = (eventType == XCB_KEY_PRESS),
            }};
        }
        case XCB_CLIENT_MESSAGE:
        {
            [[maybe_unused]] auto* cm =
                reinterpret_cast<xcb_client_message_event_t*>(e);
            return ev::Event{ev::CloseWindow{}};
        }
    }

    return std::nullopt;
}

vk::raii::SurfaceKHR XcbWindow::createVulkanSurface(
    const vk::raii::Instance& instance) const
{
    auto xcbSurfaceCreateInfo = vk::XcbSurfaceCreateInfoKHR{
        .pNext = nullptr,
        .flags = vk::XcbSurfaceCreateFlagsKHR{},
        .connection = _connection.ptr(),
        .window = _window,
    };
    return instance.createXcbSurfaceKHR(xcbSurfaceCreateInfo);
}

} // namespace rr
