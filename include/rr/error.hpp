#pragma once

#include <exception>
#include <string>
#include <string_view>

namespace rr {

class Error : public std::exception {
public:
    Error(std::string_view message)
        : _message(message)
    { }

    const char* what() const noexcept override
    {
        return _message.c_str();
    }

private:
    std::string _message;
};

} // namespace rr
