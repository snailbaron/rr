#pragma once

#include <filesystem>

#ifdef _WIN32
    #include <Windows.h>
#endif

namespace rr {

class MemoryMap {
public:
    MemoryMap() = default;
    explicit MemoryMap(const std::filesystem::path& path);
    ~MemoryMap();

    MemoryMap(MemoryMap&& other) noexcept;
    MemoryMap& operator=(MemoryMap&& other) noexcept;

    MemoryMap(const MemoryMap&) = delete;
    MemoryMap& operator=(const MemoryMap&) = delete;

    void map(const std::filesystem::path& path);
    void clear();

    void* addr() const;
    size_t size() const;

    friend void swap(MemoryMap& x, MemoryMap& y) noexcept;

private:
    void* _addr = nullptr;
    size_t _len = 0;
#if defined(_WIN32)
    HANDLE _mappingHandle = INVALID_HANDLE_VALUE;
#endif
};

} // namespace rr
