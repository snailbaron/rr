#pragma once

#include <exception>
#include <ostream>
#include <source_location>
#include <sstream>
#include <string>
#include <utility>

namespace rr {

template <class T>
concept Streamable = requires (std::ostream& output, const T& x) {
    output << x;
};

class Error : public std::exception {
public:
    Error(std::source_location sl = std::source_location::current())
    {
        auto stream = std::ostringstream{};
        stream << sl.file_name() << ":" << sl.line() << ":" << sl.column() <<
            " (" << sl.function_name() << "): ";
        _message = stream.str();
    }

    template <Streamable T>
    Error& operator<<(const T& x) &
    {
        append(x);
        return *this;
    }

    template <Streamable T>
    Error&& operator<<(const T& x) &&
    {
        append(x);
        return std::move(*this);
    }

    const char* what() const noexcept override
    {
        return _message.c_str();
    }

private:
    template <Streamable T>
    void append(const T& x)
    {
        auto stream = std::ostringstream{_message, std::ios::ate};
        stream << x;
        _message = stream.str();
    }

    std::string _message;
};

} // namespace rr
