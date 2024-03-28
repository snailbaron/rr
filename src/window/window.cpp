#include "window.hpp"

#include "error.hpp"

#if defined(__linux)
    #include "xcb_window.hpp"
#elif defined(_WIN32)
    #include "windows_window.hpp"
#endif

namespace rr {

std::unique_ptr<Window> Window::create(Api api, const WindowOptions& options)
{
    switch (api) {
#if defined(__linux__)
        case Api::XCB:
            return std::make_unique<XcbWindow>(options);
#elif defined(_WIN32)
        case Api::Win32:
            return std::make_unique<WindowsWindow>(options);
#endif
    }

    throw Error{} << "unknown API: " << std::to_underlying(api);
}

} // namespace rr
