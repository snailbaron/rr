#pragma once

#include <keys.hpp>

#include <variant>

namespace rr::ev {

struct Button {
    ButtonCode button {};
    bool press = false;
};

class CloseWindow {};

struct Key {
    KeyCode keyCode {};
    ModButtonState mod {};
    bool press = false;
};

class Event {
    using EventsVariant = std::variant<
       Button,
       CloseWindow,
       Key
    >;

public:
    Event(EventsVariant variant)
        : _event(variant)
    { }

    const Button* button() const
    {
        return std::get_if<Button>(&_event);
    }

    const CloseWindow* closeWindow() const
    {
        return std::get_if<CloseWindow>(&_event);
    }

    const Key* key() const
    {
        return std::get_if<Key>(&_event);
    }

private:
    EventsVariant _event;
};

} // namespace rr::ev
