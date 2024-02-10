#include "window.hpp"

#include "xcb_window.hpp"

namespace rr {

std::unique_ptr<Window> Window::create(Api api, const WindowOptions& options)
{
    switch (api) {
        case Api::XCB:
            return std::make_unique<XcbWindow>(options);
    }
}

} // namespace rr
