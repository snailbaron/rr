#pragma once

#if defined(_WIN32)
    #include <Windows.h>
#endif

#include <filesystem>

namespace rr {

class DynamicLibrary {
public:
    DynamicLibrary() = default;
    DynamicLibrary(const std::filesystem::path& path);
    ~DynamicLibrary();

    DynamicLibrary(const DynamicLibrary&) = delete;
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;

    DynamicLibrary(DynamicLibrary&& other) noexcept;
    DynamicLibrary& operator=(DynamicLibrary&& other) noexcept;

    explicit operator bool() const noexcept;

    void load(const std::filesystem::path& path);
    void clear() noexcept;

    template <class F>
    F getProcAddress(const char* name) const
    {
        return reinterpret_cast<F>(loadInternalProcAddress(name));
    }

    friend void swap(DynamicLibrary& lhs, DynamicLibrary& rhs) noexcept;

private:
    void* loadInternalProcAddress(const char* name) const;

#if defined(_WIN32)
    HINSTANCE _instance = NULL;
#endif
};


} // namespace rr