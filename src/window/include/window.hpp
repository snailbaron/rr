#pragma once

#include <event.hpp>

#include <memory>
#include <optional>

namespace rr {

enum class Api {
    XCB,
};

struct WindowOptions {
    const char* displayName = nullptr;
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    int borderWidth = 0;
};

class Window {
public:
    static std::unique_ptr<Window> create(Api api, const WindowOptions& options);

    virtual ~Window() = default;

    virtual std::optional<ev::Event> poll() const = 0;
};

} // namespace rr
