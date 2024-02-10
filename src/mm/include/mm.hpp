#pragma once

#include <filesystem>

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

private:
    void* _addr = nullptr;
    size_t _len = 0;
};

} // namespace rr
