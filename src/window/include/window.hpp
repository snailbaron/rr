#pragma once

#include <event.hpp>

#include <vulkan/vulkan_raii.hpp>

#include <memory>
#include <optional>

namespace rr {

enum class Api {
#if defined(__linux__)
    XCB,
#elif defined(_WIN32)
    Win32
#endif
};

struct WindowSize {
    int width = 0;
    int height = 0;
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
    virtual WindowSize size() const = 0;
    virtual vk::raii::SurfaceKHR createVulkanSurface(
        const vk::raii::Instance& instance) const = 0;
};

} // namespace rr
